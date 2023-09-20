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
SUBDIRS  := Beam ECal Target SAC ETag PVeto EVeto HEPVeto Magnet LeadGlass TDump TPix Tungsten Chamber Hall MagneticField BeamLine MCTruth
ALLTARGETDIRS := $(SUBDIRS)

LOCALDIR := ${PWD}

ifndef G4INSTALL
  G4INSTALL = ../..
endif

ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)

include $(G4INSTALL)/config/architecture.gmk

G4VERSION = $(shell dirname $(G4INSTALL) | sed s:\/.\*\/:"": )
G4MAJORVERSION = $(shell dirname $(G4INSTALL) | sed -e 's:/.*/::' -e 's:\-: :' -e 's:\.: :g' | cut -d" " -f2 )
G4MINORVERSION = $(shell dirname $(G4INSTALL) | sed -e 's:/.*/::' -e 's:\-: :' -e 's:\.: :g' | cut -d" " -f3 )

DEFINES += -D G4VERSION=$(G4VERSION) -D G4MAJORVERSION=$(G4MAJORVERSION) -D G4MINORVERSION=$(G4MINORVERSION) 

CPPFLAGS += -I$(PADMEROOT)/include $(ROOTCFLAGS) -fPIC $(DEFINES)
CPPFLAGS += $(addprefix -I,$(addsuffix /include,$(SUBDIRS)))

EXTRALIBS := $(addprefix -l,$(SUBDIRS))
EXTRALIBS += -L$(PADMEROOT)/lib -lPadmeRoot
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
	@echo G4VERSION
	@echo $(G4VERSION)

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

