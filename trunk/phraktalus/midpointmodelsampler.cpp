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

#include "midpointmodelsampler.h"


MidPointModelSampler::MidPointModelSampler(PositionalDeviation* dev,
                                           Matrix<Vertex>* source,
                                           unsigned sourceXOffset,
                                           unsigned sourceYOffset,
                                           unsigned l,
                                           unsigned n,
                                           QObject *parent) :
    ProgressControlableThread(parent),
    deviator(dev),
    minZ(1.05e200),
    _minDeviation(0.01),
    xStart((source->xSize()*sourceXOffset)/100),
    yStart((source->ySize()*sourceYOffset)/100),
    lenght((source->xSize()*l)/100),
    source(source),
    toFill(new Matrix<Vertex>(n,n))

{

    setTotalIterations(n*n);
}

void MidPointModelSampler::setMinDeviation(double d){
    _minDeviation=d;
}

double MidPointModelSampler::getSubModel(Matrix<Vertex>* &r)
{
    r = toFill;
    return minZ;
}

void MidPointModelSampler::run(){
    unsigned inc = unsigned(ceil((toFill->xSize()-1.0)/(lenght-1)));
    unsigned quadLenght = unsigned(floor((toFill->xSize()-1.0)/(lenght-1)));
    quadLenght = inc;

    for(unsigned x=0;x<(toFill->xSize()-1);x+=inc){
        for(unsigned y=0;y<(toFill->ySize()-1);y+=inc){

            unsigned x2 = x+quadLenght;
            unsigned y2 = y+quadLenght;
            x2= (x2<(toFill->xSize()-1))?x2:toFill->xSize()-1;
            y2= (y2<(toFill->ySize()-1))?y2:toFill->ySize()-1;

            if(toFill->at(x,y)==NULL){
                copyMidPointValue(x,y,source,toFill);
            }
            if(quadLenght>0){
                if(toFill->at(x2,y)==NULL){
                    copyMidPointValue(x2,y,source,toFill);
                    completeXLine(x,x2,y,toFill);
                }
                if(toFill->at(x,y2)==NULL){
                    copyMidPointValue(x,y2,source,toFill);
                    completeYLine(y,y2,x,toFill);
                }
                copyMidPointValue(x2,y2,source,toFill);
                completeXLine(x,x2,y2,toFill);
                completeYLine(y,y2,x2,toFill);

                completeQuad(x,x2,y,y2,toFill);

            }
        }
    }
}

void MidPointModelSampler::copyMidPointValue(unsigned x,unsigned y,Matrix<Vertex>* source,Matrix<Vertex>* toFill){

    unsigned xMin = xStart + unsigned(floor(((x+0.0)/(toFill->xSize()-1))*(lenght-1)));
    unsigned xMax = xStart + unsigned(ceil(((x+0.0)/(toFill->xSize()-1))*(lenght-1)));
    unsigned yMin = yStart + unsigned(floor(((y+0.0)/(toFill->ySize()-1))*(lenght-1)));
    unsigned yMax = yStart + unsigned(ceil(((y+0.0)/(toFill->ySize()-1))*(lenght-1)));

    double zValue= (source->at(xMin,yMin)->z()+source->at(xMin,yMax)->z()
                    +source->at(xMax,yMin)->z()+source->at(xMax,yMax)->z())/4;

    if(zValue<minZ)
        minZ=zValue;

    toFill->put(x,y,new Vertex(x,y,zValue));
    iterationCompleted();
}



void MidPointModelSampler::completeXLine(unsigned x1,unsigned x2,unsigned fixed,Matrix<Vertex>* toFill){
    unsigned m = (x1+x2)/2;
    if(m>x1){
        double distance=(toFill->at(x2,fixed)->z()-toFill->at(x1,fixed)->z());

        double daux = deviator->getDeviation(m,fixed);
        double deviation=(distance*daux)/2;

        double z =(toFill->at(x1,fixed)->z()+toFill->at(x2,fixed)->z())/2+deviation;
        if(z<minZ)
            minZ=z;

        toFill->put(m,fixed,new Vertex(m,fixed,z));
        iterationCompleted();
        completeXLine(x1,m,fixed,toFill);
        completeXLine(m,x2,fixed,toFill);
    }
}

void MidPointModelSampler::completeYLine(unsigned y1,unsigned y2,unsigned fixed,Matrix<Vertex>* toFill){
    unsigned m = (y1+y2)/2;
    if(m>y1){
        double distance=(toFill->at(fixed,y2)->z()-toFill->at(fixed,y1)->z());

        double daux =deviator->getDeviation(fixed,m);
        double deviation=(distance*daux)/2;

        double z =(toFill->at(fixed,y1)->z()+toFill->at(fixed,y2)->z())/2+deviation;
        if(z<minZ)
            minZ=z;

        toFill->put(fixed,m,new Vertex(fixed,m,z));
        iterationCompleted();
        completeYLine(y1,m,fixed,toFill);
        completeYLine(m,y2,fixed,toFill);
    }
}

void MidPointModelSampler::completeQuad(unsigned x1,unsigned x2,unsigned y1,unsigned y2,Matrix<Vertex>* source){

    if(((x1+2)>x2)&&(y1+2)>y2)
        return;

    unsigned m = (x1+x2)/2;
    unsigned n = (y1+y2)/2;

    double zMax = source->at(x1,y1)->z();
    zMax = (zMax>source->at(x2,y1)->z())?zMax:source->at(x2,y1)->z();
    zMax = (zMax>source->at(x1,y2)->z())?zMax:source->at(x1,y2)->z();
    zMax = (zMax>source->at(x2,y2)->z())?zMax:source->at(x2,y2)->z();
    double zMin = source->at(x1,y1)->z();
    zMin = (zMin<source->at(x2,y1)->z())?zMin:source->at(x2,y1)->z();
    zMin = (zMin<source->at(x1,y2)->z())?zMin:source->at(x1,y2)->z();
    zMin = (zMin<source->at(x2,y2)->z())?zMin:source->at(x2,y2)->z();

    double distance=(zMax-zMin);
    double daux =deviator->getDeviation(m,n);
    double deviation=(distance*daux)/2;


    double z = ((zMin+zMax)/2) +deviation;
    if(z<minZ)
        minZ=z;

    source->put(m,n,new Vertex(m,n,z));
    iterationCompleted();

    completeXLine(x1,m,n,source);
    completeXLine(m,x2,n,source);
    completeYLine(y1,n,m,source);
    completeYLine(n,y2,m,source);

    completeQuad(x1,m,y1,n,source);
    completeQuad(x1,m,n,y2,source);
    completeQuad(m,x2,y1,n,source);
    completeQuad(m,x2,n,y2,source);

}
