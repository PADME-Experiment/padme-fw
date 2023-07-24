DIRS:=$(filter-out build%,$(patsubst %/,%,$(dir $(wildcard */))))
CLEANDIRS:=$(addprefix clean-,$(DIRS))

CMAKE_FLAGS:=
ifdef BUILD_TYPE
    CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
endif
ifeq "$(SHARED)" "1"
    CMAKE_FLAGS += -DSHARED_LIB=1
endif


BUILDDIR=build

all: $(BUILDDIR)
	+@make -C $(BUILDDIR) install --no-print-directory $(MAKEFLAGS)

$(BUILDDIR): 
	@cmake -H. -B$(BUILDDIR) $(CMAKE_FLAGS) ${ARGS}

relink: $(BUILDDIR)
	@touch roofit_full.cc
	+@make -C $(BUILDDIR) install --no-print-directory $(MAKEFLAGS)

clean:
	rm -rf $(BUILDDIR)
	rm -rf lib
	rm -rf bin

.DEFAULT: $(BUILDDIR)
	+@make -C $(BUILDDIR) $@ --no-print-directory $(MAKEFLAGS)

$(DIRS): $(BUILDDIR)
	+@make -C $(BUILDDIR) $@ --no-print-directory $(MAKEFLAGS)

$(CLEANDIRS): $(BUILDDIR)
	rm -rf $(BUILDDIR)/$(patsubst clean-%,%,$@)

.PHONY: build_all clean $(DIRS) $(CLEANDIRS)   