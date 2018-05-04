OBJS = c_except.o my_setjmp.o ptr_vector.o divider.o

CFLAGS=-g
ASFLAGS=-g


divider : $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	
check: test_setjmp
	./test_setjmp

test_setjmp : test_setjmp.o my_setjmp.o
	$(CC) $(LDFLAGS) $^ -o $@

test_setjmp.o : my_setjmp.h
ptr_vector.c : ptr_vector.h
c_except.c : c_except.h my_setjmp.h
divider.c : c_except.h my_setjmp.h

clean :
	rm -f *.o *~ divider divider.exe

