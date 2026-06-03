
DEPDIR = .deps
df = $(DEPDIR)/$(*F)
MKDIR_P = mkdir -p

DEPDIR = .deps
df = $(DEPDIR)/$(*F)
MKDIR_P = mkdir -p

SRCGUI =  Dict.cpp APV_RAW_PED.cpp apv_raw.cpp xtalk_cor2.cpp
SRC =  $(SRCGUI)

OBJ=$(SRC:.cpp=.o)
HDR=$(SRC:.cpp=.h)

EXEC=CrosstalkCleaning2

CC=g++
CFLAGSBASE=-c -O3 -g -Wall 
#-Wextra
#-Wmost does not work in SLC 6 ?



#for this to work root must be set up already (source thisroot.sh)
ROOTINC=-I`root-config --incdir`
ROOTCFLAGS=`root-config --cflags --auxcflags`
ROOTLIB=`root-config --glibs` -lMinuit
ROOTLDFLAGS=`root-config --ldflags` 

CFLAGS=$(CFLAGSBASE) $(ROOTINC) $(ROOTCFLAGS)
LDFLAGS= $(ROOTLDFLAGS) $(ROOTLIB) 




.PHONY : clean directories

all: directories $(EXEC)

directories: $(DEPDIR)

$(DEPDIR):
	@$(MKDIR_P) $(DEPDIR)

$(EXEC): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)  

%.o: %.cpp    
	$(CC) $(CFLAGS) -MD -o $@ $<
	@cp $*.d $(df).P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $(df).P; \
	rm -f $*.d

-include $(SRC:%.cpp=$(DEPDIR)/%.P)

Dict.cpp:  LinkDef.h
	@echo "* Generating root dictionary"
	rootcint -f $@ -c $^ 

clean:
	@echo "* Clean"
	@rm -f $(OBJ) Dict.cpp Dict.h
	@rm -f $(DEPDIR)/*.P
	@rm -f *.o


