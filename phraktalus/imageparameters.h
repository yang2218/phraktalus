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

#ifndef IMAGEPARAMETERS_H
#define IMAGEPARAMETERS_H

#include <QtGui/QDialog>
#include <QPixmap>

namespace Ui {
    class ImageParameters;
}

class ImageParameters : public QDialog {
    Q_OBJECT
public:
    ImageParameters(QPixmap &img, QWidget *parent = 0);
    ~ImageParameters();
    void getValues(Qt::AspectRatioMode &mode, double &min, double &max);

private:
    QPixmap preview;
    Qt::AspectRatioMode mode;
    Ui::ImageParameters *ui;

private slots:
    void on_maxHeight_valueChanged(int );
    void on_minHeight_valueChanged(int );
    void on_criteria_currentIndexChanged(QString );
};

#endif // IMAGEPARAMETERS_H
