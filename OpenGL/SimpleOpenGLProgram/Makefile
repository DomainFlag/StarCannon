SYSCONF_LINK = g++
CPPFLAGS     = -std=c++11
LDFLAGS      =
LIBS         = -lm -lSOIL -lGL -lGLEW -lGLU -lglfw

DESTDIR = ./
TARGET  = simple

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

