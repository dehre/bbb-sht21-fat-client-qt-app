#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <regex>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow{parent}, ui{new Ui::MainWindow{}}, networkManager{new QNetworkAccessManager{}}
{
    ui->setupUi(this);
    QObject::connect(ui->button_fetch, SIGNAL(clicked()), this, SLOT(on_button_fetch_clicked()));

    /* Qt Designer doesn't allow styling the status bar */
    ui->statusbar->setStyleSheet("background-color: white; color: red; font-style: italic; border-top: 1px solid grey");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete networkManager;
}

void MainWindow::on_button_fetch_clicked()
{
    const QString url{ui->lineedit_url->text()};
    if (!isValidUrl(url))
    {
        ui->statusbar->showMessage("Invalid URL Provided");
        return;
    }

    networkManager->get(QNetworkRequest{QUrl{url, QUrl::StrictMode}});
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

        const JsonData data{std::get<JsonData>(reply)};
        ui->statusbar->clearMessage();
        ui->lcd_temperature->display(data.temperature);
        ui->lcd_humidity->display(data.humidity);
        ui->label_lastupdate->setText(QString{"Last Update: "}.append(QDateTime::currentDateTime().toString()));

        networkManager->disconnect();
    });
}

bool MainWindow::isValidUrl(const QString &url)
{
    /* Qt's URL validation is quite weak even in strict mode, e.g. `somegarbage` is valid;
     * see: https://doc.qt.io/qt-6/qurl.html#isValid */
    return std::regex_match(url.toStdString(),
                            std::regex{"(http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\\x3f?([^ #]*)#?([^ ]*)"});
}

std::variant<MainWindow::JsonData, MainWindow::JsonError> MainWindow::parseJson(const QByteArray &byteArray)
{
    const QJsonDocument doc{QJsonDocument::fromJson(byteArray)};
    if (!doc.isObject())
    {
        return JsonError{"Received Unexpected JSON Data"};
    }

    const QJsonObject obj{doc.object()};
    if (obj.contains("error"))
    {
        return JsonError{"Sensor Error: "}.append(obj["error"].toString());
    }

    const QJsonObject data{obj["data"].toObject()};
    const double temperature{data["temperature"].toDouble()};
    const double humidity{data["humidity"].toDouble()};
    return JsonData{.temperature = temperature, .humidity = humidity};
};
