#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "projectormanager.h"

namespace Ui {
class MainWindow;
}

namespace slightshow
{

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ProjectorManager *const m_manager;
};

}

#endif // MAINWINDOW_H
