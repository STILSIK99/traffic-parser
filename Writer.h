#ifndef WRITER_H
#define WRITER_H

#include <bits/stdc++.h>
#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QFile>
#include <QDebug>
#include <QString>

using namespace std;

class Writer : public QObject{
    Q_OBJECT

    QString path;
    map<QString, QFile * > outs;
    map<QString, set<QString> > * attributes;

public:
    queue<QJsonObject *> que;
    QString outPath;
    bool isWork;

    void write(QJsonObject *);
    void writeHdr(QJsonObject *, QFile *);
    void openFile(QString );
    Writer(QString , map<QString, set<QString> > *);
    void sleepSec(int);

public slots:

    void work();

//signals:

//    void finished();

};

#endif // WRITER_H
