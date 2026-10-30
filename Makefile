CC = gcc
CFLAGS = -O2 -g -Wall -Wextra -Wno-error=return-type -Ishared -Ishared/mechanics -Ishared/network -Ishared/libs -Iclient
LDFLAGS = -lm -lpthread -lSDL2 -lGL -lopenal -lfreetype -lphysfs

BUILDDIR = build-c
SRCDIRS = shared shared/mechanics shared/network client

SOURCES = $(wildcard shared/*.c shared/mechanics/*.c shared/network/*.c client/*.c)
OBJECTS = $(patsubst %.c,$(BUILDDIR)/%.o,$(SOURCES))

all: $(BUILDDIR)/soldat

$(BUILDDIR)/soldat: $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/client/GameRendering.o: client/GameRendering.c
	@mkdir -p $(dir $@)
	$(CC) -x c $(CFLAGS) -c $< -o $@

$(BUILDDIR)/client/Gfx.o: client/Gfx.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
