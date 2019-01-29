#
# Test arithmetic operations
#

if 2 + 3 != 5: exit(1)
if 3 + 2 != 5: exit(1)
if 2 - 3 != -1: exit(1)
if 3 - 2 != +1: exit(1)
if 2 * 3 != 6: exit(1)
if 2 * (3 + 4) != 14: exit(1)
if (2 * 3) + 4 != 10: exit(1)
if 2 * 3 + 4 != 10: exit(1)
if 2 ** 3 ** 4 == 4096: exit(1)
if (2 ** 3) ** 4 != 4096: exit(1)
