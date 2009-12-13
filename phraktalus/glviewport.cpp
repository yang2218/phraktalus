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

#include "glviewport.h"

#include <QMouseEvent>
#include <QAction>
#include <QApplication>
#include <math.h>
#include <QDebug>


//Coordinates
#define COORD_RANGE 1.0
#define COORD_ZDISTANCE 2.0

//Colors
#define COL_SKYBOX (_?QColor(50,10,10,255):QColor(10,10,50,255))
#define COL_GRID Qt::white
#define COL_AXIS Qt::blue

#define COL_SUR_TERRAIN_UNDEF (_?QColor(30, 0, 0, 100):QColor(255, 255, 255, 255))
#define COL_SUR_TERRAIN_TOP (_?QColor(30, 0, 0, 150):QColor(255, 255, 255, 255))
#define COL_SUR_TERRAIN_BOTTOM (_?QColor(255, 0, 0, 200):QColor(20, 70, 5, 255))
#define COL_SUR_WATER_TOP (_?QColor(255, 170, 0, 200):QColor(0,150,230,255))
#define COL_SUR_WATER_BOTTOM (_?QColor(40, 0, 0, 150):QColor(0,0,10,255))
#define COL_SUR_WATER_UNDEF (_?QColor(40, 0, 0, 100):QColor(0,0,0,255))

#define MAX_EARTH_HEIGHT 8848.0 //Monte everest
#define MIN_EARTH_HEIGHT 11034.0 //Fosa de las marianas

//Projection
#define PROJ_FOVY 45.0
#define PROJ_ASPECT GLfloat(this->width())/this->height()
#define PROJ_ZNEAR 1.0
#define PROJ_ZFAR 1000.0

//Zoom
#define ZOOM 1.0

//Default camera params
#define CAM_ROTX -50.0
//#define CAM_ROTY 0.0
#define CAM_ROTZ 10.0
#define CAM_POSX 0.5
#define CAM_POSY 0.5

//GL Lists
#define LISTS_NUMBER 4
#define LIST_HORIZONTAL_GRID glListsOffset
#define LIST_VERTICAL_GRID glListsOffset+1
#define LIST_SURFACE glListsOffset+2
#define LIST_WIREFRAME glListsOffset+3

GLViewPort::GLViewPort(QWidget *parent) :
        QGLWidget(parent),
        resolution(0),
        magnify(1.0),
        sizeS(NULL),
        heightSmin(NULL),
        heightSmax(NULL),
        lati(NULL),
        longi(NULL),
        rotationX(CAM_ROTX),
        rotationZ(CAM_ROTZ),
        positionX(CAM_POSX),
        positionY(CAM_POSY),
        zoom(ZOOM),
        glListsOffset(0),
        showHorGrid(true),
        showVerGrid(true),
        showSurf(true),
        zoomMode(false),
        xZoom(0.0),
        yZoom(0.0),
        sZoom(0.0),
        showLabs(true),
        _(QApplication::arguments().last() == "comin")

{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    setCursor(Qt::OpenHandCursor);
}

QSize GLViewPort::minimumSizeHint() const
{
    return QSize(300, 300);
}

QSize GLViewPort::sizeHint() const
{
    return QSize(600, 600);
}

void GLViewPort::setModel(ElevationModel *model)
{
    if (model == NULL){
        if (sizeS != NULL) {
            delete sizeS;
            sizeS = NULL;
        }
        if (heightSmin != NULL) {
            delete heightSmin;
            heightSmin = NULL;
        }
        if (heightSmax != NULL) {
            delete heightSmax;
            heightSmax = NULL;
        }
        if (lati != NULL) {
            delete lati;
            lati = NULL;
        }
        if (longi != NULL) {
            delete longi;
            longi = NULL;
        }

        return;
    }
    resolution = model->xResolution();
    if (sizeS == NULL)
        sizeS = new double(model->xSize());
    else
        *sizeS = model->xSize();
    if (heightSmin == NULL)
        heightSmin = new double(model->zOffset());
    else
        *heightSmin = model->zOffset();
    if (heightSmax == NULL)
        heightSmax = new double(model->xSize()*model->zSize()+model->zOffset());
    else
        *heightSmax = model->xSize()*model->zSize()+model->zOffset();
    if (lati == NULL)
        lati = new double(model->yOffset());
    else
        *lati = model->yOffset();
    if (longi == NULL)
        longi = new double(model->xOffset());
    else
        *longi = model->xOffset();

    double size = model->xSize();
    double offset = model->zOffset();
    vector<Strip*> *strips = model->getStrips();
    //Colors calculation
    vector<vector<QColor> > colors;
    for (unsigned i=0; i<strips->size(); i++)
    {
        vector<Vertex*> *curr = strips->at(i)->v;
        vector<QColor> currCol;
        for(unsigned j=0; j<curr->size(); j++)
        {
            currCol.push_back(colorByHeight(curr->at(j)->z()*size+offset));
        }
        colors.push_back(currCol);
    }
    vector<vector<Vertex> > normals;
    for (unsigned i=0; i<strips->size(); i++)
    {
        vector<Vertex*> *curr = strips->at(i)->v;
        bool clockwise = strips->at(i)->cw;
        vector<Vertex> currNor;
        Vertex *normal;
        for (unsigned i=2; i<curr->size(); i++)
        {
            if (!clockwise)
                normal = Vertex::crossProduct(curr->at(i), curr->at(i-1), curr->at(i-2));
            else
                normal = Vertex::crossProduct(curr->at(i), curr->at(i-2), curr->at(i-1));
            normal->normalize();
            currNor.push_back(*normal);
            delete normal;
            clockwise = !clockwise;
        }
        normals.push_back(currNor);
    }
    //Build Surface
    glNewList(LIST_SURFACE, GL_COMPILE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    makeTerrain(model, strips, colors, normals);
    glEndList();
    //Build Wireframe
    glNewList(LIST_WIREFRAME, GL_COMPILE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    makeTerrain(model, strips, colors, normals);
    glEndList();
    updateGL();
    for (unsigned i=0; i<strips->size(); i++)
    {
        vector<Vertex*> *curr = strips->at(i)->v;
        for(unsigned j=0; j<curr->size(); j++)
        {
            delete curr->at(j);
        }
        delete curr;
    }
    delete strips;
}

void GLViewPort::makeTerrain(ElevationModel *m, vector<Strip*> *s,vector<vector<QColor> > &c, vector<vector<Vertex> > &n)
{
    double zHalfRange = m->zSize() / 2;
    for (unsigned i=0; i<s->size(); i++)
    {
        vector<Vertex*> *curr = s->at(i)->v;
        glBegin(GL_TRIANGLE_STRIP);
        glNormal3d(n[i][0].x(), n[i][0].y(), n[i][0].z());
        qglColor(c[i][0]);
        glVertex3d(curr->at(0)->x()*2.0-1.0, curr->at(0)->y()*2.0-1.0, curr->at(0)->z()-zHalfRange);
        qglColor(c[i][1]);
        glVertex3d(curr->at(1)->x()*2.0-1.0, curr->at(1)->y()*2.0-1.0, curr->at(1)->z()-zHalfRange);
        qglColor(c[i][2]);
        glVertex3d(curr->at(2)->x()*2.0-1.0, curr->at(2)->y()*2.0-1.0, curr->at(2)->z()-zHalfRange);
        for (unsigned j=3; j<curr->size(); j++) {
            glNormal3d(n[i][j-2].x(), n[i][j-2].y(), n[i][j-2].z());
            qglColor(c[i][j]);
            glVertex3d(curr->at(j)->x()*2.0-1.0, curr->at(j)->y()*2.0-1.0, curr->at(j)->z()-zHalfRange);
        }
        glEnd();
    }
}

void GLViewPort::initializeGL()
{
    qglClearColor(COL_SKYBOX);
    glShadeModel(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glListsOffset = glGenLists(LISTS_NUMBER);
    loadProjection();
    makeHorizontalGrid();
    makeVerticalGrid();
}

void GLViewPort::setLighting(bool e)
{
    if (e)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
    updateGL();
}

void GLViewPort::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    loadProjection();
}

void GLViewPort::loadProjection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(PROJ_FOVY, PROJ_ASPECT, PROJ_ZNEAR, PROJ_ZFAR);
    glMatrixMode(GL_MODELVIEW);
}

void GLViewPort::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    loadCoordinates();
    if (zoom <= 1.0)
    {
        {
            float magicNumber = max(0.5+magnify/2000.0, 0.5);
            float intensity[] = {magicNumber, magicNumber, magicNumber, 1.0};
            glLightfv(GL_LIGHT0, GL_AMBIENT, intensity);
        }
        {
            float magicNumber = zoom/2.0;
            float intensity[] = {magicNumber, magicNumber, magicNumber, 1.0};
            glLightfv(GL_LIGHT0, GL_DIFFUSE, intensity);
        }
    } else {
        {
            float magicNumber = zoom/20.0+0.5;
            float intensity[] = {magicNumber, magicNumber, magicNumber, 1.0};
            glLightfv(GL_LIGHT0, GL_AMBIENT, intensity);
        }
        {
            float magicNumber = max(0.5+magnify/2000.0, 0.5);
            float intensity[] = {magicNumber, magicNumber, magicNumber, 1.0};
            glLightfv(GL_LIGHT0, GL_DIFFUSE, intensity);
        }
    }

    if (showLabs)
        drawLabels();
    if (showHorGrid)
        glCallList(LIST_HORIZONTAL_GRID);
    if (showVerGrid)
        glCallList(LIST_VERTICAL_GRID);
    glScalef(1.0,1.0,magnify);
    if (showSurf)
        glCallList(LIST_SURFACE);
    else
        glCallList(LIST_WIREFRAME);
    if (zoomMode)
        drawZoomRegion();
}

void GLViewPort::drawLabels()
{
    qglColor(Qt::white);
    QFont font("Sans", 12, 4);
    if (sizeS != NULL)
        renderText(-1.0, 1.5, 0.0, QString("Length: ") + QString::number(*sizeS) + QString(" m"), font);
    if (heightSmin != NULL)
        renderText(1.5, -1.0, -0.15, QString("Minimum height: ") + QString::number(*heightSmin) + QString(" m.a.s.l."), font);
    if (heightSmax != NULL)
        renderText(1.5, -1.0, 0.15, QString("Maximum height: ") + QString::number(*heightSmax) + QString(" m.a.s.l."), font);

    renderText(-2.0, 0.0, 0.0, "West" + (longi!=NULL?" :: "+QString::number(*longi)+" °":QString()), font);
    renderText(2.0, 0.0, 0.0, "East", font);
    renderText(0.0, -2.0, 0.0, "South"  + (lati!=NULL?" :: "+QString::number(*lati)+" °":QString()), font);
    renderText(0.0, 2.0, 0.0, "North", font);
    renderText(0.0, 0.0, 2.0, "Sky", font);
    renderText(0.0, 0.0, -2.0, "Ocean", font);
}

#define ZR_COLBASE (_?qglColor(Qt::green):qglColor(QColor(255,255,0,200)))
#define ZR_COLGLOW (_?qglColor(QColor(0,255,0,0)):qglColor(QColor(255,255,0,0)))
#define ZR_TOP(x,y) ZR_COLGLOW;glVertex3d(double(xZoom+(x==0?0:sZoom))/50-1.0,double(yZoom+(y==0?0:sZoom))/50-1.0,1.0);
#define ZR_BASE(x,y) ZR_COLBASE;glVertex3d(double(xZoom+(x==0?0:sZoom))/50-1.0,double(yZoom+(y==0?0:sZoom))/50-1.0,0.0);
#define ZR_BOT(x,y) ZR_COLGLOW;glVertex3d(double(xZoom+(x==0?0:sZoom))/50-1.0,double(yZoom+(y==0?0:sZoom))/50-1.0,-1.0);

void GLViewPort::drawZoomRegion()
{
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);
    glBegin(GL_QUADS);
    //Caras internas
    ZR_BASE(0,1);
    ZR_TOP(0,1);
    ZR_TOP(0,0);
    ZR_BASE(0,0);
    ZR_BOT(0,1);
    ZR_BASE(0,1);
    ZR_BASE(0,0);
    ZR_BOT(0,0);

    ZR_BASE(1,1);
    ZR_TOP(1,1);
    ZR_TOP(0,1);
    ZR_BASE(0,1);
    ZR_BOT(1,1);
    ZR_BASE(1,1);
    ZR_BASE(0,1);
    ZR_BOT(0,1);

    ZR_BASE(1,0);
    ZR_TOP(1,0);
    ZR_TOP(1,1);
    ZR_BASE(1,1);
    ZR_BOT(1,0);
    ZR_BASE(1,0);
    ZR_BASE(1,1);
    ZR_BOT(1,1);

    ZR_BASE(0,0);
    ZR_TOP(0,0);
    ZR_TOP(1,0);
    ZR_BASE(1,0);
    ZR_BOT(0,0);
    ZR_BASE(0,0);
    ZR_BASE(1,0);
    ZR_BOT(1,0);

    //Caras externas

    ZR_BASE(0,0);
    ZR_TOP(0,0);
    ZR_TOP(0,1);
    ZR_BASE(0,1);
    ZR_BOT(0,0);
    ZR_BASE(0,0);
    ZR_BASE(0,1);
    ZR_BOT(0,1);

    ZR_BASE(0,1);
    ZR_TOP(0,1);
    ZR_TOP(1,1);
    ZR_BASE(1,1);
    ZR_BOT(0,1);
    ZR_BASE(0,1);
    ZR_BASE(1,1);
    ZR_BOT(1,1);

    ZR_BASE(1,1);
    ZR_TOP(1,1);
    ZR_TOP(1,0);
    ZR_BASE(1,0);
    ZR_BOT(1,1);
    ZR_BASE(1,1);
    ZR_BASE(1,0);
    ZR_BOT(1,0);

    ZR_BASE(1,0);
    ZR_TOP(1,0);
    ZR_TOP(0,0);
    ZR_BASE(0,0);
    ZR_BOT(1,0);
    ZR_BASE(1,0);
    ZR_BASE(0,0);
    ZR_BOT(0,0);
    glEnd();
    glDisable(GL_CULL_FACE);
}

void GLViewPort::loadCoordinates()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(COORD_RANGE * (-1.0 + 2.0 * positionX) * zoom, COORD_RANGE * (-1.0 + 2.0 * positionY) * zoom, -COORD_ZDISTANCE);
    glRotatef(rotationX, 1.0, 0.0, 0.0);
    glRotatef(rotationZ, 0.0, 0.0, 1.0);
    glScalef(zoom, zoom, zoom);
}

void GLViewPort::makeHorizontalGrid()
{
    glNewList(LIST_HORIZONTAL_GRID, GL_COMPILE);
    qglColor(COL_GRID);
    glBegin(GL_LINES);
    for (unsigned i = 0; i<gridPrecision; i++)
    {
        GLdouble pos = COORD_RANGE * (-1.0 + 2.0 * GLdouble(i)/(gridPrecision - 1));
        glVertex3d(pos, -COORD_RANGE, 0.0);
        glVertex3d(pos, COORD_RANGE, 0.0);
        glVertex3d(-COORD_RANGE, pos, 0.0);
        glVertex3d(COORD_RANGE, pos, 0.0);
    }
    glEnd();
    glEndList();
}

void GLViewPort::makeVerticalGrid()
{
    glNewList(LIST_VERTICAL_GRID, GL_COMPILE);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    qglColor(Qt::white);
    glBegin(GL_QUAD_STRIP);
    for (unsigned i = 0; i<gridPrecision; i++)
    {
        GLdouble pos = COORD_RANGE * (-1.0 + 2.0 * GLdouble(i)/(gridPrecision - 1));
        glVertex3d(COORD_RANGE, COORD_RANGE, pos);
        glVertex3d(COORD_RANGE, -COORD_RANGE, pos);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for (unsigned i = 0; i<gridPrecision; i++)
    {
        GLdouble pos = COORD_RANGE * (-1.0 + 2.0 * GLdouble(i)/(gridPrecision - 1));
        glVertex3d(-COORD_RANGE, -COORD_RANGE, pos);
        glVertex3d(-COORD_RANGE, COORD_RANGE, pos);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for (unsigned i = 0; i<gridPrecision; i++)
    {
        GLdouble pos = COORD_RANGE * (-1.0 + 2.0 * GLdouble(i)/(gridPrecision - 1));
        glVertex3d(-COORD_RANGE, COORD_RANGE, pos);
        glVertex3d(COORD_RANGE, COORD_RANGE, pos);
    }
    glEnd();
    glBegin(GL_QUAD_STRIP);
    for (unsigned i = 0; i<gridPrecision; i++)
    {
        GLdouble pos = COORD_RANGE * (-1.0 + 2.0 * GLdouble(i)/(gridPrecision - 1));
        glVertex3d(COORD_RANGE, -COORD_RANGE, pos);
        glVertex3d(-COORD_RANGE, -COORD_RANGE, pos);
    }
    glEnd();
    glDisable(GL_CULL_FACE);
    glEndList();
}

void GLViewPort::window2modelCoords(unsigned x, unsigned y, GLdouble &posX, GLdouble &posY)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble z, auxX, auxY, posZ;
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
    gluProject(0.0, 0.0, 0.0, modelview, projection, viewport, &auxX, &auxY, &z);
    gluUnProject(GLdouble(x), GLdouble(viewport[3]) - GLdouble(y), z, modelview, projection, viewport, &posX, &posY, &posZ);
    if (posX < -COORD_RANGE) posX = -COORD_RANGE;
    if (posY < -COORD_RANGE) posY = -COORD_RANGE;
    if (posX >  COORD_RANGE) posX =  COORD_RANGE;
    if (posY >  COORD_RANGE) posY =  COORD_RANGE;
}

float toPercent(GLdouble x)
{
    if (x<-1.0) return 0;
    if (x> 1.0) return 100;
    return float(x*50+50);
}

template <typename T> T abs(T f)
{
    return (f<0?f*-1.0:f);
};

void GLViewPort::mousePressEvent(QMouseEvent *event)
{
    if (zoomMode && event->buttons() & Qt::LeftButton) {
        GLdouble x, y;
        window2modelCoords(event->x(), event->y(), x, y);
        float xP = toPercent(x);
        float yP = toPercent(y);
        if (xP >= xZoom && xP <= xZoom + sZoom && yP >= yZoom && yP <= yZoom + sZoom && (!(x == -1.0 || x == 1.0 || y == 1.0 || y == -1.0))) {
            setCursor(Qt::OpenHandCursor);
            emit doZoom(unsigned(xZoom), unsigned(yZoom), unsigned(sZoom));
            return;
        }
        zoomLastPos = event->pos();
        sZoom = 0.0;
        updateGL();
    }
    if (event->buttons() & Qt::MidButton)
        resetCamera();
    else
        lastPos = event->pos();
    setCursor(Qt::ClosedHandCursor);
}

void GLViewPort::mouseReleaseEvent(QMouseEvent *event)
{
    setCursor(Qt::OpenHandCursor);
}

void GLViewPort::wheelEvent(QWheelEvent *event)
{
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        magnify += float(event->delta()) / 300;
        if (magnify < 1.0)
            magnify = 1.0;
    }
    else
    {
        zoom += float(event->delta()) / 1200;
        if (zoom < 0.1)
            zoom = 0.1;
        else if (zoom > 10.0)
            zoom = 10.0;
    }
    updateGL();
}

void GLViewPort::mouseMoveEvent(QMouseEvent *event)
{
    if (zoomMode){
        GLdouble x, y;
        window2modelCoords(event->x(), event->y(), x, y);
        float xP = toPercent(x);
        float yP = toPercent(y);
        if (xP >= xZoom && xP <= xZoom + sZoom && yP >= yZoom && yP <= yZoom + sZoom && (!(x == -1.0 || x == 1.0 || y == 1.0 || y == -1.0))) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::CrossCursor);
        }
    }
    GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
    GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();

    if (event->buttons() & Qt::LeftButton) {
        if (zoomMode)
        {
            GLdouble x, y, xLP, yLP;
            window2modelCoords(event->x(), event->y(), x, y);
            window2modelCoords(zoomLastPos.x(), zoomLastPos.y(), xLP, yLP);
            GLdouble size;
            if (abs(xLP-x) > abs(yLP-y)) {
                size = abs(xLP-x);
                y = (yLP>y ? yLP-size : yLP + size);
            } else {
                size = abs(yLP-y);
                x = (xLP>x ? xLP-size : xLP + size);
            }
            if (x > 1.0){
                GLdouble remain = x - 1.0;
                x = 1.0;
                y = (yLP>y ? y+remain : y-remain);
            }
            if (x <-1.0){
                GLdouble remain = -1.0*(x + 1.0);
                x = -1.0;
                y = (yLP>y ? y+remain : y-remain);
            }
            if (y > 1.0){
                GLdouble remain = y - 1.0;
                y = 1.0;
                x = (xLP>x ? x+remain : x-remain);
            }
            if (y <-1.0){
                GLdouble remain = -1.0*(y + 1.0);
                y = -1.0;
                x = (xLP>x ? x+remain : x-remain);
            }
            xZoom = min(toPercent(x), toPercent(xLP));
            yZoom = min(toPercent(y), toPercent(yLP));
            if (unsigned(sZoom = abs(toPercent(x) - toPercent(xLP))) <= 1) sZoom = 1.0;

        } else {
            rotationX += 180 * dy;
            rotationZ += 180 * dx;
            while (rotationX > 180.0) rotationX -= 360.0;
            while (rotationX <-180.0) rotationX += 360.0;
            while (rotationZ > 180.0) rotationZ -= 360.0;
            while (rotationZ <-180.0) rotationZ += 360.0;
        }
    }
    else if (event->buttons() & Qt::RightButton)
    {
        positionX += dx;
        positionY -= dy;
        if (positionX >COORD_RANGE)
            positionX = COORD_RANGE;
        else if (positionX < 0.0)
            positionX = 0.0;
        if (positionY > COORD_RANGE)
            positionY = COORD_RANGE;
        else if (positionY < 0.0)
            positionY = 0.0;
    }
    updateGL();
    lastPos = event->pos();
}

void GLViewPort::zoomModeEnable(bool e)
{
    const unsigned frames = 70;
    zoomMode = e;
    if (e) {
        setMouseTracking(true);
        auxPosX = positionX;
        double deltaPosX = (positionX - 0.5) / double(frames);
        auxPosY = positionY;
        double deltaPosY = (positionY - 0.5) / double(frames);
        auxRotX = rotationX;
        double deltaRotX = rotationX / double(frames);
        auxRotZ = rotationZ;
        double deltaRotZ = rotationZ / double(frames);
        auxZoom = zoom;
        double deltaZoom = (zoom - 0.5) / double(frames);
        for (unsigned i = 0; i<frames; i++)
        {
            positionX -= deltaPosX;
            positionY -= deltaPosY;
            rotationX -= deltaRotX;
            rotationZ -= deltaRotZ;
            zoom -= deltaZoom;
            updateGL();
        }
        positionX = 0.5;
        positionY = 0.5;
        rotationX = 0.0;
        rotationZ = 0.0;
        zoom = 0.5;
    } else {
        setMouseTracking(false);
        double deltaPosX = (positionX - auxPosX) / double(frames);
        double deltaPosY = (positionY - auxPosY) / double(frames);
        double deltaRotX = (rotationX - auxRotX) / double(frames);
        double deltaRotZ = (rotationZ - auxRotZ) / double(frames);
        double deltaZoom = (zoom - auxZoom) / double(frames);
        for (unsigned i = 0; i<frames; i++)
        {
            positionX -= deltaPosX;
            positionY -= deltaPosY;
            rotationX -= deltaRotX;
            rotationZ -= deltaRotZ;
            zoom -= deltaZoom;
            updateGL();
        }
        positionX = auxPosX;
        positionY = auxPosY;
        rotationX = auxRotX;
        rotationZ = auxRotZ;
        zoom = auxZoom;
    }
    updateGL();
}

void GLViewPort::resetCamera()
{
    if (zoomMode) {
        rotationX = 0.0;
        rotationZ = 0.0;
        positionX = 0.5;
        positionY = 0.5;
        zoom = 0.5;
    } else {
        rotationX = CAM_ROTX;
        rotationZ = CAM_ROTZ;
        positionX = CAM_POSX;
        positionY = CAM_POSY;
        zoom = ZOOM;
    }
    updateGL();
}

void GLViewPort::showHorizontalGrid(bool state)
{
    showHorGrid = state;
    updateGL();
}

void GLViewPort::showVerticalGrid(bool state)
{
    showVerGrid = state;
    updateGL();
}

void GLViewPort::showSurface(bool state)
{
    showSurf = state;
    updateGL();
}

void GLViewPort::showLabels(bool state)
{
    showLabs = state;
    updateGL();
}

void GLViewPort::setGridPrecision(int p)
{
    gridPrecision = unsigned(pow(2.0, p+2)) + 1;
    makeHorizontalGrid();
    makeVerticalGrid();
    updateGL();
}

QColor GLViewPort::colorByHeight(double height) const
{
    QColor extreme, middle;
    if (height <= 0.0) {
        height /= MIN_EARTH_HEIGHT;
        if (height < -1.0)
            return COL_SUR_WATER_UNDEF;
        extreme = COL_SUR_WATER_BOTTOM;
        middle = COL_SUR_WATER_TOP;
        height *= -1.0;
    }
    else
    {
        height /= MAX_EARTH_HEIGHT;
        if (height > 1.0)
            return COL_SUR_TERRAIN_UNDEF;
        extreme = COL_SUR_TERRAIN_TOP;
        middle = COL_SUR_TERRAIN_BOTTOM;
    }
    QColor output;
    struct {
        int h, s, v, a;
    } m,e;
    middle.getHsv(&(m.h), &(m.s), &(m.v), &(m.a));
    extreme.getHsv(&(e.h), &(e.s), &(e.v), &(e.a));
    output.setHsv(
            int(height * e.h + (1-height) * m.h),
            int(height * e.s + (1-height) * m.s),
            int(height * e.v + (1-height) * m.v),
            int(height * e.a + (1-height) * m.a));
    return output;
}
