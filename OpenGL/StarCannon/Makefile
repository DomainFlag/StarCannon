GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LDFLAGS      =
LIBS         = -lSOIL -lGL -lGLEW -lglfw

DESTDIR = ./
TARGET  = Starcannon

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

SOURCE_FILES = \
    ./../Audio/other/common.cpp \
    ./../Audio/other/common_platform.cpp

INCLUDE_DIRS = \
    -I./../Audio/src/inc

LOWLEVEL_LIB = ./../Audio/lib/libfmod.so

all: Starcannon

Starcannon: Game.o Terrain.o Sky.o Thruster.o Spacecraft.o CommonPlatform.o Common.o Audio.o Matrix.o Shader.o
	$(GCC) -pthread -o Starcannon Game.o Terrain.o Sky.o Thruster.o Spacecraft.o CommonPlatform.o Common.o Audio.o Shader.o Matrix.o $(LIBS) -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}) ${LOWLEVEL_LIB} ${INCLUDE_DIRS}

Game.o: Game.cpp
	$(GCC) $(CPPFLAGS) -c Game.cpp -o Game.o

Thruster.o: ./../Thruster/Thruster.cpp
	$(GCC) $(CPPFLAGS) -c ./../Thruster/Thruster.cpp -o Thruster.o

Spacecraft.o: ./../Spacecraft/Spacecraft.cpp
	$(GCC) $(CPPFLAGS) -c ./../Spacecraft/Spacecraft.cpp -o Spacecraft.o

Terrain.o: ./../Terrain/Terrain.cpp
	$(GCC) $(CPPFLAGS) -c ./../Terrain/Terrain.cpp -o Terrain.o

Sky.o: ./../SkyBox/Sky.cpp
	$(GCC) $(CPPFLAGS) -c ./../SkyBox/Sky.cpp -o Sky.o

Audio.o: ./../Audio/Audio.cpp
	$(GCC) $(CPPFLAGS) -c ./../Audio/Audio.cpp -o Audio.o

CommonPlatform.o: ./../Audio/other/common_platform.cpp
	$(GCC) $(CPPFLAGS) -c ./../Audio/other/common_platform.cpp -o CommonPlatform.o

Common.o: ./../Audio/other/common.cpp
	$(GCC) $(CPPFLAGS) -c ./../Audio/other/common.cpp -o Common.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o

