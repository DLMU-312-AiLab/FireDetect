#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPixmap>
#include <QFileDialog>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QUrl>
#include <QDebug>
#include <QTime>
#include <QThread>
#include <QMovie>

#include "smart_fire_detect.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QString filePath;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void updateLabel();
    void detect(QString);

public slots:
    void on_action_open_triggered();
    void on_checkButton_clicked();
    void slot_updatelabel(QString msg, QString file);

protected:
    void dragEnterEvent(QDragEnterEvent*event);//拖动进入事件
    void dropEvent(QDropEvent*event);

private slots:
    void on_action_clear_triggered();

private:
    Ui::MainWindow *ui;
};


class DetectThread : public QThread
{
    Q_OBJECT
public:
    QString file;
    void run() Q_DECL_OVERRIDE
    {
        QString result = "未见火焰";

        string filename = this->file.toStdString();
        smart_fire_detect detect(filename.c_str(), filename.length());
        Mat bin_img = detect.get_binary_img();
        if(detect.is_fire_flag){
            result = "发现火情";
        }
        imwrite("tmp.jpg", bin_img);
        emit resultReady(result, "tmp.jpg");
    }

signals:
    void resultReady(const QString &msg, const QString &file);
};

#endif // MAINWINDOW_H
