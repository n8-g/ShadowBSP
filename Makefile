OS = $(shell uname)

SOURCE = BSPNode.cpp BSPTree.cpp Polygon.cpp PointLightSource.cpp Utility.cpp
HEADERS = BSPNode.h BSPTree.h PointLightSource.h Polygon.h Utility.h
LIBDIRS = -L/usr/X11/lib
INCLUDE= -I/usr/X11/include
ifeq ($(OS),Darwin)
LIB = -framework OpenGL -framework GLUT
endif
ifeq ($(OS),Linux)
LIB = -lGL -lGLUT
endif

CXXFLAG=-g


BSP: $(SOURCE) $(HEADERS)
	g++ $(CXXFLAG) -o $@ $(SOURCE) $(LIBDIR) $(INCLUDE) $(LIB) Main.cpp

all: BSP tests

tests: test test2 test3 test4
	
test: $(SOURCE) $(HEADERS)
	g++ $(CXXFLAG) -o $@ $(SOURCE) test.cpp
	
test2: $(SOURCE) $(HEADERS)
	g++ $(CXXFLAG) -o $@ $(SOURCE) test2.cpp
	
test3: $(SOURCE) $(HEADERS)
	g++ $(CXXFLAG) -o $@ $(SOURCE) test3.cpp
	
test4: $(SOURCE) $(HEADERS)
	g++ $(CXXFLAG) -o $@ $(SOURCE) test4.cpp
	
clean:
	rm -rf *.o BSP test test2 test3 test4 *.dSYM