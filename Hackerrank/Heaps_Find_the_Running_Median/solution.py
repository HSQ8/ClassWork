#!/bin/python3

import sys


n = int(input().strip())
a = []
a_i = 0
#global leftque = deque([])
#global leftque = deque([])
#def getmedian(num):
def displayMedian(a):
    if len(a) % 2 == 0:
        print('%0.1f' %((a[int(len(a) / 2) - 1] + a[int(len(a) / 2)]) /2))
    elif len(a) % 2 == 1:
        print('%0.1f' %a[int(len(a)/2)])
        
def binaryinsert(a,lower,upper,lst): 
    n = int((upper + lower) / 2)
    if upper == lower:
        lst.insert(lower,a)
    elif a == lst[n]:
        lst.insert(n,a)
    elif a > lst[n]:
        binaryinsert(a,n+1,upper,lst)
    elif a < lst[n]:
        binaryinsert(a,lower,n,lst)
        
for a_i in range(n):
    a_t = int(input().strip())
    binaryinsert(a_t,0,len(a),a)
    displayMedian(a)
    
    
   
