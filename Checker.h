#ifndef CHECKER_H
#define CHECKER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QObject>
#include <QString>
#include <QDateTime>
#include <QThread>
#include <bits/stdc++.h>
#include <QFile>
#include <QFileInfo>

#include <stdlib.h>
#include <stdio.h>

#include "Ethernet_hdr.h"
#include "Ip_hdr.h"
#include "Tcp_hdr.h"
#include "Http_hdr.h"
#include "Pcap_hdr.h"

typedef unsigned short u_short;
typedef unsigned char u_char;

using namespace std;

class Checker : public QObject{
    Q_OBJECT

    map<QString, set<QString>> &attributes;
    map <QString, map<u_short, QString>> &types;
    set<QString> founded;

    volatile int & files;

    QJsonObject * report;
    QJsonArray que;

    //    QFile *out;

    queue<QJsonObject *> & writer_que;

    bool process_eth(const u_char* pkt_data, int size);

    bool process_ip_v4(const u_char* pkt_data, int size);

    bool process_tcp(const u_char* pkt_data, int size);

    bool process_http(const u_char* data, int size);

    QJsonObject StringToJson(QString & str);

    QString JsonToString(QJsonObject *obj);

    QString fromSecToDate(long secs, long usecs);

    char getHexSymb(char);

    void finish();

    bool process(QString );

public:
    Checker(
            map<QString, set<QString>> &,
            map <QString, map<u_short, QString>> &,
            volatile int &, queue<QJsonObject *> &
            );
    virtual ~Checker();

    void sleepSec(int sec);

    queue<QString> q_file;

    int full;

public slots:

    void worker();

signals:
    void writeReport(QJsonObject *);
    void finished();

};

#endif // CHECKER_H
