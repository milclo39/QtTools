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

#include "ClNDADriver_Errors.h"

//--------------------------------------------------------------------
/*!
 *	@brief	DEBUG表示用プラグマメッセージヘルパーマクロ
 */
//--------------------------------------------------------------------
#ifndef __SX_DEBUG
#ifdef __DEBUG
#undef __DEBUG
#endif
#endif

#endif // USBDRIVER_GLOBAL_H
