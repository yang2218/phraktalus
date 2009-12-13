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

#ifndef TRIANGLE_H
#define TRIANGLE_H
#include "vertex.h"
#include <iostream>
using namespace std;
class Triangle
{
protected:
    Vertex *_a,*_b,*_c;
public:
    Vertex* a()const{return _a;}
    Vertex* b()const{return _b;}
    Vertex* c()const{return _c;}
    Triangle(Vertex *a,Vertex *b,Vertex *c){
        _a=a;
        _b=b;
        _c=c;
    }

    ~Triangle(){}

};

#endif // TRIANGLE_H
