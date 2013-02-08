src = $(wildcard src/*.cc)
obj = $(src:.cc=.o)
dep = $(obj:.o=.d)
bin = winnie


dbg = -g
opt = -O0
#inc =

CXX = g++
CXXFLAGS = -pedantic -Wall $(dbg) $(opt) $(inc)
#LDFLAGS = 

$(bin): $(obj)
	$(CXX) -o $@ $(obj) $(LDFLAGS)

-include $(dep)

%.d: %.cc
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) $(bin) $(dep)
