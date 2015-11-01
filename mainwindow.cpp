#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "SystemParams.h"

#include <QColorDialog>

QColor ToQColor(QVector3D col)
{
    return QColor(col.x() * 255.0f, col.y() * 255.0f, col.z() * 255.0f);
}

QColor InverseGrayScale(QColor col)
{
    // 0.2989, 0.5870, 0.1140
    // 0.30    0.59    0.11
    float intensity = 0.3f * ((float)col.red()) + 0.59f * ((float)col.green()) + 0.11f * ((float)col.blue());
    float invIntensity = 255.0 - intensity;

    if(invIntensity > 127.5)
        return QColor(255, 255, 255);

    return QColor(0, 0, 0);
}

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
    connect(ui->contactDeltaSpinBox, SIGNAL(valueChanged(double)),    this, SLOT(ParamsChanged()));
    connect(ui->rWidthSpinBox, SIGNAL(valueChanged(double)),    this, SLOT(ParamsChanged()));
    connect(ui->lineWidthSpinBox, SIGNAL(valueChanged(double)),    this, SLOT(ParamsChanged()));
    connect(ui->tilingCheckBox,	 SIGNAL(stateChanged(int)), this, SLOT(ParamsChanged()));

    connect(ui->starColorButton,	 SIGNAL(clicked()), this, SLOT(StarColorChanged()));
    connect(ui->ribbonColorButton,	 SIGNAL(clicked()), this, SLOT(RibbonColorChanged()));
    connect(ui->backgroundColorButton,	 SIGNAL(clicked()), this, SLOT(BackgroundColorChanged()));
    connect(ui->interlacingColorButton,	 SIGNAL(clicked()), this, SLOT(InterlacingColorChanged()));


    QColor col1 = ToQColor(SystemParams::star_color);
    ui->starColorButton->setAutoFillBackground(true);
    QPalette palette1 = ui->starColorButton->palette();
    palette1.setColor(ui->starColorButton->backgroundRole(), col1);
    //palette1.setColor(ui->starColorButton->foregroundRole(), QColor(255 - col1.red(), 255 - col1.green(), 255 - col1.blue()));
    palette1.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(col1));
    ui->starColorButton->setPalette(palette1);

    QColor col2 = ToQColor(SystemParams::ribbon_color);
    ui->ribbonColorButton->setAutoFillBackground(true);
    QPalette palette2 = ui->ribbonColorButton->palette();
    palette2.setColor(ui->ribbonColorButton->backgroundRole(), col2);
    //palette2.setColor(ui->ribbonColorButton->foregroundRole(), QColor(255 - col2.red(), 255 - col2.green(), 255 - col2.blue()));
    palette2.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(col2));
    ui->ribbonColorButton->setPalette(palette2);

    QColor col3 = ToQColor(SystemParams::background_color);
    ui->backgroundColorButton->setAutoFillBackground(true);
    QPalette palette3 = ui->backgroundColorButton->palette();
    palette3.setColor(ui->backgroundColorButton->backgroundRole(), col3);
    //palette3.setColor(ui->backgroundColorButton->foregroundRole(), QColor(255 - col3.red(), 255 - col3.green(), 255 - col3.blue()));
    palette3.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(col3));
    ui->backgroundColorButton->setPalette(palette3);

    QColor col4 = ToQColor(SystemParams::interlacing_color);
    ui->interlacingColorButton->setAutoFillBackground(true);
    QPalette palette4 = ui->interlacingColorButton->palette();
    palette4.setColor(ui->interlacingColorButton->backgroundRole(), col4);
    //palette4.setColor(ui->interlacingColorButton->foregroundRole(), QColor(255 - col4.red(), 255 - col4.green(), 255 - col4.blue()));
    palette1.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(col4));
    ui->interlacingColorButton->setPalette(palette4);
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

void MainWindow::StarColorChanged()
{
    //std::cout << "StarColorChanged\n";
    QColor col1 = ToQColor(SystemParams::star_color);
    QColor newColor = QColorDialog::getColor(col1, this);
    QPalette palette1 = ui->starColorButton->palette();
    palette1.setColor(ui->starColorButton->backgroundRole(), newColor);
    //palette1.setColor(ui->starColorButton->foregroundRole(), QColor(255 - newColor.red(), 255 - newColor.green(), 255 - newColor.blue()));
    palette1.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(newColor));
    ui->starColorButton->setPalette(palette1);

    //std::cout << newColor.redF() << " " << newColor.greenF() << " " << newColor.blueF() << "\n";
    SystemParams::star_color = QVector3D(newColor.redF(), newColor.greenF(), newColor.blueF());

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

void MainWindow::RibbonColorChanged()
{
    //std::cout << "RibbonColorChanged\n";
    QColor col2 = ToQColor(SystemParams::ribbon_color);
    QColor newColor = QColorDialog::getColor(col2, this);
    QPalette palette2 = ui->ribbonColorButton->palette();
    palette2.setColor(ui->ribbonColorButton->backgroundRole(), newColor);
    //palette2.setColor(ui->ribbonColorButton->foregroundRole(), QColor(255 - newColor.red(), 255 - newColor.green(), 255 - newColor.blue()));
    palette2.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(newColor));
    ui->ribbonColorButton->setPalette(palette2);

    SystemParams::ribbon_color = QVector3D(newColor.redF(), newColor.greenF(), newColor.blueF());

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

void MainWindow::BackgroundColorChanged()
{
    //std::cout << "BackgroundColorChanged\n";
    QColor col3 = ToQColor(SystemParams::background_color);
    QColor newColor = QColorDialog::getColor(col3, this);
    QPalette palette3 = ui->backgroundColorButton->palette();
    palette3.setColor(ui->backgroundColorButton->backgroundRole(), newColor);
    //palette3.setColor(ui->backgroundColorButton->foregroundRole(), QColor(255 - newColor.red(), 255 - newColor.green(), 255 - newColor.blue()));
    palette3.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(newColor));
    ui->backgroundColorButton->setPalette(palette3);

    SystemParams::background_color = QVector3D(newColor.redF(), newColor.greenF(), newColor.blueF());

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

void MainWindow::InterlacingColorChanged()
{
    //std::cout << "InterlacingColorChanged\n";
    QColor col4 = ToQColor(SystemParams::interlacing_color);
    QColor newColor = QColorDialog::getColor(col4, this);
    QPalette palette4 = ui->interlacingColorButton->palette();
    palette4.setColor(ui->interlacingColorButton->backgroundRole(), newColor);
    //palette4.setColor(ui->interlacingColorButton->foregroundRole(), QColor(255 - newColor.red(), 255 - newColor.green(), 255 - newColor.blue()));
    palette4.setColor(ui->starColorButton->foregroundRole(), InverseGrayScale(newColor));
    ui->interlacingColorButton->setPalette(palette4);

    SystemParams::interlacing_color = QVector3D(newColor.redF(), newColor.greenF(), newColor.blueF());

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

void MainWindow::ParamsChanged()
{
    float degAngle = ui->angleSpinBox->value();
    float radAngle = degAngle * M_PI / 180.0;
    SystemParams::rad_angle = radAngle;
    SystemParams::ribbon_width = ui->rWidthSpinBox->value();
    SystemParams::line_width = ui->lineWidthSpinBox->value();
    SystemParams::w = ui->wSpinBox->value();
    SystemParams::h = ui->hSpinBox->value();
    SystemParams::show_tiling = ui->tilingCheckBox->isChecked();
    SystemParams::contact_delta = ui->contactDeltaSpinBox->value();

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
