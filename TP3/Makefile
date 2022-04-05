#SYSTEM     = x86-64_osx
#LIBFORMAT  = static_pic
#CPLEXDIR=/Users/renaudchicoisne/Applications/IBM/ILOG/CPLEX_Studio1262/cplex
#CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
#CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
#CPLEXINCDIR   = $(CPLEXDIR)/include


# CHANGEME: This should be the name of your executable
EXE = TP3 

# CHANGEME: Here is the name of all object files corresponding to the source
#           code that you wrote in order to define the problem statement
OBJS = TP3Functions.o TP3.o 

# CHANGEME: Additional libraries
#ADDLIBS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -m64 -lm -lpthread

# CHANGEME: Additional flags for compilation (e.g., include flags)
#DEFINES= -DSYS_UNIX=1 

#INCL = -I$(CPLEXINCDIR) -g $(DEFINES)

# C Compiler command
CC = gcc

# C Compiler options
CFLAGS = -m64 -fPIC -fexceptions -O3 -pipe -DNDEBUG -Wall -g

CYGPATH_W = echo

all: $(EXE)

.SUFFIXES: .c .o .obj

$(EXE): $(OBJS)
	bla=;\
	for file in $(OBJS); do bla="$$bla `$(CYGPATH_W) $$file`"; done; \
	$(CC) $(CFLAGS) -o $@ $$bla $(LIBS) $(ADDLIBS)

clean:
	rm -f $(EXE) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INCL) -c -o $@ $<


.c.obj:
	$(CC) $(CFLAGS) $(INCL) -c -o $@ `$(CYGPATH_W) '$<'`
