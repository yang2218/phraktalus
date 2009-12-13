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

#ifndef ELEVATIONMODELFACTORY_H
#define ELEVATIONMODELFACTORY_H

#include "fileformatexception.h"
#include "operationcancelledexception.h"
#include "matrix.h"
#include "elevationmodel.h"
#include "imageparameters.h"
#include <QString>
#include <QPixmap>
#include <QWidget>
#include <QMessageBox>
#include <QRgb>

#define SRTMHGT_NODATA_VALUE -32768


class ElevationModelFactory
{
public:
    static ElevationModel* build(QString filename, QWidget* parent = 0) {
        QString ext(filename.mid(filename.lastIndexOf(QChar('.'))));
        QString name(filename.mid(filename.lastIndexOf(QChar('/'))+1));
        name = name.left(name.lastIndexOf(QChar('.')));
        Matrix<Vertex> *grid;
        double xOffset, yOffset, zOffset, xSize, ySize, zSize;

        //Handle multiple filetypes
        if (ext == ".hgt" || ext == ".HGT")
        {
            //Try to open file
            QString fnameFixed(filename);
            FILE * file = fopen(fnameFixed.replace(QChar('\\'),QString("\\\\")).toStdString().c_str(),"rb");
            if(file==NULL)
                throw FileFormatException("Can't open file: " + filename);

            //Check file size
            fseek (file , 0 , SEEK_END);
            int size = ftell (file);
            unsigned resolution;
            if(size==25934402)
                resolution=3601;
            else if(size==2884802)
                resolution=1201;
            else {
                fclose(file);
                throw FileFormatException("Invalid file size.");
            }
            rewind(file);

            //Try to parse filename
            if (!parseHGTName(name, xOffset, yOffset)) {
                if (QMessageBox::question(parent, "Warning",
                                          "This HGT filename doesn't match the standards.\nI can't be able to handle earth coordinates.\nContinue anyway?",
                                          QMessageBox::Yes | QMessageBox::No,
                                          QMessageBox::No) == QMessageBox::No) {
                    fclose(file);
                    throw OperationCancelledException();
                } else {
                    xOffset = 0.0;
                    yOffset = 0.0;
                }
            }
            xSize = ONE_DEGREE_METERS;
            ySize = ONE_DEGREE_METERS;


            fillGridHGT(file, grid, resolution, zSize, zOffset);
        }

        else if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".xpm" || ext == ".gif" ||
                 ext == ".JPG" || ext == ".JPEG" || ext == ".PNG" || ext == ".BMP" || ext == ".XPM" || ext == ".GIF") {
            QPixmap img(filename);
            if (img.isNull()) throw FileFormatException("Can't open file: " + filename + "\nUnknown format.");
            xOffset = 0.0;
            yOffset = 0.0;
            xSize = ONE_DEGREE_METERS;
            ySize = ONE_DEGREE_METERS;

            fillGridPM(img, grid, zSize, zOffset);
        }
        else
            throw FileFormatException("Unknown file extension: " + ext +
                                      "\n(Hint: Try renaming the file to the extension that matches the format)");

        return new ElevationModel(grid, xOffset, yOffset, zOffset, xSize, ySize, zSize, parent);
    }
private:
    static void fillGridHGT(FILE* file, Matrix<Vertex>* &grid, unsigned resolution, double &maxdistance, double &min){
        grid = new Matrix<Vertex>(resolution, resolution);
        double height;
        short byte;
        unsigned char b,a;
        double minZ = 100000.0;
        double maxZ = -100000.0;
        vector<Vertex*> voids;

        //Data reading
        for(unsigned x=0;x<resolution;x++){
            for(unsigned y=0;y<resolution;y++){

                //16 signed meters above sea level
                fread(&a,1,1,file);
                fread(&b,1,1,file);

                byte=((short)a)*256 + b;

                if(byte==SRTMHGT_NODATA_VALUE){
                    voids.push_back(new Vertex(resolution-x-1,y,0));
                    grid->put(resolution-x-1,y,NULL);
                }else{
                    height = (double)byte;
                    if(minZ>height)
                        minZ=height;
                    if(maxZ<height)
                        maxZ=height;

                    grid->put(resolution-x-1,y,new Vertex(resolution-x-1,y,height));
                }
            }
        }

        //Void Filling
        for(unsigned i=0;i<voids.size();i++){
            Vertex * v = voids.at(i);
            unsigned samples = 0;
            double sum = 0.0;

            int xAux,yAux;
            xAux = int(v->x())-1;
            yAux = int(v->y());
            while((xAux>=0)&&(grid->at(unsigned(xAux),unsigned(yAux))==NULL))
                xAux--;
            if((xAux>=0)&&(grid->at(unsigned(xAux),unsigned(yAux))!=NULL)){
                samples++;
                sum +=grid->at(unsigned(xAux),unsigned(yAux))->z();
            }

            xAux = int(v->x())+1;
            yAux = int(v->y());
            while((xAux<signed(resolution))&&(grid->at(unsigned(xAux),unsigned(yAux))==NULL))
                xAux++;
            if((xAux<signed(resolution))&&(grid->at(unsigned(xAux),unsigned(yAux))!=NULL)){
                samples++;
                sum +=grid->at(unsigned(xAux),unsigned(yAux))->z();
            }

            xAux = int(v->x());
            yAux = int(v->y())-1;
            while((yAux>=0)&&(grid->at(unsigned(xAux),unsigned(yAux))==NULL))
                yAux--;
            if((yAux>=0)&&(grid->at(unsigned(xAux),unsigned(yAux))!=NULL)){
                samples++;
                sum +=grid->at(unsigned(xAux),unsigned(yAux))->z();
            }

            xAux = int(v->x());
            yAux = int(v->y())+1;
            while((yAux<signed(resolution))&&(grid->at(unsigned(xAux),unsigned(yAux))==NULL))
                yAux++;
            if((yAux<signed(resolution))&&(grid->at(unsigned(xAux),unsigned(yAux))!=NULL)){
                samples++;
                sum +=grid->at(unsigned(xAux),unsigned(yAux))->z();
            }

            if(samples==0)
                sum=(minZ + maxZ)/2;
            else
                sum/=samples;

            v->setZ(sum);
            grid->put(unsigned(v->x()),unsigned(v->y()),v);
        }

        fclose (file);
        maxdistance = maxZ - minZ;
        min = minZ;
        for(unsigned x=0;x<resolution;x++)
            for(unsigned y=0;y<resolution;y++)
                grid->at(x,y)->setZ(grid->at(x,y)->z()-minZ);
    }

    static void fillGridPM(QPixmap &image, Matrix<Vertex>* &grid, double &maxdistance, double &_min){
        unsigned resolution = min(image.width(), image.height());
        Qt::AspectRatioMode mode;
        double infLimit, supLimit;

        ImageParameters ip(image);
        ip.exec();
        if (ip.result() != QDialog::Accepted)
            throw OperationCancelledException();
        ip.getValues(mode, infLimit, supLimit);

        QPixmap transformedPixmap(image.scaled(resolution, resolution, mode));
        QImage finalImage(transformedPixmap.toImage());
        grid = new Matrix<Vertex>(resolution, resolution);
        double zMin = 100000.0, zMax = -100000.0;
        for (unsigned i=0; i<resolution; i++)
            for (unsigned j=0; j<resolution; j++) {
                double curr = infLimit+(supLimit-infLimit)*(double(qGray(finalImage.pixel(i, j)))/255.0);
                if (curr < zMin) zMin = curr;
                if (curr > zMax) zMax = curr;
                grid->put(i, resolution-j-1, new Vertex(double(i), double(resolution-j-1), curr));
            }
        maxdistance = zMax - zMin;
        _min = zMin;
        for (unsigned i=0; i<resolution; i++)
            for (unsigned j=0; j<resolution; j++)
                grid->at(i, j)->setZ(grid->at(i, j)->z() - _min);
    }

    static bool parseHGTName(QString filename, double &xOffset, double &yOffset) {
        if (filename.size() != 7) return false;
        if (filename.at(0) != QChar('N') && filename.at(0) != QChar('S')) return false;
        if (filename.at(3) != QChar('E') && filename.at(3) != QChar('W')) return false;
        QString latitudeS(filename.mid(1, 2)), longitudeS(filename.mid(4, 3));
        bool ok;
        yOffset = latitudeS.toDouble(&ok)*(filename.at(0) == QChar('N')?1.0:-1.0);
        xOffset = longitudeS.toDouble(&ok)*(filename.at(3) == QChar('E')?1.0:-1.0);
        return ok;
    }
};

#endif // ELEVATIONMODELFACTORY_H
