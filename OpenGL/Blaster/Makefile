GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LIBS         = -lGL -lGLEW -lglfw

TARGET  	 = blaster

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: $(TARGET)

$(TARGET): $(TARGET).o Blaster.o Shot.o Matrix.o Shader.o
	$(GCC) -o $(TARGET) $(TARGET).o Blaster.o Shot.o Matrix.o Shader.o $(LIBS)

$(TARGET).o: ./$(TARGET).cpp
	$(GCC) $(CPPFLAGS) -c ./$(TARGET).cpp -o $(TARGET).o

Blaster.o: ./Blaster.cpp
	$(GCC) $(CPPFLAGS) -c ./Blaster.cpp -o Blaster.o

Shot.o: ./../Shot/Shot.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shot/Shot.cpp -o Shot.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o