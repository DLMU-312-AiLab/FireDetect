#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_open_triggered()
{
    this->filePath = QFileDialog::getOpenFileName(this,"图片","","打开图片(*.jpg;*.png)");

    QPixmap qp(this->filePath);
    ui->imglabel->setPixmap(qp.scaled(ui->imglabel->width(), ui->imglabel->height()));//显示图片

    this->on_checkButton_clicked();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    QString ext = event->mimeData()->urls()[0].fileName().right(3).toLower();
   if(!ext.compare("jpg") || !ext.compare("jpeg") || !ext.compare("png"))
   {
       event->acceptProposedAction();
   }
   else
   {
       event->ignore();
   }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    this->filePath = event->mimeData()->urls()[0].toLocalFile();    
    QPixmap qp(this->filePath);
    ui->imglabel->setPixmap(qp.scaled(ui->imglabel->width(), ui->imglabel->height()));//显示图片            
    this->on_checkButton_clicked();
}

void MainWindow::on_checkButton_clicked()
{        
    QMovie* movie = new QMovie("processing.gif");
    ui->anslabel->setMovie(movie);
    movie->start();
    DetectThread *thread = new DetectThread();
    thread->file = this->filePath;
    connect(thread, SIGNAL(resultReady(QString,QString)), this, SLOT(slot_updatelabel(QString,QString)));
    thread->start();
}

void MainWindow::slot_updatelabel(QString msg, QString file)
{
    QPixmap qp(file);
    ui->reslabel->setPixmap(qp.scaled(ui->reslabel->width(), ui->reslabel->height()));//显示图片
    ui->anslabel->setText(msg);
}

void MainWindow::on_action_clear_triggered()
{
    ui->anslabel->clear();
    ui->anslabel->setText(QString("检测结果"));
    ui->imglabel->clear();
    ui->imglabel->setText(QString("拖拽至此"));
    ui->reslabel->clear();
    ui->reslabel->setText(QString("处理后效果"));
}
