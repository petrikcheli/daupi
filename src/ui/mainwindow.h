#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void call(const std::string& ip, const std::string& port);

private slots:
    void on_connect_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
