# Makefile

NAME := PadmeReco

#SUBDIRS := Target EVeto PVeto HEPVeto TPix ECal SAC
SUBDIRS := ECal SAC
#ALLTARGETDIRS := $(SUBDIRS) RecoBase EventDisplay Service
ALLTARGETDIRS := $(SUBDIRS) RecoBase

STATIC := -static

ISSLC6 = $(shell cat /etc/redhat-release |grep -ic "Scientific Linux CERN SLC release 6")

CPPFLAGS += $(addprefix -I,$(addsuffix /include,$(SUBDIRS)))
#CPPFLAGS += -IEventDisplay/include -IService/include
CPPFLAGS += -IRecoBase/include -g

include ./config/global.gmk

# Define libraries used for the creation of the final executable
LIBS += -Llib -lPadmeReco$(STATIC) 
LIBS += $(addprefix -L,$(addsuffix /lib,$(SUBDIRS))) $(addprefix -l,$(addsuffix Reco$(STATIC),$(SUBDIRS)))
#LIBS += -LService/lib -lService$(STATIC)
LIBS += -LRecoBase/lib -lRecoBase$(STATIC)
LIBS += -L$(PADMEROOT)/lib -lPadmeRoot
LIBS += -L/usr/lib64/mysql -lmysqlclient

#EXTRALIBS += -LEventDisplay/lib -lEventDisplay$(STATIC) -lEve -lGeom -lRGL -lFTGL -lGui -lGed -lTreePlayer -lEG 

#LIBRARY_PATH = $(addprefix $(NA62MCSOURCE)/,$(addsuffix /Persistency/lib:,$(SUBDIRS))) \
#		$(addsuffix /lib:,$(SUBDIRS))
#LD_LIBRARY_PATH =$(shell echo $(ROOTSYS)/lib:lib:$(NA62MCSOURCE)/Persistency/lib:$(LIBRARY_PATH) | awk '{print gensub(" ","","g")}')

.PHONY: clean cleansub cleanbin env $(ALLTARGETDIRS)

#all: PadmeReco PadmeEventDisplay
all: PadmeReco

PadmeReco: PadmeReco.cc makesub
	@echo "Compiling $< ..."
	$(CXX) -o $@ $< $(CXXFLAGS) $(LIBS) $(LDFLAGS)

#PadmeEventDisplay: PadmeEventDisplay.cc makesub
#	@echo "Compiling $< ..."
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
	@rm -f PadmeReco
#	@rm -f PadmeReco PadmeEventDisplay

cleanall: clean cleanbin
	@$(MAKE) TARGET=clean makesub

makesub: GNUmakefile $(ALLTARGETDIRS)

cleansub:
	@$(MAKE) TARGET=clean makesub
