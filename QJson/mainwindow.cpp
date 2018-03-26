#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QJsonObject json;
    json.insert("Type",QString("Rectangle"));
    json.insert("widght",42);
    json.insert("height",23);

    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    qDebug()<<byteArray;

    QJsonParseError jsonError;
    QJsonDocument parseDoc = QJsonDocument::fromJson(byteArray,&jsonError);
    if(jsonError.error == QJsonParseError::NoError)
    {
        if(parseDoc.isObject())
        {
            QJsonObject jsonObj = parseDoc.object();
            if(jsonObj.contains("Type"))
            {
                QJsonValue typeValue = jsonObj.take("Type");
                if(typeValue.isString())
                {
                    QString strValue= typeValue.toString();
                    qDebug()<<"Type : "<<strValue;
                }
            }
            if(jsonObj.contains("height"))
            {
                QJsonValue heightValue = jsonObj.take("height");
                if(heightValue.isDouble())
                {
                    int iValue = heightValue.toVariant().toInt();
                    qDebug()<<"height : "<<iValue;
                }
            }
            if(jsonObj.contains("widght"))
            {
                QJsonValue widghtValue = jsonObj.take("widght");
                if(widghtValue.isDouble())
                {
                    int iValue =widghtValue.toVariant().toInt();
                    qDebug()<<"widght : "<<iValue;
                }
            }
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
