
#include "ElWorker.h"

ElWorker::ElWorker(QQuickItem *parent)
	: QQuickItem(parent)
{
}

ElWorker::~ElWorker()
{
}

void ElWorker::setNotification(const QVariantMap &notification)
{
	m_notification = notification;
}

QVariantMap ElWorker::notification() const
{
	return m_notification;
}

void ElWorker::setRequest(const QString strCmd, const QString strParam = "", const QString strValue = "")
{
	m_strParam = strParam;
	m_strValue = strValue;
	QJsonDocument doc(getObject(strCmd));
	QByteArray qArray(doc.toJson(QJsonDocument::Compact));
	reqMsg_sys(0, qArray.data(), qArray.size(), NULL, 0);	// メッセージ送信
	qDebug(qArray);
}

QJsonObject ElWorker::getObject(QString strMethod)
{
	QJsonObject	l_clObjParamRoot;
	QJsonObject jsonObjDetail;

	l_clObjParamRoot["dest"] = "system";
	l_clObjParamRoot["src"] = "screen";
	if("zoom" == strMethod)
	{
		QJsonObject jsonObj;
		l_clObjParamRoot["category"] = "button";
		jsonObj[m_strParam] = m_strValue.toInt();
		jsonObjDetail[strMethod] = jsonObj;
		l_clObjParamRoot["command"] = jsonObjDetail;
	}
	else if("resolution" == strMethod)
	{
		l_clObjParamRoot["category"] = "request";
		jsonObjDetail["name"] = strMethod;
		jsonObjDetail["param"] = m_strParam;
		l_clObjParamRoot["command"] = jsonObjDetail;
	}
	else if("touch" == strMethod || "tap" == strMethod)
	{
		l_clObjParamRoot["category"] = "touch";
		l_clObjParamRoot["command"] = strMethod;
	}
	else if("output image angle" == strMethod)
	{
		l_clObjParamRoot["category"] = "request";
		jsonObjDetail["name"] = "output image angle";
		jsonObjDetail["param"] = "normal";
		l_clObjParamRoot["command"] = jsonObjDetail;
	}
	else if("get status" == strMethod)
	{
		l_clObjParamRoot["category"] = "status";
		jsonObjDetail["name"] = "get status";
		jsonObjDetail["param"] = m_strParam;
		l_clObjParamRoot["command"] = jsonObjDetail;
	}
	else
	{	// パラメータなし(とりあえずボタンにしておく)
		l_clObjParamRoot["category"] = "button";
		l_clObjParamRoot["command"] = strMethod;
	}

	return l_clObjParamRoot;
}

int ElWorker::reqMsg_sys(int messageId, void *msg_data = NULL, int msg_data_size = 0,
			   void *result_addr = NULL, int result_max_size = 0)
{
	Q_UNUSED(messageId)
	Q_UNUSED(msg_data)
	Q_UNUSED(msg_data_size)
	Q_UNUSED(result_addr)
	Q_UNUSED(result_max_size)
	return 0;
}
