#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

import sys
import argparse


def fprint(msg="", end="\n", file=sys.stdout):
    file.write(msg)
    file.write(end)


builtin_id = 1


class SnekBuiltin:
    def __init__(self, name, param, value, globals=None, condition=None):
        self.name = name
        self.value = None
        self.init = None
        self.alias = None
        self.condition = condition
        self.id = -1
        self.full_name = name.replace(".", "_")
        self.global_alias = False
        if param[0].isalpha():
            self.keyword = param
            self.nformal = -3
        else:
            self.keyword = False
            self.nformal = int(param)
        if value and value[0] == "=":
            if self.nformal == -2:
                self.init = value[1:]
            else:
                self.alias = value[1:]
        else:
            self.value = value
            for g in globals:
                if name.startswith(g + "."):
                    self.alias = name
                    self.global_alias = True
                    self.name = name.removeprefix(g + ".")
                    self.full_name = self.name
                    break

    def __eq__(self, other):
        return self.name == other.name

    def is_name(self):
        return self.nformal == -2 and self.value is None

    def is_value(self):
        return self.nformal == -2 and self.value is not None

    def is_init(self):
        return self.nformal == -2 and self.init is not None

    def is_func(self):
        return self.nformal >= -1

    def kind_order(self):
        if self.is_func():
            return 0
        elif self.keyword:
            return 2
        else:
            return 1

    def value_order(self, other):
        if self.value is None and other.value is not None:
            return 1
        if other.value is None and self.value is not None:
            return -1
        return 0

    def __lt__(self, other):
        o = self.kind_order() - other.kind_order()
        if o < 0:
            return True
        elif o > 0:
            return False
        o = self.value_order(other)
        if o != 0:
            return o < 0
        return self.name < other.name

    def snek_name(self):
        return self.name

    def cpp_name(self):
        return "SNEK_BUILTIN_%s" % self.full_name

    def func_name(self):
        if self.is_value():
            if self.value[0].isdigit():
                return "(snek_poly_t)(float)%s" % self.value
            return self.value
        name = self.name
        if self.alias is not None:
            name = self.alias
        return "snek_builtin_%s" % (name.replace(".", "_"))

    def func_field(self):
        if self.is_value():
            return ".value"
        elif self.nformal == -1:
            return ".funcv"
        return ".func%d" % self.nformal

    def set_id(self):
        global builtin_id
        if not self.keyword and self.id == -1:
            self.id = builtin_id
            builtin_id += 1

    def __str__(self):
        return (
            "builtin(name='%s', value='%r', init='%r', alias='%s', full_name='%s')"
            % (self.name, self.value, self.init, self.alias, self.full_name)
        )


headers = []
builtins = []
globals = []
aliases = False


def add_builtin(name, id, value, condition):
    global builtins, globals, aliases
    builtin = SnekBuiltin(name, id, value, globals, condition)
    for b in builtins:
        if b.name == builtin.name:
            print("%s: skipping duplicate" % name)
            return
    builtins += [builtin]
    if aliases and builtin.global_alias:
        builtin = SnekBuiltin(name, id, value, [], condition)
        builtins += [builtin]


def load_builtins(filename):
    global headers
    current_condition = None
    f = open(filename)
    for line in f.readlines():
        line = line.rstrip()
        if len(line) == 0:
            pass
        elif line.startswith("#ifdef"):
            current_condition = line.split(" ")[1].strip()
        elif line.startswith("#endif") and current_condition:
            current_condition = None
        elif line[0] == "#":
            if len(line) > 1 and line[1] != " ":
                headers += [line]
        else:
            bits = line.split(",")
            value = None
            if len(bits) > 2:
                value = ",".join(bits[2:]).strip()
            add_builtin(bits[0].strip(), bits[1].strip(), value, current_condition)


def dump_headers(fp):
    for line in headers:
        fprint("%s" % line, file=fp)


init_header = """#define snek_init() {\\
	snek_globals = snek_alloc(sizeof (snek_frame_t));\\
	snek_globals->prev = SNEK_OFFSET_NONE;\\
	snek_globals->code = SNEK_OFFSET_NONE;\\"""


def dump_init(fp):
    use_list_build = False
    use_string_build = False
    fprint(init_header, file=fp)
    for name in sorted(builtins):
        if name.is_init():
            if "snek_list_build" in name.init:
                use_list_build = True
            if "snek_string_build" in name.init:
                use_string_build = True
            fprint("    {\\", file=fp)
            fprint("        snek_stack_push((%s));\\" % name.init, file=fp)
            fprint(
                "        *snek_id_ref(%s, true) = snek_stack_pop();\\"
                % name.cpp_name(),
                file=fp,
            )
            fprint("    }\\", file=fp)
    fprint("}", file=fp)
    if use_list_build:
        fprint("#define SNEK_LIST_BUILD", file=fp)
    if use_string_build:
        fprint("#define SNEK_STRING_BUILD", file=fp)


def dump_max_len(fp):
    max_len = 0
    for name in builtins:
        if len(name.name) > max_len:
            max_len = len(name.name)
    fprint("#define SNEK_BUILTIN_NAMES_MAX_LEN %d" % max_len, file=fp)
    fprint("#define SNEK_BUILTIN_NAMES_MAX_ARGS %d" % max_args(), file=fp)


def set_ids():
    for name in sorted(builtins):
        name.set_id()


def dump_names(fp):
    fprint(
        "static CONST uint8_t snek_builtin_names[] = {",
        file=fp,
    )
    total = 0
    for name in sorted(builtins):
        if name.condition:
            fprint("#ifdef %s" % name.condition, file=fp)

        if name.keyword:
            fprint("\t%s, " % name.keyword, end="", file=fp)
            total += 1
        else:
            fprint("\t", end="", file=fp)
        for c in name.name:
            fprint("'%c', " % c, end="", file=fp)
            total += 1
        fprint("0,", file=fp)

        if name.condition:
            fprint("#endif", file=fp)

        total += 1
    fprint("};", file=fp)
    fprint("#define SNEK_BUILTIN_NAMES_SIZE %d" % total, file=fp)


def trim_mu(n):
    if n.find(".") >= 0:
        return n[: n.find(".")]
    return n


mu_skip = ["len", "print", "end", "False", "True", "ord", "chr", "None"]


def dump_mu(fp):
    all_names = {}
    for name in sorted(builtins):
        if not name.keyword and not name.name in mu_skip:
            all_names[trim_mu(name.name)] = True

    for name in sorted(all_names.keys()):
        fprint("        '%s'," % name, file=fp)


def max_args():
    max = 0
    for name in sorted(builtins):
        if name.keyword or not name.is_func():
            continue
        if name.nformal > max:
            max = name.nformal
    return max


def dump_decls(fp):
    for name in sorted(builtins):
        if not name.is_func():
            continue
        fprint("extern snek_poly_t", file=fp)
        fprint("%s(" % name.func_name(), file=fp, end="")
        if name.nformal == -1:
            fprint(
                "uint8_t nposition, uint8_t nnamed, snek_poly_t *args", end="", file=fp
            )
        elif name.nformal == 0:
            fprint("void", end="", file=fp)
        else:
            for a in range(name.nformal):
                fprint("snek_poly_t a%d" % a, end="", file=fp)
                if a < name.nformal - 1:
                    fprint(", ", end="", file=fp)
        fprint(");", file=fp)
        fprint(file=fp)


def dump_builtins(fp):
    fprint("CONST snek_builtin_t snek_builtins[] = {", file=fp)

    for name in sorted(builtins):
        if name.keyword:
            continue

        if name.is_name():
            continue

        fprint("\t[%s - 1] = {" % name.cpp_name(), file=fp)
        if name.is_func():
            fprint("\t\t.nformal=%d," % name.nformal, file=fp)
        fprint("\t\t%s = %s," % (name.func_field(), name.func_name()), file=fp)
        fprint("\t},", file=fp)
    fprint("};", file=fp)


def dump_cpp(fp):
    marked_funcs = False
    marked_values = False
    for name in sorted(builtins):
        if name.keyword:
            continue
        if name.is_value() and not marked_funcs:
            fprint("#define SNEK_BUILTIN_MAX_FUNC %d" % name.id, file=fp)
            marked_funcs = True
        if name.is_name() and not marked_values:
            fprint("#define SNEK_BUILTIN_MAX_BUILTIN %d" % name.id, file=fp)
            marked_values = True
        fprint("#define %s %d" % (name.cpp_name(), name.id), file=fp)

    fprint("#define SNEK_BUILTIN_END %d" % (builtin_id), file=fp)


def builtin_main():
    global globals, aliases

    parser = argparse.ArgumentParser(description="Construct Snek builtin data.")
    parser.add_argument(
        "builtins", metavar="F", nargs="+", help="input files describing builtins"
    )
    parser.add_argument("-o", "--output", dest="output", help="output file")
    parser.add_argument(
        "-m", "--mu", action="store_true", help="output just non-keyword constants"
    )
    parser.add_argument(
        "-a", "--alias", action="store_true", help="--globals are also namespaced"
    )
    parser.add_argument(
        "-g",
        "--global",
        action="append",
        dest="globals",
        help="remove this leading namespace",
    )

    args = parser.parse_args()

    if args.globals:
        globals = args.globals

    aliases = args.alias

    for b in args.builtins:
        load_builtins(b)

    fp = sys.stdout

    if args.output:
        fp = open(args.output, mode="w")

    if args.mu:
        dump_mu(fp)
        return

    fprint("#ifdef SNEK_BUILTIN_DATA", file=fp)
    fprint("#undef SNEK_BUILTIN_DATA", file=fp)

    set_ids()

    dump_names(fp)

    fprint(file=fp)

    max_formals = max_args()

    dump_builtins(fp)

    fprint(file=fp)

    fprint("#else /* SNEK_BUILTIN_DATA */", file=fp)

    fprint("#ifdef SNEK_BUILTIN_DECLS", file=fp)
    fprint("#undef SNEK_BUILTIN_DECLS", file=fp)
    dump_decls(fp)
    fprint("#else /* SNEK_BUILTIN_DECLS */", file=fp)

    dump_max_len(fp)

    dump_headers(fp)

    dump_init(fp)

    dump_cpp(fp)

    fprint("#endif /* SNEK_BUILTIN_DECLS */", file=fp)
    fprint("#endif /* SNEK_BUILTIN_DATA */", file=fp)


builtin_main()
