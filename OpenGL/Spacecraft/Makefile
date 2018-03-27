COMMAND = g++ -g
FLAGS = --std=c++11 
LIBS = -lGL -lGLEW -lglfw

DESTDIR = ./
TARGET  = Spacecraft

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(COMMAND) $(FLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)

$(DESTDIR)%.o: $(DESTDIR)%.cpp
	$(COMMAND) $(FLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

