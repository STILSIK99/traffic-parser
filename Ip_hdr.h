#ifndef IP_HDR_H
#define IP_HDR_H

#include <QJsonObject>
#include <QString>
#include <QDebug>

using namespace std;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

class IP_v4_hdr {
    //    u_char version;
    //DSCP and ECN
    //    u_char tos;
    //    u_short total_len;
    //    u_short id;
    //    u_char flag;
    //    u_short fragment;
    //    u_char ttl;
    //    u_short check_sum;
    const u_char * src;
    const u_char * dest;

    QString getIp(const u_char* begin);

public:
    const u_char * protocol;
    const u_char * hdr_len;

    IP_v4_hdr(const u_char* begin);

    QJsonObject writeReport(map<u_short, QString> &);

};


#endif // IP_HDR_H
