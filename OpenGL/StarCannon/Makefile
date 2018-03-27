GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LDFLAGS      =
LIBS         = -lSOIL -lGL -lGLEW -lglfw

DESTDIR = ./
TARGET  = Starcannon

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: Starcannon

Starcannon: Game.o Terrain.o Sky.o Thruster.o Spacecraft.o Matrix.o Shader.o
	$(GCC) -o Starcannon Game.o Terrain.o Sky.o Thruster.o Spacecraft.o Shader.o Matrix.o $(LIBS)

Game.o: Game.cpp
	$(GCC) $(CPPFLAGS) -c Game.cpp -o Game.o

Thruster.o: ./../Thruster/Thruster.cpp
	$(GCC) $(CPPFLAGS) -c ./../Thruster/Thruster.cpp -o Thruster.o

Spacecraft.o: ./../Spacecraft/Spacecraft.cpp
	$(GCC) $(CPPFLAGS) -c ./../Spacecraft/Spacecraft.cpp -o Spacecraft.o

Terrain.o: ./../Terrain/Terrain.cpp
	$(GCC) $(CPPFLAGS) -c ./../Terrain/Terrain.cpp -o Terrain.o

Sky.o: ./../Sky/Sky.cpp
	$(GCC) $(CPPFLAGS) -c ./../Sky/Sky.cpp -o Sky.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o

