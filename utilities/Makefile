CFILES= ./sharedmem.c ./messaging.c
HFILES=$(CFILES:.c=.h)

library:	$(CFILES) $(HFILES)	
			gcc -Wall -c -std=c99 -D_SVID_SOURCE $(CFILES)
			ar -cvq libtseutil.a *.o

clean:
			rm -f *~
			rm -f *.o
			rm -f *.a
