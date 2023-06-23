#ifndef CLEVENTPROC_H
#define CLEVENTPROC_H
#include <QtGlobal>
#if defined(Q_OS_WIN)
#include "ClEventProc_win.h"
#elif defined(Q_OS_IOS)
#include "ClEventProc_ios.h"
#else
#include "ClEventProc_mac.h"
#endif
#endif CLEVENTPROC_H
