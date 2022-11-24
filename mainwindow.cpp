#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), networkManager(new QNetworkAccessManager())
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete networkManager;
}

void MainWindow::on_fetchButton_clicked()
{
    networkManager->get(QNetworkRequest(QUrl("http://localhost:8080/")));
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        if (reply->error())
        {
            ui->statusbar->showMessage(QString{"Network Error: "}.append(reply->errorString()));
            return;
        }
        // TODO LORIS: separate method to parse JSON
        const QJsonDocument jsonDoc{QJsonDocument::fromJson(reply->readAll())};
        if (!jsonDoc.isObject())
        {
            ui->statusbar->showMessage("Received invalid JSON data");
            return;
        }
        QJsonObject jsonObj{jsonDoc.object()};
        if (jsonObj.contains("error"))
        {
            ui->statusbar->showMessage(QString{"Sensor Error: "}.append(jsonObj["error"].toString()));
            return;
        }
        const QJsonObject jsonData{jsonObj["data"].toObject()};
        const double temperature{jsonData["temperature"].toDouble()};
        const double humidity{jsonData["humidity"].toDouble()};
        ui->statusbar->showMessage(QString{"Temperature: "}
                                       .append(QString::number(temperature))
                                       .append(" Humidity: ")
                                       .append(QString::number(humidity)));
        // TODO LORIS: end of JSON method
        networkManager->disconnect();
    });
}
