########################################################################
#
# Compile PadmeRoot shared library
#
# Author Emanuele Leonardi <emanuele.leonardi@roma1.infn.it>
#
#########################################################################

# Set name of library
LIBNAME := PadmeGeom

# Define directories for different file types
SDIR	= src
ODIR	= obj
IDIR	= include
LDIR	= lib
IEXTDIR = /cvmfs/geant4.cern.ch/externals/clhep/2.3.4.3/x86_64-slc6-gcc49-opt/include

## Define root environment
#ROOTCONFIG   := root-config
#ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)

# Define c++ compiler and flags
CXX = g++
#CXXFLAGS = -O2 -g -fPIC -Wall $(ROOTCFLAGS) -I$(IDIR) -I$(IEXTDIR)
CXXFLAGS = -O2 -g -fPIC -Wall -std=c++11 -I$(IDIR) -I$(IEXTDIR)

EXTLIBS = -L/cvmfs/geant4.cern.ch/externals/clhep/2.3.4.3/x86_64-slc6-gcc49-opt/lib -lCLHEP

# Get list of files to process
INCLUDES := $(wildcard $(IDIR)/*.hh)
SOURCES  := $(wildcard $(SDIR)/*.cc)
OBJECTS  := $(addprefix $(ODIR)/,$(notdir $(SOURCES:.cc=.o)))

# Define comlete library name
LIBRARY := $(LDIR)/lib$(LIBNAME).so

#########################################################################

all: $(LIBNAME)

$(LIBNAME) : $(OBJECTS)
	$(CXX) -shared -o $(LIBRARY) $(CXXFLAGS) $(OBJECTS) ${EXTLIBS}

$(ODIR)/%.o : $(SDIR)/%.cc 
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY:	all clean cleanall try

clean:
	@echo Cleaning object files...
	rm -f $(ODIR)/*.o

cleanall:
	@echo Cleaning all...
	rm -f $(ODIR)/*.o
	rm -f $(LIBRARY)

try:
	@echo Current make parameters:
	@echo LIBNAME = $(LIBNAME)
	@echo LIBRARY = $(LIBRARY)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo LIBS = $(LIBS)
	@echo SOURCES = $(SOURCES)
	@echo OBJECTS = $(OBJECTS)

#include $(DEPS)
