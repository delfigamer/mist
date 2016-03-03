ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
CCFLAGS =
TARGET_SUFFIX = -win64
else
CCFLAGS = -march=i686
TARGET_SUFFIX = -win32
endif
else
CCFLAGS = -march=armv7-a
TARGET_SUFFIX = -arm7a
endif
TARGET = ../libutils$(TARGET_SUFFIX).a

all: $(TARGET)

CCFLAGS += -O2 -Wall -Werror -I.. -std=c++11

OBJS = \
	cbase$(TARGET_SUFFIX).o \
	configset$(TARGET_SUFFIX).o \
	console$(TARGET_SUFFIX).o \
	cyclicbuffer$(TARGET_SUFFIX).o \
	databuffer$(TARGET_SUFFIX).o \
	encoding$(TARGET_SUFFIX).o \
	flaglock$(TARGET_SUFFIX).o \
	mpscqueue$(TARGET_SUFFIX).o \
	path$(TARGET_SUFFIX).o \
	profile$(TARGET_SUFFIX).o \
	ref$(TARGET_SUFFIX).o \
	refobject$(TARGET_SUFFIX).o \
	strexception$(TARGET_SUFFIX).o \
	string$(TARGET_SUFFIX).o

$(TARGET): $(OBJS)
		$(AR) -rcs $@ $^

%$(TARGET_SUFFIX).o: %.cpp %.hpp
		$(CXX) -c -o $@ $< $(CCFLAGS)

clean:
		rm $(OBJS) $(TARGET)
