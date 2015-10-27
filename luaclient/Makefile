TARGET = luaclient.exe
ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
# LUALIB = luajit-win64
endif
ifeq ($(PROCESSOR_ARCHITECTURE),x86)
# LUALIB = luajit-win32
endif

all: $(TARGET)

CFLAGS = -O2 -Wall -I.. -std=c++11
LDFLAGS = \
	-L. -llua51
	# -L.. -l$(LUALIB)

OBJS = main.o

$(TARGET): $(OBJS)
		$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	 	$(CXX) -c -o $@ $< $(CFLAGS)

clean:
		rm $(OBJS) $(TARGET)

run:
		$(TARGET)
