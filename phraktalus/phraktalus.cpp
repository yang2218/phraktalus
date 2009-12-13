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

 #define PHRAKTALUS_VERSION " 0.8rc"
 
#include "phraktalus.h"
#include "ui_phraktalus.h"
#include "glviewport.h"
#include "modelstacker.h"
#include "elevationmodel.h"
#include "elevationmodelfactory.h"
#include "operationcancelledexception.h"
#include "fileformatexception.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QLabel>

#define VISIBLEDOTS 2
#define GRIDPRECISION 3

Phraktalus::Phraktalus(QWidget *parent) :
        QMainWindow(parent),
        lastDir(QString()),
        ui(new Ui::Phraktalus),
        viewport(new GLViewPort(this)),
        ms(new ModelStacker(this)),
        dots(new QSpinBox(this)),
        statusLabel(new QLabel("Welcome to Phraktalus"))
{
    //Sets up the premade ui
    ui->setupUi(this);
    setWindowTitle(windowTitle());
    setStatusBar(new QStatusBar(this));
    statusBar()->addWidget(statusLabel);
    statusBar()->show();
    //Populate the menuBar
    ui->menuBar->addAction(ui->action_About);
    //Populate toolBars
    //File menu
    ui->toolBar->addAction(ui->action_New);
    ui->toolBar->addAction(ui->action_Open);
    ui->toolBar->addSeparator();
    //View Menu
    QSpinBox *gridPrecision = new QSpinBox(this);
    gridPrecision->setMinimum(1);
    gridPrecision->setMaximum(5);
    gridPrecision->setToolTip("Adjusts Grid precision");
    ui->toolBar->addWidget(gridPrecision);

    QAction* showGrids = new QAction(QIcon(":/icons/rule.png"), "Show Grids", this);
    showGrids->setCheckable(true);
    showGrids->setChecked(true);
    showGrids->setMenu(ui->menu_Grids);
    ui->toolBar->addAction(showGrids);

    ui->toolBar->addAction(ui->action_Show_Labels);
    ui->toolBar->addAction(ui->action_Surface);
    ui->toolBar->addAction(ui->action_Lighting);
    ui->toolBar->addAction(ui->action_Reset_Camera);
    ui->toolBar->addSeparator();
    //Process Menu
    dots->setMinimum(1);
    dots->setMaximum(4);
    dots->setToolTip("Adjusts number of visible dots");
    ui->toolBar->addWidget(dots);

    ui->toolBar->addAction(ui->action_Previous);
    connect(ms, SIGNAL(hasPrevious(bool)), ui->action_Previous, SLOT(setEnabled(bool)));
    ui->toolBar->addAction(ui->action_Next);
    connect(ms, SIGNAL(hasNext(bool)), ui->action_Next, SLOT(setEnabled(bool)));
    ui->toolBar->addAction(ui->action_Zoom);
    connect(ms, SIGNAL(isEmpty(bool)), ui->action_Zoom, SLOT(setDisabled(bool)));
    ms->emitSignals();

    ui->toolBar->setVisible(true);
    //Set main viewport
    setCentralWidget(viewport);
    //Connect actions relative to GL
    connect(ui->action_Lighting, SIGNAL(toggled(bool)), viewport, SLOT(setLighting(bool)));
    connect(ui->action_Reset_Camera, SIGNAL(triggered()), viewport, SLOT(resetCamera()));
    connect(ui->action_Horizontal, SIGNAL(toggled(bool)), viewport, SLOT(showHorizontalGrid(bool)));
    connect(ui->action_Vertical, SIGNAL(toggled(bool)), viewport, SLOT(showVerticalGrid(bool)));
    connect(ui->action_Surface, SIGNAL(toggled(bool)), viewport, SLOT(showSurface(bool)));
    connect(gridPrecision, SIGNAL(valueChanged(int)), viewport, SLOT(setGridPrecision(int)));
    connect(ui->action_Show_Labels, SIGNAL(toggled(bool)), viewport, SLOT(showLabels(bool)));
    connect(ms, SIGNAL(topChanged(ElevationModel*)), viewport, SLOT(setModel(ElevationModel*)));
    connect(ms, SIGNAL(topChanged(ElevationModel*)), this, SLOT(onTopChanged(ElevationModel*)));
    //Zoom region
    connect(ui->action_Zoom, SIGNAL(toggled(bool)), viewport, SLOT(zoomModeEnable(bool)));
    connect(viewport, SIGNAL(doZoom(uint,uint,uint)), this, SLOT(zoomHandle(uint, uint, uint)));
    connect(showGrids, SIGNAL(toggled(bool)), ui->action_Vertical, SLOT(setChecked(bool)));
    connect(showGrids, SIGNAL(toggled(bool)), ui->action_Horizontal, SLOT(setChecked(bool)));

    gridPrecision->setValue(GRIDPRECISION);
    dots->setValue(VISIBLEDOTS);
}

Phraktalus::~Phraktalus()
{
    delete ui;
    delete viewport;
    delete ms;
}

void Phraktalus::on_action_Open_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, QString("Select the source file"), lastDir, "All supported files (*.hgt *.png *.jpg *.jpeg *.bmp *.xpm);;SRTM DEMs (*.hgt);;Pixmaps (*.png *.jpg *.jpeg *.bmp *.xpm)");
    if (filepath.isNull()) return;
    lastDir = filepath.left(filepath.lastIndexOf(QChar('/')));
    ElevationModel* oM = NULL;
    ElevationModel* newM = NULL;
    try {
        oM = ElevationModelFactory::build(filepath, this);
        newM = new ElevationModel(unsigned(pow(2.0, 6 + dots->value())),oM,0,0,100,this);
    } catch (FileFormatException e){
        if (oM != NULL) delete oM;
        if (newM != NULL) delete newM;
        QMessageBox::critical(this, "Error opening file", e.getError());
        return;
    } catch (OperationCancelledException e) {
        if (oM != NULL) delete oM;
        if (newM != NULL) delete newM;
        return;
    }
    if (oM != NULL) delete oM;
    ms->clear();
    ms->push(newM);
}

void Phraktalus::on_action_About_triggered()
{
    QMessageBox::about(this, QString("About Phraktalus") + QString(PHRAKTALUS_VERSION),
                       QString("Phraktalus fractal terrain generator and triangulator\n")+
                       QString("\n\n") +
                       QString("This program is \"Free Software\". You can use and redistribute it under the terms of the GNU General Public License version 3 or (at your option) any later version.\n\n") +
                       QString("Authors:\n") +
                       QString("Gabriel Orlando Lopez <glopez@alumnos.exa.unicen.edu.ar>\n") +
                       QString("Matias Javier Rossi <marossi@alumnos.exa.unicen.edu.ar>\n\n") +
                       QString("Phraktalus' logo is distributed under Creative Commons Atribution\nShare-Alike License. For more details see <http://creativecommons.org/licenses/by-sa/2.5/>\n\n") +
                       QString("The Tango Icon Theme used within this application has been created\nby the Tango Project. Please visit them at <http://tango.freedesktop.org/>"));
}

void Phraktalus::zoomHandle(unsigned x, unsigned y, unsigned s)
{
    ElevationModel *newM;
    try {
        newM = new ElevationModel(unsigned(pow(2.0, 6 + dots->value())), ms->top(), x, y, s, this);
    } catch (OperationCancelledException e) {return;}
    ms->push(newM);
    ui->action_Zoom->toggle();
}

void Phraktalus::on_action_New_triggered()
{
    ElevationModel* newM;
    try {
        newM = new ElevationModel(unsigned(pow(2.0, 6 + dots->value())), this);
    } catch (OperationCancelledException e) {return;}
    ms->clear();
    ms->push(newM);
}

void Phraktalus::on_action_Previous_triggered()
{
    ms->previous();
}

void Phraktalus::on_action_Next_triggered()
{
    ms->next();
}

void Phraktalus::onTopChanged(ElevationModel* em)
{
    if (em == NULL) {
        statusLabel->setText("Ready");
        return;
    }
    statusLabel->setText(QString("Showing ") + QString::number(unsigned(pow(float(em->xResolution()),2))) + " points (" +
                         QString::number(em->xResolution()) + "x" + QString::number(em->xResolution()) + QString(" matrix)"));
}
