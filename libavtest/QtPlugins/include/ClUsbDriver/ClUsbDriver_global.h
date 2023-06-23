#ifndef USBDRIVER_GLOBAL_H
#define USBDRIVER_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QMetaType>
#include <QDebug>
#include <QObject>
#include <QIODevice>
#include <QList>
#include <QVector>
#include <QThread>
#include <QByteArray>
#include <QChar>
#include <QVariant>
#include <QPixmap>
#include <QImage>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QDataStream>
#include <QTextStream>
#include <QBuffer>
#include <QFile>
#include <QFileDialog>
#include <QMutex>
#include <QUuid>
#include <QMap>
#include <QProcess>
#include <QQueue>
#include <QDir>
#include <QDirIterator>

#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#include <sys/socket.h>
#ifdef Q_OS_LINUX
#include <linux/netlink.h>
#include <libusb/libusb.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


#ifdef Q_OS_LINUX
// for v4l2

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>

#include <linux/videodev2.h>
#endif

#ifndef Q_OS_MAC
#define DWORD	quint32
#define WORD	quint16
#define ULONG	qint32
#define BYTE	quint8
#define PBYTE	quint8*
#endif

#elif defined(Q_OS_WIN)
#include <windows.h>
#include <wininet.h>
#include <windowsx.h>
#include <commctrl.h>

#include <dbt.h>
#include "setupapi.h"

#include "commctrl.h"
#if 0
#include "api/usbioctl.h"
#include "api/usbiodef.h"
#include "api/basetyps.h"
#include "api/devioctl.h"
#endif
#endif	//Q_OS_WIN

#include "ClUsbDriver_Errors.h"

enum eKINDSTREAM{				//
	JPEG_STREAM = 0,
	RAW_STREAM,
	TERM_STREAM
};

enum eKINDRECTID{
	RECTID_UNSUPPORT = 0,
	RECTID_PICTURE_HIGH = 1,
	RECTID_PICTURE_MID = 2,
	RECTID_PICTURE_LOW = 3,
	RECTID_PICTURE_DEFAULT
};

Q_DECLARE_METATYPE(eKINDSTREAM);
Q_DECLARE_METATYPE(eKINDRECTID);

enum eENDPOINT{
	eEP1,			//EndPoint1
	eEP2,			//EndPoint2
	eEP3,			//EndPoint3
	eEP4,			//EndPoint4
};
//--------------------------------------------------------------------
/*!
 *	@brief	Vendor ID for ELMO
 */
//--------------------------------------------------------------------
#define		VENDOR_NONE                 0x0000
#define		VENDOR_ELMO                 0x09a1
#define		VENDOR_SUNPLUS				0x04fc	//for CO-10
//--------------------------------------------------------------------
/*!
 *	@brief	Product ID for presenter of ELMO
 */
//--------------------------------------------------------------------
#define		PRODUCT_NONE                0x00000000UL
#define		PRODUCT_P10S                0x09a10008UL
#define		PRODUCT_L1N                 0x09a1000EUL
#define		PRODUCT_P100                0x09a1000FUL
#define		PRODUCT_FF1                 0x09a10012UL
#define		PRODUCT_P30S                0x09a10013UL
#define		PRODUCT_L1EX                0x09a10015UL
#define		PRODUCT_P10                 0x09a10019UL
#define		PRODUCT_MC1EX               0x09a1001BUL
#define		PRODUCT_SESAME              0x09a1001DUL

#define		PRODUCT_CO10                0x04fc0156UL

#define		PRODUCT_MOBILE				0x09a1001FUL
#define		PRODUCT_TX1					0x09a10020UL
#define		PRODUCT_MX1					0x09a10021UL
#define		PRODUCT_PX1					0x09a10022UL

inline		qint32 ATTACH_V2P(qint32 vendor,qint32 product)
{
	union{
		qint32 attached;
		struct{
#ifdef Q_LITTLE_ENDIAN
			qint16 split_l;
			qint16 split_h;
#else
			qint16 split_h;
			qint16 split_l;
#endif
		};
	}conv;
	conv.split_l = product;
	conv.split_h = vendor;
	return conv.attached;
}

#define		GETVENDORID(p)				(quint16)((((quint32)p) >> 16) & 0x0000ffffUL)
#define		GETPRODUCTID(p)				(quint16)(((quint32)p) & 0x0000ffffUL)

//--------------------------------------------------------------------
/*!
 *	@brief	Product name for presenter of ELMO
 */
//--------------------------------------------------------------------
#define		PRODUCTNAME_P10S             "P10S"
#define		PRODUCTNAME_L1N              "L1N"
#define		PRODUCTNAME_P100             "P100"
#define		PRODUCTNAME_FF1              "FF1"
#define		PRODUCTNAME_P30S             "P30S"
#define		PRODUCTNAME_L1EX             "L1EX"
#define		PRODUCTNAME_P10              "P10"
#define		PRODUCTNAME_MC1EX            "MC1EX"
#define		PRODUCTNAME_L12              "L12"

#define		PRODUCTNAME_CO10             "CO10"

#define		PRODUCTNAME_MOBILE           "MO-1/MO-2"

//--------------------------------------------------------------------
/*!
 *	@brief	DEBUG表示用プラグマメッセージヘルパーマクロ
 */
//--------------------------------------------------------------------
#ifndef __USB_DEBUG
#ifdef __DEBUG
#undef __DEBUG
#endif
#endif

#endif // USBDRIVER_GLOBAL_H
