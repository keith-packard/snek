#
# Check short-circuiting and/or
#

or00 = (0 or 0) + 0
or10 = (1 or 0) + 0
or01 = (0 or 1) + 0
or11 = (1 or 1) + 0

and00 = (0 and 0) + 0
and10 = (1 and 0) + 0
and01 = (0 and 1) + 0
and11 = (1 and 1) + 0

if or00 != False: exit(1)
if or10 != True: exit(1)
if or01 != True: exit(1)
if or11 != True: exit(1)

if and00 != False: exit(1)
if and10 != False: exit(1)
if and01 != False: exit(1)
if and11 != True: exit(1)
