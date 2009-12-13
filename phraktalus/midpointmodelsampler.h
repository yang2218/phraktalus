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

#ifndef MIDPOINTMODELSAMPLER_H
#define MIDPOINTMODELSAMPLER_H
#include "progresscontrolablethread.h"
#include "vertex.h"
#include "positionaldeviation.h"
#include "matrix.h"
#include <math.h>

class MidPointModelSampler : public ProgressControlableThread
{
    PositionalDeviation* deviator;
    double minZ;
    double _minDeviation;
    unsigned xStart;
    unsigned yStart;
    unsigned lenght;
    Matrix<Vertex> *source, *toFill;

    void copyMidPointValue(unsigned,unsigned,Matrix<Vertex>*,Matrix<Vertex>*);
    void completeXLine(unsigned,unsigned,unsigned,Matrix<Vertex>*);
    void completeYLine(unsigned,unsigned,unsigned,Matrix<Vertex>*);
    void completeQuad(unsigned,unsigned,unsigned,unsigned,Matrix<Vertex>*);
public:
    MidPointModelSampler(PositionalDeviation* dev,
                         Matrix<Vertex>* source,
                         unsigned sourceXOffset,
                         unsigned sourceYOffset,
                         unsigned l,
                         unsigned n,
                         QObject *parent = 0);
    void setMinDeviation(double d);
    double getSubModel(Matrix<Vertex>* &r);
    void run();
};

#endif // MIDPOINTMODELSAMPLER_H
