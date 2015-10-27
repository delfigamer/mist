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
TARGET = ../libgraphics-gles$(TARGET_SUFFIX).a

all: $(TARGET)

CCFLAGS = -O2 -Wall -I.. $(NAG_FLAG) -std=c++11 -ffast-math

OBJS = \
	display$(TARGET_SUFFIX).o
	
	# clearshape$(TARGET_SUFFIX).o \
	# coloredshape$(TARGET_SUFFIX).o \
	# common$(TARGET_SUFFIX).o \
	# lineshape$(TARGET_SUFFIX).o \
	# methodlist$(TARGET_SUFFIX).o \
	# pixelprogram$(TARGET_SUFFIX).o \
	# rendertarget$(TARGET_SUFFIX).o \
	# resource$(TARGET_SUFFIX).o \
	# shape$(TARGET_SUFFIX).o \
	# shapegroup$(TARGET_SUFFIX).o \
	# sprite$(TARGET_SUFFIX).o \
	# spritefield$(TARGET_SUFFIX).o \
	# statictexture$(TARGET_SUFFIX).o \
	# texture$(TARGET_SUFFIX).o

$(TARGET): $(OBJS)
		$(AR) -rcs $@ $^

%$(TARGET_SUFFIX).o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)
