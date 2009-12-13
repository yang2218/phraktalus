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

#ifndef ELEVATIONMODEL_H
#define ELEVATIONMODEL_H

#define ONE_DEGREE_METERS 111319.4555;

#include <QObject>
#include "bintreetriangulation.h"
#include "midpointmodelsampler.h"
#include "matrix.h"
#include "vertex.h"

class ElevationModel : public QObject
{
    Q_OBJECT

    protected:
        double _xOffset,_yOffset,_zOffset,_xSize,_ySize,_zSize;
        BinTreeTriangulation* triangulation;
        Matrix<Vertex>* _grid;
        void makeSample(MidPointModelSampler* s);
        void makeTriangles();
    public:
        ElevationModel(Matrix<Vertex>* grid, double xOffset,double yOffset,double zOffset,double xSize,double ySize,double zSize, QWidget *parent = 0);
        ElevationModel(unsigned n,ElevationModel * source,unsigned sourceXOffset,unsigned sourceYOffset,unsigned l,  QWidget *parent = 0);
        ElevationModel(unsigned n,  QWidget *parent = 0);
        ~ElevationModel();
        double xOffset();
        double xSize();
        double yOffset();
        double ySize();
        double zOffset();
        double zSize();
        unsigned xResolution();
        unsigned yResolution();
        ElevationModel * getSubModel(double,double,double);
        vector<Strip*>* getStrips();


};

#endif // ELEVATIONMODEL_H
