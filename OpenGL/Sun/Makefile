GCC 		 = g++
CPPFLAGS     = -g -std=c++11
LIBS         = -lGL -lGLEW -lglfw

TARGET  	 = sun

OBJECTS := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

.PHOONY: all run clean

all: clean $(TARGET) run

$(TARGET): $(TARGET).o Sun.o Matrix.o Shader.o
	$(GCC) -o $(TARGET) $(TARGET).o Sun.o Matrix.o Shader.o $(LIBS)

$(TARGET).o: ./$(TARGET).cpp
	$(GCC) $(CPPFLAGS) -c ./$(TARGET).cpp -o $(TARGET).o

Sun.o: ./Sun.cpp
	$(GCC) $(CPPFLAGS) -c ./Sun.cpp -o Sun.o

Shader.o: ./../Shader/Shader.cpp
	$(GCC) $(CPPFLAGS) -c ./../Shader/Shader.cpp -o Shader.o

Matrix.o: ./../Tools/Matrix/Matrix.cpp
	$(GCC) $(CPPFLAGS) -c ./../Tools/Matrix/Matrix.cpp -o Matrix.o

run: ./$(TARGET)
	./$(TARGET)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.o