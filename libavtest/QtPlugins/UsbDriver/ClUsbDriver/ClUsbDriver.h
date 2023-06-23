#include <QtCore/qglobal.h>

#if defined(Q_OS_MAC)
	#include "ClUsbDriver/ClUsbDriver_mac.h"
#endif
#if defined(Q_OS_LINUX)
	#include "ClUsbDriver/ClUsbDriver_linux.h"
#endif
#if defined(Q_OS_WIN)
	#include "ClUsbDriver/ClUsbDriver_win.h"
#endif
