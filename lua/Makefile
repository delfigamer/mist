ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_SUFFIX = -win64
else
TARGET_SUFFIX = -win32
endif
else
TARGET_SUFFIX = -arm7a
endif
TARGETNAME = libluajit$(TARGET_SUFFIX).a
TARGET = ../$(TARGETNAME)
SOURCEPATH = ../external/luajit/src
SOURCE = $(SOURCEPATH)/libluajit.a

all: $(TARGET)

$(TARGET): $(SOURCE)
		copy $(SOURCE) $(TARGET)

$(SOURCE):
	 	$(MAKE) -C $(SOURCEPATH) BUILDMODE=static

clean:
		rm $(TARGET)
	 	$(MAKE) -C $(SOURCEPATH) clean BUILDMODE=static
