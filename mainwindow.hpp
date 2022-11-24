#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

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
    void on_fetchButton_clicked();

  private:
    Ui::MainWindow *ui{nullptr};
    QNetworkAccessManager *networkManager{nullptr};

    struct JsonData
    {
        double temperature;
        double humidity;
    };
    using JsonError = QString;
    std::variant<JsonData, JsonError> parseJson(const QByteArray &);
};
#endif // MAINWINDOW_HPP
