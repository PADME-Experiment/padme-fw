# Directories
SRC_DIR = X17src
INCLUDE_DIR = X17include
BUILD_DIR = build
BIN_DIR = bin

# Define root environment
ROOTCONFIG   := root-config
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)

# Define c++ compiler and flags
CXX = g++
CXXFLAGS = -O2 -g -fPIC -Wall $(ROOTCFLAGS) -I$(INCLUDE_DIR)

# Linker flags
LDFLAGS = $(ROOTLDFLAGS) $(ROOTLIBS) $(ROOTGLIBS) -I$(INCLUDE_DIR) -lRooFit -lRooFitCore -lHtml -lMinuit -fpermissive

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cxx)
OBJS = $(patsubst $(SRC_DIR)/%.cxx,$(BUILD_DIR)/%.o,$(SRCS))

# Add ChannelModel.o to OBJS
OBJS += $(BUILD_DIR)/ChannelModel.o

# Executable name
TARGET = roofit_full

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS) $(BUILD_DIR)/$(TARGET).o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/$(TARGET).o: /home/mancinima/padme-fw/X17Padme/roofit_full.cxx
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cxx
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(BUILD_DIR)/* $(BIN_DIR)/*
