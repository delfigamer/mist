ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_SUFFIX = -win64
else
CCFLAGS = -march=i686
TARGET_SUFFIX = -win32
endif
OS_LIBS =
TARGETNAME = client-console$(TARGET_SUFFIX).exe
else
CCFLAGS = -march=i686
TARGET_SUFFIX = -arm7a
OS_LIBS =
TARGETNAME = client-console$(TARGET_SUFFIX).out
endif
TARGET = ../output/$(TARGETNAME)

all: $(TARGET)

CCFLAGS += -O2 -Wall -I.. -std=c++11
LDFLAGS = \
	-L.. \
	-lrsbin$(TARGET_SUFFIX) \
	-lutils$(TARGET_SUFFIX) \
	-lluajit$(TARGET_SUFFIX) \
	-lpng$(TARGET_SUFFIX) \
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
	luainit$(TARGET_SUFFIX).o \
	main$(TARGET_SUFFIX).o \
	methodlist$(TARGET_SUFFIX).o \
	window$(TARGET_SUFFIX).o
LUABINS = \
	main.lb \
	baselib.lb \
	object.lb \
	ffipure.lb \
	methodlist.lb \
	hostlib.lb \
	defer.lb \
	input.lb
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
EXLIBS = \
	../librsbin$(TARGET_SUFFIX).a \
	../libutils$(TARGET_SUFFIX).a

ifeq ($(OS),Windows_NT)
#~ RUNCMD = pushd ../output && $(TARGETNAME) && popd
RUNCMD = echo "Not supported"
else
RUNCMD = run_ca $(TARGET)
endif

$(TARGET): $(OBJS) $(EXLIBS)
		$(CXX) -o $@ $(OBJS) $(LDFLAGS)

main$(TARGET_SUFFIX).o: main.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

%$(TARGET_SUFFIX).o: %.cpp %.hpp
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
		rm $(OBJS) methodlist$(TARGET_SUFFIX).o methodlist.cpp methodlist.hpp \
			methodlist.lua luainit.cpp luainit.hpp $(LUABINS) \
			$(TARGET)

clean_rsbin: clean
		$(MAKE) -C ../rsbin clean

clean_utils: clean_rsbin clean
		$(MAKE) -C ../utils clean

clean_full: clean_utils

run:
		$(RUNCMD)
