#include <iostream>
#include "homomorphism/Graph.h"

int main(int argc, char *argv[])
{
    Graph a = Graph::fromGraph6("This is graph6");
    Graph b = Graph::fromFile("This is a path");
    std::cout << a.edgeCount() << std::endl;
    std::cout << a.isIsomorphic(b) << std::endl;
    
    return 0;
}
