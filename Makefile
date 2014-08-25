
CC ?= cc
LD ?= ld
AR ?= ar
RM ?= rm -f
MKDIR ?= mkdir -p

BRIEFC = CC MAKE LD AR MKDIR RM
-include make/brief.mk

CWD ?= $(shell pwd)
OUT ?= $(CWD)/out
MAIN = degossip/main.c
BIN ?= degossip
SRC = $(filter-out $(MAIN), $(wildcard degossip/*.c))
SRC += $(wildcard deps/*/*.c)
OBJS = $(SRC:.c=.o)

CFLAGS += -std=c99
CFLAGS += -Iinclude -Izmq/include -Iv8/include

.PHONY: $(BIN)
$(MAIN): $(OBJS) build
	$(CC) $(CFLAGS) $(OBJS) degossip/main.c -o $(OUT)/$(BIN)

.PHONY: $(OUT)
build: $(OUT)
$(OUT):
	$(MKDIR) $(@)

$(OBJS):
	$(CC) $(CFLAGS) -c $(@:.o=.c) -o $(@)

.PHONY: v8 zmq
dependencies: v8 zmq
v8:
	$(MAKE) i18nsupport=off native -C v8

zmq:
	@{ cd zmq && ./autogen.sh && ./configure; }
	$(MAKE) -C $(@)

clean: BRIEF_ARGS=*.o
clean:
	$(RM) $(OBJS)

