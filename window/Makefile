ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_ARCH =
TARGET_SUFFIX = -win64
else
TARGET_ARCH = -march=i686
TARGET_SUFFIX = -win32
endif
NAG_FLAG =
else
TARGET_ARCH = -march=armv7-a
TARGET_SUFFIX = -arm7a
NAG_FLAG = -I$(CCTOOLSDIR)/sources/native_app_glue
endif
TARGET = ../libwindow$(TARGET_SUFFIX).a

all: $(TARGET)

CCFLAGS = -O2 -Wall -I.. $(NAG_FLAG) -std=c++11

OBJS = \
	asyncluastate$(TARGET_SUFFIX).o \
	window$(TARGET_SUFFIX).o \
	methodlist$(TARGET_SUFFIX).o \
	hostmethodlist$(TARGET_SUFFIX).o

$(TARGET): $(OBJS)
		$(AR) -rcs $@ $^

%$(TARGET_SUFFIX).o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)
