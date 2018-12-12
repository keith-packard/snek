n = 0
m = 3

def g():
    global n
    n = 1
    m = 2

g()

print("n %d m %d" % (n,m))
if not (n == 1 and m == 3):
    exit(1)
