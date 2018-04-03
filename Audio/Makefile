NAME = Audio

FLAGS = --std=c++11

SOURCE_FILES = \
    ./Audio.cpp \
    ./other/common.cpp \
    ./other/common_platform.cpp

INCLUDE_DIRS = \
    -I./src/inc

LOWLEVEL_LIB = ./lib/libfmod.so

all:
	g++ -pthread ${FLAGS} -o ${NAME} ${SOURCE_FILES} -Wl,-rpath=\$$ORIGIN/$(dir ${LOWLEVEL_LIB}) ${LOWLEVEL_LIB} ${INCLUDE_DIRS}

clean:
	rm ${NAME} 