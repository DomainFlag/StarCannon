GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LIBS         = -lGL -lGLEW -lglfw

TARGET  	 = Thruster

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: $(TARGET)

$(TARGET): thruster.o Thruster.o Matrix.o Shader.o
	$(GCC) -o $(TARGET) thruster.o Thruster.o Matrix.o Shader.o $(LIBS)

thruster.o: ./thruster.cpp
	$(GCC) $(CPPFLAGS) -c ./thruster.cpp -o thruster.o

Thruster.o: ./Thruster.cpp
	$(GCC) $(CPPFLAGS) -c ./Thruster.cpp -o Thruster.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o

