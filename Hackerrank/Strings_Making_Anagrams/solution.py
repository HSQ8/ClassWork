def hasCommon(a,b):
    for i in a:
        if i in b:
            return True
    return False
def findCommon(a,b):
    for i in a:
        if i in b:
            return i
    return None

def number_needed(a,b):
    list_a = list(a)
    list_b = list(b)
    while (hasCommon(list_a, list_b)):
        removechar = findCommon(list_a, list_b)
        list_a.remove(removechar)
        list_b.remove(removechar)
    return (len(list_a) + len(list_b))
        
a = input().strip()
b = input().strip()

print(number_needed(a, b))



