#ifndef HTTP_HDR_H
#define HTTP_HDR_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include <bits/stdc++.h>
using namespace std;

typedef unsigned char u_char;

struct Http_hdr {
    //    QJsonObject body;
    //    QJsonObject add;
    const u_char * begin;
    int len;

    Http_hdr(const u_char* pkt_data, int size);

    Http_hdr();

    bool isDataFound();

    bool checkApplication(const u_char * , int);

    void foundKey( pair<bool, QString> &, const u_char *, int );

    void getValue(QString &, const u_char *, int );

    pair<QString, QString> getToken(QString);

    QJsonObject writeReport(set<QString> & attributes);
};

#endif // HTTP_HDR_H
