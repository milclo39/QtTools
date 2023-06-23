#ifndef QTDXUTTIMER_H
#define QTDXUTTIMER_H

#include "CmnDefine.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#include <time.h>
#include <sys/time.h>
#endif

class QtDXUTTimer : public QObject
{
Q_OBJECT


public:
    explicit QtDXUTTimer(QObject *parent = 0);

	void resetFunc();					// resets the timer
	void startFunc();					// starts the timer
	void stopFunc();					// stop (or pause) the timer
	void advanceFunc();					// advance the timer by 0.1 seconds
	double getAbsoluteTime();			// get the absolute system time
	double getElapsedTime();			// get the time that elapsed between GetElapsedTime() calls
	bool isStopped();					// returns true if timer stopped

protected:
	bool m_bUsingQPF;
	bool m_bTimerStopped;
	qint64 m_llQPFTicksPerSec;

	qint64 m_llStopTime;
	qint64 m_llLastElapsedTime;
	qint64 m_llBaseTime;

signals:
	void absoluteTime(double);			//絶対時間の通知
	void elapsedTime(double);			//StartからStopまでの時間

public slots:
	void reset();						// resets the timer
	void start();						// starts the timer
	void stop();						// stop (or pause) the timer
	void advance();						// advance the timer by 0.1 seconds
};

#endif // QTDXUTTIMER_H
