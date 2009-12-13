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

#include "elevationmodel.h"
#include "randompositionaldeviation.h"
#include "progresscontroller.h"
#include "operationcancelledexception.h"
#include "fileformatexception.h"

ElevationModel::ElevationModel(Matrix<Vertex>* grid,
                               double xOffset, double yOffset, double zOffset,
                               double xSize, double ySize, double zSize, QWidget *parent) :
QObject(parent),
_xOffset(xOffset),
_yOffset(yOffset),
_zOffset(zOffset),
_xSize(xSize),
_ySize(ySize),
_zSize(zSize),
triangulation(NULL),
_grid(grid)
{
}

ElevationModel::ElevationModel(unsigned n, QWidget * parent) :
        QObject(parent),
        triangulation(NULL)
{

    Matrix<Vertex>* source= new Matrix<Vertex>(5,5);
    for(unsigned x=0;x<source->xSize();x++)
        for(unsigned y=0;y<source->ySize();y++)
            source->put(x,y,new Vertex(x,y,(19000*(rand()%100)/100.0) -11000));

    RandomPositionalDeviation *dev = new RandomPositionalDeviation();
    MidPointModelSampler * sampler = new MidPointModelSampler(dev, source, 0, 0, 100, n, this);
    sampler->setMinDeviation(1.0);
    makeSample(sampler);
    double zMax = -100000.0;
    double zMin = sampler->getSubModel(_grid);
    delete sampler;
    delete dev;

    for(unsigned x=0;x<n;x++)
        for(unsigned y=0;y<n;y++){
        _grid->at(x,y)->setZ((_grid->at(x,y)->z()-zMin));
        if(_grid->at(x,y)->z()>zMax)
            zMax=_grid->at(x,y)->z();
    }

    _xOffset=0;
    _yOffset=0;
    _zOffset=zMin;
    _xSize=ONE_DEGREE_METERS;
    _ySize=ONE_DEGREE_METERS;
    _zSize=zMax;

    makeTriangles();
}

ElevationModel::ElevationModel(unsigned n,ElevationModel * source,unsigned sourceXOffset,unsigned sourceYOffset,unsigned l, QWidget *parent)
        : QObject(parent),triangulation(NULL)
{
    RandomPositionalDeviation *dev = new RandomPositionalDeviation();
    MidPointModelSampler * sampler = new MidPointModelSampler(dev, source->_grid, sourceXOffset, sourceYOffset, l, n, this);
    sampler->setMinDeviation(source->xSize()/source->xResolution());
    makeSample(sampler);
    double zMin = sampler->getSubModel(_grid);
    delete sampler;
    delete dev;

    double zMax = -100000.0;

    for(unsigned x=0;x<n;x++)
        for(unsigned y=0;y<n;y++){
        _grid->at(x,y)->setZ(_grid->at(x,y)->z()-zMin);
        if(_grid->at(x,y)->z()>zMax)
            zMax=_grid->at(x,y)->z();
    }

    _xSize=source->xSize()*l/100.0;
    _ySize=source->ySize()*l/100.0;
    _xOffset = source->xOffset()+(source->xSize()*sourceXOffset/100.0)/ONE_DEGREE_METERS;
    _yOffset = source->yOffset()+(source->ySize()*sourceYOffset/100.0)/ONE_DEGREE_METERS;
    _zOffset =source->zOffset()+zMin;
    _zSize = zMax;
    makeTriangles();
}

ElevationModel::~ElevationModel(){
    delete _grid;
    delete triangulation;
}

void ElevationModel::makeSample(MidPointModelSampler* s)
{
    ProgressController * con = new ProgressController(s, dynamic_cast<QWidget*>(parent()), QString("Generating terrain"), QString("Terrain generation in progress..."));
    s->start();
    con->exec();
    if (con->result() != QDialog::Accepted){
        delete con;
        delete s;
        throw OperationCancelledException();
    }
    delete con;
}

void ElevationModel::makeTriangles()
{
    triangulation = new BinTreeTriangulation(_grid,4*_zSize/(_grid->xSize()-1), this);
    ProgressController *con = new ProgressController(triangulation, dynamic_cast<QWidget*>(parent()), QString("Triangulating"), QString("Triangulation in progress..."));
    triangulation->start();
    con->exec();
    if (con->result() != QDialog::Accepted){
        delete con;
        delete triangulation;
        throw OperationCancelledException();
    }
    delete con;
}

vector<Strip*>* ElevationModel::getStrips(){
    if(triangulation==NULL)
        return NULL;

    vector<Strip*>* strips = triangulation->getStrips();

    vector<Strip*>* strips2 = new vector<Strip*>();

    double xFactor= 1.0/(_grid->xSize()-1);
    double yFactor= 1.0/(_grid->ySize()-1);
    double zFactor= 1.0/(_xSize);

    for(unsigned i=0;i<strips->size();i++){
        Strip* s = strips->at(i);
        Strip* s2 = new Strip;
        s2->v = new vector<Vertex*>();
        s2->cw=s->cw;
        for(unsigned j=0;j<s->v->size();j++){
            Vertex* v= s->v->at(j);
            s2->v->push_back(new Vertex(v->x()*xFactor,
                                     v->y()*yFactor,
                                     v->z()*zFactor));
        }
        delete s->v;
        delete s;
        strips2->push_back(s2);
    }

    delete strips;

    return strips2;

}

double ElevationModel::xOffset(){return _xOffset;}
double ElevationModel::xSize(){return _xSize;}
double ElevationModel::yOffset(){return _yOffset;}
double ElevationModel::ySize(){return _ySize;}
double ElevationModel::zOffset(){return _zOffset;}
double ElevationModel::zSize(){return (_zSize/_xSize);}
unsigned ElevationModel::xResolution(){return _grid->xSize();}
unsigned ElevationModel::yResolution(){return _grid->ySize();}


