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

#ifndef MATRIX_H
#define MATRIX_H

#include <cassert>

template<typename T> class Matrix
{
protected:
    T** _matrix;
    unsigned _xSize,_ySize;
public:
    T* at(unsigned x,unsigned y)const{
        assert((x<_xSize)&&(y<_ySize));
        return _matrix[x*_xSize + y];
    }

    void put(unsigned x,unsigned y,T* data){
        assert((x<_xSize)&&(y<_ySize));
        _matrix[x*_xSize + y]=data;
    }

    unsigned xSize()const{return _xSize;}

    unsigned ySize()const{return _ySize;}

    ~Matrix(){
        delete[] _matrix;
    }
    Matrix(unsigned xSize,unsigned ySize){
        _xSize=xSize;
        _ySize=ySize;
        _matrix=new T*[xSize*ySize];
        for (unsigned i = 0; i<_xSize*_ySize; i++)
            _matrix[i] = NULL;
    }
};

#endif // MATRIX_H
