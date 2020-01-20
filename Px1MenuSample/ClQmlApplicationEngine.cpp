#include "ClQmlApplicationEngine.h"
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QResource>
#include <QFile>
#include <QDir>
#include <QDebug>

#include "ClScreenMsgProc.h"

static QMap<QString, QString> g_clMapImageDateTime;

//QML
ClQmlApplicationEngine::ClQmlApplicationEngine(QObject *parent) : QQmlApplicationEngine(parent)
{
	//初期リソース登録
	m_clStrLanguage = "J";
#if defined(Q_OS_WIN)
	m_clStrRccPath = "resource/resource_screen_j.rcc";
#else
	m_clStrRccPath = "/usr/share/misc/resource_screen_j.rcc";
#endif
	QResource::registerResource(m_clStrRccPath);
#if 1 //matsubara add
	//登録
	ClScreenMsgProc::registerElmoLibrary();
#endif
	//QML読込
	this->loadQml();
}
void ClQmlApplicationEngine::loadQml()
{
	this->addImageProvider(QString("image_provider"), new ImageProvider);
	this->addImageProvider(QString("image_provider_thumbnail"), new ImageProviderThumbnail);
	this->load(QUrl(QStringLiteral("qrc:/qmls/main.qml")));

	ClScreenMsgProc *pMsgProc = ClScreenMsgProc::getInstance();
	m_connections << QObject::connect(pMsgProc, SIGNAL(sigSendChangeLanguage(QString)), this, SLOT(slotRecvChangeLanguage(QString)));
	m_connections << QObject::connect(pMsgProc, SIGNAL(sigSendListFilePath(QString)), this, SLOT(slotRecvListFilePath(QString)));
	m_connections << QObject::connect(pMsgProc, SIGNAL(sigSendImageDateTime(QString)), this, SLOT(slotRecvImageDateTime(QString)));
	m_connections << QObject::connect(this, SIGNAL(sigSendListFilePath(QVariant)), pMsgProc, SLOT(slotRecvListFilePath(QVariant)));
	m_connections << QObject::connect(this, SIGNAL(sigSendImageDateTime(QVariant)), pMsgProc, SLOT(slotRecvImageDateTime(QVariant)));
}
void ClQmlApplicationEngine::deleteQml()
{
	foreach(auto var, m_connections)
	{
		QObject::disconnect(var);
	}
	this->removeImageProvider(QString("image_provider"));
	this->removeImageProvider(QString("image_provider_thumbnail"));
	this->clearComponentCache();
	this->rootObjects().first()->deleteLater();
	this->loadQml();
}

void ClQmlApplicationEngine::slotRecvChangeLanguage(QString msg)
{
	QResource::unregisterResource(m_clStrRccPath);
	if(QString::compare(msg, "J") == 0)
	{
		m_clStrLanguage = "J";
#if defined(Q_OS_WIN)
		m_clStrRccPath = "resource/resource_screen_j.rcc";
#else
		m_clStrRccPath = "/usr/share/misc/resource_screen_j.rcc";
#endif
	}
	else if(QString::compare(msg, "D") == 0)
	{
		m_clStrLanguage = "D";
#if defined(Q_OS_WIN)
		m_clStrRccPath = "resource/resource_screen_d.rcc";
#else
		m_clStrRccPath = "/usr/share/misc/resource_screen_d.rcc";
#endif
	}
	else if(QString::compare(msg, "F") == 0)
	{
		m_clStrLanguage = "F";
#if defined(Q_OS_WIN)
		m_clStrRccPath = "resource/resource_screen_f.rcc";
#else
		m_clStrRccPath = "/usr/share/misc/resource_screen_f.rcc";
#endif
	}
	else if(QString::compare(msg, "A") == 0)
	{
		m_clStrLanguage = "A";
#if defined(Q_OS_WIN)
		m_clStrRccPath = "resource/resource_screen_a.rcc";
#else
		m_clStrRccPath = "/usr/share/misc/resource_screen_a.rcc";
#endif
	}
	else
	{
		m_clStrLanguage = "E";
#if defined(Q_OS_WIN)
		m_clStrRccPath = "resource/resource_screen_e.rcc";
#else
		m_clStrRccPath = "/usr/share/misc/resource_screen_e.rcc";
#endif
	}
	QResource::registerResource(m_clStrRccPath);
	deleteQml();
}

void ClQmlApplicationEngine::slotRecvListFilePath(QString type)
{
	g_clMapImageDateTime.clear();
	QStringList listFilePath;
	QDir clDir("/mnt");
	QFileInfoList clList = clDir.entryInfoList();										//指定パス内のファイルリストを取得、この時点で更新日時順にソート
	while(clList.isEmpty() == false)
	{
		QString clStrFilePath = clList.first().filePath();								//ファイルパス
		if(type.contains("image") && !clStrFilePath.contains("jpg"))
		{
			/* 何もしない */
		}
		else if(QFile::exists(clStrFilePath) == true && clStrFilePath.contains("jpg"))
		{
			listFilePath.append("image://image_provider_thumbnail/" + clStrFilePath);
		}
		clList.removeFirst();
	}
#if 1 //テスト用
	if(type.contains("image"))
	{
		listFilePath.append("image://image_provider_thumbnail/IMG_20171107170500.JPG");
	}
#endif
	emit sigSendListFilePath(listFilePath);
}
void ClQmlApplicationEngine::slotRecvImageDateTime(QString filepath)
{
	QString clStrDateTime = g_clMapImageDateTime.value(filepath.replace("image://image_provider_thumbnail/",""));
	if(clStrDateTime.isEmpty())
	{
		clStrDateTime = QString("0000:00:00 00:00:00");
	}
	emit sigSendImageDateTime(clStrDateTime);
}

//色変換 (RGB-<BGR)
ImageProvider::ImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}
ImageProvider::~ImageProvider()
{
}
QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
	QImage clImage;
	if (id.contains("qrc:///"))
	{
		QString str = id;
		str = str.replace("qrc:///", ":/");
		clImage.load(str);
	}
	else
	{
		clImage.load(id);
	}

	int width = clImage.width();
	int height = clImage.height();

	if (size)
	*size = QSize(width, height);
	QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
				   requestedSize.height() > 0 ? requestedSize.height() : height);
	pixmap = QPixmap::fromImage(clImage.rgbSwapped());

	return pixmap;
}

//サムネイル取得
ImageProviderThumbnail::ImageProviderThumbnail() : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}
ImageProviderThumbnail::~ImageProviderThumbnail()
{
}
QPixmap ImageProviderThumbnail::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
	Q_UNUSED(requestedSize)

	QByteArray clArrayThumbnail;
	char cMarker[1];
	unsigned char cStartTag1[] = {0xFF};
	unsigned char cStartTag2[] = {0xD8};
	unsigned char cEndTag1[] = {0xFF};
	unsigned char cEndTag2[] = {0xD9};
	int iLength = 0;
	bool bExistTunmbnail = false;

	QFile *pclFileTemp = new QFile(id);
	pclFileTemp->open(QIODevice::ReadOnly);

	QDataStream clStreamIn(pclFileTemp);
	pclFileTemp->seek(pclFileTemp->pos() + 2);
	while (!clStreamIn.atEnd())
	{
		clStreamIn.readRawData(cMarker,1);
		if (memcmp(cMarker, cStartTag1,1) == 0)
		{
			clStreamIn.readRawData(cMarker,1);
			if (memcmp(cMarker, cStartTag2, 1) == 0)
			{
				iLength = 2;
				bExistTunmbnail = true;
#if 1 //ファイル変更日時取得
				pclFileTemp->seek(pclFileTemp->pos() - 22);
				QByteArray tmpDateTime = pclFileTemp->read(22);
				g_clMapImageDateTime.insert(id, QString("%1").arg(tmpDateTime.constData()));
#endif
				break;
			}
		}
	}

	if (bExistTunmbnail)
	{
		while (!clStreamIn.atEnd())
		{
			clStreamIn.readRawData(cMarker,1);
			iLength += 1;
			if (memcmp(cMarker, cEndTag1, 1) == 0)
			{
				clStreamIn.readRawData(cMarker, 1);
				iLength += 1;
				if (memcmp(cMarker, cEndTag2, 1) == 0)
				{
					pclFileTemp->seek(pclFileTemp->pos() - iLength);
					clArrayThumbnail = pclFileTemp->read(iLength);
					break;
				}
			}
		}
	}

	QImage clImage = QImage::fromData(clArrayThumbnail, "jpg");
	int width = clImage.width();
	int height = clImage.height();
	*size = QSize(width, height);

	return QPixmap::fromImage(clImage.rgbSwapped());
}
