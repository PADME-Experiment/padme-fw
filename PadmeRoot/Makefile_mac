########################################################################
#
# Compile PadmeRoot shared library
#
# Author Emanuele Leonardi <emanuele.leonardi@roma1.infn.it>
#
#########################################################################

# Set name of library
LIBNAME := PadmeRoot

# Define directories for different file types
SDIR	= src
ODIR	= obj
DDIR	= deps
IDIR	= include
LDIR	= lib

# Define root environment
ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTINCDIR   := $(shell $(ROOTCONFIG) --incdir)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)

# Define c++ compiler and flags
CXX = g++
CXXFLAGS = -O2 -g -fPIC -Wall $(ROOTCFLAGS) -I$(IDIR)
CPPFLAGS = $(ROOTCFLAGS) -I$(IDIR) -I/usr/include
CINTFLAGS = -I$(IDIR)

# Define command to create dependencies files
DEPEND = $(CXX) -MM

# Get list of files to process
INCLUDES := $(wildcard $(IDIR)/*.hh)
SOURCES  := $(wildcard $(SDIR)/*.cc)
DEPS     := $(addprefix $(DDIR)/,$(notdir $(SOURCES:.cc=.ccd)))
OBJECTS  := $(addprefix $(ODIR)/,$(notdir $(SOURCES:.cc=.o))) $(ODIR)/PersistencyDict.o

# Define comlete library name
LIBRARY := $(LDIR)/lib$(LIBNAME).so

#########################################################################

all: $(LIBNAME)

$(LIBNAME) : $(DEPS) $(OBJECTS)
	$(CXX) $(ROOTCFLAGS) $(ROOTLIBS) -shared -o $(LIBRARY) $(ROOTLDFLAGS) $(CXXFLAGS) $(OBJECTS)

$(ODIR)/%.o : $(SDIR)/%.cc $(DDIR)/%.ccd
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DDIR)/%.ccd : $(SDIR)/%.cc
	$(DEPEND) $(CPPFLAGS) -MT $(addprefix $(ODIR)/,$(notdir $(<:.cc=.o))) $< > $@

PersistencyDict.cc: $(INCLUDES) PersistencyLinkDef.hh
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c $(CINTFLAGS) -p $^

$(ODIR)/PersistencyDict.o: PersistencyDict.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY:	all clean cleanall try

clean:
	@echo Cleaning object files...
	rm -f $(ODIR)/*.o

cleanall:
	@echo Cleaning all...
	rm -f $(ODIR)/*.o
	rm -f $(LIBRARY)
	rm -f PersistencyDict.*

try:
	@echo Current make parameters:
	@echo LIBNAME = $(LIBNAME)
	@echo LIBRARY = $(LIBRARY)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo LIBS = $(LIBS)
	@echo SOURCES = $(SOURCES)
	@echo OBJECTS = $(OBJECTS)
	@echo DEPS = $(DEPS)
	@echo ROOTCFLAGS = $(ROOTCFLAGS)
	@echo ROOTINCDIR = $(ROOTINCDIR)
	@echo ROOTLDFLAGS = $(ROOTLDFLAGS)
	@echo ROOTLIBS = $(ROOTLIBS)
	@echo ROOTGLIBS = $(ROOTGLIBS)

include $(DEPS)
