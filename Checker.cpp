#include "Checker.h"


Checker::Checker(
        map<QString, set<QString>> & a,
        map <QString, map<u_short, QString>> & b,
        volatile int & d, queue<QJsonObject *> & e): attributes(a), types(b),
    files(d), writer_que(e)
{

}

void Checker::worker(){
    sleepSec(1);
    while(!q_file.empty()){
        if (process(q_file.front())){
            qDebug() << q_file.front();
        }
        ++files;
        qDebug() << "Checked " << files << " of " << full;
        q_file.pop();
    }
    emit finished();
};

void Checker::sleepSec(int sec){
    QDateTime cur = QDateTime::currentDateTime().addSecs(sec);
    while(cur > QDateTime::currentDateTime()){
    }
};


bool Checker::process(QString object){
    Pcap_hdr *pcap = new Pcap_hdr(object);
    if (!pcap->isValid) {
        return false;
    }
    while(pcap->hasNext()){
        Packet_hdr * pkt_hdr = pcap->getNextPacket();
        u_char * pkt_data = pcap->getPacketData(pkt_hdr->getLength());
        report = new QJsonObject();
        que = QJsonArray();
        bool isFound = false;
        try {
            isFound = process_eth(pkt_data, pkt_hdr->getLength());
        } catch (...) {
        }
        if (isFound){
            qDebug() << "Founded";
            (*report)["Time"] = fromSecToDate(pkt_hdr->getSec(), pkt_hdr->getUsec());
            (*report)["queue"] = que;
            (*report)["File"] = object;
            writer_que.push(report);
        }
        else{
            delete report;
        }
        delete []pkt_data;
        delete pkt_hdr;
    }
    delete pcap;
    emit finished();
    return true;
}

bool Checker::process_eth(const u_char* pkt_data, int size) {
    //get Ethernet header
    if (types.count("ethernet") == 0) return false;
    auto etherType = types["ethernet"];
    Eth_hdr eth_pkt(pkt_data);
    if (size - 14 < 20) return false;
    if (etherType.count(eth_pkt.getType()) != 0) {
        if (etherType[eth_pkt.getType()] == "ip_v4" && size > 14) {
            const u_char *next_hdr = (pkt_data + 14);
            bool isFound = process_ip_v4(next_hdr, size - 14);
            if (isFound){
                que.push_back("ethernet");
                (*report)["ethernet"] = eth_pkt.writeReport(attributes["ethernet"]);
            }
            return isFound;
        }
    }
    return false;
}

bool Checker::process_ip_v4(const u_char* pkt_data, int size) {
    //get IP_v4 header
    if (types.count("ip_v4") == 0) return false;
    auto protocols_ip = types["ip_v4"];
    IP_v4_hdr ip_pkt(pkt_data);
    if ((size - *(ip_pkt.hdr_len) % 16 * 4) < 20) return false;
    if (protocols_ip.count(*(ip_pkt.protocol)) != 0) {
        if (protocols_ip[*(ip_pkt.protocol)] == "tcp") {
            const u_char* next_hdr = (pkt_data + (*ip_pkt.hdr_len % 16 * 4));
            bool isFound = process_tcp(next_hdr, size - (*ip_pkt.hdr_len % 16 * 4));
            if (isFound){
                que.push_back("ip");
                (*report)["ip"] = ip_pkt.writeReport(protocols_ip);
            }
            return isFound;
        }
    }
    return false;
}

bool Checker::process_tcp(const u_char* pkt_data, int size) {
    if (size < 20) return false;
    if (types.count("tcp") == 0) return false;
    auto tcp_ports = types["tcp"];
    TCP_hdr tcp_pkt(pkt_data);
    if (size <= (*tcp_pkt.hdr_len >> 2)) return false;
    if (tcp_ports.count(tcp_pkt.getDestPort()) != 0) {
        if (tcp_ports[tcp_pkt.getDestPort()] == "http") {
            const u_char* http_data = pkt_data + (tcp_pkt.getLen());
            bool isFound = process_http(http_data, size - tcp_pkt.getLen());
            if (isFound){
                que.push_back("tcp");
                (*report)["tcp"] = tcp_pkt.writeReport(tcp_ports);
                return isFound;
            }
        }
    }
    return false;
}

bool Checker::process_http(const u_char* data, int size) {
    //qDebug() << "HTTP size: " << size << "\n";
    Http_hdr http_pkt(data, size);
    if (http_pkt.isDataFound()){
        //        qDebug() << "Founded.";
        auto result = http_pkt.writeReport(attributes["http"]);
        QString arg = (result.contains("GET")) ? "GET" :
                                                 ((result.contains("POST")) ? "POST" : "Unknown");
        arg += result["Username"].toString();
        arg += ":" + result["Password"].toString();
        if (founded.count(arg) != 0) return false;
        founded.insert(arg);
        que.push_back("http");
        (*report)["http"] = result;
        return true;
    }
    return false;
};

Checker::~Checker(){

};

QString Checker::JsonToString(QJsonObject *obj){
    return QString(QJsonDocument(*obj).toJson(QJsonDocument::Indented));
}

QJsonObject Checker::StringToJson(QString & str){
    return QJsonDocument::fromJson(str.toUtf8()).object();
}

QString Checker::fromSecToDate(long secs, long usecs)
{
    return QDateTime::fromSecsSinceEpoch(secs).toString("yyyy-MM-dd HH:mm:ss.")
            + QString::number(usecs);
}

char Checker::getHexSymb(char a) {
    if (a >= 0 && a <= 9) {
        return 48 + a;
    }
    return 'a' + a - 10;
};
