ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_ARCH =
TARGET_SUFFIX = -win64
else
TARGET_ARCH = -march=i686
TARGET_SUFFIX = -win32
endif
OSLIBS = \
	-lgdi32 \
	-ld3d9 \
	-ld3dx9 \
	-lz
GRAPHICSLIB = graphics-d3d
NAG_FLAG =
TARGETNAME = client$(TARGET_SUFFIX).exe
LDFLAGS =
else
TARGET_ARCH = -march=armv7-a
TARGET_SUFFIX = -arm7a
OSLIBS = \
	-llog \
	-landroid \
	-lEGL \
	-lGLESv2 \
	-lz
GRAPHICSLIB = graphics-gles
NAG_FLAG = -I$(CCTOOLSDIR)/sources/native_app_glue
TARGETNAME = libclient$(TARGET_SUFFIX).so
LDFLAGS = \
	-Wl,-soname,$(TARGETNAME) \
	-shared \
	-Wl,--no-undefined \
	-Wl,-z,noexecstack
endif
TARGET = ../output/$(TARGETNAME)

all: $(TARGET)

CCFLAGS = -O2 -Wall -I.. $(NAG_FLAG) -std=c++11
CFLAGS = -O2 -Wall -I.. $(NAG_FLAG)
LDFLAGS += \
	-L.. \
	-L../output \
	-lwindow$(TARGET_SUFFIX) \
	-l$(GRAPHICSLIB)$(TARGET_SUFFIX) \
	-lrsbin$(TARGET_SUFFIX) \
	-lutils$(TARGET_SUFFIX) \
	-lluajit$(TARGET_SUFFIX) \
	-lpng$(TARGET_SUFFIX) \
	$(OSLIBS)

ifeq ($(OS),Windows_NT)
OBJS = \
	main$(TARGET_SUFFIX).o
else
OBJS = \
	main$(TARGET_SUFFIX).o \
	android_native_app_glue.o
endif
EXLIBS = \
	../libwindow$(TARGET_SUFFIX).a \
	../lib$(GRAPHICSLIB)$(TARGET_SUFFIX).a \
	../librsbin$(TARGET_SUFFIX).a \
	../libutils$(TARGET_SUFFIX).a

ifeq ($(OS),Windows_NT)
#~ RUNCMD = pushd ../output && $(TARGETNAME) && popd
RUNCMD = echo "Not supported"
else
RUNCMD = run_na $(TARGET)
endif

$(TARGET): $(OBJS) $(EXLIBS)
		$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(TARGET_ARCH)

%$(TARGET_SUFFIX).o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

../lib%$(TARGET_SUFFIX).a:
	 	$(MAKE) -C ../$*

android_native_app_glue.o:
	 	$(CC) -c -o $@ $(CCTOOLSDIR)/sources/native_app_glue/android_native_app_glue.c $(CFLAGS) $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)

clean_window: clean
		$(MAKE) -C ../window clean

clean_graphics: clean_window
		$(MAKE) -C ../$(GRAPHICSLIB) clean

clean_rsbin: clean
		$(MAKE) -C ../rsbin clean

clean_utils: clean_rsbin clean_graphics
		$(MAKE) -C ../utils clean

clean_full: clean_utils

run:
		$(RUNCMD)
