#!/usr/bin/python3

import sys
import argparse

builtin_id = 1

class NewtBuiltin:
    name = ""
    id = 0
    nformal = 0
    def __init__(self, name, nformal):
        global builtin_id
        self.id = builtin_id
        builtin_id += 1
        self.name = name
        self.nformal = nformal

    def __eq__(self,other):
        return self.name == other.name

    def __lt__(self,other):
        return self.name < other.name

    def newt_name(self):
        return self.name

    def cpp_name(self):
        return "NEWT_BUILTIN_%s" % (self.name.replace(".", "_"))

    def func_name(self):
        return "newt_builtin_%s" % (self.name.replace(".", "_"))


    def func_field(self):
        if self.nformal == -1:
            return ".funcv"
        return ".func%d" % self.nformal

builtins = []

def add_builtin(name, id):
    global builtins
    builtins += [NewtBuiltin(name, id)]

def load_builtins(filename):
    f = open(filename)
    for line in f.readlines():
        bits = line.split(",")
        add_builtin(bits[0], int(bits[1]))

def dump_names(fp):
    print("static const uint8_t newt_builtin_names[] = {", file=fp)
    for name in sorted(builtins):
        print("\t%d, " % name.id, end='', file=fp)
        for c in name.name:
            print("'%c', " % c, end='', file=fp)
        print("0,", file=fp)
    print("};", file=fp)

def dump_builtins(fp):
    for name in sorted(builtins):
        print("extern newt_poly_t", file=fp)
        print("%s(" % name.func_name(), file=fp, end='')
        if name.nformal == -1:
            print("int nactuals, ...", end='', file=fp)
        else:
            for a in range(name.nformal):
                print("newt_poly_t a%d" % a, end='', file=fp)
                if a < name.nformal-1:
                    print(", ", file=fp)
        print(");", file=fp)
        print(file=fp)

    print("const newt_builtin_t newt_builtins[] = {", file=fp)

    for name in sorted(builtins):
        print("\t[%s - 1] {" % name.cpp_name(), file=fp)
        print("\t\t.nformal=%d," % name.nformal, file=fp)
        print("\t\t%s = %s," % (name.func_field(), name.func_name()), file=fp)
        print("\t},", file=fp)

    print("};", file=fp)

def dump_cpp(fp):
    for name in sorted(builtins):
        print("#define %s %d" % (name.cpp_name(), name.id), file=fp)

    print("#define NEWT_BUILTIN_END %d" % (builtin_id), file=fp)


def builtin_main():

    parser = argparse.ArgumentParser(description="Construct Newt builtin data.")
    parser.add_argument('builtins', metavar='F', nargs='+',
                        help='input files describing builtins')
    parser.add_argument('-o', '--output', dest='output', help='output file')

    args=parser.parse_args()

    for b in args.builtins:
        load_builtins(b)

    fp = sys.stdout

    if args.output:
        fp = open(args.output, mode='w')

    print("#ifdef NEWT_BUILTIN_DATA", file=fp)

    dump_names(fp)

    print(file=fp)

    dump_builtins(fp)

    print(file=fp)

    print("#else /* NEWT_BUILTIN_DATA */", file=fp)

    dump_cpp(fp)

    print("#endif /* NEWT_BUILTIN_DATA */", file=fp)

builtin_main()
