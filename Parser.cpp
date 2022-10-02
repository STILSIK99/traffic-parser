#include "Parser.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QTime>
#include <QDateTime>
#include <QByteArray>

void Parser::startScan(){
    QDateTime startTime = QDateTime::currentDateTime();
    QThread * write_thread = new QThread;
    writer->moveToThread(write_thread);
    connect(write_thread, &QThread::started, writer, &Writer::work);
    write_thread->start();
    qDebug() << "Start - " << startTime.toString("HH:mm:ss");

    vector<pair<QThread*, Checker*>> v_th(max_threads);
    process_files = 0;
    for (int i = 0; i < max_threads; ++i){
        Checker *checker = new Checker(attributes, types, process_files, writer->que);
        checker->full = research.size();
        QThread *thread = new QThread;
        connect(thread, &QThread::started, checker, &Checker::worker);
        connect(checker, &Checker::finished, checker, &Checker::deleteLater);
        checker->moveToThread(thread);
        v_th[i] = make_pair(thread, checker);
    }

    size_t i = 0;
    //distribution of files between threads
    for(; i < min(max_threads * 3, (int)research.size()); ++i){
        v_th[i % max_threads].second->q_file.push(research[i]);
    }
    for(auto p : v_th){
        p.first->start();
    }
    for(; i < research.size();++i){
        v_th[i % max_threads].second->q_file.push(research[i]);
    }

    //wait finish
    while(process_files < research.size()){
        sleepSec(1);
    }
    //delete threads
    for(int i = 0; i < max_threads; ++i){
        if (v_th[i].first->isRunning()){
            v_th[i].first->quit();
            v_th[i].first->wait();
        }
        delete v_th[i].first;
    }
    writer->isWork = false;
    sleepSec(3);
    if (write_thread->isRunning()){
        write_thread->quit();
        write_thread->wait();
    }
    delete write_thread;
    QDateTime finishTime = QDateTime::currentDateTime();
    qDebug() << "Finish - " << finishTime.toString("HH:mm:ss");
    int bettwen = startTime.secsTo(finishTime);
    qDebug () << bettwen / 3600 << " hours "
              << (bettwen % 3600) / 60 << " minutes "
              << bettwen % 60 << " seconds.";
}

void Parser::readDir(QString path){
    if (!QFileInfo(path).isDir()){
        return;
    }
    QDirIterator it(path, QDir::Files, QDirIterator::Subdirectories);
    while(it.hasNext()){
        QFileInfo info(it.next());
        research.push_back(info.filePath());
    }
};

void Parser::readConfigFile(QString file){
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen()) return;
    QJsonObject *obj = new QJsonObject(QJsonDocument::fromJson(f.readAll()).object());
    initGlobalVars(obj);
    delete obj;
};

QString Parser::JsonToString(QJsonObject *obj){
    return QString(QJsonDocument(*obj).toJson(QJsonDocument::Compact));
}

QJsonObject Parser::StringToJson(QString & str){
    return QJsonDocument::fromJson(str.toUtf8()).object();
}

void Parser::initGlobalVars(QJsonObject * cfg){
    //read Attributes from configuration file
    if (cfg->contains("Attibutes")){
        if ((*cfg)["Attibutes"].isObject()){
            const auto attr = (*cfg)["Attibutes"].toObject();
            for (auto key : attr.keys()){
                if (attr[key].isArray()){
                    for(auto el : attr[key].toArray()){
                        if (!el.isString()) continue;
                        if (attributes.count(key) == 0){
                            attributes[key] = set<QString>();
                        }
                        attributes[key].insert(el.toString());
                    }
                }
            }
        }
    }
    //read Types from configuration file
    if ((*cfg).contains("Type")){
        if ((*cfg)["Type"].isObject()){
            const auto type = (*cfg)["Type"].toObject();
            //перебор протоколов
            for (auto key : type.keys()){
                if (type[key].isObject()){
                    //protocol - объект в котором поля соответствуют определенным портам
                    auto protocol = type[key].toObject();
                    if (types.count(key) == 0){
                        types[key] = map<u_short, QString>();
                    }
                    //kkey - тип
                    for (auto kkey : protocol.keys()){
                        if (protocol[kkey].isDouble()){
                            types[key][protocol[kkey].toInt()] = kkey;
                        }
                        if (protocol[kkey].isArray()){
                            auto arr = protocol[kkey].toArray();
                            for (auto el : arr){
                                types[key][el.toInt()] = kkey;
                            }
                        }
                    }
                }
            }
        }
    }
    auto req = {"ethernet", "ip", "tcp", "http"};
    for (auto k : req){
        if (attributes.count(k) == 0){
            attributes[k] = {};
        }
    }
}

Parser::Parser(){
}

Parser::~Parser(){
    delete writer;
}

void Parser::init(QString file, QString path, int threads){
    readConfigFile(file);
    readDir(path);
    dir = path;
    max_threads = threads;
}

void Parser::deleteThreads(){
}

void Parser::sleepSec(int sec){
    QDateTime cur = QDateTime::currentDateTime().addSecs(sec);
    while(cur > QDateTime::currentDateTime()){
    }
};

void Parser::sleepMSec(int msec){
    QDateTime cur = QDateTime::currentDateTime().addMSecs(msec);
    while(cur > QDateTime::currentDateTime()){
    }
};
