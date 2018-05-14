"""
Detect a cycle in a linked list. Note that the head pointer may be 'None' if the list is empty.

A Node is defined as: 
 
    class Node(object):
        def __init__(self, data = None, next_node = None):
            self.data = data
            self.next = next_node
"""
def hascyclelist(head,lstheads):
    if head in lstheads:
        return True
    lstheads.append(head)
    if head.next == None:
        return False
    if not head.next == None:
        return hascyclelist(head.next,lstheads)
    

def has_cycle(head):
    return hascyclelist(head,[])
    
    
