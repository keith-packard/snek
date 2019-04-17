#!/usr/bin/python3
# -*- coding: utf-8 -*-
#
# Copyright © 2019 Keith Packard <keithp@keithp.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#

import sys
import argparse

def fprint(msg='', end='\n', file=sys.stdout):
    file.write(msg)
    file.write(end)

builtin_id = 1

class SnekBuiltin:
    name = ""
    id = -1
    nformal = 0
    keyword = False
    def __init__(self, name, param):
        self.name = name
        if param[0].isalpha():
            self.keyword = param
        else:
            self.keyword = False
            self.nformal = int(param)

    def __eq__(self,other):
        return self.name == other.name

    def __lt__(self,other):
        if self.nformal != other.nformal:
            if self.nformal == -2:
                return False
            if other.nformal == -2:
                return True
        return self.name < other.name

    def snek_name(self):
        return self.name

    def cpp_name(self):
        return "SNEK_BUILTIN_%s" % (self.name.replace(".", "_"))

    def func_name(self):
        return "snek_builtin_%s" % (self.name.replace(".", "_"))

    def func_field(self):
        if self.nformal == -1:
            return ".funcv"
        return ".func%d" % self.nformal

    def set_id(self):
        global builtin_id
        if not self.keyword and self.id == -1:
            self.id = builtin_id
            builtin_id += 1

headers=[]
builtins = []

def add_builtin(name, id):
    global builtins
    builtins += [SnekBuiltin(name, id)]

def load_builtins(filename):
    global headers
    f = open(filename)
    for line in f.readlines():
        line = line.rstrip()
        if len(line) == 0:
            pass
        elif line[0] == '#':
            if len(line) > 1 and line[1] != ' ':
                headers += [line]
        else:
            bits = line.split(",")
            add_builtin(bits[0].strip(), bits[1].strip())

def dump_headers(fp):
    for line in headers:
        fprint("%s" % line, file=fp)

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
    fprint("static const uint8_t SNEK_BUILTIN_NAMES_DECLARE(snek_builtin_names)[] = {", file=fp)
    total = 0
    for name in sorted(builtins):
        if name.keyword:
            fprint("\t%s | 0x80, " % name.keyword, end='', file=fp)
        else:
            fprint("\t%d, " % name.id, end='', file=fp)
        total += 1
        for c in name.name:
            fprint("'%c', " % c, end='', file=fp)
            total += 1
        fprint("0,", file=fp)
        total += 1
    fprint("};", file=fp)
    fprint("#define SNEK_BUILTIN_NAMES_SIZE %d" % total, file=fp)

def max_args():
    max = 0
    for name in sorted(builtins):
        if name.keyword or name.nformal == -2:
            continue
        if name.nformal > max:
            max = name.nformal
    return max

def dump_decls(fp):
    for name in sorted(builtins):
        if name.keyword or name.nformal == -2:
            continue
        fprint("extern snek_poly_t", file=fp)
        fprint("%s(" % name.func_name(), file=fp, end='')
        if name.nformal == -1:
            fprint("uint8_t nposition, uint8_t nnamed, snek_poly_t *args", end='', file=fp)
        elif name.nformal == 0:
            fprint("void", end='', file=fp)
        else:
            for a in range(name.nformal):
                fprint("snek_poly_t a%d" % a, end='', file=fp)
                if a < name.nformal-1:
                    fprint(", ", end='', file=fp)
        fprint(");", file=fp)
        fprint(file=fp)

def dump_builtins(fp):
    fprint("const snek_builtin_t SNEK_BUILTIN_DECLARE(snek_builtins)[] = {", file=fp)

    for name in sorted(builtins):
        if name.keyword or name.nformal == -2:
            continue

        fprint("\t[%s - 1] = {" % name.cpp_name(), file=fp)
        fprint("\t\t.nformal=%d," % name.nformal, file=fp)
        fprint("\t\t%s = %s," % (name.func_field(), name.func_name()), file=fp)
        fprint("\t},", file=fp)
    fprint("};", file=fp)

def dump_cpp(fp):
    marked_funcs = False
    for name in sorted(builtins):
        if name.keyword:
            continue
        if name.nformal == -2 and not marked_funcs:
            fprint("#define SNEK_BUILTIN_MAX_FUNC %d" % name.id, file=fp)
            marked_funcs = True
        fprint("#define %s %d" % (name.cpp_name(), name.id), file=fp)

    fprint("#define SNEK_BUILTIN_END %d" % (builtin_id), file=fp)


def builtin_main():

    parser = argparse.ArgumentParser(description="Construct Snek builtin data.")
    parser.add_argument('builtins', metavar='F', nargs='+',
                        help='input files describing builtins')
    parser.add_argument('-o', '--output', dest='output', help='output file')

    args=parser.parse_args()

    for b in args.builtins:
        load_builtins(b)

    fp = sys.stdout

    if args.output:
        fp = open(args.output, mode='w')

    fprint("#ifdef SNEK_BUILTIN_DATA", file=fp)
    fprint("#undef SNEK_BUILTIN_DATA", file=fp)
    fprint("#ifndef SNEK_BUILTIN_NAMES_DECLARE", file=fp)
    fprint("#define SNEK_BUILTIN_NAMES_DECLARE(n) n", file=fp)
    fprint("#endif", file=fp)

    set_ids()

    dump_names(fp)

    fprint(file=fp)

    max_formals = max_args()

    fprint("#ifndef SNEK_BUILTIN_DECLARE", file=fp)
    fprint("#define SNEK_BUILTIN_DECLARE(n) n", file=fp)
    fprint("#endif", file=fp)

    dump_builtins(fp)

    fprint(file=fp)

    fprint("#else /* SNEK_BUILTIN_DATA */", file=fp)

    fprint("#ifdef SNEK_BUILTIN_DECLS", file=fp)
    fprint("#undef SNEK_BUILTIN_DECLS", file=fp)
    dump_decls(fp)
    fprint("#else /* SNEK_BUILTIN_DECLS */", file=fp)

    dump_max_len(fp)

    dump_headers(fp)

    fprint("#ifndef SNEK_BUILTIN_NFORMAL", file=fp)
    fprint("#define SNEK_BUILTIN_NFORMAL(b) ((b)->nformal)", file=fp)
    fprint("#define SNEK_BUILTIN_FUNCV(b) ((b)->funcv)", file=fp)
    for f in range(max_formals+1):
        fprint("#define SNEK_BUILTIN_FUNC%d(b) ((b)->func%d)" % (f, f), file=fp)
    fprint("#endif", file=fp)

    dump_cpp(fp)

    fprint("#endif /* SNEK_BUILTIN_DECLS */", file=fp)
    fprint("#endif /* SNEK_BUILTIN_DATA */", file=fp)

builtin_main()
