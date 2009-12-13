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

#ifndef PROGRESSCONTROLLER_H
#define PROGRESSCONTROLLER_H

#include <QtGui/QDialog>

namespace Ui {
    class ProgressController;
}

class ProgressControlableThread;

class ProgressController : public QDialog {

    Q_OBJECT

public:
    ProgressController(ProgressControlableThread *thread,
                       QWidget *parent = 0,
                       QString title = "Please wait...",
                       QString text = "Operation in progress...");
    virtual ~ProgressController();

private:
    Ui::ProgressController *ui;
    ProgressControlableThread *thread;
private slots:
    void on_cancelButton_clicked();
};

#endif // PROGRESSCONTROLLER_H
