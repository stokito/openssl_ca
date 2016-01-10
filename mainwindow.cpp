#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    bits=512;
    ui->setupUi(this);
    setFixedSize(722,481);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//证书请求文件按钮事件
void MainWindow::on_pushButton_clicked()
{
    //执行请求文件
    careq();
    //写入message
    showMessage();
}

//选择待验证证书
void MainWindow::on_pushButton_7_clicked()
{
    verify.userCerUrl = QFileDialog::getOpenFileName(this,"select file","./",NULL);
    Load_Cer();
    message += getTime() + "rootCert loaded ...\n";
    message += noTime() + "userCert loaded ...\n";
    message += noTime() + "PrivateKey loaded ...\n";
    showMessage();
    certInfo info;
    QString tmpstr_0=GetCertSubjectString(&info);
    if(!tmpstr_0.isNull())
    {
        ui->lineEdit_22->setText(info.client);
        ui->lineEdit_23->setText(info.country);
        ui->lineEdit_24->setText(info.state);
        ui->lineEdit_25->setText(info.location);
        ui->lineEdit_26->setText(info.organization);
        ui->lineEdit_27->setText(info.organizationalUnitName);
        ui->lineEdit_28->setText(info.email);
        message += getTime() + "Certificate Detail:";
        QString tmpstr_1 = GetCertSerialNumber();
        if(!tmpstr_1.isNull())
        {
            message += "\n" + noTime() + "SerialNumber: ";
            message += tmpstr_1;
        }
        message += tmpstr_0;
        message += "\n";
    }
    showMessage();
}

//验证证书
void MainWindow::on_pushButton_8_clicked()
{
    if (verify.userCerUrl==NULL)
    {
        QMessageBox::warning(this,"警告","请选择证书！","确定");
        return;
    }
    else
    {
        if(CheckCertWithRoot())
        {
            message += getTime() + "Verify with ca, ok ...\n";
        }
        else
        {
            QMessageBox::warning(this,"警告","不受根证书信任的证书！","确定");
            message+= noTime()+ "Verify with ca, false ...\n";
        }
        if(CheckCertTime())
        {
            message+= noTime() + "Verify certificate life time, ok ...\n";
        }
        else
        {
            QMessageBox::warning(this,"警告","证书过期！","确定");
            message+= noTime() + "Verify certificate life time, false ...\n";
        }
    }
    showMessage();
}

// (～￣▽￣)→))*￣▽￣*)o主要用来签名
void MainWindow::on_pushButton_2_clicked()
{
    int serial;     //证书编号
    int day;        //申请天数
    char name1[100];//申请文件名
    char name2[100];//签发证书名
    char name3[100];//子证书私钥
    strcpy(name1,(ui->lineEdit->text()+".csr").toStdString().c_str());
    strcpy(name2,(ui->lineEdit->text()+".crt").toStdString().c_str());
    strcpy(name3,(ui->lineEdit->text()+".key").toStdString().c_str());
    day = ui->lineEdit_8->text().toInt();

    ifstream infile;
    infile.open("sign.txt");
    if(!infile)
    {
        QMessageBox::information(NULL,"error","open this file failed\n");
    }
    else
    {
        infile >> serial;
        infile.close();
    }

    if(CreateCertFromRequestFile(serial,day,name1,name2,name3,3))
    {
        ofstream outfile;
        outfile.open("sign.txt");
        serial += 1;
        outfile << serial;
        outfile.close();
        message += getTime() + "signature success\n";
        showMessage();
    }
    else
    {
        message+="signature failed\n";
        showMessage();
    }

    //测试输出
    //detail();
}

QString MainWindow::getTime()
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("[hh:mm:ss]  ");
    return current_date;
}
QString MainWindow::noTime()
{
    return "[        ]  ";
}
