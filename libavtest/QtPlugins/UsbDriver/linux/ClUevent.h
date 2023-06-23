#ifndef CLUEVENT_H
#define CLUEVENT_H

#include "ClUsbDriver/ClUsbDriver_global.h"

class ClUevent : public QThread
{
	Q_OBJECT

#define RECV_TIMEOUT	(0)							//send timeout by select
public:
	ClUevent(QObject *parent = 0);

protected:
	bool					bTerminate;				//Terminate flag
	void run();										//main

	qint32 lFd;

signals:
	void sigFatal();								//is sent when fatal error
	void sigTerminate();							//is sent when terminate thread
	void sigUevent(QString);						//is sent when receive event from OS
public slots:
	void slotLoopTerminate();						//Signal is end of loop

};

#endif // CLUEVENT_H
