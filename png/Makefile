ifeq ($(OS),Windows_NT)
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
TARGET_SUFFIX = -win64
else
TARGET_SUFFIX = -win32
endif
else
TARGET_SUFFIX = -arm7a
endif
TARGETNAME = libpng$(TARGET_SUFFIX).a
TARGET = ../$(TARGETNAME)
SOURCEPATH = libpng
SOURCE = $(SOURCEPATH)/libpng.a

all: $(TARGET)

$(TARGET): $(SOURCE)
		cp $(SOURCE) $(TARGET)

$(SOURCE): $(SOURCEPATH)/Makefile
	 	$(MAKE) -C $(SOURCEPATH)

$(SOURCEPATH)/Makefile:
		cp $(SOURCEPATH)/scripts/makefile.gcc $(SOURCEPATH)/Makefile

clean:
		rm $(TARGET)
	 	$(MAKE) -C $(SOURCEPATH) clean
		rm $(SOURCEPATH)/Makefile
