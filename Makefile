ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
PLATFORM = win64
else
CCFLAGS = -march=i686
PLATFORM = win32
endif
OSLIBS-CONSOLE = \
	-lz
OSLIBS-MAIN = \
	-lgdi32 \
	-ld3d9 \
	-ld3dx9 \
	-lz
BINARY-EXT = .exe
TARGETNAME-CONSOLE = client-console-$(PLATFORM)$(BINARY-EXT)
TARGETNAME-MAIN = client-main-$(PLATFORM)$(BINARY-EXT)
else
CCFLAGS = -march=i686
PLATFORM = arm7a
OSLIBS-CONSOLE =
OSLIBS-MAIN = \
	-llog \
	-landroid \
	-lEGL \
	-lGLESv2 \
	-lz
CCFLAGS = -I$(CCTOOLSDIR)/sources/native_app_glue
CFLAGS = -I$(CCTOOLSDIR)/sources/native_app_glue
LDFLAGS-MAIN = \
	-Wl,-soname,$(TARGETNAME) \
	-shared \
	-Wl,--no-undefined \
	-Wl,-z,noexecstack
BINARY-EXT = .out
TARGETNAME-CONSOLE = client-console-$(PLATFORM)$(BINARY-EXT)
TARGETNAME-MAIN = libclient-main-$(PLATFORM).so
endif
TARGET-CONSOLE = output/$(TARGETNAME-CONSOLE)
TARGET-MAIN = output/$(TARGETNAME-MAIN)

BUILDDIR = build/$(PLATFORM)
CCFLAGS += -O2 -Wall -I. -std=c++11 -pipe -I$(BUILDDIR)
CFLAGS += -O2 -Wall -I..
LDFLAGS =  -pipe \
	-L. \
	-lluajit-$(PLATFORM) \
	-lpng-$(PLATFORM) \
	-lz
LDFLAGS-CONSOLE = $(LDFLAGS) $(OSLIBS-CONSOLE)
LDFLAGS-MAIN = $(LDFLAGS) $(OSLIBS-MAIN)
CCFLAGS-LUAC = $(CCFLAGS) -L. -lluajit-$(PLATFORM)
BINDFLAGS = \
	-g extraclasses "" \
	-g extraheaders "common.hpp cinttypes" \
	-g structname "methodlist" \
	-g packageprefix "host." \
	-g compactffi "true" \
	-g defaultlparent "base.ffipure"
BINDFLAGS-CONSOLE = \
	$(BINDFLAGS) \
	-g fileprefix "$(BUILDDIR)/client-console/methodlist"
BINDFLAGS-MAIN = \
	$(BINDFLAGS) \
	-g fileprefix "$(BUILDDIR)/client-main/methodlist"
EMBEDFLAGS = \
	-g structname "luainit"
EMBEDFLAGS-CONSOLE = \
	$(EMBEDFLAGS) \
	-g fileprefix "$(BUILDDIR)/client-console/luainit"
EMBEDFLAGS-MAIN = \
	$(EMBEDFLAGS) \
	-g fileprefix "$(BUILDDIR)/client-main/luainit"
LUAC = $(BUILDDIR)/luac$(BINARY-EXT)
LUACFLAGS =

OBJS = \
	$(BUILDDIR)/utils/cbase.o \
	$(BUILDDIR)/utils/configset.o \
	$(BUILDDIR)/utils/console.o \
	$(BUILDDIR)/utils/cyclicbuffer.o \
	$(BUILDDIR)/utils/databuffer.o \
	$(BUILDDIR)/utils/encoding.o \
	$(BUILDDIR)/utils/flaglock.o \
	$(BUILDDIR)/utils/mpscqueue.o \
	$(BUILDDIR)/utils/path.o \
	$(BUILDDIR)/utils/profile.o \
	$(BUILDDIR)/utils/ref.o \
	$(BUILDDIR)/utils/refobject.o \
	$(BUILDDIR)/utils/strexception.o \
	$(BUILDDIR)/utils/string.o \
	$(BUILDDIR)/rsbin/common.o \
	$(BUILDDIR)/rsbin/fileio.o \
	$(BUILDDIR)/rsbin/fsthread.o \
	$(BUILDDIR)/rsbin/pngreader.o \
	$(BUILDDIR)/rsbin/pngwriter.o
OBJS-CONSOLE = \
	$(OBJS) \
	$(BUILDDIR)/client-console/luainit.o \
	$(BUILDDIR)/client-console/main.o \
	$(BUILDDIR)/client-console/methodlist.o \
	$(BUILDDIR)/client-console/window.o
OBJS-MAIN = \
	$(OBJS) \
	$(BUILDDIR)/graphics/common.o \
	$(BUILDDIR)/graphics/context.o \
	$(BUILDDIR)/graphics/display.o \
	$(BUILDDIR)/graphics/resource.o \
	$(BUILDDIR)/graphics/shape.o \
	$(BUILDDIR)/client-main/luainit.o \
	$(BUILDDIR)/client-main/main.o \
	$(BUILDDIR)/client-main/methodlist.o \
	$(BUILDDIR)/client-main/window.o

	# $(BUILDDIR)/graphics/clearshape.o \
	# $(BUILDDIR)/graphics/indexbuffer.o \
	# $(BUILDDIR)/graphics/primitiveshape.o \
	# $(BUILDDIR)/graphics/program.o \
	# $(BUILDDIR)/graphics/rendertarget.o \
	# $(BUILDDIR)/graphics/shader.o \
	# $(BUILDDIR)/graphics/shapegroup.o \
	# $(BUILDDIR)/graphics/staticindexbuffer.o \
	# $(BUILDDIR)/graphics/statictexture.o \
	# $(BUILDDIR)/graphics/staticvertexbuffer.o \
	# $(BUILDDIR)/graphics/texture.o \
	# $(BUILDDIR)/graphics/vertexbuffer.o \

ifeq ($(OS),Windows_NT)
else
OBJS-MAIN += \
	$(BUILDDIR)/client-main/android_native_app_glue.o
endif
LUABINS-CONSOLE = \
	$(BUILDDIR)/luainit/main.lb \
	$(BUILDDIR)/luainit/baselib.lb \
	$(BUILDDIR)/luainit/object.lb \
	$(BUILDDIR)/luainit/ffipure.lb \
	$(BUILDDIR)/client-console/methodlist.lb \
	$(BUILDDIR)/luainit/hostlib.lb
LUABINS-MAIN = \
	$(BUILDDIR)/luainit/main.lb \
	$(BUILDDIR)/luainit/baselib.lb \
	$(BUILDDIR)/luainit/object.lb \
	$(BUILDDIR)/luainit/ffipure.lb \
	$(BUILDDIR)/client-main/methodlist.lb \
	$(BUILDDIR)/luainit/hostlib.lb
BINDHEADERS = \
	utils/cbase.hpp \
	utils/configset.hpp \
	utils/databuffer.hpp \
	utils/encoding.hpp \
	utils/refobject.hpp \
	rsbin/common.hpp \
	rsbin/fsthread.hpp \
	rsbin/fileio.hpp \
	rsbin/pngreader.hpp \
	rsbin/pngwriter.hpp
BINDHEADERS-CONSOLE = \
	$(BINDHEADERS) \
	client-console/window.hpp
BINDHEADERS-MAIN= \
	$(BINDHEADERS) \
	graphics/resource.hpp \
	graphics/shape.hpp \
	client-main/event.hpp \
	client-main/window.hpp

	# graphics/clearshape.hpp \
	# graphics/indexbuffer.hpp \
	# graphics/primitiveshape.hpp \
	# graphics/program.hpp \
	# graphics/rendertarget.hpp \
	# graphics/shader.hpp \
	# graphics/shapegroup.hpp \
	# graphics/staticindexbuffer.hpp \
	# graphics/statictexture.hpp \
	# graphics/staticvertexbuffer.hpp \
	# graphics/texture.hpp \
	# graphics/vertexbuffer.hpp \

ifeq ($(OS),Windows_NT)
# RUNCMD = pushd output && $(TARGETNAME) && popd
RUNCMD = echo "Not supported"
else
RUNCMD = run_ca $(TARGET)
endif

all: $(TARGET-CONSOLE) $(TARGET-MAIN)

$(BUILDDIR):
		"md" $(BUILDDIR)
		"md" $(BUILDDIR)/utils
		"md" $(BUILDDIR)/rsbin
		"md" $(BUILDDIR)/graphics
		"md" $(BUILDDIR)/luainit
		"md" $(BUILDDIR)/client-console
		"md" $(BUILDDIR)/client-main

$(LUAC): "offline utility/luac.cpp" $(BUILDDIR)
		$(CXX) -o $@ "offline utility/luac.cpp" $(CCFLAGS-LUAC)

$(BUILDDIR)/client-console/methodlist.cpp: $(BINDHEADERS-CONSOLE) $(BUILDDIR)
		lua $(BINDFLAGS-CONSOLE) bind.lua $(BINDHEADERS-CONSOLE)

$(BUILDDIR)/client-console/methodlist.lua: $(BUILDDIR)/client-console/methodlist.cpp

$(BUILDDIR)/luainit/%.lb: luainit/%.lua $(BUILDDIR)
		$(LUAC) $@ $< $(LUACFLAGS)

$(BUILDDIR)/client-console/methodlist.lb: $(BUILDDIR)/client-console/methodlist.lua $(BUILDDIR)
		$(LUAC) $@ $< $(LUACFLAGS)

$(BUILDDIR)/client-console/luainit.cpp: $(LUABINS-CONSOLE) $(BUILDDIR)
		lua $(EMBEDFLAGS-CONSOLE) embed.lua $(LUABINS-CONSOLE)

$(BUILDDIR)/client-console/luainit.o: $(BUILDDIR)/client-console/luainit.cpp $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-console/methodlist.o: $(BUILDDIR)/client-console/methodlist.cpp $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-console/window.o: client-console/window.cpp client-console/window.hpp $(BUILDDIR)/client-console/methodlist.o $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-console/main.o: client-console/main.cpp $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-main/methodlist.cpp: $(BINDHEADERS-MAIN) $(BUILDDIR)
		lua $(BINDFLAGS-MAIN) bind.lua $(BINDHEADERS-MAIN)

$(BUILDDIR)/client-main/methodlist.lua: $(BUILDDIR)/client-main/methodlist.cpp

$(BUILDDIR)/client-main/methodlist.lb: $(BUILDDIR)/client-main/methodlist.lua $(BUILDDIR)
		$(LUAC) $@ $< $(LUACFLAGS)

$(BUILDDIR)/client-main/luainit.cpp: $(LUABINS-MAIN) $(BUILDDIR)
		lua $(EMBEDFLAGS-MAIN) embed.lua $(LUABINS-MAIN)

$(BUILDDIR)/client-main/luainit.o: $(BUILDDIR)/client-main/luainit.cpp $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-main/methodlist.o: $(BUILDDIR)/client-main/methodlist.cpp $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-main/android_native_app_glue.o:
	 	$(CC) -c -o $@ $(CCTOOLSDIR)/sources/native_app_glue/android_native_app_glue.c $(CFLAGS)

$(BUILDDIR)/client-main/window.o: client-main/window.cpp client-main/window.hpp $(BUILDDIR)/client-main/methodlist.o $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/client-main/main.o: client-main/main.cpp $(BUILDDIR) $(BUILDDIR)/client-main/methodlist.o
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(BUILDDIR)/%.o: %.cpp %.hpp $(BUILDDIR)
	 	$(CXX) -c -o $@ $< $(CCFLAGS)

$(TARGET-CONSOLE): $(OBJS-CONSOLE)
		$(CXX) -o $@ $(OBJS-CONSOLE) $(LDFLAGS-CONSOLE)

$(TARGET-MAIN): $(OBJS-MAIN)
		$(CXX) -o $@ $(OBJS-MAIN) $(LDFLAGS-MAIN)

clean:
		rd /s /q build
		rm $(TARGET-CONSOLE) $(TARGET-MAIN)

run:
		$(RUNCMD)
