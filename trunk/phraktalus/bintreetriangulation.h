/****************************************************************************************
 * Copyright (c) 2009 López, Gabriel Orlando <glopez@alumnos.exa.unicen.edu.ar>         *
 * Copyright (c) 2009 Rossi, Matías Javier <marossi@alumnos.exa.unicen.edu.ar>          *
 *                                                                                      *
 * This program is free software; you can redistribute it and/or modify it under        *
 * the terms of the GNU General Public License as published by the Free Software        *
 * Foundation; either version 3 of the License, or (at your option) any later           *
 * version.                                                                             *
 *                                                                                      *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY      *
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A      *
 * PARTICULAR PURPOSE. See the GNU General Public License for more details.             *
 *                                                                                      *
 * You should have received a copy of the GNU General Public License along with         *
 * this program.  If not, see <http://www.gnu.org/licenses/>.                           *
 ****************************************************************************************/

#ifndef BINTREETRIANGULATION_H
#define BINTREETRIANGULATION_H
#include "progresscontrolablethread.h"
#include "triangulationnode.h"
#include "matrix.h"
#include <vector>
using namespace std;

typedef struct{
    vector<Vertex*>* v;
    bool cw;
}Strip;

class BinTreeTriangulation : public ProgressControlableThread
{
protected:
    Matrix<Vertex> * grid;
    TriangulationNode *v1,*v2;

    double _deltaX,_deltaY,_deltaZ;
    void _split(TriangulationNode* n);

    bool _shouldSplit(TriangulationNode* n);

    void split(TriangulationNode* n);

    vector<TriangulationNode*> * finished;
    bool * visited;
    void agr(TriangulationNode*,vector<TriangulationNode*>*);
    void _triangulate();

    unsigned simpleStripLenght(TriangulationNode * n,Vertex *v1,Vertex *v2,bool ccw,unsigned current=0);

    void completeSimpleStrip(vector<Vertex*>* strip,TriangulationNode * n,Vertex *v1,Vertex *v2,bool ccw);
public:

    BinTreeTriangulation(Matrix<Vertex> * g,double deltaZ, QObject *parent = 0);

    ~BinTreeTriangulation();

    vector<Triangle*> * getTriangulation() const;

    void run();

    vector<Strip*> * getStrips();
};

#endif // BINTREETRIANGULATION_H
