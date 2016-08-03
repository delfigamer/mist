ifeq ($(OS),Windows_NT)
CCFLAGS =
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_ARCH =
TARGET_SUFFIX = -win64
else
TARGET_ARCH = -march=i686
TARGET_SUFFIX = -win32
endif
OSLIBS = \
	-lz
TARGETNAME = tempbin$(TARGET_SUFFIX).exe
LDFLAGS =
else
TARGET_ARCH = -march=armv7-a
TARGET_SUFFIX = -arm7a
OSLIBS = \
	-lz
TARGETNAME = tempbin$(TARGET_SUFFIX)
LDFLAGS =
endif
TARGET = ../output/$(TARGETNAME)

all: $(TARGET)

CCFLAGS += -O2 -Wall -I.. -std=c++11
LDFLAGS += \
	-L.. \
	-lluajit$(TARGET_SUFFIX) \
	$(OSLIBS)

OBJS = \
	main$(TARGET_SUFFIX).o

ifeq ($(OS),Windows_NT)
#~ RUNCMD = pushd ../output && $(TARGETNAME) && popd
RUNCMD = echo "Not supported"
else
RUNCMD = run $(TARGET)
endif

$(TARGET): $(OBJS) $(EXLIBS)
		$(CXX) -o $@ $(OBJS) $(LDFLAGS) $(TARGET_ARCH)

%$(TARGET_SUFFIX).o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)

run:
		$(RUNCMD)
