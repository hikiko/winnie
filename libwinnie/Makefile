PREFIX=/usr/local
src = $(wildcard src/*.cc) $(wildcard src/fbdev/*.cc) $(wildcard src/sdl/*.cc)
obj = $(src:.cc=.o)
#dep = $(obj:.o=.d)
lib_so = libwinnie.so

dbg = -g
opt = -O0
inc = -Isrc

#backend = SDL

ifeq ($(backend), SDL)
	def = -DWINNIE_SDL
	libs = -lSDL
else
	def = -DWINNIE_FBDEV
endif

CXX = g++
CXXFLAGS = -pedantic -Wall $(dbg) $(opt) $(inc) $(def) `freetype-config --cflags`
LDFLAGS = $(libs) `freetype-config --libs` -lrt

$(lib_so): $(obj)
	$(CXX) -o $@ -shared $(obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(lib_so)

.PHONY: install
install: $(lib_so)
	mkdir -p $(PREFIX)/lib
	mkdir -p $(PREFIX)/bin
	cp $(lib_so) $(PREFIX)/lib/$(lib_so)
	ldconfig

.PHONY: uninstall
uninstall:
	rm -f $(PREFIX)/lib/$(lib_so)
