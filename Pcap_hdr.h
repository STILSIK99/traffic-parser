#ifndef PCAP_HDR_H
#define PCAP_HDR_H

#include <QString>
#include <QFile>
#include <QDebug>

using namespace std;

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;

class Packet_hdr
{
    char *pkt_hdr;

public:
    bool isValid;

    Packet_hdr (char *);
    ~Packet_hdr();
    u_int getLength();
    u_int getSec();
    u_int getUsec();

};

class Pcap_hdr
{
    QFile *file;
    char *pcap_hdr;

public:
    bool isValid;

    Pcap_hdr (QString&);
    ~Pcap_hdr();
    Packet_hdr *getNextPacket();
    u_char * getPacketData(u_int);
    bool hasNext();

};




#endif // PCAP_HDR_H
