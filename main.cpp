#include <QCoreApplication>
#include <QString>
#include <QJsonObject>
#include <QDebug>

#include "Writer.h"

#include <vector>
#include <map>
#include <utility>

#include "Parser.h"

using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString fileConfig = "config.txt";// = "D:/Parser tcpdump/qt/config.txt";
    QString globalPath = "";// = "D:/Parser tcpdump/dumps";
    QString outPath = "";
    int threads = 5;// = 5;
    for(int i = 0; i < argc; ++i){
        if ((i + 1 < argc) &&
                (QString::fromStdString(argv[i]) == "--dir" ||
                 QString::fromStdString(argv[i]) == "-d")){
            globalPath = QString::fromStdString(argv[i + 1]);
            if (globalPath.left(1) == "\"" && globalPath.right(1) == "\""){
                globalPath = globalPath.mid(1, globalPath.length() - 2);
            }
            ++i;
            continue;
        }
        if ((i + 1 < argc) &&
                (QString::fromStdString(argv[i]) == "--out" ||
                 QString::fromStdString(argv[i]) == "-o")){
            outPath = QString::fromStdString(argv[i + 1]);
            if (outPath.left(1) == "\"" && outPath.right(1) == "\""){
                outPath = outPath.mid(1, outPath.length() - 2);
            }
            ++i;
            continue;
        }
        if ((i + 1 < argc) &&
                (QString::fromStdString(argv[i]) == "--threads" ||
                 QString::fromStdString(argv[i]) == "-t")){
            threads = QString::fromStdString(argv[i + 1]).toInt();
            threads = min(threads, QThread::idealThreadCount() - 2);
            ++i;
            continue;
        }
        if ((i + 1 < argc) &&
                (QString::fromStdString(argv[i]) == "--config" ||
                 QString::fromStdString(argv[i]) == "-c")){
            fileConfig = QString::fromStdString(argv[i + 1]);
            if (fileConfig.left(1) == "\"" && fileConfig.right(1) == "\""){
                fileConfig = fileConfig.mid(1, fileConfig.length() - 2);
            }
            ++i;
            continue;
        }
        if (
                (QString::fromStdString(argv[i]) == "--help" ||
                 QString::fromStdString(argv[i]) == "/?")){
            qDebug() << "  Analyze dump of network traffic.                ";
            qDebug() << "  PARSER [--dir | -d[:]source] [--config | -c[:]configuration file]";
            qDebug() << "      [--threads | -t[:]count of threads] [--out | -o[:]report]    ";
            qDebug() << "";
            qDebug() << "  Parameters:\n";
            qDebug() << "    --dir\t Directory with dump for analyze.       ";
            qDebug() << "    --config\t Json-formatted file with some parameters.        ";
            qDebug() << "    --threads\t Count of threads that will be used for analyze.    ";
            qDebug() << "    --out\t Reports will be save in that directory.";
            qDebug() << "";
            qDebug() << "  By default:\n";
            qDebug() << "    Configuration file selected in tool directory.";
            qDebug() << "    Source directory selected for save reports.";
            qDebug() << "    The count of threads selected as 5.";
            qDebug() << "";
            qDebug() << "  Example of command stores in tool directory.";
            qDebug() << "";
            return 0;
        }
    }
    bool flag = true;
    if (globalPath == ""){
        qDebug() << "Select directory with dumps.";
        flag = false;
    }
    if (outPath == "") {
        outPath = globalPath;
    }
    if (!QFileInfo(fileConfig).isFile()){
        qDebug() << "Select file with configuration.";
        flag = false;
    }
    if (!flag){
        return 0;
    }

    qDebug() << "File configuration - \t" << fileConfig;
    qDebug() << "Parsing directory - \t" << globalPath;
    qDebug() << "Path with result - \t" << outPath;
    qDebug() << "Count of threads - \t" << threads;

    ios::sync_with_stdio(0);
    cin.tie(NULL);
    cout.tie(NULL);



    Parser *p = new Parser();
    p->init(fileConfig, globalPath, threads);
    p->writer = new Writer(globalPath, &p->attributes);
    p->writer->outPath = outPath;

    p->startScan();

    delete p;
    return a.exec();
}
