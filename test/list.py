#
# tuples don't have in-place operation,
# so += makes a new tuple
#

a = ()
b = a
a += (1,2)

if a != (1,2): exit(1)
if b != (): exit(1)

#
# lists have in-place operation,
# so += does not make a new list
# while a + b does make a new list
#

a = []
b = a
c = a + [1,2]
a += [1,2]

if a != [1,2]: exit(1)
if b != [1,2]: exit(1)
if c != [1,2]: exit(1)

a[0] = 3

if a != [3,2]: exit(1)
if b != [3,2]: exit(1)
if c != [1,2]: exit(1)
