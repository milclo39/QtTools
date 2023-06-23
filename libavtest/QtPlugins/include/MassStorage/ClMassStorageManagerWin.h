//--------------------------------------------------------------------------------//
/*!
	@file	ClMassStorageManagerWin.h
	@brief	MassStorage
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef CLMASSSTORAGEMANAGERWIN_H
#define CLMASSSTORAGEMANAGERWIN_H

#include <QObject>

#include "WinUsbDataTransferClient.h"
#include "../Common/CmnDefine.h"
#include "common/Command.h"

class ClMassStorageManagerWin : public QThread
{
	Q_OBJECT
public:
	ClMassStorageManagerWin();
	~ClMassStorageManagerWin();

	void initialize();
protected slots:
	void slotChangeDevie(bool bIsConnect);
	void slotSendCommand(EL_CMD clCommand);

signals:
	void sigRecvCmd(EL_CMD clCommand);
protected:
	void recvCommand(QByteArray recvData);
	void run();
protected:
	spyder::WinUsbDataTransferClient m_clTransfer;
	QString m_strMethod;
	bool m_bIsConnect;
	QByteArray m_clData;
};

#endif // CLMASSSTORAGEMANAGERWIN_H
