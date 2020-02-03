########################################################################
#
# Compile PadmeTrig code
#
# Author Emanuele Leonardi <emanuele.leonardi@roma1.infn.it>
#
#########################################################################

EXE =	PadmeTrig.exe

SDIR	= src
ODIR	= obj
IDIR	= include

SRC =	$(wildcard $(SDIR)/*.c)
INC =	$(wildcard $(IDIR)/*.h)

OBJ =	$(addprefix $(ODIR)/,$(notdir $(SRC:.c=.o)))

DEPS = $(INC) Makefile

CC	=	gcc
CFLAGS	=	-fPIC -DLINUX -O2 -g -Wall -I$(IDIR)
# Use these for better debug
#CC	=	g++
#CFLAGS	=	-DLINUX -O0 -g -Wall -I$(IDIR)

#LIBS	=	-L/usr/lib64/mysql -lmysqlclient -lm -lrt
LIBS	=	-lm -lrt

#########################################################################

all:	$(EXE)

$(EXE):	$(OBJ)
	$(CC) $(FLAGS) -o $(EXE) $(OBJ) $(LIBS)

$(ODIR)/%.o:	$(SDIR)/%.c $(DEPS)
		$(CC) $(CFLAGS) -c -o $@ $<

.PHONY:	clean cleanall try

clean:
	rm -f $(ODIR)/*.o

cleanall:
	rm -f $(EXE) $(ODIR)/*.o

try:
	@echo $(EXE)
	@echo $(SRC)
	@echo $(INC)
	@echo $(OBJ)
