#include "Http_hdr.h"



Http_hdr::Http_hdr(const u_char* pkt_data, int size): begin(pkt_data), len(size) {
}

Http_hdr::Http_hdr() {}

QJsonObject Http_hdr::writeReport(set<QString> &attributes){
    QJsonObject http;
    int s_len = 0;
    const u_char * pos = begin;
    bool isFirst = true;
    QString value = "";
    QString key = "";
    int i = 0;
    for (i = 0; i < len - 1; ++i) {
        //перебор всех символов
        if (begin[i] == '\r' && begin[i + 1] == '\n'){
            //если найдено окончание строки
            if (s_len == 0) {
                //если найдена пустая строка, то окончание поиска
                i += 2;
                break;
            }
            else{
                //читаем значение
                getValue(value, begin + i - s_len, s_len);
                if (key == "Authorization"){
                    if (value.left(5) == "Basic") {
                        auto token = getToken(value.right(value.length() - 6));
                        if (attributes.count("Username") != 0){
                            http["Username"] = token.first;
                        }
                        if (attributes.count("Password") != 0){
                            http["Password"] = token.second;
                        }
                    }
                }
                if (attributes.count(key) != 0){
                    http[key] = value;
                }
                s_len = 0;
                i += 1;
                isFirst = true;
                continue;
            }
        }
        else{
            //не встречен символ окончания строки
            if ((begin[i] == ':' || begin[i] == ' ')
                    && isFirst){
                //встречен символ : или пробел первый раз
                //значит читаем ключ
                getValue(key, begin + i - s_len, s_len);
                s_len = 0;
                isFirst = false;
                if (begin[i + 1] == ' ') ++i;
                continue;
            }
            else{
                ++s_len;
            }
        }
    }
    if (i < len){
        int j = 0;
        string app(len - i, 0);
        for(; i < len; ++i){
            app[j++] = (begin[i]);
        }
        QJsonObject obj = QJsonDocument::fromJson(QByteArray(app.c_str(), app.length())).object();
        for (auto key : obj.keys()){
            if (attributes.count(key) != 0){
                http[key] = obj[key].toString();
            }
        }
    }
    return http;
}

bool Http_hdr::isDataFound(){
    int s_len = 0;
    const u_char * pos = begin;
    bool isFirst = true;
    QString value = "";
    QString key = "";
    int i = 0;
    for (i = 0; i < len - 1; ++i) {
        //перебор всех символов
        if (begin[i] == '\r' && begin[i + 1] == '\n'){
            //если найдено окончание строки
            if (s_len == 0) {
                //если найдена пустая строка, то окончание поиска
                i += 2;
                break;
            }
            else{
                //читаем значение
                isFirst = true;
                if (key == "Authorization"){
                    getValue(value, begin + i - s_len, s_len);
                    if (value.left(5) == "Basic") {
                        return true;
                    }
                }
                if (key == "Content-Type"){
                    getValue(value, begin + i - s_len, s_len);
                    if (value == "application/json; charset=utf-8"){
                        //дойти до конца и вернуть результат проверки приложения
                        while(i < len - 1){
                            if (
                                    begin[i] == '\r' &&
                                    begin[i + 1] == '\n' &&
                                    begin[i + 2] == '\r' &&
                                    begin[i + 3] == '\n'
                                    ){
                                return checkApplication(begin + i + 4, len - i - 4);
                            }
                            ++i;
                        }
                    }
                }
                s_len = 0;
                i += 1;
                continue;
            }
        }
        else{
            //не встречен символ окончания строки
            if ((begin[i] == ':' || begin[i] == ' ')
                    && isFirst){
                //встречен символ : или пробел первый раз
                //значит читаем ключ
                getValue(key, begin + i - s_len, s_len);
                s_len = 0;
                isFirst = false;
                if (begin[i + 1] == ' ') ++i;
                continue;
            }
            else{
                ++s_len;
            }
        }
    }
    return false;
}

void Http_hdr::foundKey(pair<bool, QString>& pair, const u_char * begin, int len){
    if (len == 12){
        //проверка на слово Content-Type
        if (
                begin[0] == 'C' &&
                begin[1] == 'o' &&
                begin[2] == 'n' &&
                begin[3] == 't' &&
                begin[4] == 'e' &&
                begin[5] == 'n' &&
                begin[6] == 't' &&
                begin[7] == '-' &&
                begin[8] == 'T' &&
                begin[9] == 'y' &&
                begin[10] == 'p' &&
                begin[11] == 'e'
                ){
            pair.first = true;
            pair.second = "Content-Type";
            return;
        }
    }


    pair.first = false;
}

void Http_hdr::getValue(QString & key, const u_char * begin, int len){
    key = QString(len, 0);
    for (int i = 0; i < len; ++i){
        key[i] = begin[i];
    }

}

bool Http_hdr::checkApplication(const u_char * pkt, int appLen){
    string app(appLen, ' ');
    for(int i = 0 ; i < appLen; ++i){
        app[i] = char(pkt[i]);
    }
    QJsonObject obj = QJsonDocument::fromJson(QByteArray(app.c_str(), app.length())).object();
    if (obj.contains("Username") && obj.contains("Password")) return true;
    return false;
}

pair<QString, QString> Http_hdr::getToken(QString str){
    string token = QByteArray::fromBase64(str.toUtf8()).toStdString();
    int i = token.find_first_of(':');
    if (i == -1) return {};
    return make_pair(
                QString::fromStdString(token.substr(0, i)),
                QString::fromStdString(token.substr(i + 1, token.length() - i - 1)));
}
