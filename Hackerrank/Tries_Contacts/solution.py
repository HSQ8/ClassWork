class Letter(object):
    def __init__(self):
        self.offset = 97
        self.lst = [None] * 26
        self.count = 0
        
    def add(self,_str):
        self.count += 1
        if ( len(_str) > 0):
            _char = _str[:1]
            index = ord(_char) - self.offset
            if(self.lst[index] != None):
                self.lst[index].add(_str[1:])
            else:
                self.lst[index] = Letter()
                self.lst[index].add(_str[1:])
            
    def find(self, _str):
        if (len(_str) == 0):
            return self.count
        elif (len(_str) > 0):
            _char = _str[:1]
            index = ord(_char) - self.offset
            if (self.lst[index] != None ):
                return self.lst[index].find(_str[1:])
            return 0
        
class Contact(object):
    def __init__(self):
        self.lst = [None] * 26
        self.offset = 97
    def add(self, _str):
        if ( len(_str) > 0):
            _char = _str[:1]
            index = ord(_char) - self.offset
            if(self.lst[index] != None):
                self.lst[index].add(_str[1:])
            else:
                self.lst[index] = Letter()
                self.lst[index].add(_str[1:])
                
    def find(self, _str):
        if (len(_str) == 0):
            return 0
        elif (len(_str) > 0):
            _char = _str[:1]
            index = ord(_char) - self.offset
            if (self.lst[index] != None ):
                return self.lst[index].find(_str[1:])
            return 0 
                
n = int(input().strip())
my_contact = Contact()
for a0 in range(n):
    op, contact = input().strip().split(' ')
    if(op == "add"):
        my_contact.add(contact)
    elif(op == "find"):
        print(my_contact.find(contact))