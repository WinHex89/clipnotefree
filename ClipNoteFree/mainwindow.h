#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSettings>
#include <QNetworkInterface>
#include <QList>
#include <QFileDialog>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QKeySequenceEdit>
#include <QInputDialog>
#include <QProcess>
#include <QCoreApplication>
#include "QHotkey/qhotkey.h"
#include <QUrl>
#include <QRegularExpression>
#include <QThread>
#include <QClipboard>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <qt_windows.h>
#include <QCloseEvent>
#include <QStandardPaths>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
extern QSettings settings;
extern QNetworkAccessManager manager;
extern QString host;
extern QString localHost;
extern bool isVideoLink(QString);
extern bool isProcessRunning(QString);
extern QString httpGet(QString);
extern QString HttpGet(QString);
extern QString httpPost(QString,QByteArray);
extern void openFileOrUrl(QString);
extern int operation;
extern void simulateCtrlV();
extern QString getHostMacAddress();
extern QString PotPlayerPath();
extern QString baiduOcr(QString path);
extern QString oss(QString file);
extern QString addLink(QString url,QString time);
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QString version="V1.3";
    QWebSocketServer *server;
    QList<QWebSocket*> clients;
    QList<QHotkey*> keyList;
    QSystemTrayIcon* trayIcon;
    QMenu* trayMenu;
    QAction* actionShow;

    QAction* actionExit;

    QEventLoop loop;
    QString tempTime;

    QString download;


public:
    MainWindow(QWidget *parent = nullptr);
    void onNewConnection();
    void onTextMessageReceived(QString);
    void onClientDisconnected();
    void broadcastMessage(QString);

    ~MainWindow();
protected:
    bool nativeEvent(const QByteArray &eventType, void *message,long long *result)
    {
        Q_UNUSED(eventType);
        MSG *msg = static_cast<MSG *>(message);
        if (msg->message == WM_COPYDATA)
        {
            COPYDATASTRUCT *cds = reinterpret_cast<COPYDATASTRUCT *>(msg->lParam);
            if (cds->dwData == 0x6020)
            {
                const char *data = reinterpret_cast<const char *>(cds->lpData);
                int dataSize = cds->cbData;
                QString filename = QString::fromUtf8(data, dataSize);

                settings.setValue("url",filename.replace("\\","/"));

                *result = 1;
                return true;
            }
        }
        return QWidget::nativeEvent(eventType, message, result);
    }
    void closeEvent(QCloseEvent *event) override {
        event->ignore();
        this->hide();
    }
private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_9_clicked();


    void on_pushButton_11_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_12_clicked();




    void on_pushButton_5_clicked();


    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_lineEdit_4_textChanged(const QString &arg1);

    void on_lineEdit_5_textChanged(const QString &arg1);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_checkBox_1_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_checkBox_3_stateChanged(int arg1);

    void on_keySequenceEdit_1_editingFinished();

    void on_keySequenceEdit_2_editingFinished();

    void on_keySequenceEdit_3_editingFinished();

    void on_keySequenceEdit_4_editingFinished();

    void on_keySequenceEdit_5_editingFinished();

    void on_keySequenceEdit_6_editingFinished();

    void on_keySequenceEdit_7_editingFinished();

    void on_keySequenceEdit_8_editingFinished();

    void on_keySequenceEdit_9_editingFinished();

    void on_keySequenceEdit_10_editingFinished();

    void on_keySequenceEdit_11_editingFinished();

    void on_keySequenceEdit_12_editingFinished();

    void on_pushButton_16_clicked();

    void on_pushButton_15_clicked();

public:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
