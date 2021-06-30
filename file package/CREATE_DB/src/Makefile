CC ?= gcc
CXX ?= g++
OBJEXT ?= .o
BUILD_PATH = build

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

LINKEDLIBS = -ljson-c -lsqlite3

DEVICESRC += $(wildcard src/*.c)

CPPSRC = $(wildcard *.) $(DEVICESRC)
CPPOBJ = $(CPPSRC:.c=$(OBJEXT))
BUILTOBJ = $(addprefix $(BUILD_PATH)/,$(CPPOBJ))

APP = CREATE_DB

all: $(APP)

.PHONY: all $(APP) clean

$(APP): $(BUILTOBJ)
	$(CXX) $(LINKFLAGS) -o $@ $(BUILTOBJ) $(LINKEDLIBS)

$(BUILD_PATH)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(COMPFLAGS) $(INCLUDES) -c $< -o $@
	
install: $(APP)
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 644 $(APP) $(DESTDIR)$(PREFIX)/bin/

clean: 
	rm -rf $(APP) $(BUILD_PATH)

