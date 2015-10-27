ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_ARCH =
TARGET_SUFFIX = -win64
else
TARGET_ARCH = -march=i686
TARGET_SUFFIX = -win32
endif
else
TARGET_ARCH = -march=armv7-a
TARGET_SUFFIX = -arm7a
endif
TARGET = ../librsbin$(TARGET_SUFFIX).a

all: $(TARGET)

CCFLAGS = -O2 -Wall -I.. -std=c++11

OBJS = \
	common$(TARGET_SUFFIX).o \
	fileio$(TARGET_SUFFIX).o \
	fsthread$(TARGET_SUFFIX).o \
	methodlist$(TARGET_SUFFIX).o \
	pngreader$(TARGET_SUFFIX).o \
	pngwriter$(TARGET_SUFFIX).o

$(TARGET): $(OBJS)
		$(AR) -rcs $@ $^

%$(TARGET_SUFFIX).o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)
