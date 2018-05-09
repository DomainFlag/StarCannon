GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LIBS         = -lm -lSOIL -lGL -lGLEW -lglfw

TARGET  	 = X-Fighter

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: $(TARGET)

$(TARGET): x-fighter.o X-Fighter.o ObjReader.o Matrix.o Shader.o
	$(GCC) -o $(TARGET) x-fighter.o X-Fighter.o ObjReader.o Matrix.o Shader.o $(LIBS)

x-fighter.o: ./x-fighter.cpp
	$(GCC) $(CPPFLAGS) -c ./x-fighter.cpp -o x-fighter.o

X-Fighter.o: ./X-Fighter.cpp
	$(GCC) $(CPPFLAGS) -c ./X-Fighter.cpp -o X-Fighter.o

ObjReader.o: ./../Tools/ObjReader/ObjReader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/ObjReader/ObjReader.cpp -o ObjReader.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o