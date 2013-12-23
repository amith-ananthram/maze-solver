CC=gcc
CFLAGS=-g -Wall -pedantic -std=c99 -ggdb -lm -D_SVID_SOURCE
SOURCES=./amazing.c ./amazing.h ./mazefuncs.c ./mazefuncs.h 
SOURCES2=./amstartup2.c $(SOURCES)
CFILES=./amazing.c ./mazefuncs.c 
CFILES2=./amstartup2.c 

UTILDIR=./utilities/
UTILFLAG=-ltseutil
UTILLIB=$(UTILDIR)libtseutil.a
UTILC=$(UTILDIR)messaging.c $(UTILDIR)sharedmem.c
UTILH=$(UTILC:.c=.h)

amazing:	$(SOURCES) $(UTILLIB)
			$(CC) $(CFLAGS) -o amazing $(CFILES) -L$(UTILDIR) $(UTILFLAG)

amstartup2:	$(SOURCES2) $(UTILLIB)
			$(CC) $(CFLAGS) -o AMStartup2 $(CFILES2) -L$(UTILDIR) $(UTILFLAG)

$(UTILLIB): $(UTILC) $(UTILH)
			cd $(UTILDIR); make;

clean:
		rm -f *~
		rm -f core*
		rm -f amazing
		rm -f AMStartup2
		rm -f utilities/libtseutil.a
		rm -f utilities/*~
