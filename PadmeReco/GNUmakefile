# Makefile

NAME := PadmeReco

SUBDIRS := Target EVeto PVeto HEPVeto ECal SAC TPix
#ALLTARGETDIRS := $(SUBDIRS) RecoBase EventDisplay
ALLTARGETDIRS := $(SUBDIRS) RecoBase

STATIC := -static

ISSLC6 = $(shell cat /etc/redhat-release |grep -ic "Scientific Linux CERN SLC release 6")

CPPFLAGS += $(addprefix -I,$(addsuffix /include,$(SUBDIRS)))
#CPPFLAGS += -IEventDisplay/include -IRecoBase/include -g
CPPFLAGS += -IRecoBase/include -g

include ./config/global.gmk

# Define libraries used for the creation of the final executable
LIBS += -Llib -lPadmeReco$(STATIC) 
LIBS += $(addprefix -L,$(addsuffix /lib,$(SUBDIRS))) $(addprefix -l,$(addsuffix Reco$(STATIC),$(SUBDIRS)))
LIBS += -LRecoBase/lib -lRecoBase$(STATIC)
LIBS += -L$(PADMEROOT)/lib -lPadmeRoot
LIBS += -L/usr/lib64/mysql -lmysqlclient

#EXTRALIBS += -LEventDisplay/lib -lEventDisplay$(STATIC) -lEve -lGeom -lRGL -lFTGL -lGui -lGed -lTreePlayer -lEG 
EXTRALIBS += -lEve -lGeom -lRGL -lFTGL -lGui -lGed -lTreePlayer -lEG 

.PHONY: clean cleansub cleanbin env $(ALLTARGETDIRS)

all: PadmeReco PadmeEventDisplay

PadmeReco: PadmeReco.cc makesub
	@echo "Compiling $< ..."
	$(CXX) -o $@ $< $(CXXFLAGS) $(LIBS) $(LDFLAGS)
#	@$(CXX) -o $@ $< $(CXXFLAGS) $(LIBS) $(LDFLAGS)

PadmeEventDisplay: PadmeEventDisplay.cc makesub
	@echo "Compiling $< ..."
	$(CXX) -o $@ $< $(CXXFLAGS) $(LIBS) $(EXTRALIBS) $(LDFLAGS)
#	@$(CXX) -o $@ $< $(CXXFLAGS) $(LIBS) $(EXTRALIBS) $(LDFLAGS)

try:
	@echo CPPFLAGS
	@echo $(CPPFLAGS)
	@echo LDFLAGS
	@echo $(LDFLAGS)
	@echo LIBS
	@echo $(LIBS)
	@echo EXTRALIBS
	@echo $(EXTRALIBS)
	@echo CXXFLAGS
	@echo $(CXXFLAGS)
	@echo LD_LIBRARY_PATH
	@echo $(LD_LIBRARY_PATH)
	@echo ALLTARGETDIRS
	@echo $(ALLTARGETDIRS)

$(ALLTARGETDIRS):
	@echo Entering $@ ...
	@$(MAKE) -C $@ $(TARGET)

cleanbin:
	@rm -f PadmeReco PadmeEventDisplay

cleanall: clean cleanbin
	@$(MAKE) TARGET=clean makesub

makesub: GNUmakefile $(ALLTARGETDIRS)

cleansub:
	@$(MAKE) TARGET=clean makesub
