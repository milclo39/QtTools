#ifndef CLQUICKVIEW_H
#define CLQUICKVIEW_H

#include <QQmlApplicationEngine>
#include <QQuickImageProvider>
#include <QQmlFileSelector>
#include <QQuickItem>

//QML
class ClQmlApplicationEngine : public QQmlApplicationEngine
{
    Q_OBJECT
public:
    explicit ClQmlApplicationEngine(QObject *parent = 0);

public slots:
	void slotRecvChangeLanguage(QString msg);
	void slotRecvListFilePath(QString type);
	void slotRecvImageDateTime(QString filepath);

signals:
	void sigSendListFilePath(QVariant list);
	void sigSendImageDateTime(QVariant datetime);

private:
	void loadQml();
	void deleteQml();

	QString m_clStrLanguage;
	QString m_clStrRccPath;
	QList<QMetaObject::Connection> m_connections;
};

//色変換 (RGB-<BGR)
class ImageProvider : public QQuickImageProvider
{
public:
	ImageProvider();
	~ImageProvider();

	QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};

//サムネイル取得
class ImageProviderThumbnail : public QQuickImageProvider
{
public:
	ImageProviderThumbnail();
	~ImageProviderThumbnail();

	QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
};
#endif // CLQUICKVIEW_H
