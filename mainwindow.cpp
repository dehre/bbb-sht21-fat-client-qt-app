#include "mainwindow.hpp"
#include "ui_mainwindow.h"
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
        ui->statusbar->showMessage(reply->readAll());
        networkManager->disconnect();
    });
}
