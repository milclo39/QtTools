#ifndef EVKMANAGER_H
#define EVKMANAGER_H
#include "AbstLibUsbManager.h"
#include <QUuid>

#define LIBUSB_CMD_USB			QUuid("{133790ca-d2f5-4afd-92f9-55c52e394fc3}")
#define EL_CMD_KEY_METHOD		"METHOD"
#define EL_CMD_KEY_PARAMCOUNT	"PARAMCOUNT"
#define EL_CMD_KEY_RESULT		"RESULT"
#define EL_CMD_KEY_JSON			"JSON"

class libUsbManager : public AbstLibUsbManager
{
public:
	libUsbManager();

protected:
	virtual QVector<QPair<quint16, quint16> > getSupportVendorProduct();

	virtual bool isAddCommand(EL_CMD cmd);
	virtual void procCommand(EL_CMD cmd);

private:
	int m_id;	//!< 通信ID
};
#endif
