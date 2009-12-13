#ifndef TRIANGULATIONNODE_H
#define TRIANGULATIONNODE_H
#include "triangle.h"

class TriangulationNode:public Triangle{

protected:

    pair<TriangulationNode*,TriangulationNode*> *_split;
    TriangulationNode *_abNeighbor,*_bcNeighbor,*_caNeighbor;
    unsigned _id;
    unsigned _depth;

public:


    TriangulationNode(Vertex *v1,Vertex *v2,Vertex *v3,unsigned depth):
        Triangle(v1,v2,v3),
        _split(NULL),
        _abNeighbor(NULL),
        _bcNeighbor(NULL),
        _caNeighbor(NULL),
        _id(0),_depth(depth)
        {}

    ~TriangulationNode(){
        if(_split!=NULL){
          delete _split->first;
          delete _split->second;
          delete _split;
      }
    }

    TriangulationNode * abNeighbor(){return _abNeighbor;}
    TriangulationNode * bcNeighbor(){return _bcNeighbor;}
    TriangulationNode * caNeighbor(){return _caNeighbor;}

    void abNeighbor(TriangulationNode * n){_abNeighbor=n;}
    void bcNeighbor(TriangulationNode * n){_bcNeighbor=n;}
    void caNeighbor(TriangulationNode * n){_caNeighbor=n;}

    void split(TriangulationNode * n1,TriangulationNode * n2){
            _split = new pair<TriangulationNode*,TriangulationNode*>(n1,n2);
        }
    pair<TriangulationNode*,TriangulationNode*> * split(){return _split;}

    void depth(unsigned depth){_depth=depth;}
    unsigned depth(){return _depth;}

    unsigned id(){return _id;}
    void id(unsigned id){_id=id;}
};
#endif // TRIANGULATIONNODE_H
