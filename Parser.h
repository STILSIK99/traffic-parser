#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QDebug>
#include <QThread>

#include <bits/stdc++.h>

#include "Checker.h"
#include "Writer.h"

typedef unsigned short u_short;

using namespace std;

class Parser : public QObject{
    Q_OBJECT
    map <QString, map<u_short, QString>> types;
    vector<QString> research;
    volatile int process_files;
    int max_threads;
    QString dir;

    queue<int> q_th;

    void initGlobalVars(QJsonObject * cfg);

    QJsonObject StringToJson(QString & str);

    QString JsonToString(QJsonObject *obj);

    void readConfigFile(QString file);

    void readDir(QString path);

    void deleteThreads();
    void sleepSec(int);
    void sleepMSec(int);

public:
    map<QString, set<QString>> attributes;

    void startScan();

    void init(QString, QString, int);

    Writer *writer;

    Parser();

    ~Parser();

};


#endif // PARSER_H
