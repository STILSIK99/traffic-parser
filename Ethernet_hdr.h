#ifndef ETHERNET_HDR_H
#define ETHERNET_HDR_H

#include <QString>
#include <QJsonObject>
#include <QDebug>

#include <set>

using namespace std;

typedef unsigned char u_char;
typedef unsigned short u_short;

class Eth_hdr {
    const u_char * dest; //[6];
    const u_char * src;  //[6];
    const u_char * type; //[2];

    char getHexSymb(char);

    QString getHex(const u_char * str, bool isMac);

   public:

    Eth_hdr(const u_char * begin);

    QJsonObject writeReport(set<QString>& );

    u_short getType();

};

#endif // ETHERNET_HDR_H
