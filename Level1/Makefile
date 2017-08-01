########################################################################
#
# Compile PadmeMerge code
#
# Author Emanuele Leonardi <emanuele.leonardi@roma1.infn.it>
#
#########################################################################

# Define directories for different file types
SDIR	= src
ODIR	= obj
DDIR	= deps
IDIR	= include

# Define root environment
ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTINCDIR   := $(shell $(ROOTCONFIG) --incdir)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)

# Directory where the PadmeRoot package is installed
PADMEROOTDIR := $(PADME)/PadmeRoot

# Define c++ compiler and flags
CXX = g++
CXXFLAGS = -O2 -g -Wall $(ROOTCFLAGS) -I$(IDIR) -I$(PADMEROOTDIR)/include
CPPFLAGS =  $(ROOTCFLAGS) -I$(IDIR) -I$(PADMEROOTDIR)/include -I/usr/include

# Define command to create dependencies files
DEPEND = $(CXX) -MM

# Define parameters for link command
LIBS	= -L$(PADMEROOTDIR)/lib -lPadmeRoot -L/usr/lib64/mysql -lmysqlclient
LDFLAGS = -O4 $(ROOTLDFLAGS) $(LIBS) $(ROOTLIBS)

# Get list of files to process
SOURCES  := $(wildcard $(SDIR)/*.cc)
INCLUDES := $(wildcard $(IDIR)/*.hh)
OBJECTS  := $(addprefix $(ODIR)/,$(notdir $(SOURCES:.cc=.o)))
DEPS     := $(addprefix $(DDIR)/,$(notdir $(SOURCES:.cc=.ccd)))

# Get list of main programs to compile
EXESRC            := $(wildcard *.cpp)
EXECUTABLES       := $(EXESRC:.cpp=.exe)
DEPEXE		  := $(EXESRC:.cpp=.d)

#########################################################################

all: $(EXECUTABLES)

%.exe : %.cpp $(DEPS) $(OBJECTS) %.d   
	${CXX} $(CXXFLAGS) $< $(OBJECTS) $(LDFLAGS) -o $@ 

$(ODIR)/%.o : $(SDIR)/%.cc $(DDIR)/%.ccd
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(DDIR)/%.ccd : $(SDIR)/%.cc
	$(DEPEND) $(CPPFLAGS) -MT $(addprefix $(ODIR)/,$(notdir $(<:.cc=.o))) $< > $@

%.d : 	%.cpp
	$(DEPEND) $(CPPFLAGS) -MT $(notdir $(<:.cpp=.exe)) $< > $@

.PHONY:	all clean cleanall try

clean:
	@echo Cleaning object files...
	rm -f $(ODIR)/*.o

cleanall:
	@echo Cleaning all...
	rm -f $(ODIR)/*.o
	rm -f *.exe

try:
	@echo Current make parameters:
	@echo EXECUTABLES = $(EXECUTABLES)
	@echo CXXFLAGS = $(CXXFLAGS)
	@echo CPPFLAGS = $(CPPFLAGS)
	@echo LIBS = $(LIBS)
	@echo LDFLAGS = $(LDFLAGS)
	@echo SOURCES = $(SOURCES)
	@echo INCLUDES = $(INCLUDES)
	@echo OBJECTS = $(OBJECTS)
	@echo DEPS = $(DEPS)
	@echo EXESRC = $(EXESRC)
	@echo DEPEXE = $(DEPEXE)
	@echo ROOTCFLAGS = $(ROOTCFLAGS)
	@echo ROOTINCDIR = $(ROOTINCDIR)
	@echo ROOTLDFLAGS = $(ROOTLDFLAGS)
	@echo ROOTLIBS = $(ROOTLIBS)
	@echo ROOTGLIBS = $(ROOTGLIBS)

include $(DEPS)
include $(DEPEXE)
