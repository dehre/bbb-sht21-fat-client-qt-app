#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}, ui{new Ui::MainWindow}, networkManager{new QNetworkAccessManager()}
{
    ui->setupUi(this);
    ui->statusbar->setStyleSheet("background-color: white; color: black; border-top: 1px solid grey");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete networkManager;
}

void MainWindow::on_button_fetch_clicked()
{
    networkManager->get(QNetworkRequest(QUrl("http://localhost:8080/")));
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *networkReply) {
        if (networkReply->error())
        {
            ui->statusbar->showMessage(QString{"Network Error: "}.append(networkReply->errorString()));
            return;
        }

        const std::variant<JsonData, JsonError> reply{parseJson(networkReply->readAll())};
        if (std::holds_alternative<JsonError>(reply))
        {
            ui->statusbar->showMessage(std::get<JsonError>(reply));
            return;
        }

        JsonData data{std::get<JsonData>(reply)};
        ui->statusbar->showMessage("");
        ui->lcd_temperature->display(data.temperature);
        ui->lcd_humidity->display(data.humidity);
        ui->label_lastupdate->setText(QString("Last update: ").append(QDateTime::currentDateTime().toString()));
        networkManager->disconnect();
    });
}

std::variant<MainWindow::JsonData, MainWindow::JsonError> MainWindow::parseJson(const QByteArray &byteArray)
{
    const QJsonDocument jsonDoc{QJsonDocument::fromJson(byteArray)};
    if (!jsonDoc.isObject())
    {
        return QString{"Received unexpected JSON data"};
    }
    QJsonObject jsonObj{jsonDoc.object()};
    if (jsonObj.contains("error"))
    {
        return QString{"Sensor Error: "}.append(jsonObj["error"].toString());
    }
    const QJsonObject jsonData{jsonObj["data"].toObject()};
    const double temperature{jsonData["temperature"].toDouble()};
    const double humidity{jsonData["humidity"].toDouble()};
    return JsonData{.temperature = temperature, .humidity = humidity};
};
