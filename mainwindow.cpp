#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SystemParams.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionSaveSVG,	 SIGNAL(triggered()), this, SLOT(SaveSVG()));
    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int )),    this, SLOT(ItemClicked(QTreeWidgetItem*, int)));
    connect(ui->angleSpinBox, SIGNAL(valueChanged(int)),    this, SLOT(ParamsChanged()));
    connect(ui->wSpinBox, SIGNAL(valueChanged(int)),    this, SLOT(ParamsChanged()));
    connect(ui->hSpinBox, SIGNAL(valueChanged(int)),    this, SLOT(ParamsChanged()));
    connect(ui->rWidthSpinBox, SIGNAL(valueChanged(double)),    this, SLOT(ParamsChanged()));
    connect(ui->tilingCheckBox,	 SIGNAL(stateChanged(int)), this, SLOT(ParamsChanged()));

    //float radAngle = 32.0f * M_PI / 180.0;
    //SystemParams::rad_angle = radAngle;

    QVector3D colVec1 = SystemParams::star_color;
    QColor col1(colVec1.x() * 255.0f, colVec1.y() * 255.0f, colVec1.z() * 255.0f);
    ui->starColorLabel->setAutoFillBackground(true);
    QPalette palette1 = ui->starColorLabel->palette();
    palette1.setColor(ui->starColorLabel->backgroundRole(), col1);
    palette1.setColor(ui->starColorLabel->foregroundRole(), Qt::white);
    ui->starColorLabel->setPalette(palette1);

    QVector3D colVec2 = SystemParams::ribbon_color;
    QColor col2(colVec2.x() * 255.0f, colVec2.y() * 255.0f, colVec2.z() * 255.0f);
    ui->ribbonColorLabel->setAutoFillBackground(true);
    QPalette palette2 = ui->ribbonColorLabel->palette();
    palette2.setColor(ui->ribbonColorLabel->backgroundRole(), col2);
    palette2.setColor(ui->ribbonColorLabel->foregroundRole(), Qt::white);
    ui->ribbonColorLabel->setPalette(palette2);

    QVector3D colVec3 = SystemParams::background_color;
    QColor col3(colVec3.x() * 255.0f, colVec3.y() * 255.0f, colVec3.z() * 255.0f);
    ui->backgroundColorLabel->setAutoFillBackground(true);
    QPalette palette3 = ui->backgroundColorLabel->palette();
    palette3.setColor(ui->backgroundColorLabel->backgroundRole(), col3);
    palette3.setColor(ui->backgroundColorLabel->foregroundRole(), Qt::white);
    ui->backgroundColorLabel->setPalette(palette3);

    QVector3D colVec4 = SystemParams::interlacing_color;
    QColor col4(colVec4.x() * 255.0f, colVec4.y() * 255.0f, colVec4.z() * 255.0f);
    ui->interlacingColorLabel->setAutoFillBackground(true);
    QPalette palette4 = ui->interlacingColorLabel->palette();
    palette4.setColor(ui->interlacingColorLabel->backgroundRole(), col4);
    palette4.setColor(ui->interlacingColorLabel->foregroundRole(), Qt::white);
    ui->interlacingColorLabel->setPalette(palette4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SaveSVG()
{
    //std::cout << "save SVG\n";
    ui->widget->GetGLWidget()->SaveToSvg();
}

void MainWindow::ParamsChanged()
{
    float degAngle = ui->angleSpinBox->value();
    float radAngle = degAngle * M_PI / 180.0;
    SystemParams::rad_angle = radAngle;
    SystemParams::ribbon_width = ui->rWidthSpinBox->value();
    SystemParams::w = ui->wSpinBox->value();
    SystemParams::h = ui->hSpinBox->value();
    SystemParams::show_tiling = ui->tilingCheckBox->isChecked();

    std::string tilingName;
    if(ui->treeWidget->selectedItems().size() > 0)
    {
        QTreeWidgetItem* item = ui->treeWidget->selectedItems()[0];
        tilingName = item->text(0).toStdString();
    }
    else
    {
        tilingName = SystemParams::default_tiling;
    }

    ui->widget->GetGLWidget()->GeneratePattern(tilingName);
    ui->widget->GetGLWidget()->repaint();
}

void MainWindow::ItemClicked(QTreeWidgetItem* item, int column)
{
    if(item)
    {
        std::string tilingName = item->text(column).toStdString();

        if(item->childCount() == 0)
        {
            ui->widget->GetGLWidget()->GeneratePattern(tilingName);
            ui->widget->GetGLWidget()->repaint();
            //std::cout << item->text(column).toStdString() << "\n";
        }
    }
}
