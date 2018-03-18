SYSCONF_LINK = g++
CPPFLAGS     = -std=c++11
LDFLAGS      =
LIBS         = -lm -lSOIL -lGL -lGLEW -lGLU -lglfw

DESTDIR = ./
TARGET  = game

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: game.out

game.out: game.o Ocean.o Sky.o Matrix.o
	g++ -o game.out game.o Ocean.o Sky.o Matrix.o $(LIBS)

game.o: game.cpp ./../Ocean/Ocean.h ./../Sky/Sky.h ./../Tools/Matrix/Matrix.h
	g++ --std=c++11 -c game.cpp ./../Ocean/Ocean.h ./../Sky/Sky.h ./../Tools/Matrix/Matrix.h -o game.o

Ocean.o: ./../Ocean/Ocean.cpp ./../Ocean/Ocean.h ./../Tools/Matrix/Matrix.h
	g++ --std=c++11 -c ./../Ocean/Ocean.cpp ./../Ocean/Ocean.h ./../Tools/Matrix/Matrix.h -o Ocean.o

Sky.o: ./../Sky/Sky.cpp ./../Sky/Sky.h ./../Tools/Matrix/Matrix.h
	g++ --std=c++11 -c ./../Sky/Sky.cpp ./../Sky/Sky.h ./../Tools/Matrix/Matrix.h -o Sky.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp ./../Tools/Matrix/Matrix.h
	g++ --std=c++11 -c ./../Tools/Matrix/Matrix.cpp ./../Tools/Matrix/Matrix.h -o Matrix.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o

