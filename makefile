EXE=final
SRC_DIR=src
INC_DIR=include

# Main target
all: $(EXE)

# Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW -I$(INC_DIR)
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.o $(EXE)
else
# OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r | sed -E 's/(.).*/\1/' | tr 12 21)
ARCH=$(shell uname -m)
ifeq ($(ARCH),arm64)
CFLG=-O3 -Wall -Wno-deprecated-declarations -I/opt/homebrew/include -arch arm64 -DRES=$(RES) -I$(INC_DIR)
else
CFLG=-O3 -Wall -Wno-deprecated-declarations -I/usr/local/include -DRES=$(RES) -I$(INC_DIR)
endif
LIBS=-framework GLUT -framework OpenGL
# Linux/Unix/Solaris
else
CFLG=-O3 -Wall -I$(INC_DIR)
LIBS=-lglut -lGLU -lGL -lm
endif
CLEAN=rm -f *.o $(EXE)
endif

# Object files
OBJS=main.o scene.o util.o rover.o

$(EXE): $(OBJS)
	g++ $(CFLG) -o $(EXE) $(OBJS) $(LIBS)

main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/scene.hpp
	g++ -c $(CFLG) $(SRC_DIR)/main.cpp

scene.o: $(SRC_DIR)/scene.cpp $(INC_DIR)/scene.hpp
	g++ -c $(CFLG) $(SRC_DIR)/scene.cpp

util.o: $(SRC_DIR)/util.cpp $(INC_DIR)/util.hpp
	g++ -c $(CFLG) $(SRC_DIR)/util.cpp

rover.o: $(SRC_DIR)/rover.cpp $(INC_DIR)/rover.hpp
	g++ -c $(CFLG) $(SRC_DIR)/rover.cpp

clean:
	$(CLEAN)