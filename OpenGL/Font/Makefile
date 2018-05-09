SYSCONF_LINK = g++
CPPFLAGS     = -std=c++11
CFLAGS       = -I /usr/local/include/freetype2 -L /usr/local/lib/freetype2
LDFLAGS      = 
LIBS         = -lGL -lGLEW -lglfw -lfreetype

DESTDIR = ./
TARGET  = font

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

