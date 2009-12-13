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

#ifndef GLVIEWPORT_H
#define GLVIEWPORT_H

#include <QGLWidget>
#include <vector>
using namespace std;

#include "elevationmodel.h"
#include "vertex.h"

class GLViewPort : public QGLWidget
{
    Q_OBJECT

signals:
    void doZoom(unsigned, unsigned, unsigned);
public:
    GLViewPort(QWidget *parent = 0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
public slots:
    void setLighting(bool);
    void setModel(ElevationModel *);
    void resetCamera();
    void showVerticalGrid(bool);
    void showHorizontalGrid(bool);
    void showSurface(bool);
    void showLabels(bool);
    void setGridPrecision(int);
    void zoomModeEnable(bool);
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    unsigned resolution;
    float magnify;
    double *sizeS;
    double *heightSmin, *heightSmax;
    double *lati, *longi;
    GLfloat rotationX, auxRotX;
    GLfloat rotationZ, auxRotZ;
    GLfloat positionX, auxPosX;
    GLfloat positionY, auxPosY;
    GLfloat zoom, auxZoom;
    unsigned gridPrecision;
    QPoint lastPos, zoomLastPos;
    void drawLabels();
    void drawZoomRegion();
    void loadProjection();
    void loadCoordinates();
    void makeHorizontalGrid();
    void makeVerticalGrid();
    void window2modelCoords(unsigned x, unsigned y, GLdouble &posX, GLdouble &posY);
    QColor colorByHeight(double height) const;
    void makeTerrain(ElevationModel*, vector<Strip*> *s, vector<vector<QColor> >&, vector<vector<Vertex> >&);
    GLuint glListsOffset;
    bool showHorGrid;
    bool showVerGrid;
    bool showSurf;
    bool zoomMode;
    float xZoom, yZoom, sZoom;
    bool showLabs;
    bool _;
};

#endif // GLVIEWPORT_H
