#ifndef MAINWINDOWSTRIPPER_H
#define MAINWINDOWSTRIPPER_H

#include <QMainWindow>

namespace Ui {
    class MainWindowStripper;
}

class MainWindowStripper : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowStripper(QWidget *parent = 0);
    ~MainWindowStripper();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private slots:
    void on_openFileButton_clicked();
    void on_clear_clicked();

private:
    Ui::MainWindowStripper *ui;
    void process(QStringList fileNameList);
};

#endif // MAINWINDOWSTRIPPER_H
