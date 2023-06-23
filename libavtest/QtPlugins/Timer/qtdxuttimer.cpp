#include "QtdxutTimer/qtdxuttimer.h"
#include "../Common/CmnDefine.h"

//--------------------------------------------------------------------------------------
QtDXUTTimer::QtDXUTTimer(QObject *parent) :
	QObject(parent)
{
	m_bUsingQPF         = false;
	m_bTimerStopped     = true;
	m_llQPFTicksPerSec  = 0;

	m_llStopTime        = 0;
	m_llLastElapsedTime = 0;
	m_llBaseTime        = 0;
#ifdef Q_OS_WIN

	// Use QueryPerformanceFrequency() to get frequency of timer.
	LARGE_INTEGER qwTicksPerSec;
	m_bUsingQPF = (bool) (QueryPerformanceFrequency( &qwTicksPerSec ) != 0);
	m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
#endif
}

//--------------------------------------------------------------------------------------
void QtDXUTTimer::resetFunc()
{
#ifdef Q_OS_WIN
	if( !m_bUsingQPF )
		return;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	m_llBaseTime        = qwTime.QuadPart;
	m_llLastElapsedTime = qwTime.QuadPart;
//#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)
	double fTime = 0;
	timeval tv;
	gettimeofday(&tv, NULL);
	fTime = tv.tv_sec + (double)tv.tv_usec*1e-6;
	m_llBaseTime        = fTime;
	m_llLastElapsedTime = fTime;
#endif
	m_llStopTime        = 0;
	//	m_bTimerStopped     = FALSE;
	m_bTimerStopped     = false;
}
//--------------------------------------------------------------------------------------
void QtDXUTTimer::startFunc()
{
#ifdef Q_OS_WIN
	if( !m_bUsingQPF )
		return;

	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	if( m_bTimerStopped )
		m_llBaseTime += qwTime.QuadPart - m_llStopTime;
	m_llLastElapsedTime = qwTime.QuadPart;
//#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)
	double fTime = 0;
	timeval tv;
	if( m_llStopTime != 0 ){
		fTime = m_llStopTime;
	}else{
		gettimeofday(&tv, NULL);
		fTime = tv.tv_sec + (double)tv.tv_usec*1e-6;
	}
	if( m_bTimerStopped )
		m_llBaseTime += fTime - m_llStopTime;
	m_llLastElapsedTime = fTime;

#endif
	m_llStopTime = 0;
	//	m_bTimerStopped = FALSE;
	m_bTimerStopped = false;

}

//--------------------------------------------------------------------------------------
void QtDXUTTimer::stopFunc()
{
#ifdef Q_OS_WIN
	if( !m_bUsingQPF )
		return;
	if( !m_bTimerStopped )
	{
		// Get either the current time or the stop time
		LARGE_INTEGER qwTime;
		if( m_llStopTime != 0 )
			qwTime.QuadPart = m_llStopTime;
		else
			QueryPerformanceCounter( &qwTime );

		m_llStopTime = qwTime.QuadPart;
		m_llLastElapsedTime = qwTime.QuadPart;
	}
//#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)
	double fTime = 0;
	timeval tv;
	if( m_llStopTime != 0 ){
		fTime = m_llStopTime;
	}else{
		gettimeofday(&tv, NULL);
		fTime = tv.tv_sec + (double)tv.tv_usec*1e-6;
	}
	m_llStopTime = fTime;
	m_llLastElapsedTime = fTime;
#endif
	//	m_bTimerStopped = TRUE;
	m_bTimerStopped = true;
}


//--------------------------------------------------------------------------------------
void QtDXUTTimer::advanceFunc()
{
#ifdef Q_OS_WIN
	if( !m_bUsingQPF )
		return;
	m_llStopTime += m_llQPFTicksPerSec/10;
#endif
}
//--------------------------------------------------------------------------------------
double QtDXUTTimer::getElapsedTime()
{
	double fTime = 0;
	if(m_bTimerStopped){
		fTime = m_llStopTime - m_llBaseTime;
	}else{
		fTime = getAbsoluteTime() - m_llBaseTime;
	}
	emit elapsedTime(fTime);
	return fTime;
}

//--------------------------------------------------------------------------------------
double QtDXUTTimer::getAbsoluteTime()
{
	double fTime = 0;
#ifdef Q_OS_WIN
	if( !m_bUsingQPF )
		return -1.0;
	// Get either the current time or the stop time
	LARGE_INTEGER qwTime;
	if( m_llStopTime != 0 )
		qwTime.QuadPart = m_llStopTime;
	else
		QueryPerformanceCounter( &qwTime );

	fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
//#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)
	timeval tv;
	gettimeofday(&tv, NULL);
	fTime = tv.tv_sec + (double)tv.tv_usec*1e-6;
#endif
	emit absoluteTime(fTime);
	return fTime;
}


//--------------------------------------------------------------------------------------
void QtDXUTTimer::reset()
{
	resetFunc();
}


//--------------------------------------------------------------------------------------
void QtDXUTTimer::start()
{
	startFunc();
}


//--------------------------------------------------------------------------------------
void QtDXUTTimer::stop()
{
	stopFunc();
}


//--------------------------------------------------------------------------------------
void QtDXUTTimer::advance()
{
	advanceFunc();
}

