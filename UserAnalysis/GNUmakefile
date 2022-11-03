# Makefile

NAME := PadmeAnalysis

SUBDIRS :=  UserAnalysis Analyzers
ALLTARGETDIRS := $(SUBDIRS)

STATIC := -static

ISSLC6 = $(shell cat /etc/redhat-release |grep -ic "Scientific Linux CERN SLC release 6")

CPPFLAGS += $(addprefix -I,$(addsuffix /include,$(SUBDIRS))) -pg

include ./config/global.gmk

# Define libraries used for the creation of the final executable
LIBS += $(addprefix -L,$(addsuffix /lib,$(SUBDIRS))) $(addprefix -l,$(addsuffix $(STATIC),$(SUBDIRS)))
LIBS += -Llib -lPadmeAnalysis$(STATIC)
LIBS += -L$(PADMEROOT)/lib -lPadmeRoot

.PHONY: clean cleansub cleanbin env $(ALLTARGETDIRS)

all: PadmeAnalysis

PadmeAnalysis: PadmeAnalysis.cc makesub
	@echo "Compiling $< ..."
	$(CXX) -o $@ $< $(CXXFLAGS) $(LIBS) $(LDFLAGS)

try:
	@echo CPPFLAGS
	@echo $(CPPFLAGS)
	@echo LDFLAGS
	@echo $(LDFLAGS)
	@echo LIBS
	@echo $(LIBS)
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
	@rm -f PadmeAnalysis

#cleanall: clean cleanbin
#	@$(MAKE) TARGET=clean makesub

cleanall: cleansub clean cleanbin

makesub: GNUmakefile $(ALLTARGETDIRS)

cleansub:
	@$(MAKE) TARGET=clean makesub
