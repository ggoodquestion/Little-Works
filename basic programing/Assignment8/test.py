def D1(fn):
    def newFn(n):
        if n <= 0:
            return "The argument must be greater than 0."
        elif isinstance(n, int) == False:
            return "The argument must be a integer."
        else:
            return fn(n)
            
    return newFn

@D1
def adder(n):
    res = 0
    for i in range(1, n+1):
        res += i
    return res

    
print(adder(5))
print(adder(3.2))
print(adder(-2))