#ifndef ElWorker_H
#define ElWorker_H

#include <QObject>
#include <QQuickItem>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

class ElWorker : public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(QVariantMap notification READ notification WRITE setNotification NOTIFY notificationChanged)

public:
	//QMLへの登録
	static void registerElmoLibrary(){
		qmlRegisterSingletonType<ElWorker>("elmo.Worker",1,0,"ElWorker", ElWorker::qmlSingleton);
	}
	static QObject *qmlSingleton(QQmlEngine *, QJSEngine *){ return new ElWorker; }

	explicit ElWorker(QQuickItem *parent = 0);
	virtual ~ElWorker();

	// プロパティ用インタフェース（未使用）
	void setNotification(const QVariantMap &notification);
	QVariantMap notification() const;

public slots:
	// QML間インタフェース
	void setRequest(const QString strCmd, const QString strParam, const QString strValue);

signals:
	void notificationChanged();
	void sigSetUrl(QString);

protected:
	QJsonObject getObject(QString strMethod);
	// メッセージ送信
	int reqMsg_sys(int messageId, void *msg_data, int msg_data_size, void *result_addr, int result_max_size);

private:
	QVariantMap m_notification;
	QString m_strParam;
	QString m_strValue;
};

#endif // ElWorker_H
