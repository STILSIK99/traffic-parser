#ifndef TCP_HDR_H
#define TCP_HDR_H

#include <QJsonObject>
#include <string>
#include <QDebug>
#include <map>

using namespace std;

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

//extern map <QString, map<u_short, QString>> types;

class TCP_hdr {
    //    u_int seq_num;
    //    u_int ack_num;
    //    u_short flags;
    //    u_short w_size_val;
    //    u_short check_sum;
    //    u_short urg_point;
    //    string opt;
    const u_char * dest_port;
    const u_char * src_port;


public:
    const u_char * hdr_len;

//    u_short dest_port;
//    u_short src_port;

    TCP_hdr(const u_char* begin);

    QJsonObject writeReport(map<u_short, QString> &);
    u_short getDestPort();
    u_short getSrcPort();
    u_char getLen();
};

#endif // TCP_HDR_H
