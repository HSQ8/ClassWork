""" Node is defined as
class node:
    def __init__(self, data):
        self.data = data
        self.left = None
        self.right = None
"""

'''
import sys
def check(root,min,max):
    if root == None:
        return True
    if root.left != None:
        return (check(root.left,root.data,max) and root.data > min and root.data < max)
    if root.right != None:
        return (check(root.right,root.data,max) and root.data > min and root.data < max)
def checkBST(root):
    check(root,-(sys.maxsize),sys.maxsize)
       '''
def checkBST(root):
    return(check_in_order(root,[-1]))
    
def check_in_order(root,prev):
    result = True
    if root.left is not None:
        result &= check_in_order(root.left,prev)
    if prev[0] >= root.data:
        return False
    prev[0] = root.data
    if root.right is not None:
        result &= check_in_order(root.right,prev)
    return result
        
    
    
   