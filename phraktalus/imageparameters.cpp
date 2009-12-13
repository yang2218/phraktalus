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

#include "imageparameters.h"
#include "ui_imageparameters.h"

ImageParameters::ImageParameters(QPixmap &img, QWidget *parent) :
    QDialog(parent),
    preview(img),
    mode(Qt::IgnoreAspectRatio),
    ui(new Ui::ImageParameters)
{
    ui->setupUi(this);
    ui->previewImage->setPixmap(preview.scaled(ui->previewImage->size(),this->mode));
    ui->criteria->addItem("Expanded");
    ui->criteria->addItem("Tiled");
    ui->criteria->addItem("Cropped");
}

ImageParameters::~ImageParameters()
{
    delete ui;
}

void ImageParameters::on_criteria_currentIndexChanged(QString mode)
{
    if (mode == "Expanded") {
        this->mode = Qt::IgnoreAspectRatio;
    } else if (mode == "Tiled") {
        this->mode = Qt::KeepAspectRatio;
    } else if (mode == "Cropped") {
        this->mode = Qt::KeepAspectRatioByExpanding;
    }
    ui->previewImage->setPixmap(preview.scaled(ui->previewImage->size(),this->mode));
}

#define max(x,y) (x>y?x:y)
#define min(x,y) (x<y?x:y)

void ImageParameters::on_minHeight_valueChanged(int v)
{
    ui->maxHeight->setMinimum(max(v+1, -9999));
}

void ImageParameters::on_maxHeight_valueChanged(int v)
{
    ui->minHeight->setMaximum(min(v-1, 9999));
}

void ImageParameters::getValues(Qt::AspectRatioMode &mode, double &min, double &max)
{
    mode = this->mode;
    min = double(ui->minHeight->value());
    max = double(ui->maxHeight->value());
}
