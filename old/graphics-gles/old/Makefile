ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_ARCH =
LIB_SUFFIX = -win64
else
TARGET_ARCH = -march=i686
LIB_SUFFIX = -win32
endif
else
TARGET_ARCH = -march=armv7-a
LIB_SUFFIX = -arm7a
endif
TARGET = ../libgraphics$(LIB_SUFFIX).a

all: $(TARGET)

CCFLAGS = -O2 -Wall -I.. -std=c++11

OBJS = \
	opengl.o \
	common.o \
	resource.o \
	texture.o \
	statictexture.o \
	shader.o \
	program.o \
	shape.o \
	shapegroup.o \
	coloredshape.o \
	lineshape.o \
	clearshape.o \
	sprite.o \
	spritefield.o \
	methodlist.o

$(TARGET): $(OBJS)
		$(AR) -rcs $@ $^

%.o: %.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) $(TARGET_ARCH)

spritefield.o: spritefield.cpp
	 	$(CXX) -c -o $@ $< $(CCFLAGS) -ffast-math $(TARGET_ARCH)

clean:
		rm $(OBJS) $(TARGET)
