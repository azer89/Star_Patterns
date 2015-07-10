#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int )),    this, SLOT(ItemClicked(QTreeWidgetItem*, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
