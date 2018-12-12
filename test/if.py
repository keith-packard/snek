def g(i):
    if i == 0:
        return 100
    elif i == 1:
        return 101
    elif i == 2:
        return 102
    else:
        return 103

if not (g(0) == 100 and g(1) == 101 and g(2) == 102 and g(3) == 103 and g("hello") == 103):
    exit(1)
