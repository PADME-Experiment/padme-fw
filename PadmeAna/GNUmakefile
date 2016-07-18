# Makefile

NAME := PadmeAna

#SUBDIRS := Target EVeto PVeto HEPVeto TPix ECal SAC
SUBDIRS := 
#ALLTARGETDIRS := $(SUBDIRS) RecoBase EventDisplay Service
ALLTARGETDIRS := $(SUBDIRS) # AnaBase

STATIC := -static

ISSLC6 = $(shell cat /etc/redhat-release |grep -ic "Scientific Linux CERN SLC release 6")

CPPFLAGS += $(addprefix -I,$(addsuffix /include,$(SUBDIRS)))
#CPPFLAGS += -IEventDisplay/include -IService/include
CPPFLAGS += -IAnaBase/include -g

include ./config/global.gmk

# Define libraries used for the creation of the final executable
LIBS += -Llib -lPadmeAna$(STATIC) 
LIBS += $(addprefix -L,$(addsuffix /lib,$(SUBDIRS))) $(addprefix -l,$(addsuffix Ana$(STATIC),$(SUBDIRS)))
#LIBS += -LService/lib -lService$(STATIC)
#LIBS += -LAnaBase/lib -lAnaBase$(STATIC)
LIBS += -L$(PADMEROOT)/lib -lPadmeRoot
LIBS += -L/usr/lib64/mysql -lmysqlclient



.PHONY: clean cleansub cleanbin env $(ALLTARGETDIRS)

all: PadmeAna

PadmeAna: PadmeAna.cc makesub
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
	@rm -f PadmeAna

cleanall: clean cleanbin
	@$(MAKE) TARGET=clean makesub

makesub: GNUmakefile $(ALLTARGETDIRS)

cleansub:
	@$(MAKE) TARGET=clean makesub
