#include "Ip_hdr.h"

IP_v4_hdr::IP_v4_hdr(const u_char* begin) {
    hdr_len = begin;
    protocol = begin + 9;
    src = begin + 12;
    dest = begin + 16;
    //       for (int i = 0; i < 32; ++i)
    //           printf("%.2x ", begin[i]);
    //    version = begin[0] / 16;
    //hdr_len = count word (32 bit) in hdr
//    if (hdr_len != 20) return;
    //    tos = begin[1];
    //    total_len = begin[2] * 256 + begin[3];
    //    id = begin[4] * 256 + begin[5];
    //    flag = begin[6] >> 1;
    //    fragment = (begin[6] % 2) * 256 + begin[7];
    //    ttl = begin[8];
    //    check_sum = begin[10] * 256 + begin[11];
//    src = ((int)begin[12] << 24) + ((int)begin[13] << 16) +
//            ((int)begin[14] << 8) + begin[15];
//    dest = ((int)begin[16] << 24) + ((int)begin[17] << 16) +
//            ((int)begin[18] << 8) + begin[19];
}

QJsonObject IP_v4_hdr::writeReport(map<u_short, QString> & protocols_ip) {
    QJsonObject ip;
    ip["Source IP"] = getIp(src);
    ip["Destination IP"] = getIp(dest);
    ip["Protocol"] = (protocols_ip.count(*protocol) != 0)
            ? protocols_ip[*protocol] : QString::number((int)*protocol);
    return ip;
}

QString IP_v4_hdr::getIp(const u_char * addr) {
    return QString("%1.%2.%3.%4").arg(
                QString::number(*addr),
            QString::number(*(addr + 1)),
            QString::number(*(addr + 2)),
            QString::number(*(addr + 3))
            );
}
