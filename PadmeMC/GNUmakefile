# -------------------------------------------------------------------
# GNUmakefile 
# -------------------------------------------------------------------
# -------------------------------------------------------------------
# History:
# 
# Created by Emanuele Leonardi (emanuele.leonardi@roma1.infn.it) 2015-12-11
# Based on NA62MC GNUMakefile
# -------------------------------------------------------------------

name := PADMEMC
G4TARGET := $(name)
G4EXLIB  := true
SUBDIRS  := ECal Target SAC LAV PVeto HEPVeto
ALLTARGETDIRS := $(SUBDIRS)

# Directory where the PadmeRoot package is installed
PADMEROOTDIR := $(PADME)/PadmeRoot

LOCALDIR := ${PWD}

ifndef G4INSTALL
  G4INSTALL = ../..
endif

ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)

include $(G4INSTALL)/config/architecture.gmk

CPPFLAGS += -I$(PADMEROOTDIR)/include $(ROOTCFLAGS) -fPIC $(DEFINES)
CPPFLAGS += $(addprefix -I,$(addsuffix /include,$(SUBDIRS)))

EXTRALIBS := $(addprefix -l,$(SUBDIRS))
EXTRALIBS += -L$(PADMEROOTDIR)/lib -lPadmeRoot
EXTRALIBS += $(ROOTLIBS)

.PHONY: all bin makesub clean cleansub cleanall cleanallsub $(SUBDIRS)
#all: turtle cmc persistency makesub lib bin
all: makesub lib exe

try:
	@echo CPPFLAGS
	@echo $(CPPFLAGS)
	@echo CXXFLAGS
	@echo $(CXXFLAGS)
	@echo EXTRALIBS
	@echo $(EXTRALIBS)
	@echo MDEFINES
	@echo $(MDEFINES)
	@echo DEFINES
	@echo $(DEFINES)

$(ALLTARGETDIRS):
	@echo Entering $@ ...
	@$(MAKE) -C $@ $(TARGET)

makesub: $(SUBDIRS)

exe: | makesub lib
	@$(MAKE) bin 

cleanall: clean
	@$(MAKE) TARGET=cleanall makesub

cleanallsub:
	@for dir in $(SUBDIRS); do ( \
		echo Entering $$dir ...; \
		cd $$dir; \
		$(MAKE) cleanall );\
	done

include $(G4INSTALL)/config/binmake.gmk

#Standard flags from G4
CXXFLAGS := -W -Wall -ansi -pedantic -Wno-non-virtual-dtor -Wno-long-long -Wwrite-strings -Wpointer-arith -Woverloaded-virtual -pipe -Wno-shadow -O2 -fPIC 
CXXFLAGS += -g
#CXXFLAGS += -pg -fprofile-generate

