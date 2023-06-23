#ifndef CLLOGGER_H
#define CLLOGGER_H
#include <QDateTime>
#include <QDebug>
#include <QtDebug>
#define LOG(content) qDebug().noquote() << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << __FUNCTION__ << "[" << __LINE__ << "]" << content
#include <QTextStream>

class ClLogger
{
private:
    static ClLogger m_instance;                     ///< Instance
    QTextStream m_logStream;                        ///< Log stream
	bool	m_bNotOpen;								///< This flag was set when log file can not open
public:
    static ClLogger* getInstance();                 ///< Get instance
    void setup();                                   ///< Set up log environment
    static void appMessageHandler(
            QtMsgType type,
                const QMessageLogContext&, const QString & msg);       ///< Message handler
};
#endif // CLLOGGER_H
