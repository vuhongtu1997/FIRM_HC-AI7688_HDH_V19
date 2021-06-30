CC ?= gcc
CXX ?= g++
OBJEXT ?= .o
BUILD_PATH = build

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

LINKEDLIBS = -lpthread -lsqlite3 -lmosquitto # Librarys to include in the link

DEVICESRC += $(wildcard src/*.cpp)
DEVICESRC += $(wildcard src/RD/AHG/*.cpp)
DEVICESRC += $(wildcard src/RD/AHG/JSON/*.cpp)
DEVICESRC += $(wildcard src/RD/GHA/*.cpp)
DEVICESRC += $(wildcard src/RD/GHA/JSON/*.cpp)
DEVICESRC += $(wildcard src/RD/MQTT/*.cpp)
DEVICESRC += $(wildcard src/RD/TIMER/*.cpp)

CPPSRC = $(wildcard *.cpp) $(DEVICESRC)
CPPOBJ = $(CPPSRC:.cpp=$(OBJEXT))
BUILTOBJ = $(addprefix $(BUILD_PATH)/,$(CPPOBJ))

APP = RD_SMART

all: $(APP)

.PHONY: all $(APP) clean

$(APP): $(BUILTOBJ)
	$(CXX) $(LINKFLAGS) -o $@ $(BUILTOBJ) $(LINKEDLIBS)
	
$(BUILD_PATH)/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(COMPFLAGS) $(INCLUDES) -c $< -o $@
	
install: $(APP)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 644 $(APP) $(DESTDIR)$(PREFIX)/bin/

clean: 
	rm -rf $(APP) $(BUILD_PATH)

