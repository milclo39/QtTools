#ifndef CLHIDTHREAD_H
#define CLHIDTHREAD_H

#include <QThread>

class ClHidThread : public QObject
{
	Q_OBJECT
public:
	explicit ClHidThread();
	~ClHidThread();

public:
	bool isRunning();
	qint32 m_iVid;
	qint32 m_iPid;

signals:
	void sigRecv(QByteArray);

private slots:
	void procThread();

private:
	QThread *_thread;
	bool _bActive;
	QByteArray m_clTabletData;
};

#endif // CLHIDTHREAD_H
