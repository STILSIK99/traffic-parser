#include "Pcap_hdr.h"

u_int Packet_hdr::getLength(){
    return * (reinterpret_cast<u_int *> (pkt_hdr + 8));
}

u_int Packet_hdr::getSec(){
    return * (reinterpret_cast<u_int *> (pkt_hdr));
}

u_int Packet_hdr::getUsec(){
    return * (reinterpret_cast<u_int *> (pkt_hdr + 4));
}

Packet_hdr::~Packet_hdr (){
    delete []pkt_hdr;
}

Packet_hdr::Packet_hdr (char *data)
{
    pkt_hdr = data;
}

//PCAP FORMAT HEADER

u_char * Pcap_hdr::getPacketData(u_int len)
{
    u_char * data = new u_char[len];
    file->read(reinterpret_cast<char *> (data), len);
    return data;
}

bool Pcap_hdr::hasNext(){
    if (file->bytesAvailable() >= 16){
        return true;
    }
    return false;
}

Packet_hdr *Pcap_hdr::getNextPacket()
{
    char *data = new char[16];
    file->read (data, 16);
    Packet_hdr *pack = new Packet_hdr (data);
    return pack;
}

Pcap_hdr::Pcap_hdr (QString & path)
{
    file = new QFile;
    file->setFileName(path);
    file->open (QIODevice::ReadOnly);
    if (!file->isOpen()) {
        qDebug() << path << ": isn't open.";
        isValid = false;
        return;
    }
    if (file->bytesAvailable() <= 24) {
        qDebug() << path << ": very short.";
        isValid = false;
        return;
    }
    pcap_hdr = new char [24];
    file->read (pcap_hdr, 24);
    if (* (reinterpret_cast<u_int *> (pcap_hdr)) != 0xa1b2c3d4) {
        qDebug() << path << ": has another format.";
        isValid = false;
        return;
    }
    if ( * (reinterpret_cast<u_short *> (pcap_hdr + 4)) != 2 ||
         * (reinterpret_cast<u_short *> (pcap_hdr + 6)) != 4) {
        qDebug() << path << ": has another version.";
        isValid = false;
        return;
    }
    if (*(reinterpret_cast<u_int *> (pcap_hdr + 20)) != 1){
        qDebug() << path << ": hasn't ethernet traffic.";
        isValid = false;
        return;
    }
    isValid = true;
}

Pcap_hdr::~Pcap_hdr()
{
    delete []pcap_hdr;
    if (file->isOpen()) {
        file->close();
    }
    delete file;
}
