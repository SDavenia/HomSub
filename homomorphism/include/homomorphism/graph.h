#ifndef HOMOMORPHISM_GRAPH_H_
#define HOMOMORPHISM_GRAPH_H_

#include <string>
#include <memory>
#include <tuple>
#include <set>

class Graph
{
public:
    Graph(int v) : vertices_(v), edges_(0), matrix_(new bool[v * v]{ 0 }) { };
    Graph(int v, int e) : vertices_(v), edges_(e), matrix_(new bool[v * v]{ 0 }) { };
    Graph(int v, int e, bool* a) : vertices_(v), edges_(e), matrix_(a) { };
    
    static std::shared_ptr<Graph> fromGraph6(std::string graph6);
    static std::shared_ptr<Graph> fromFile(std::string path);
    static std::shared_ptr<Graph> testGraph();
    
    int vertCount();
    int edgeCount();
    void addEdge(int u, int v);
    bool edgeExist(int u, int v);
    bool isIsomorphic(std::shared_ptr<Graph> g);
    std::string toString();
    std::string toGraph6();
    void prettyPrint(std::ostream& os);
    std::shared_ptr<Graph> partition(std::set<int>* parts, int size);

    void operator delete(void* p)
    {
        delete ((Graph*)p)->matrix_;
        free(p);
    }
    
private:
    int vertices_;
    int edges_;
    bool* matrix_;
};

#endif
