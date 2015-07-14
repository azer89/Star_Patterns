#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void ItemClicked(QTreeWidgetItem * item, int column);
    void ParamsChanged();
    void StarColorChanged();
    void RibbonColorChanged();
    void BackgroundColorChanged();
    void InterlacingColorChanged();
    void SaveSVG();
};

#endif // MAINWINDOW_H
