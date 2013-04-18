src = $(wildcard src/*.cc)
obj = $(src:.cc=.o)
dep = $(obj:.o=.d)
lib_so = winnie_tunnel.so

dbg = -g
opt = -O3 -ffast-math -march=native
inc = -Isrc -I../libwinnie/src

include ../backend.mk

CXX = g++
CXXFLAGS = -pedantic -Wall -Wno-trigraphs -fPIC $(dbg) $(opt) $(inc) $(def) `freetype-config --cflags`
LDFLAGS = -L../libwinnie/ $(libs) `freetype-config --libs` -lrt -lwinnie -ldl

$(lib_so): $(obj)
	$(CXX) -o $@ -shared $(obj) $(LDFLAGS) -Wl,-rpath=../libwinnie

-include $(dep)

%.d: %.cc
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(lib_so) $(dep)

