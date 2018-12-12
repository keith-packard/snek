#!/usr/bin/python3

builtins = [
    "exit",
    "len",
    "print",
    "printn",
]


import sys

fp = sys.stdout

if len(sys.argv) > 1:
    fp = open(sys.argv[1], mode='w')

print("#ifdef NEWT_BUILTIN_DATA", file=fp)

print("static const uint8_t newt_builtin_names[] = {", file=fp)
id = 1
for name in sorted(builtins):
    print("\t%d, " % id, end='', file=fp)
    for c in name:
        print("'%c', " % c, end='', file=fp)
    print("0,", file=fp)
    id += 1
print("};", file=fp)

print("#else /* NEWT_BUILTIN_DATA */", file=fp)

id = 1
for name in sorted(builtins):
    print("#define NEWT_BUILTIN_%s %d" % (name, id), file=fp)
    id += 1

print("#define NEWT_BUILTIN_END %d" % (id), file=fp)

print("#endif /* NEWT_BUILTIN_DATA */", file=fp)
