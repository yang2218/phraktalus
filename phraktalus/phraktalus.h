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

#ifndef PHRAKTALUS_H
#define PHRAKTALUS_H

#include <QtGui/QMainWindow>

namespace Ui
{
    class Phraktalus;
}

class GLViewPort;
class ModelStacker;
class QSpinBox;
class QLabel;
class ElevationModel;

class Phraktalus : public QMainWindow
{
    Q_OBJECT

public:
    Phraktalus(QWidget *parent = 0);
    ~Phraktalus();

private:
    QString lastDir;
    Ui::Phraktalus *ui;
    GLViewPort *viewport;
    unsigned resolution;
    ModelStacker *ms;
    QSpinBox *dots;
    QLabel *statusLabel;

private slots:
    void onTopChanged(ElevationModel*);
    void on_action_Next_triggered();
    void on_action_Previous_triggered();
    void on_action_New_triggered();
    void zoomHandle(unsigned, unsigned, unsigned);
    void on_action_About_triggered();
    void on_action_Open_triggered();
};

#endif // PHRAKTALUS_H
