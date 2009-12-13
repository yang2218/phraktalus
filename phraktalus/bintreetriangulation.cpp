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

#include "bintreetriangulation.h"
#include <cassert>
#include <queue>
using namespace std;

BinTreeTriangulation::BinTreeTriangulation(Matrix<Vertex> * g, double deltaZ, QObject *parent) :
        ProgressControlableThread(parent),
        grid(g),
        v1(NULL),
        v2(NULL),
        _deltaX(1),
        _deltaY(1),
        _deltaZ(deltaZ),
        finished(NULL)
{
    setTotalIterations(((g->xSize()-1)*(g->ySize()-1))*2);
}


BinTreeTriangulation::~BinTreeTriangulation(){
    if(finished!=NULL)
        delete finished;
    if(v1!=NULL)
        delete v1;
    if(v2!=NULL)
        delete v2;
}


void BinTreeTriangulation::_split(TriangulationNode* n){
    
    if(n->split()!=NULL)
        return;
    
    if((n->abNeighbor()!=NULL)&&(n->abNeighbor()->abNeighbor()!=n)){
        _split(n->abNeighbor());
    }
    
    Vertex * v;
    if((n->abNeighbor()==NULL)||(n->abNeighbor()->split()==NULL))
        v = grid->at(unsigned((n->a()->x() + n->b()->x())/2),unsigned((n->a()->y() + n->b()->y())/2));
    else
        v = n->abNeighbor()->split()->first->c();
    
    TriangulationNode *v1,*v2;
    v1 = new TriangulationNode(n->c(),n->a(),v,n->depth()+1);
    v2 = new TriangulationNode(n->b(),n->c(),v,n->depth()+1);
    
    v1->caNeighbor(v2);
    v1->abNeighbor(n->caNeighbor());
    v2->bcNeighbor(v1);
    v2->abNeighbor(n->bcNeighbor());
    n->split(v1,v2);
    
    // update neighborhood
    if(n->caNeighbor()!=NULL){
        if(n->caNeighbor()->abNeighbor()==n){
            n->caNeighbor()->abNeighbor(v1);
        }else if(n->caNeighbor()->caNeighbor()==n){
            n->caNeighbor()->caNeighbor(v1);
        }else if(n->caNeighbor()->bcNeighbor()==n){
            n->caNeighbor()->bcNeighbor(v1);
        }else{
            assert(false);
        }
    }
    
    if(n->bcNeighbor()!=NULL){
        if(n->bcNeighbor()->abNeighbor()==n){
            n->bcNeighbor()->abNeighbor(v2);
        }else if(n->bcNeighbor()->caNeighbor()==n){
            n->bcNeighbor()->caNeighbor(v2);
        }else if(n->bcNeighbor()->bcNeighbor()==n){
            n->bcNeighbor()->bcNeighbor(v2);
        }else{
            assert(false);
        }
    }
    
    if(n->abNeighbor()!=NULL){
        if(n->abNeighbor()->split()==NULL)
            _split(n->abNeighbor());
        else{
            n->abNeighbor()->split()->first->bcNeighbor(v2);
            n->abNeighbor()->split()->second->caNeighbor(v1);
            v1->bcNeighbor(n->abNeighbor()->split()->second);
            v2->caNeighbor(n->abNeighbor()->split()->first);
        }
    }
    
}

bool BinTreeTriangulation::_shouldSplit(TriangulationNode* n){
    if((n==NULL)||(n->split()!=NULL))
        return false;

    if(n->depth()>1000)
        return false;

    if((fabs(n->a()->x()-n->b()->x())<=_deltaX)&&(fabs(n->a()->y()-n->b()->y())<=_deltaY))
        return false;
    if ((n->a()->z()-n->b()->z()>_deltaZ)
        ||(n->b()->z()-n->c()->z()>_deltaZ)
        ||(n->c()->z()-n->a()->z()>_deltaZ))

        return true;

    Vertex * v = grid->at(unsigned((n->a()->x() + n->b()->x())/2),unsigned((n->a()->y() + n->b()->y())/2));

    TriangulationNode* aux =  new TriangulationNode(n->c(),n->a(),v,n->depth()+1);
    bool b = false;
    b = _shouldSplit(aux);
    delete aux;
    if(!b){
        aux =  new TriangulationNode(n->b(),n->c(),v,n->depth()+1);

        b = _shouldSplit(aux);
        delete aux;
    }

    return b;



}

void BinTreeTriangulation::split(TriangulationNode* n){
    if(_shouldSplit(n))
        _split(n);
}

void BinTreeTriangulation::_triangulate(){
    finished = new vector<TriangulationNode*>();
    
    queue<TriangulationNode*> enqueued;
    
    v1 = new TriangulationNode(grid->at(0,0),grid->at(grid->xSize()-1,grid->ySize()-1)
                               ,grid->at(grid->xSize()-1,0),0);
    v2 = new TriangulationNode(grid->at(grid->xSize()-1,grid->ySize()-1),grid->at(0,0)
                               ,grid->at(0,grid->ySize()-1),0);
    
    v1->abNeighbor(v2);
    v2->abNeighbor(v1);
    
    enqueued.push(v1);
    enqueued.push(v2);

    
    while(!enqueued.empty()){
        TriangulationNode *next = enqueued.front();
        enqueued.pop();
        
        split(next);
        if(next->split()!=NULL){
            enqueued.push(next->split()->first);
            enqueued.push(next->split()->second);
        }

        iterationCompleted();
    }
    
    agr(v1,finished);
    agr(v2,finished);
    
}

vector<Triangle*> * BinTreeTriangulation::getTriangulation() const
{
    vector<Triangle*>* toReturn = new vector<Triangle*>();
    
    if(finished!=NULL)
        for(unsigned i=0;i<finished->size();i++)
            toReturn->push_back(finished->at(i));
    
    return toReturn;
}

void BinTreeTriangulation::run(){
    if(finished==NULL)
        _triangulate();
}



void BinTreeTriangulation::agr(TriangulationNode* v,vector<TriangulationNode*>* t){
    if(v==NULL)
        return;
    if(v->split()==NULL){
        v->id(t->size());
        t->push_back(v);
    }else{
        agr(v->split()->first,t);
        agr(v->split()->second,t);
    }
}

vector<Strip*> * BinTreeTriangulation::getStrips(){

    vector<Strip*>* toReturn = new vector<Strip*>();
    
    if(finished==NULL)
        _triangulate();

    visited = new bool[finished->size()];
    for(unsigned i=0;i<finished->size();i++)
        visited[i]=false;
    unsigned ab,bc,ca,ba,cb,ac,max;
    TriangulationNode * n;
    for(unsigned i=0;i<finished->size();i++)
        if(!visited[i]){
        n = finished->at(i);
        if(n->split()==NULL){
            Strip* strip= new Strip;
            strip->v = new vector<Vertex*>();

            ab=bc=ca=ba=cb=ac=max=0;
            
            ab = simpleStripLenght(n,n->a(),n->b(),true);
            ba = simpleStripLenght(n,n->b(),n->a(),false);
            bc = simpleStripLenght(n,n->b(),n->c(),true);
            cb = simpleStripLenght(n,n->c(),n->b(),false);
            ca = simpleStripLenght(n,n->c(),n->a(),true);
            ac = simpleStripLenght(n,n->a(),n->c(),false);
            
            if((ab>=ba)&&(ab>=bc)&&(ab>=cb)&&(ab>=ca)&&(ab>=ac)){
                strip->cw=false;
                strip->v->push_back(n->a());
                strip->v->push_back(n->b());
                completeSimpleStrip(strip->v,n,n->a(),n->b(),true);
            }else if((ba>=ab)&&(ba>=bc)&&(ba>=cb)&&(ba>=ca)&&(ba>=ac)){
                strip->cw=true;
                strip->v->push_back(n->b());
                strip->v->push_back(n->a());
                completeSimpleStrip(strip->v,n,n->b(),n->a(),false);
            }else if((bc>=ab)&&(bc>=ba)&&(bc>=cb)&&(bc>=ca)&&(bc>=ac)){
                strip->cw=false;
                strip->v->push_back(n->b());
                strip->v->push_back(n->c());
                completeSimpleStrip(strip->v,n,n->b(),n->c(),true);
            }else if((cb>=ab)&&(cb>=ba)&&(cb>=bc)&&(cb>=ca)&&(cb>=ac)){
                strip->cw=true;
                strip->v->push_back(n->c());
                strip->v->push_back(n->b());
                completeSimpleStrip(strip->v,n,n->c(),n->b(),false);
            }else if((ca>=ab)&&(ca>=ba)&&(ca>=bc)&&(ca>=cb)&&(ca>=ac)){
                strip->cw=false;
                strip->v->push_back(n->c());
                strip->v->push_back(n->a());
                completeSimpleStrip(strip->v,n,n->c(),n->a(),true);
            }else{
                strip->cw=true;
                strip->v->push_back(n->a());
                strip->v->push_back(n->c());
                completeSimpleStrip(strip->v,n,n->a(),n->c(),false);
            }
            toReturn->push_back(strip);
            
        }
    }
    
    delete[] visited;
    return toReturn;
}


unsigned BinTreeTriangulation::simpleStripLenght(TriangulationNode * n,Vertex *v1,Vertex *v2,bool ccw,unsigned current){
    if((n==NULL)||visited[n->id()])
        return current;
    Vertex* v3;
    TriangulationNode * n2;
    if(ccw){
        if((n->a()==v1)&&(n->b()==v2)){//counter clock wise
            v3=n->c();
            n2=n->bcNeighbor();
        }else if((n->b()==v1)&&(n->c()==v2)){
            v3=n->a();
            n2=n->caNeighbor();
        }else if((n->c()==v1)&&(n->a()==v2)){
            v3=n->b();
            n2=n->abNeighbor();
        }
    }else{
        if((n->a()==v2)&&(n->b()==v1)){//clock wise
            v3=n->c();
            n2=n->caNeighbor();
        }else if((n->b()==v2)&&(n->c()==v1)){
            v3=n->a();
            n2=n->abNeighbor();
        }else if((n->c()==v2)&&(n->a()==v1)){
            v3=n->b();
            n2=n->bcNeighbor();
        }
    }
    visited[n->id()]=true;
    unsigned lenght = simpleStripLenght(n2,v2,v3,!ccw,current+1);
    visited[n->id()]=false;
    
    return lenght;
}

void BinTreeTriangulation::completeSimpleStrip(vector<Vertex*>* strip,TriangulationNode * n,Vertex *v1,Vertex *v2,bool ccw){
    if((n==NULL)||visited[n->id()])
        return;

    Vertex* v3;
    TriangulationNode * n2;
    if(ccw){
        if((n->a()==v1)&&(n->b()==v2)){//counter clock wise
            v3=n->c();
            n2=n->bcNeighbor();
        }else if((n->b()==v1)&&(n->c()==v2)){
            v3=n->a();
            n2=n->caNeighbor();
        }else if((n->c()==v1)&&(n->a()==v2)){
            v3=n->b();
            n2=n->abNeighbor();
        }
    }else{
        if((n->a()==v2)&&(n->b()==v1)){//clock wise
            v3=n->c();
            n2=n->caNeighbor();
        }else if((n->b()==v2)&&(n->c()==v1)){
            v3=n->a();
            n2=n->abNeighbor();
        }else if((n->c()==v2)&&(n->a()==v1)){
            v3=n->b();
            n2=n->bcNeighbor();
        }
    }
    
    strip->push_back(v3);
    visited[n->id()]=true;
    completeSimpleStrip(strip,n2,v2,v3,!ccw);
    
}
