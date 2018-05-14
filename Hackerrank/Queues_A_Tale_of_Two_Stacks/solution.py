class MyQueue(object):
    def __init__(self):
        self.lst_a = list() # use for enqueue, this is default storage
        self.lst_b = list() # use for dequeue
    
    def peek(self):
        self.dump()
        a = self.lst_b[-1:]
        self.dump()
        if(len(a)!= 0):
            return a[0]
        
        
    def pop(self):
        a = []
        self.dump()
        if(len(self.lst_b) != 0):
            a = self.lst_b.pop()
        self.dump()
        if (a != None):
            return a
        
        
    def put(self, value):
        self.lst_a.append(value)
    
    def dump(self):
        if(len(self.lst_a) == 0 and len(self.lst_b) != 0):
            while(len(self.lst_b) != 0):
                self.lst_a.append(self.lst_b.pop())
        elif(len(self.lst_b) == 0 and len(self.lst_a) != 0):
            while(len(self.lst_a) != 0):
                self.lst_b.append(self.lst_a.pop())
        

queue = MyQueue()
t = int(input())
for line in range(t):
    values = map(int, input().split())
    values = list(values)
    if values[0] == 1:
        queue.put(values[1])        
    elif values[0] == 2:
        queue.pop()
    else:
        print(queue.peek())
        
