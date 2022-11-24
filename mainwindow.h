#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void on_button_fetch_clicked();

  private:
    Ui::MainWindow *ui{nullptr};
    QNetworkAccessManager *networkManager{nullptr};

    struct JsonData
    {
        double temperature;
        double humidity;
    };
    using JsonError = QString;

    bool isValidUrl(const QString &);
    std::variant<JsonData, JsonError> parseJson(const QByteArray &);
};
#endif // MAINWINDOW_H
