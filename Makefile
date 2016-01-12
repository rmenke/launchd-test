CXX = xcrun clang++
CXXFLAGS = -std=c++11 -MMD

all : daemon

install : $(HOME)/bin/daemon

$(HOME)/bin/daemon : daemon
	@install -b -C -s -v $^ $@

clean :
	$(RM) daemon daemon.o daemon.d

daemon : daemon.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include *.d
