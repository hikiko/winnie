src = $(wildcard src/*.cc)
obj = $(src:.cc=.o)
dep = $(obj:.o=.d)
lib_so = winnie_clock.so

dbg = -g
opt = -O0
inc = -Isrc -I../libwinnie/src

backend = SDL

ifeq ($(backend), SDL)
	def = -DWINNIE_SDL
	libs = -lSDL
else
	def = -DWINNIE_FBDEV
endif

CXX = g++
CXXFLAGS = -pedantic -Wall $(dbg) $(opt) $(inc) $(def) `freetype-config --cflags`
LDFLAGS = -L../libwinnie/ $(libs) `freetype-config --libs` -lrt -lwinnie -ldl

$(lib_so): $(obj)
	$(CXX) -o $@ -shared $(obj) $(LDFLAGS) -Wl,-rpath=../libwinnie

-include $(dep)

%.d: %.cc
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(lib_so) $(dep)

