GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LDFLAGS      =
LIBS         = -lm -lSOIL -lGL -lGLEW -lglfw

DESTDIR = ./
TARGET  = StarCannon

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

SOURCE_FILES = \
    ./../Audio/other/common.cpp \
    ./../Audio/other/common_platform.cpp

INCLUDE_DIRS = \
    -I./../Audio/src/inc

LOWLEVEL_LIB = ./../Audio/lib/libfmod.so

.PHONY: all clean run

all: clean $(TARGET) run

$(TARGET): StarCannon.o Terrain.o Sky.o Sun.o Blaster.o Shot.o Thruster.o Audio.o CommonPlatform.o Common.o X-Fighter.o Noise.o ObjReader.o Matrix.o Shader.o
	$(GCC) -pthread -o $(TARGET) Terrain.o Sky.o Sun.o Blaster.o Shot.o Thruster.o StarCannon.o Audio.o CommonPlatform.o Common.o X-Fighter.o Noise.o ObjReader.o Matrix.o Shader.o $(LIBS) -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}) ${LOWLEVEL_LIB} ${INCLUDE_DIRS}

StarCannon.o: StarCannon.cpp
	$(GCC) $(CPPFLAGS) -c StarCannon.cpp -o StarCannon.o

Thruster.o: ./../Thruster/Thruster.cpp
	$(GCC) $(CPPFLAGS) -c ./../Thruster/Thruster.cpp -o Thruster.o

Blaster.o: ./../Blaster/Blaster.cpp
	$(GCC) $(CPPFLAGS) -c ./../Blaster/Blaster.cpp -o Blaster.o

Shot.o: ./../Shot/Shot.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shot/Shot.cpp -o Shot.o

Terrain.o: ./../Terrain/Terrain.cpp
	$(GCC) $(CPPFLAGS) -c ./../Terrain/Terrain.cpp -o Terrain.o

Sky.o: ./../Sky/Sky.cpp
	$(GCC) $(CPPFLAGS) -c ./../Sky/Sky.cpp -o Sky.o

Sun.o: ./../Sun/Sun.cpp
	$(GCC) $(CPPFLAGS) -c ./../Sun/Sun.cpp -o Sun.o

Audio.o: ./../Audio/Audio.cpp
	$(GCC) $(CPPFLAGS) -c ./../Audio/Audio.cpp -o Audio.o

CommonPlatform.o: ./../Audio/other/common_platform.cpp
	$(GCC) $(CPPFLAGS) -c ./../Audio/other/common_platform.cpp -o CommonPlatform.o

Common.o: ./../Audio/other/common.cpp
	$(GCC) $(CPPFLAGS) -c ./../Audio/other/common.cpp -o Common.o

X-Fighter.o: ./../X-Fighter/X-Fighter.cpp
	$(GCC) $(CPPFLAGS) -c ./../X-Fighter/X-Fighter.cpp -o X-Fighter.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Noise.o: ./../Tools/Noise/Noise.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Noise/Noise.cpp -o Noise.o

ObjReader.o: ./../Tools/ObjReader/ObjReader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/ObjReader/ObjReader.cpp -o ObjReader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

run: $(TARGET)
	./$(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o
	-rm -f ./../*/*.o