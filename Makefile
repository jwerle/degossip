
## system dependencies
OS ?= $(shell uname)
CC ?= gcc
CXX = g++
LD ?= ld
AR ?= ar
RM ?= rm -f
MKDIR ?= mkdir -p

PREFIX ?= /usr/local

## brief output
ifndef V
BRIEFC = CC CXX MAKE LD AR MKDIR
-include make/brief.mk
endif

## meta
CWD ?= $(shell pwd)
OUT ?= $(CWD)/out
MAIN = degossip/main.cc
BIN ?= degossip
SRC = $(filter-out $(MAIN), $(wildcard degossip/*.cc))
OBJS = $(SRC:.cc=.o)

## flags
CFLAGS += -std=c99
CXXFLAGS += -Iinclude -Idegossip -Izmq/include -Iv8/include
CXXFLAGS += -std=gnu++11

LDFLAGS += -L$(CWD)/zmq/src/.libs
LDFLAGS += -lzmq

ifdef D
	CXXFLAGS += -DDG_JS_PATH='"$(CWD)/degossip/dg.js"'
else
	CXXFLAGS += -DDG_JS_PATH='"$(PREFIX)/lib/degossip/dg.js"'
endif

ifeq ($(OS), Darwin)
	CXXFLAGS += -stdlib=libstdc++
else
	CXXFLAGS += -lrt
endif

## dependencies
ZMQ ?= $(shell ls zmq/src/.libs/*.a)
V8 ?= $(shell ls v8/out/native/{libv8_base,libv8_libbase,libv8_snapshot}.a)

## Target libs
TARGET_NAME = libdegossip
TARGET_STATIC = $(TARGET_NAME).a

.PHONY: $(BIN)
$(MAIN): build $(TARGET_STATIC)
	$(CXX) $(OUT)/$(TARGET_STATIC) $(V8) $(MAIN) $(CXXFLAGS) $(LDFLAGS) -o $(OUT)/$(BIN)

.PHONY: $(OUT)
build: $(OUT)
$(OUT):
	$(MKDIR) $(@)

.PHONY: $(OBJS)
$(OBJS):
	$(CXX) $(CXXFLAGS) -c $(@:.o=.cc) -o $(@)

$(TARGET_STATIC): $(OBJS)
	$(AR) crus $(OUT)/$(TARGET_STATIC) $(OBJS) $(DOBJS)

.PHONY: v8 zmq
dependencies: v8 zmq
v8:
	$(MAKE) i18nsupport=off native -C v8

zmq:
	@{ cd zmq && ./autogen.sh && ./configure; }
	$(MAKE) -C $(@)

clean:
	$(RM) $(OBJS)
	$(RM) -r $(OUT)
	$(RM) $(TARGET_STATIC)

