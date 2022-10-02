#include "Writer.h"

Writer::Writer(QString globalPath, map<QString, set<QString> > * attr)
{
    path = globalPath;
    attributes = attr;
}

void Writer::work(){
    qDebug() << "Writer is started.";
    isWork = true;
    QDateTime timeWrite = QDateTime::currentDateTime().addSecs(30);
    while(isWork || (!que.empty())){
        if (timeWrite < QDateTime::currentDateTime()){
            for(auto f : outs){
                f.second->flush();
            }
            timeWrite = QDateTime::currentDateTime().addSecs(30);
        }
        if (que.empty()) {
            sleepSec(1);
            continue;
        }
        write(que.front());
        que.pop();
    }
    if (outs.size() == 0)
        qDebug() << "Not found any credentials.";
    for (auto p : outs){
        p.second->close();
        delete p.second;
    }
    //    emit finished();
};

void Writer::write(QJsonObject *obj){
    if (!obj->contains("queue")) return;
    if (!(*obj)["queue"].isArray()) return;
    QString protocol = (*obj)["queue"].toArray()[0].toString();
    if (outs.count(protocol) == 0){
        //        qDebug() << outPath + "/report_" + protocol + ".csv";
        QFile * file = new QFile(outPath + "/report_" + protocol + ".csv");
        file->open(QIODevice::WriteOnly | QIODevice::Append);
        if (!file->isOpen()){
            qDebug() << "Can't open file for write.";
            return;
        }
        outs[protocol] = file;
        writeHdr(obj, file);
    }
    auto arr = (*obj)["queue"].toArray();
    for (int i = 0; i < arr.size(); ++i) {
        //перебор протоколов в очереди
        auto protocol_obj = (*obj)[arr[i].toString()].toObject();
        for (auto field : (*attributes)[arr[i].toString()]){
            if (protocol_obj[field].isString()){
                //                outs[protocol]->write((protocol_obj[field].toString() + ";").toUtf8());
                outs[protocol]->write((protocol_obj[field].toString().replace(";", "\\") + ";").toUtf8());
            }
            else {
                if (protocol_obj[field].isDouble()){
                    outs[protocol]->write((QString::number(protocol_obj[field].toInt()) + ";").toUtf8());
                }
                else {
                    outs[protocol]->write(";");
                }
            }
        }
    }
    outs[protocol]->write((*obj)["Time"].toString().toUtf8());
    outs[protocol]->write(";");
    outs[protocol]->write((*obj)["File"].toString().toUtf8());
    outs[protocol]->write(";");
    outs[protocol]->write("\n");
    delete obj;
};

void Writer::writeHdr(QJsonObject * obj, QFile *file){
    if (!obj->contains("queue")) return;
    if (!(*obj)["queue"].isArray()) return;
    auto arr = (*obj)["queue"].toArray();
    for (int i = 0; i < arr.size(); ++i) {
        file->write(arr[i].toString().toUtf8());
        for (int i = 0; i < (*attributes)[arr[i].toString()].size(); ++i) {
            file->write(QString(";").toUtf8());
        }
    }
    file->write(QString("Other;;").toUtf8());
    file->write(QString("\n").toUtf8());
    //write second header - attribute
    for (int i = 0; i < arr.size(); ++i) {
        for(auto field : (*attributes)[arr[i].toString()]){
            file->write((field + ";").toUtf8());
        }
    }
    file->write(QString("Time;File;").toUtf8());
    file->write(QString("\n").toUtf8());
}

void Writer::sleepSec(int sec){
    QDateTime cur = QDateTime::currentDateTime().addSecs(sec);
    while(cur > QDateTime::currentDateTime()){
    }
};
