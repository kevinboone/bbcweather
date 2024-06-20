NAME    := bbcweather
VERSION := 0.2a
CC      :=  gcc 
EXTRA_CFLAGS ?=
EXTRA_LDFLAGS ?=
EXTRA_LIBS ?=
LIBS    := -lm -lcurl ${EXTRA_LIBS} 
TARGET	:= $(NAME) 
SOURCES := $(shell find src/ -type f -name *.c)
OBJECTS := $(patsubst src/%,build/%,$(SOURCES:.c=.o))
DEPS	:= $(OBJECTS:.o=.deps)
DESTDIR := /
PREFIX  := /usr
SHAREDIR := $(PREFIX)/share
MYSHAREDIR := $(SHAREDIR)/$(NAME)
MANDIR  := $(SHAREDIR)/man
BINDIR  := $(PREFIX)/bin
CFLAGS  := -Wall -O3 -Wno-unused-result -DSHAREDIR=\"${MYSHAREDIR}\" -DNAME=\"$(NAME)\" -DVERSION=\"$(VERSION)\" -g -I include ${EXTRA_CFLAGS}
LDFLAGS := -s  ${EXTRA_LDFLAGS}

all: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) 

build/%.o: src/%.c
	@mkdir -p build/
	$(CC) $(CFLAGS) -MD -MF $(@:.o=.deps) -c -o $@ $<

clean:
	@echo "  Cleaning..."; $(RM) -r build/ $(TARGET) 

install: $(TARGET)
	mkdir -p $(DESTDIR)/$(BINDIR)
	mkdir -p $(DESTDIR)/$(SHAREDIR)
	mkdir -p $(DESTDIR)/$(MYSHAREDIR)
	install -D -m 755 $(TARGET) $(DESTDIR)/${BINDIR}
	mkdir -p $(DESTDIR)/$(MANDIR)/man1
	install -m 644 man1/* $(DESTDIR)/${MANDIR}/man1/
	install -D -m 644 share/* $(DESTDIR)/${MYSHAREDIR}/

-include $(DEPS)

.PHONY: clean

