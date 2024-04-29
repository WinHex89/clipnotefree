#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSharedMemory>
#include <QSettings>
#include <QHotkey>
#include <QUrl>
#include <QUrlQuery>
#include <QRegularExpression>
#include <QThread>
#include <QClipboard>
#include <QJsonObject>
#include <QJsonDocument>
#include <QtConcurrent>
#include <QUuid>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSqlQuery>
#include <QSqlError>
QString host="https://clipnote.cn/";
QString localHost="http://127.0.0.1:5437/";
int operation=0;
HWND mainHwnd;
MainWindow *w;
QSettings settings("ClipNote","Settings");
QString dbstr=QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)+ "/ClipNoteFree/clipnote.db";
void action(int);
bool isVideoLink(QString);
bool isProcessRunning(QString);
QNetworkAccessManager *networkManager;
#ifdef Q_OS_WIN
HWND PotPlayer();
#endif
QString httpGet(QString);
QString HttpGet(QString);
QString httpPost(QString,QByteArray);
void simulateCtrlV();
void simulateCtrlC();
double convertTimeToSeconds(QString*);
QString baiduOcr(QString path);
void play();
void pause();
QString addLink(QString url,QString time){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbstring=settings.value("database",dbstr).toString();
    db.setDatabaseName(dbstring);
    db.open();
    QSqlQuery query;
    query.prepare("INSERT INTO links (url,time) VALUES (:url, :time)");
    query.bindValue(":url", url);
    query.bindValue(":time", time);
    query.exec();
    QString id="cn://"+QString::number(query.lastInsertId().toInt());
    db.close();
    return id;
}
QString getLink(QString id){
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbstring=settings.value("database",dbstr).toString();
    db.setDatabaseName(dbstring);
    db.open();
    QSqlQuery query;
    query.prepare("SELECT * FROM links WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
    query.next();
    QString url = query.value(1).toString();
    QString time = query.value(2).toString();
    db.close();
    return QString("{\"url\":\"%1\",\"time\":\"%2\"}").arg(url).arg(time);
}
void open(QString go){
    QFuture<void> future;
    QWebSocket webSocket;
    QEventLoop loop;
    const QUrl serverUrl = QUrl("ws://127.0.0.1:5488");
    QObject::connect(&webSocket, &QWebSocket::connected, &loop, &QEventLoop::quit);
    webSocket.open(serverUrl);
    loop.exec();
    QUrl cn(go);
    QUrlQuery query(cn);
    QString link = query.queryItemValue("url");
    if(!cn.fragment().isEmpty()){
        link+="#"+cn.fragment();
    }
    QString *time = new QString(query.queryItemValue("time"));
    int id=go.replace("cn:","").replace("/","").toInt();
    if(id>0){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(getLink(QString::number(id)).toUtf8());
        QJsonObject jsonObj=jsonDoc.object();
        link=jsonObj["url"].toString();
        time=new QString(jsonObj["time"].toString());
    }

    bool isv=isVideoLink(link);
    if(time->contains("-")){
        if(!time->contains(".")){
            *time=time->split("-")[0]+".000"+"-"+time->split("-")[1]+".000";
        }
        if(isv){

            if(PotPlayer()){
                if(settings.value("url")==link){
                    SendMessage(PotPlayer(), 0x0400, 0x5005, QTime::fromString(time->split("-")[0],"hh:mm:ss.zzz").msecsSinceStartOfDay());
                    SendMessage(PotPlayer(), 0x0400, 0x5007, 2);
                }else{
                    settings.setValue("url",link);

                    QString program = settings.value("potplayer").toString();
                    QString arguments =link + " /seek=" + time->split("-")[0];
                    QProcess::startDetached(program, QStringList() << arguments);
                }
            }else{
                settings.setValue("url",link);
                QString program = settings.value("potplayer").toString();
                QString arguments =link + " /seek=" + time->split("-")[0];
                QProcess::startDetached(program, QStringList() << arguments);
            }
            future= QtConcurrent::run([&]() {
                while (true) {
                    if(PotPlayer()){
                        break;
                    }
                    QThread::sleep(1);
                }
                while (true&&PotPlayer()) {
                    int milliseconds =SendMessage(PotPlayer(), 0x0400, 0x5004, 1);
                    if(milliseconds>=QTime::fromString(time->split("-")[1],"hh:mm:ss.zzz").msecsSinceStartOfDay()){
                        SendMessage(PotPlayer(), 0x0400, 0x5007, 1);
                        return;}
                    QThread::msleep(10);
                }
            });

        }else if(isv){

            if(settings.value("url")==link){
                QJsonObject json;
                json["action"] = "mpv";
                json["time"] =*time;
                QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
                webSocket.sendTextMessage(jsonString);
            }else{
                settings.setValue("url",link);
                QJsonObject json;
                json["action"] = "mpv";
                json["data"] = link;
                json["time"] =*time;

                QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
                webSocket.sendTextMessage(jsonString);
            }
        }
        else{
            QJsonObject json;
            json["action"] = "seek";
            QUrl url(link);
            QUrlQuery query;
            query.setQuery(url.query());
            QString cnt =QString::number(convertTimeToSeconds(&time->split("-")[0]))+"-"+QString::number(convertTimeToSeconds(&time->split("-")[1]));
            query.addQueryItem("cnt",cnt);
            url.setQuery(query);
            json["data"] =url.toString();
            json["url"] =link;
            json["cnt"] =cnt;
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            webSocket.sendTextMessage(jsonString);
        }


    }
    else if(time->contains("~")){
        if(!time->contains(".")){
            *time=time->split("~")[0]+".000"+"~"+time->split("~")[1]+".000";
        }
        if(isv){

            if(PotPlayer()){
                if(settings.value("url")==link){
                    SendMessage(PotPlayer(), 0x0400, 0x5005, QTime::fromString(time->split("~")[0],"hh:mm:ss.zzz").msecsSinceStartOfDay());
                    SendMessage(PotPlayer(), 0x0400, 0x5007, 2);
                }else{
                    settings.setValue("url",link);

                    QString program =settings.value("potplayer").toString();
                    QString arguments =link + " /seek=" + time->split("~")[0];
                    QProcess::startDetached(program, QStringList() << arguments);
                }

            }else{

                settings.setValue("url",link);
                QString program = settings.value("potplayer").toString();
                QString arguments =link + " /seek=" + time->split("~")[0];
                QProcess::startDetached(program, QStringList() << arguments);
            }
            future= QtConcurrent::run([&]() {
                while (true) {
                    if(PotPlayer()){
                        break;
                    }
                    QThread::sleep(1);
                }
                while (true&&PotPlayer()) {
                    int milliseconds =SendMessage(PotPlayer(), 0x0400, 0x5004, 1);
                    if(milliseconds>=QTime::fromString(time->split("~")[1],"hh:mm:ss.zzz").msecsSinceStartOfDay()){
                        SendMessage(PotPlayer(), 0x0400, 0x5005, QTime::fromString(time->split("~")[0],"hh:mm:ss.zzz").msecsSinceStartOfDay());
                    }
                    QThread::msleep(10);
                }
            });

        }
        else if(isv){
            if(settings.value("url")==link){
                QJsonObject json;
                json["action"] = "mpv";
                json["time"] =*time;
                QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
                webSocket.sendTextMessage(jsonString);
            }else{
                settings.setValue("url",link);
                QJsonObject json;
                json["action"] = "mpv";
                json["data"] = link;
                json["time"] =*time;

                QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
                webSocket.sendTextMessage(jsonString);
            }
        }
        else{

            QJsonObject json;
            json["action"] = "seek";
            QUrl url(link);
            QUrlQuery query;
            query.setQuery(url.query());
            QString cnt =QString::number(convertTimeToSeconds(&time->split("~")[0]))+"~"+QString::number(convertTimeToSeconds(&time->split("~")[1]));
            query.addQueryItem("cnt",cnt);
            url.setQuery(query);
            json["data"] =url.toString();
            json["url"] =link;
            json["cnt"] =cnt;
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            webSocket.sendTextMessage(jsonString);
        }



    }
    else{
        if(!time->contains(".")){
            *time=*time+".000";
        }
        if(isv){
            if(PotPlayer()){
                if(settings.value("url")==link){
                    SendMessage(PotPlayer(), 0x0400, 0x5005, QTime::fromString(*time,"hh:mm:ss.zzz").msecsSinceStartOfDay());
                    SendMessage(PotPlayer(), 0x0400, 0x5007, 2);
                }else{
                    settings.setValue("url",link);
                    QString program = settings.value("potplayer").toString();
                    QString arguments =link + " /seek=" + *time;
                    QProcess::startDetached(program, QStringList() << arguments);

                }
            }else{
                settings.setValue("url",link);
                QString program = settings.value("potplayer").toString();
                QString arguments =link + " /seek=" + *time;
                QProcess::startDetached(program, QStringList() << arguments);
            }
        }else if(isv){
            if(settings.value("url")==link){
                QJsonObject json;
                json["action"] = "mpv";
                json["time"] =*time;
                QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
                webSocket.sendTextMessage(jsonString);
            }else{
                settings.setValue("url",link);
                QJsonObject json;
                json["action"] = "mpv";
                json["data"] = link;
                json["time"] =*time;
                QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
                webSocket.sendTextMessage(jsonString);
            }
        }
        else{


            QJsonObject json;
            json["action"] = "seek";

            QUrl url(link);
            QUrlQuery query;
            query.setQuery(url.query());
            QString cnt =QString::number(convertTimeToSeconds(time));
            query.addQueryItem("cnt",cnt);
            url.setQuery(query);
            json["data"] =url.toString();
            json["cnt"] =cnt;
            json["url"] =link;
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            webSocket.sendTextMessage(jsonString);
        }
    }
    future.waitForFinished();
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    networkManager=new QNetworkAccessManager;
    if(argc==2){
        QString go = QString::fromUtf8(argv[1]);
        if(!go.isEmpty()){

            QSharedMemory shared("ClipNoteLink");

            if(shared.attach())
            {
#ifdef Q_OS_WIN
                QProcess process;
                process.start("taskkill", QStringList() << "/F" << "/PID" << QString::number(settings.value("pid").toLongLong()));
                process.waitForFinished();
#endif
                shared.detach();
            }
            shared.create(1);
            settings.setValue("pid",QCoreApplication::applicationPid());
            open(go);
        }
        return 0;
    }

    QSharedMemory shared("ClipNote");

    if(shared.attach())
    {
        return 0;
    }
    shared.create(1);
#ifdef Q_OS_WIN
    QSettings reg("HKEY_CURRENT_USER\\Software\\Classes\\cn", QSettings::NativeFormat);
    reg.setValue("Default","URL:cn");
    reg.setValue("URL Protocol","");
    reg.setValue("shell/open/command/Default",a.applicationFilePath().replace("/","\\")+" %1");
#endif

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ClipNote_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    QTranslator qtTranslator;
    qtTranslator.load(":/qm/qt_zh_CN.qm");
    a.installTranslator(&qtTranslator);

    QTranslator qtBaseTranslator;
    qtBaseTranslator.load(":/qm/qtbase_zh_CN.qm");
    a.installTranslator(&qtBaseTranslator);

    if(!isProcessRunning("snipaste")){
        QString executable = settings.value("snipaste").toString();
        QProcess::startDetached(executable);
    }
    settings.remove("url");
    w = new MainWindow();
    w->show();
    w->activateWindow();
    if(settings.value("checkBox_2").toBool()){
        w->close();
    }
    for(int i=1;i<=12;i++){

        QHotkey *key=new QHotkey(QKeySequence(settings.value("keySequenceEdit_"+QString::number(i)).toString()), true,&a);
        QObject::connect(key, &QHotkey::activated,  [i]() {
            action(i);
        });
        w->keyList.append(key);
    }
#ifdef Q_OS_WIN
    mainHwnd= reinterpret_cast<HWND>(w->winId());
#endif
    return a.exec();
}

double convertTimeToSeconds(QString *timeStr) {
    QTime time = QTime::fromString(*timeStr, "hh:mm:ss.zzz");
    int hoursToSeconds = time.hour() * 3600;
    int minutesToSeconds = time.minute() * 60;
    int seconds = time.second();
    int milliseconds = time.msec();
    return hoursToSeconds + minutesToSeconds + seconds + milliseconds / 1000.0;
}
#ifdef Q_OS_WIN
HWND PotPlayer(){
    HWND hwnd = FindWindow(L"PotPlayer64", NULL);
    if (hwnd == Q_NULLPTR) {
        hwnd=FindWindow(L"PotPlayer32", NULL);
    }
    return hwnd;
}

#endif
bool isProcessRunning(QString processName){
    QProcess process;
#ifdef Q_OS_WIN
    process.start("tasklist");
#else
    process.start("pgrep", QStringList() << processName);
#endif
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    return output.toLower().contains(processName);
}
void simulateCtrlV()
{
#ifdef Q_OS_WIN
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_RCONTROL;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'V';
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'V';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_RCONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inputs[0], sizeof(INPUT));
    QThread::msleep(100);
    SendInput(1, &inputs[1], sizeof(INPUT));
    SendInput(1, &inputs[2], sizeof(INPUT));
    SendInput(1, &inputs[3], sizeof(INPUT));
#endif
#ifdef Q_OS_MAC
#endif
}
void simulateCtrlC()
{
#ifdef Q_OS_WIN
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_RCONTROL;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'C';
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'C';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_RCONTROL;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inputs[0], sizeof(INPUT));
    QThread::msleep(100);
    SendInput(1, &inputs[1], sizeof(INPUT));
    SendInput(1, &inputs[2], sizeof(INPUT));
    SendInput(1, &inputs[3], sizeof(INPUT));
#endif
#ifdef Q_OS_MAC
#endif
}

void action(int index){
    if(index==1){
        if(PotPlayer()){
            int milliseconds =SendMessage(PotPlayer(), 0x0400, 0x5004, 1);
            if(milliseconds==0){
                return;
            }
            QTime time(0, 0);
            time = time.addMSecs(milliseconds);
            QString formattedTime = time.toString("hh:mm:ss.zzz");
            LRESULT result = SendMessage(PotPlayer(), 0x0400,0x6020,reinterpret_cast<LPARAM>(mainHwnd));
            QString encodedUrl = settings.value("url").toString();


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
        else{
            operation=1;
            QJsonObject json;
            json["action"] = "pos";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
            w->loop.exec();
        }
    }
    if(index==2){

        if(PotPlayer()){
            keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
            SendMessage(PotPlayer(), 0x0400, 0x5010, 0x43);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
            simulateCtrlV();


        }
        else{

            QJsonObject json;
            json["action"] = "snapshots";
            operation=2;
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }

    }
    if(index==3){
        if(PotPlayer()){
            SendMessage(PotPlayer(), 0x0400, 0x5007, 1);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
            SendMessage(PotPlayer(), 0x0400, 0x5010, 0x43);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
                QString executable =settings.value("snipaste").toString();
                QStringList arguments;
                arguments << "paste" << "--clipboard";
                QProcess::startDetached(executable, arguments);
                QTimer::singleShot(1000, [&]() {
                    keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
                    keybd_event(VK_SPACE, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
                });


        }
        else{
            QJsonObject json;
            json["action"] = "pause";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
            operation=3;
            json["action"] = "snapshots";
            jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
    if(index==4){
        if(PotPlayer()){

            keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
            SendMessage(PotPlayer(), 0x0400, 0x5010, 0x43);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
            QString img = QDir(QCoreApplication::applicationDirPath()).filePath(QUuid::createUuid().toString()+".png");
            QApplication::clipboard()->image().save(img);

                QString data=baiduOcr(img);
                QApplication::clipboard()->setText(data);
                simulateCtrlV();
                QFile::remove(img);



        }else{
            QJsonObject json;
            operation=6;
            json["action"] = "snapshots";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);

        }
    }
    if(index==5){
        if(PotPlayer()){
            int ms = SendMessage(PotPlayer(), 0x0400, 0x5004, 1);
            SendMessage(PotPlayer(), 0x0400, 0x5005, ms+=5000);
            SendMessage(PotPlayer(), 0x0400, 0x5007, 2);
        }else{
            QJsonObject json;
            json["action"] = "kj";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
    if(index==6){
        if(PotPlayer()){
            int ms = SendMessage(PotPlayer(), 0x0400, 0x5004, 1);
            SendMessage(PotPlayer(), 0x0400, 0x5005, ms-=5000);
            SendMessage(PotPlayer(), 0x0400, 0x5007, 2);
        }else{
            QJsonObject json;
            json["action"] = "kt";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }

    if(index==7){
        if(PotPlayer()){
            int code = SendMessage(PotPlayer(), 0x0400, 0x5006, 0);
            if (code == 2) { SendMessage(PotPlayer(), 0x0400, 0x5007, 1); } else { SendMessage(PotPlayer(), 0x0400, 0x5007, 2); }
        }else{
            QJsonObject json;
            json["action"] = "playorpause";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }

    if(index==8){
        if(PotPlayer()){
            SendMessage(PotPlayer(), 0x0400, 0x5010, 0x43);
        }else{
            QJsonObject json;
            json["action"] = "jiasu";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
    if(index==9){
        if(PotPlayer()){
            SendMessage(PotPlayer(), 0x0400, 0x5010, 0x58);
        }else{
            QJsonObject json;
            json["action"] = "jiansu";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
    if(index==10){
        if(PotPlayer()){
            SendMessage(PotPlayer(), 0x0400, 0x5010, 0x5A);
        }else{
            QJsonObject json;
            json["action"] = "changsu";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
    if(index==11){

        if(PotPlayer()){

            int vol = SendMessage(PotPlayer(), 0x0400, 0x5000, 1);
            SendMessage(PotPlayer(), 0x0400, 0x5001, vol+=10);
        }else{
            QJsonObject json;
            json["action"] = "yljia";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
    if(index==12){
        if(PotPlayer()){
            int vol = SendMessage(PotPlayer(), 0x0400, 0x5000, 1);
            SendMessage(PotPlayer(), 0x0400, 0x5001, vol-=10);
        }else{
            QJsonObject json;
            json["action"] = "yljian";
            QString jsonString = QJsonDocument(json).toJson(QJsonDocument::Compact);
            w->broadcastMessage(jsonString);
        }
    }
}
bool isVideoLink(QString link){
    if(!link.startsWith("http")){
        QFile file(link);

        if (file.exists()) {
            return true;
        } else {
            return false;
        }
    }else if(link.startsWith("http")){
        QUrl url(link);
        QNetworkRequest request(url);
        QString userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36 Edg/122.0.0.0";
        request.setRawHeader("User-Agent", userAgent.toUtf8());
        QNetworkReply *reply = networkManager->head(request);
        QEventLoop loop;
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        QString ct = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        reply->deleteLater();
        if(ct.contains("video")||ct.contains("audio")){
            return true;
        }else{
            return false;
        }

    }
    return false;
}
QString httpGet(QString link){
    QUrl url(localHost+link);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}

QString HttpGet(QString link){
    QUrl url(host+link);
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}
QString httpPost(QString link,QByteArray postData){
    QUrl url(host+link);
    QNetworkRequest request(url);
     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply= networkManager->post(request,postData);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    return QString::fromUtf8(responseData);
}

QString baidu_access_token(){
    QString api="https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2";
    QUrl url(api.arg(settings.value("api_key").toString(),settings.value("secret_key").toString()));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager->post(request,"");
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj=jsonDoc.object();
    QString accessToken=jsonObj["access_token"].toString();
    return accessToken;
}

QString baiduOcr(QString path){
    if(settings.value("api_key").toString().isEmpty()||settings.value("secret_key").toString().isEmpty()){
        QMessageBox::critical(w, "错误", "请点击参数填写OCR接口信息", QMessageBox::Ok);
        return "";
    }
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QByteArray imageData = file.readAll();
    QString base64ImageData = QString::fromLatin1(imageData.toBase64());
    file.close();
    QUrl url("https://aip.baidubce.com/rest/2.0/ocr/v1/accurate_basic?access_token="+baidu_access_token());

    QByteArray postData;
    postData.append("image="+ QUrl::toPercentEncoding(base64ImageData));
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QNetworkReply *reply = networkManager->post(request,postData);
    QByteArray responseData;
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
    }
    reply->deleteLater();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
    QJsonObject jsonObj=jsonDoc.object();
    QJsonArray wordsResultArray = jsonObj["words_result"].toArray();

    QStringList wordsList;
    for (const QJsonValue& value : wordsResultArray) {
        QString word = value.toObject()["words"].toString();
        wordsList << word;
    }
    return wordsList.join("");
}
