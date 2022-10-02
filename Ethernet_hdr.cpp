#include "Ethernet_hdr.h"

Eth_hdr::Eth_hdr(const u_char * begin) {
    dest = begin;
    src = begin + 6;
    type = begin + 12;
    //    for (int i = 0; i < 6; ++i) dest[i] = begin[i];
    //    for (int i = 0; i < 6; ++i) src[i] = begin[i + 6];
    //    for (int i = 0; i < 2; ++i) type[i] = begin[i + 12];
}

QString Eth_hdr::getHex(const u_char * str, bool isMac) {
    QString s(4, 0);
    if (isMac) {
        s = QString(17, ':');
        for (int i = 0; i < 12; ++i) {
            s[i + i / 2] = getHexSymb( (i % 2 == 0) ? (str[i / 2] / 16) : (str[i / 2] % 16));
        }
        return s;
    }
    s = QString(4, 0);
    for (int i = 0; i < 4; ++i) {
        s[i] = getHexSymb((i % 2 == 0) ? (str[i / 2] / 16) : (str[i / 2] % 16));
    }
    return s;
}

QJsonObject Eth_hdr::writeReport(set<QString> & attributes) {
    QJsonObject eth;
    if (attributes.count("Destination MAC") != 0)
        eth["Destination MAC"] = getHex(dest, true);
    if (attributes.count("Source MAC") != 0)
        eth["Source MAC"] = getHex(src, true);
    if (attributes.count("Type") != 0)
        eth["Type"] = getHex(type, false);
    return eth;
}

u_short Eth_hdr::getType() {
    return type[0] * 256 + type[1];
}

char Eth_hdr::getHexSymb(char a) {
    if (a >= 0 && a <= 9) {
        return 48 + a;
    }
    return 'a' + a - 10;
};

