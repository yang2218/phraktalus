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

#include "vertex.h"


void Vertex::normalize()
{
    Vertex *origin = new Vertex(0.0,0.0,0.0);
    double norm = Vertex::euclideanDistance(origin, this);
    delete origin;
    _x /= norm;
    _y /= norm;
    _z /= norm;
}

double Vertex::x()const{return _x;}

double Vertex::y()const{return _y;}

double Vertex::z()const{return _z;}

void Vertex::setX(double d) { _x = d; }

void Vertex::setY(double d) { _y = d; }

void Vertex::setZ(double d) { _z = d; }

Vertex& Vertex::operator =(const Vertex& v){
    if(this ==&v) return *this;
    _x=v.x();
    _y=v.y();
    _z=v.z();
    return *this;
}

Vertex::Vertex(const Vertex& v){
    _x=v.x();
    _y=v.y();
    _z=v.z();
}

Vertex::Vertex(double x,double y,double z){
    _x=x;
    _y=y;
    _z=z;
}

Vertex::~Vertex(){}
