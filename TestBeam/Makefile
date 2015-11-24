########################################################################
#
# Compile TestBeam code
#
# Author Emanuele Leonardi <emanuele.leonardi@roma1.infn.it>
#
#########################################################################

# Define directories for different file types
SDIR = src
ODIR = obj
DDIR = deps
IDIR = include

# Define root environment
ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTINCDIR   := $(shell $(ROOTCONFIG) --incdir)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)
ROOTEVELIBS  := $(shell $(ROOTCONFIG) --evelibs)

# Directory where the PadmeRoot package is installed
PADMEROOTDIR := $(PADME)/PadmeRoot

# Define c++ compiler and flags
CXX = g++
CXXFLAGS = -O2 -g -Wall $(ROOTCFLAGS) -I$(IDIR) -I$(PADMEROOTDIR)/include
CPPFLAGS = $(ROOTCFLAGS) -I$(IDIR) -I$(PADMEROOTDIR)/include -I/usr/include
CINTFLAGS = -I$(IDIR) -I$(PADMEROOTDIR)/include

# Define command to create dependencies files
DEPEND = $(CXX) -MM

# Define parameters for link command
LIBS	= -L$(PADMEROOTDIR)/lib -lPadmeRoot
LDFLAGS = -O4 $(ROOTLDFLAGS) $(ROOTEVELIBS) $(LIBS)

# Get list of files to process

SOURCES  := $(wildcard $(SDIR)/*.cc)
INCLUDES := $(wildcard $(IDIR)/*.hh)
DEPS     := $(addprefix $(DDIR)/,$(notdir $(SOURCES:.cc=.ccd)))
OBJECTS  := $(addprefix $(ODIR)/,$(notdir $(SOURCES:.cc=.o)))

LINKDEFS := $(wildcard *LinkDef.hh)
SRCDICTS := $(LINKDEFS:LinkDef.hh=Dict.cc)
INCDICTS := $(SRCDICTS:.cc=.h)
OBJDICTS := $(SRCDICTS:.cc=.o)

# Get list of main programs to compile
EXESRC            := $(wildcard *.cpp)
EXECUTABLES       := $(EXESRC:.cpp=.exe)
DEPEXE		  := $(EXESRC:.cpp=.d)

#########################################################################

all : $(EXECUTABLES)

%.exe : %.cpp $(DEPS) $(OBJECTS) $(OBJDICTS) %.d   
	${CXX} $(CXXFLAGS) $< $(OBJECTS) $(OBJDICTS) $(LDFLAGS) -o $@ 

$(ODIR)/%.o : $(SDIR)/%.cc $(DDIR)/%.ccd
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DDIR)/%.ccd : $(SDIR)/%.cc
	$(DEPEND) $(CPPFLAGS) -MT $(addprefix $(ODIR)/,$(notdir $(<:.cc=.o))) $< > $@

%Dict.cc : $(INCLUDES) %LinkDef.hh
	@echo "Generating dictionary $@..."
	rootcint -f $@ -c $(CINTFLAGS) $^

%Dict.o : %Dict.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.d : 	%.cpp
	$(DEPEND) $(CPPFLAGS) -MT $(notdir $(<:.cpp=.exe)) $< > $@

.SECONDARY : $(SRCDICTS) $(OBJDICTS)

.PHONY :	all clean cleanall try

clean :
	@echo Cleaning object files...
	rm -f $(OBJECTS) $(OBJDICTS)

cleanall :
	@echo Cleaning all...
	rm -f $(SRCDICTS) $(INCDICTS) $(OBJDICTS)
	rm -f $(OBJECTS)
	rm -f *.exe

try :
	@echo Current make parameters:
	@echo EXECUTABLES = $(EXECUTABLES)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo CPPFLAGS = $(CPPFLAGS)
	@echo LIBS = $(LIBS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo SOURCES = $(SOURCES)
	@echo INCLUDES = $(INCLUDES)
	@echo DEPS = $(DEPS)
	@echo OBJECTS = $(OBJECTS)
	@echo LINKDEFS = $(LINKDEFS)
	@echo SRCDICTS = $(SRCDICTS)
	@echo INCDICTS = $(INCDICTS)
	@echo OBJDICTS = $(OBJDICTS)
	@echo EXESRC = $(EXESRC)
	@echo DEPEXE = $(DEPEXE)
	@echo ROOTCFLAGS = $(ROOTCFLAGS)
	@echo ROOTINCDIR = $(ROOTINCDIR)
	@echo ROOTLDFLAGS = $(ROOTLDFLAGS)
	@echo ROOTLIBS = $(ROOTLIBS)
	@echo ROOTGLIBS = $(ROOTGLIBS)
	@echo ROOTEVELIBS = $(ROOTEVELIBS)

include $(DEPS)
include $(DEPEXE)
