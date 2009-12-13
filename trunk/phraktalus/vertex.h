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

#ifndef VERTEX_H
#define VERTEX_H

#include <math.h>

class Vertex
{
protected:
    double _x,_y,_z;
public:

    static  double euclideanDistance2D(double x1,double y1,double x2,double y2){
        return sqrt(pow(x1 - x2,2) + pow(y1 - y2,2));
    }

    static  double euclideanDistance(Vertex *v,Vertex *u){
        return sqrt(pow(v->x() - u->x(),2) + pow(v->y() - u->y(),2) + pow(v->z() - u->z(),2));
    }

    static  Vertex* crossProduct(Vertex *a,Vertex *b,Vertex *c){
        double ax = b->x() - a->x(),
        ay = b->y() - a->y(),
        az = b->z() - a->z(),
        bx = c->x() - a->x(),
        by = c->y() - a->y(),
        bz = c->z() - a->z();
        return new Vertex(bz*ay-az*by,
                          bx*az-ax*bz,
                          ax*by-bx*ay);
    }

    static  double dotProduct(Vertex *a,Vertex *b){
        return (b->x()*a->x())+(b->y()*a->y())+(b->z()*a->z());
    }

    static  double crossProductNorm(Vertex *a,Vertex *b,Vertex *c){
        return ((b->x() - a->x())*(c->y() - a->y()) - (c->x() - a->x())*(b->y() - a->y()));
    }

    void normalize();
    static  double circumSphereRadious(Vertex *a,Vertex *b,Vertex *c){
        double dA = euclideanDistance(a,c);
        double dB = euclideanDistance(b,c);
        double dC = euclideanDistance(a,b);
        double d = sqrt((dA+dC-dB)*(dA+dB-dC)*(dA+dB+dC)*(dB+dC-dA));
        if(d==0)
            return d;
        return (dA*dB*dC)/d;
    }

    double x()const;

    double y()const;

    double z()const;

    void setX(double d);

    void setY(double d);

    void setZ(double d);

    Vertex& operator =(const Vertex& v);

    Vertex(const Vertex& v);

    Vertex(double x,double y,double z);

    ~Vertex();
};

#endif // VERTEX_H
