ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
PLATFORM = win64
else
CCFLAGS = -march=i686
PLATFORM = win32
endif
OS_LIBS =
TARGETNAME = client-console-$(PLATFORM).exe
else
CCFLAGS = -march=i686
PLATFORM = arm7a
OS_LIBS =
TARGETNAME = client-console-$(PLATFORM).out
endif
TARGET = ../output/$(TARGETNAME)

all: $(TARGET)

CCFLAGS += -O2 -Wall -I.. -std=c++11
LDFLAGS = \
	-L.. \
	-lluajit-$(PLATFORM) \
	-lpng-$(PLATFORM) \
	-lz \
	$(OS_LIBS)
BINDFLAGS = \
	-g extraclasses "" \
	-g extraheaders "cinttypes" \
	-g structname "methodlist" \
	-g packageprefix "host." \
	-g compactffi "true" \
	-g defaultlparent "base.ffipure"
LUACFLAGS =

OBJS = \
	build/$(PLATFORM)/client-console/luainit.o \
	build/$(PLATFORM)/client-console/main.o \
	build/$(PLATFORM)/client-console/methodlist.o \
	build/$(PLATFORM)/client-console/window.o \
	build/$(PLATFORM)/utils/cbase.o \
	build/$(PLATFORM)/utils/configset.o \
	build/$(PLATFORM)/utils/console.o \
	build/$(PLATFORM)/utils/cyclicbuffer.o \
	build/$(PLATFORM)/utils/databuffer.o \
	build/$(PLATFORM)/utils/encoding.o \
	build/$(PLATFORM)/utils/flaglock.o \
	build/$(PLATFORM)/utils/mpscqueue.o \
	build/$(PLATFORM)/utils/path.o \
	build/$(PLATFORM)/utils/profile.o \
	build/$(PLATFORM)/utils/ref.o \
	build/$(PLATFORM)/utils/refobject.o \
	build/$(PLATFORM)/utils/strexception.o \
	build/$(PLATFORM)/utils/string.o \
	build/$(PLATFORM)/rsbin/common.o \
	build/$(PLATFORM)/rsbin/fileio.o \
	build/$(PLATFORM)/rsbin/fsthread.o \
	build/$(PLATFORM)/rsbin/pngreader.o \
	build/$(PLATFORM)/rsbin/pngwriter.o
LUABINS = \
	build/main.lb \
	build/baselib.lb \
	build/object.lb \
	build/ffipure.lb \
	build/methodlist.lb \
	build/hostlib.lb \
	build/defer.lb \
	build/input.lb
BIND_HEADERS = \
	../utils/cbase.hpp \
	../utils/configset.hpp \
	../utils/databuffer.hpp \
	../utils/encoding.hpp \
	../utils/refobject.hpp \
	../rsbin/common.hpp \
	../rsbin/fsthread.hpp \
	../rsbin/fileio.hpp \
	../rsbin/pngreader.hpp \
	../rsbin/pngwriter.hpp \
	window.hpp

ifeq ($(OS),Windows_NT)
# RUNCMD = pushd ../output && $(TARGETNAME) && popd
RUNCMD = echo "Not supported"
else
RUNCMD = run_ca $(TARGET)
endif

$(TARGET): $(OBJS)
		$(CXX) -o $@ $(OBJS) $(LDFLAGS)

build/$(PLATFORM)/%.o: %.cpp %.hpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

luainit$(TARGET_SUFFIX).o: luainit.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

methodlist.cpp: $(BIND_HEADERS)
		lua $(BINDFLAGS) ../bind.lua $^

%.lb: ../luainit/%.lua
		lua $(LUACFLAGS) ../luac.lua $@ $<

methodlist.lb: methodlist.cpp
		lua $(LUACFLAGS) ../luac.lua $@ methodlist.lua

luainit.cpp: $(LUABINS)
		lua -g structname "luainit" ../embed.lua $(LUABINS)

../lib%$(TARGET_SUFFIX).a:
	 	$(MAKE) -C ../$*

clean:
		rm $(OBJS) methodlist.cpp methodlist.hpp \
			methodlist.lua luainit.cpp luainit.hpp $(LUABINS) \
			$(TARGET)

clean_rsbin: clean
		$(MAKE) -C ../rsbin clean

clean_utils: clean_rsbin clean
		$(MAKE) -C ../utils clean

clean_full: clean_utils

run:
		$(RUNCMD)
