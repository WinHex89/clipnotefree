#include "mainwindow.h"
#include "./ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    setWindowIcon(QIcon(":/img/logo.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setMaximumSize(size());
    setMinimumSize(size());
    server = new QWebSocketServer("ClipNote", QWebSocketServer::NonSecureMode, this);
    if (server->listen(QHostAddress::Any, 5488)) {
        connect(server, &QWebSocketServer::newConnection, this, &MainWindow::onNewConnection);
    }
    QList<QCheckBox*> checkboxes = ui->groupBox->findChildren<QCheckBox*>();
    foreach (QCheckBox *checkbox, checkboxes) {
        checkbox->setChecked(settings.value(checkbox->objectName()).toBool());
    }
    QList<QKeySequenceEdit*> keySequenceEdits = ui->groupBox_2->findChildren<QKeySequenceEdit*>();
    foreach (QKeySequenceEdit *keySequenceEdit, keySequenceEdits) {
        keySequenceEdit->setKeySequence(settings.value(keySequenceEdit->objectName()).toString());
    }
    ui->lineEdit_9->setText(settings.value("potplayer").toString());
    ui->lineEdit_10->setText(settings.value("snipaste").toString());
    ui->lineEdit_4->setText(settings.value("api_key").toString());
    ui->lineEdit_5->setText(settings.value("secret_key").toString());
    if(settings.value("link_style").isNull()){
        settings.setValue("link_style","[$time]($url)");
    }else{
        ui->lineEdit->setText(settings.value("link_style").toString());
    }

    trayIcon = new QSystemTrayIcon(QIcon(":/img/logo.png"), this);
    trayMenu = new QMenu();
    actionShow = new QAction("显示", this);

    actionExit = new QAction("退出", this);
    trayMenu->addAction(actionShow);
    trayMenu->addSeparator();

    trayMenu->addAction(actionExit);
     trayMenu->addSeparator();


    trayIcon->setContextMenu(trayMenu);
    connect(actionShow, &QAction::triggered, [&]() {
        show();
        activateWindow();
    });

    connect(actionExit, &QAction::triggered, [&]() {
        settings.remove("url");
        close();
        QCoreApplication::exit();
    });
    connect(trayIcon, &QSystemTrayIcon::messageClicked,[&]() {
        QDesktopServices::openUrl(QUrl(download));
    });
    connect(trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::DoubleClick) {
            show();
            activateWindow();
        }
    });

    trayIcon->show();
    download=HttpGet("Auth/getVersionFree?ver="+version);
    qDebug()<<download;
    if(!download.isEmpty()){
        trayIcon->showMessage("提示", "有新版本(请点击这里跳转到下载链接)",QSystemTrayIcon::Warning,5000);
    }

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString destinationPath = appDataPath + "/clipnote.db";
    if(!QDir(appDataPath).exists()){
         QDir().mkpath(appDataPath);
    }

    QString resourcePath = ":/db/clipnote.db";

    if (!QFile::exists(destinationPath)) {
        if (QFile::copy(resourcePath, destinationPath)) {
             QFile::setPermissions(destinationPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
        }
    }
}
void MainWindow::onNewConnection(){
    QWebSocket *client = server->nextPendingConnection();
    connect(client, &QWebSocket::textMessageReceived, this, &MainWindow::onTextMessageReceived);
    connect(client, &QWebSocket::disconnected, this, &MainWindow::onClientDisconnected);
    if (client) {
        clients.append(client);
    }
}
void MainWindow::onTextMessageReceived(QString message){
    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonObj=jsonDoc.object();
    jsonObj["url"]=jsonObj["url"].toString().replace("file:///","");
    if(jsonObj["action"].toString()=="pos"){
        settings.setValue("url", jsonObj["url"].toString());
    }


    if(operation==1&&jsonObj["action"].toString()=="pos"){
        double result = jsonObj["data"].toDouble();
        if((int)result==0){
            return;
        }
        QTime time(0, 0, 0, 0);
        time = time.addMSecs(int(result * 1000));
        QString formattedTime = time.toString("hh:mm:ss.zzz");
        QString encodedUrl = jsonObj["url"].toString();
        QString id=addLink(encodedUrl,formattedTime);
        QString link = settings.value("link_style").toString()
                           .replace("$time",formattedTime)
                           .replace("$url",id)
                           .replace("$name", QFileInfo(encodedUrl).baseName())
                           .replace("$filename", QFileInfo(encodedUrl).fileName())
                           .replace("$path", encodedUrl);
        if(settings.value("checkBox_1").toBool()){link.replace(QRegularExpression("(\\d\\d:\\d\\d:\\d\\d)\\.\\d\\d\\d"), "\\1");}

        QApplication::clipboard()->setText(link);
        simulateCtrlV();

    }
    if(jsonObj["action"].toString()=="seek"){

            if(clients.size()>1){
                broadcastMessage(message);

            }else{
                QUrl url(jsonObj["data"].toString());
                QDesktopServices::openUrl(url);
            }


    }
    if(operation==2&&jsonObj["action"].toString()=="snapshots"){
        if(jsonObj["data"].toString().startsWith("data")){
            QByteArray imageData = QByteArray::fromBase64(jsonObj["data"].toString().replace("data:image/png;base64,", "").toLocal8Bit());
            QImage image;
            bool loaded = image.loadFromData(imageData);

            if(loaded){
                    QApplication::clipboard()->setImage(image);
                    simulateCtrlV();

            }
        }
        operation=0;
    }
    if(operation==3&&jsonObj["action"].toString()=="snapshots"){

        if(jsonObj["data"].toString().startsWith("data")){
            QByteArray imageData = QByteArray::fromBase64(jsonObj["data"].toString().replace("data:image/png;base64,", "").toLocal8Bit());
            QImage image;
            bool loaded = image.loadFromData(imageData);
            if(loaded){
                QApplication::clipboard()->setImage(image);
                QString executable = settings.value("snipaste").toString();
                QStringList arguments;
                arguments << "paste" << "--clipboard";
                QProcess::startDetached(executable, arguments);
                QTimer::singleShot(1000, []() {
                    keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
                    keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
                });

            }}
        operation=0;
    }
    if(operation==6&&jsonObj["action"].toString()=="snapshots"){
        QByteArray imageData = QByteArray::fromBase64(jsonObj["data"].toString().replace("data:image/png;base64,", "").toLocal8Bit());
        QString img = QDir(QCoreApplication::applicationDirPath()).filePath(QUuid::createUuid().toString()+".png");
        QImage image;
        bool loaded = image.loadFromData(imageData);
        if(loaded){

            image.save(img);

            QString data=baiduOcr(img);
            QApplication::clipboard()->setText(data);
            simulateCtrlV();
            QFile::remove(img);

        }
        operation=0;
    }
    if(operation==4&&jsonObj["action"].toString()=="pos"){

        double result = jsonObj["data"].toDouble();
        QTime time(0, 0, 0, 0);
        time = time.addMSecs(int(result * 1000));
        tempTime = time.toString("hh:mm:ss.zzz");

        operation=0;

    }
    loop.quit();
}
void MainWindow::onClientDisconnected(){
    QWebSocket *client = qobject_cast<QWebSocket*>(sender());
    clients.removeOne(client);
    client->deleteLater();
}
void MainWindow::broadcastMessage(QString message){
    for (QWebSocket *client : clients) {
        if (client->state() == QAbstractSocket::ConnectedState) {
            client->sendTextMessage(message);
        }
    }
}



void MainWindow::on_pushButton_4_clicked()
{
    QUrl url("https://qm.qq.com/cgi-bin/qm/qr?k=Bvs2uQxc0rCDzn0Wklaw76wfUdxO1dV2&jump_from=webapi&authKey=vTkp0Nojj6owKfLwmPCRWSc2qdkUzP5FLmhU9SIdkSoR8pp4i/PinyGmh5rxfO8m");
    QDesktopServices::openUrl(url);

}


void MainWindow::on_pushButton_9_clicked()
{
    QUrl url(QString(host+"App/Buy?email=%1&password=%2").arg(settings.value("email").toString(),settings.value("password").toString()));
    QDesktopServices::openUrl(url);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete trayIcon;
    delete trayMenu;
    delete actionShow;
    delete actionExit;
}


void MainWindow::on_pushButton_11_clicked()
{
    QUrl url("https://www.yuque.com/zhangyewuxianxiaozhang/ghzwdl/qy049zf24bq7f05v");
    QDesktopServices::openUrl(url);
}


void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"打开视频文件",settings.value("history").toString(),"视频文件 (*.*)");
    if (!fileName.isEmpty()) {
        QString lastDirectory = QFileInfo(fileName).absolutePath();
        settings.setValue("history",lastDirectory);
        settings.setValue("url",fileName);
        QString program = settings.value("potplayer").toString();
        QString arguments =fileName;
        QProcess::startDetached(program, QStringList() << arguments);
    }
}
void MainWindow::on_pushButton_2_clicked()
{

    QString text = QInputDialog::getText(this, "请输入网址", "请输入网址http或https开头（链接必须是视频文件本身才能播放）(直链视频易失效！建议把视频下载到本地！)", QLineEdit::Normal, "");
    if (!text.isEmpty()) {
        if(!text.startsWith("http")){
            QMessageBox::critical(this, "错误", "链接格式错误", QMessageBox::Ok);
            return;
        }
        if(!isVideoLink(text)){
              QMessageBox::critical(this, "错误", "该链接不是视频直链无法播放（链接必须是视频文件本身才能播放）（请点击打开网页或安装浏览器扩展在浏览器看视频做笔记）", QMessageBox::Ok);
            return;
        }
        settings.setValue("url",text);
        QString program = settings.value("potplayer").toString();
        QProcess::startDetached(program, QStringList() <<text);
    }
}


void MainWindow::on_pushButton_12_clicked()
{
    QUrl url("https://www.yuque.com/zhangyewuxianxiaozhang/er8rsh/huntnk8a9me4iegn");
    QDesktopServices::openUrl(url);
}


void MainWindow::on_pushButton_5_clicked()
{
       QUrl url("https://cn.bing.com/?FORM=Z9FD1&rdr=1&rdrig=6572F3EDF1E94290AB51E76419FA8310");
       QDesktopServices::openUrl(url);
}


void MainWindow::on_pushButton_13_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "选择PotPlayer.exe",
                                                    "",
                                                    "可执行程序 (*.exe)");

    if (!fileName.isEmpty()) {
        if(!fileName.toLower().contains("potplayer")){
            QMessageBox::critical(this, "错误", "所选的不是PotPlayer.exe", QMessageBox::Ok);
            return;
        }else{
            settings.setValue("potplayer",fileName);
            ui->lineEdit_9->setText(fileName);
        }
    }
}


void MainWindow::on_pushButton_14_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "选择Snipaste.exe",
                                                    "",
                                                    "可执行程序 (*.exe)");

    if (!fileName.isEmpty()) {
        if(!fileName.toLower().contains("snipaste")){
            QMessageBox::critical(this, "错误", "所选的不是Snipaste.exe", QMessageBox::Ok);
            return;
        }else{
            settings.setValue("snipaste",fileName);
            ui->lineEdit_10->setText(fileName);
        }
    }
}


void MainWindow::on_lineEdit_4_textChanged(const QString &arg1)
{
    settings.setValue("api_key",arg1);
}


void MainWindow::on_lineEdit_5_textChanged(const QString &arg1)
{
settings.setValue("secret_key",arg1);
}


void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    settings.setValue("link_style",arg1);
}


void MainWindow::on_checkBox_1_stateChanged(int arg1)
{
settings.setValue(sender()->objectName(),arg1);
}


void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
settings.setValue(sender()->objectName(),arg1);
}


void MainWindow::on_checkBox_3_stateChanged(int arg1)
{
    settings.setValue(sender()->objectName(),arg1);
    if (arg1)
    {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.setValue("ClipNote", QCoreApplication::applicationFilePath().replace("/", "\\"));
    }
    else
    {
        QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
        settings.remove("ClipNote");
    }
}


void MainWindow::on_keySequenceEdit_1_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[0]->setRegistered(false);
    keyList[0]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[0]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_2_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[1]->setRegistered(false);
    keyList[1]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[1]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_3_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[2]->setRegistered(false);
    keyList[2]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[2]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_4_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[3]->setRegistered(false);
    keyList[3]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[3]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_5_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[4]->setRegistered(false);
    keyList[4]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[4]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_6_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[5]->setRegistered(false);
    keyList[5]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[5]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_7_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[6]->setRegistered(false);
    keyList[6]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[6]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_8_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[7]->setRegistered(false);
    keyList[7]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[7]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_9_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[8]->setRegistered(false);
    keyList[8]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[8]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_10_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[9]->setRegistered(false);
    keyList[9]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[9]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_11_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[10]->setRegistered(false);
    keyList[10]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[10]->setRegistered(true);
}


void MainWindow::on_keySequenceEdit_12_editingFinished()
{
    settings.setValue(sender()->objectName(),qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[1]->setRegistered(false);
    keyList[1]->setShortcut(qobject_cast<QKeySequenceEdit *>(sender())->keySequence());
    keyList[1]->setRegistered(true);
}


void MainWindow::on_pushButton_16_clicked()
{
    download=HttpGet("Auth/getVersionFree?ver="+version);

    if(!download.isEmpty()){
        trayIcon->showMessage("提示", "有新版本(请点击这里跳转到下载链接)",QSystemTrayIcon::Warning,5000);
    }else{
        trayIcon->showMessage("提示", "当前是最新版",QSystemTrayIcon::Warning,5000);
    }
}


void MainWindow::on_pushButton_15_clicked()
{
    QUrl url("https://www.yuque.com/zhangyewuxianxiaozhang/ghzwdl/befieqdxi16hbhix");
    QDesktopServices::openUrl(url);

}

