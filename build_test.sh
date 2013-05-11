
CXXFLAG=-g

g++ -c Polygon.cpp $CXXFLAG
g++ -c Utility.cpp $CXXFLAG
g++ -c BSPNode.cpp $CXXFLAG
g++ -c BSPTree.cpp $CXXFLAG
g++ -c PointLightSource.cpp $CXXFLAG

g++ -c test.cpp $CXXFLAG
g++ -c test2.cpp $CXXFLAG
g++ -c test3.cpp $CXXFLAG
g++ -c test4.cpp $CXXFLAG

g++ -o test $CXXFLAG Polygon.o Utility.o BSPNode.o BSPTree.o PointLightSource.o test.o
g++ -o test2 $CXXFLAG Polygon.o Utility.o BSPNode.o BSPTree.o PointLightSource.o test2.o
g++ -o test3 $CXXFLAG Polygon.o Utility.o BSPNode.o BSPTree.o PointLightSource.o test3.o
g++ -o test4 $CXXFLAG Polygon.o Utility.o BSPNode.o BSPTree.o PointLightSource.o test4.o