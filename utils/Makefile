ifeq ($(OS),Windows_NT)
CCFLAGS =
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_ARCH =
TARGET_SUFFIX = -win64
else
TARGET_ARCH = -march=i686
TARGET_SUFFIX = -win32
endif
else
CCFLAGS =
TARGET_ARCH = -march=armv7-a
TARGET_SUFFIX = -arm7a
endif
TARGET = ../libutils$(TARGET_SUFFIX).a

all: $(TARGET)

CCFLAGS += -O2 -Wall -I.. -std=c++11

OBJS = \
	cbase$(TARGET_SUFFIX).o \
	configset$(TARGET_SUFFIX).o \
	console$(TARGET_SUFFIX).o \
	cyclicbuffer$(TARGET_SUFFIX).o \
	databuffer$(TARGET_SUFFIX).o \
	encoding$(TARGET_SUFFIX).o \
	flaglock$(TARGET_SUFFIX).o \
	methodlist$(TARGET_SUFFIX).o \
	mpscqueue$(TARGET_SUFFIX).o \
	path$(TARGET_SUFFIX).o \
	profile$(TARGET_SUFFIX).o \
	ref$(TARGET_SUFFIX).o \
	refobject$(TARGET_SUFFIX).o \
	strexception$(TARGET_SUFFIX).o \
	string$(TARGET_SUFFIX).o

$(TARGET): $(OBJS)
		$(AR) -rcs $@ $^

%$(TARGET_SUFFIX).o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)
