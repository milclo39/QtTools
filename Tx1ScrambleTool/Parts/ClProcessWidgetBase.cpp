//--------------------------------------------------------------------------------//
/*!
	@file	ClProcessWidgetBase.cpp
	@brief	BaseWidget
	@author	星島
*/
//--------------------------------------------------------------------------------//
#include "ClProcessWidgetBase.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QByteArray>
#include <QScrollArea>
#include <QDebug>
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/err.h"

#include "KeyDefine.h"

#define INDICATOR_BG_COLOR		QColor(255, 255, 255, 100)
#define INDICATOR_FG_COLOR		QColor(0, 0, 255, 100)

//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
	@param[in] pclParent : 親
*/
//--------------------------------------------------------------------------------//
ClProcessWidgetBase::ClProcessWidgetBase(QWidget *pclParent) : QWidget(pclParent)
{
	m_pclProcessLayout = new QStackedLayout();
	setLayout(m_pclProcessLayout);

	m_pclIndicator = new ClProgressIndicator("", this);
	m_pclIndicator->setMinimumSize(QSize(300, 300));
	m_pclIndicator->move(50, 0);

	setOverlayIndicator(true);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	デストラクタ
*/
//--------------------------------------------------------------------------------//
ClProcessWidgetBase::~ClProcessWidgetBase()
{
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
 @brief　初期化
*/
//--------------------------------------------------------------------------------//
bool ClProcessWidgetBase::initalize()
{
	QScrollArea *pclScrollArea = new QScrollArea;
	pclScrollArea->setWidgetResizable(true);
	pclScrollArea->setFrameShape(QFrame::NoFrame);
	QWidget *pclWidget = createStartWidget();
	pclWidget->setContentsMargins(10, 10, 10, 10);
	pclScrollArea->setWidget(pclWidget);
	m_pclProcessLayout->addWidget(pclScrollArea);	// スクロールエリアにのせる
	m_pclProcessLayout->addWidget(createIndicatorWidget());
	return true;
}

//--------------------------------------------------------------------------------//
/*!
 @brief　開始Widget生成
*/
//--------------------------------------------------------------------------------//
QWidget *ClProcessWidgetBase::createStartWidget()
{
	QWidget *pclStartWidget = new QWidget();
	QVBoxLayout *pclLayout = new QVBoxLayout();
	pclStartWidget->setLayout(pclLayout);

	return pclStartWidget;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
 @brief　インジケータWidget生成
*/
//--------------------------------------------------------------------------------//
QWidget *ClProcessWidgetBase::createIndicatorWidget()
{
	QWidget *pclIndicatorWidget = new QWidget();
	return pclIndicatorWidget;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
 @brief　Json形成
*/
//--------------------------------------------------------------------------------//
QByteArray ClProcessWidgetBase::getObject(QString strCmd)
{
	QJsonObject	l_clObjParamRoot;
	Q_UNUSED(strCmd);

	return QJsonDocument(l_clObjParamRoot).toJson();
}
//--------------------------------------------------------------------------------//



static const unsigned char INI_IV_VEC[AES_BLOCK_SIZE] = {
	0x7d, 0xbd, 0xaa, 0x19, 0x38, 0x94, 0x45, 0x52, 0x64, 0x67, 0x4d, 0x9f, 0xb1, 0x48, 0xb5, 0x2e
};


//--------------------------------------------------------------------------------//
/*!
 @brief　encrypt
*/
//--------------------------------------------------------------------------------//
QByteArray ClProcessWidgetBase::encrypt(const QByteArray qCommand)
{
	if (qCommand.isEmpty() || m_AESKey.isNull()) {
		return QByteArray();
	}

	AES_KEY key;
	int ret = AES_set_encrypt_key((unsigned char*)m_AESKey.data(),  m_AESKey.length() * 8, &key);

	if (ret != 0) {
		qDebug() << "ERROR: Could not set encrypt key: " << ERR_error_string(ERR_get_error(), NULL);
		return QByteArray();
	}

	unsigned char iv_enc[AES_BLOCK_SIZE];
	memcpy(iv_enc, INI_IV_VEC, sizeof(iv_enc));
	unsigned long commandLength = qCommand.size();
	size_t iActCmdLen = ((commandLength + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
	QByteArray qActCmdData(iActCmdLen,'\0');
	qActCmdData = qCommand;
	qActCmdData.resize(iActCmdLen);
	size_t iAesTotalSize = AES_BLOCK_SIZE + iActCmdLen;

	QByteArray qEncryptData(iAesTotalSize,'\0');
	AES_cbc_encrypt((const unsigned char*)&commandLength, (uchar *)qEncryptData.data(), sizeof(commandLength), &key, iv_enc, AES_ENCRYPT);
	AES_cbc_encrypt((const unsigned char*)qActCmdData.data(), (uchar *)(qEncryptData.data()+AES_BLOCK_SIZE), qActCmdData.size(), &key, iv_enc, AES_ENCRYPT);
	qDebug() << "aes encrypt:" << commandLength << "byte";
	return qEncryptData;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
 @brief　decrypt
*/
//--------------------------------------------------------------------------------//
QByteArray ClProcessWidgetBase::decrypt(QByteArray qArryData)
{
	if(qArryData.isEmpty() || qArryData.size() < AES_BLOCK_SIZE){
		return QByteArray();
	}

	AES_KEY key;
	int ret = AES_set_decrypt_key((unsigned char*)m_AESKey.data(), m_AESKey.length() * 8, &key);
	if (ret != 0) {
		qDebug() << "ERROR: Could not set decrypt key: " << ERR_error_string(ERR_get_error(), NULL);
		return QByteArray();
	}

	unsigned char iv_enc[AES_BLOCK_SIZE];
	memcpy(iv_enc, INI_IV_VEC, sizeof(iv_enc));
	unsigned char tmpBuffer[AES_BLOCK_SIZE];
	AES_cbc_encrypt((uchar *)qArryData.data(), tmpBuffer, AES_BLOCK_SIZE, &key, iv_enc, AES_DECRYPT);
	unsigned long commandLength = *((unsigned long*)tmpBuffer);
	size_t bufferLength = ((commandLength + AES_BLOCK_SIZE - 1) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
#if 0
	if(bufferLength > 0x20000){	// 128KB以上はおかしい
		qDebug() << "ERROR: Could not set decrypt key: " << ERR_error_string(ERR_get_error(), NULL);
		return QByteArray();
	}
#endif
	unsigned char* decrypted(new unsigned char[bufferLength]);
	AES_cbc_encrypt((uchar *)(qArryData.data()+AES_BLOCK_SIZE), decrypted, qArryData.size()-AES_BLOCK_SIZE, &key, iv_enc, AES_DECRYPT);
	qDebug() << "aes decrypt:" << bufferLength << "byte";
	return QByteArray((const char*)decrypted, commandLength);
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
 @brief　RSA_encrypt
*/
//--------------------------------------------------------------------------------//
QByteArray ClProcessWidgetBase::RSA_encrypt(const QByteArray input)
{
#if 0
	BIO *bio = BIO_new_mem_buf((void*)CP1_PUBKEY, -1);
	//BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
#else
	BIO *bio = BIO_new_mem_buf((void*)PUBKEY, -1);
	RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, const_cast<char*>("spyder"));
#endif
	BIO_free(bio);
	if (rsa == NULL) {
		qDebug("ERROR: Could not load public key! PEM_read_bio_RSA_PUBKEY: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}
	int maxSize = RSA_size(rsa);
	QByteArray rsaCrypto(maxSize+1, '\0');
//	rsaCrypto.resize(maxSize+1);
	int length = RSA_public_encrypt((int)input.size(), (unsigned char*)input.data(),
								(unsigned char*)rsaCrypto.data(), rsa, RSA_PKCS1_OAEP_PADDING);
	RSA_free(rsa);
	if (length == -1) {
		qDebug("ERROR: Failed to encrypt: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}
//	rsaCrypto[length] = '\0';
	rsaCrypto.resize(length);
	return rsaCrypto;
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
 @brief　RSA_decrypt
*/
//--------------------------------------------------------------------------------//
QByteArray ClProcessWidgetBase::RSA_decrypt(const QByteArray rsaCrypto)
{
	BIO *bio = BIO_new_mem_buf((void*)PRIVKEY, -1);
	RSA *rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, const_cast<char*>("spyder"));
	BIO_free(bio);
	if (rsa == NULL) {
		qDebug("ERROR: Could not load private key! PEM_read_bio_RSAPrivateKey: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}
	int maxSize = RSA_size(rsa);
	QByteArray output(maxSize+1, '\0');
//	output.resize(maxSize+1);
	int length = RSA_private_decrypt((int)rsaCrypto.size(), (const unsigned char*)rsaCrypto.data(),
									 (unsigned char*)output.data(), rsa, RSA_PKCS1_OAEP_PADDING);	//
	RSA_free(rsa);
	if (length == -1) {
		qDebug("ERROR: Failed to decrypt: %s\n", ERR_error_string(ERR_get_error(), NULL));
		return NULL;
	}
//	output[length] = '\0';
	output.resize(length);
	return output;
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
 @brief　base64
*/
//--------------------------------------------------------------------------------//
QString ClProcessWidgetBase::getSecret(int secretLength)
{
	QString strSecret;
	QVector<uchar> secretBytes(secretLength);

	RAND_bytes(secretBytes.data(), secretLength);

	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* out = BIO_new(BIO_s_mem());
	BIO_push(b64, out);
	BIO_write(b64, secretBytes.data(), secretLength);
	BIO_flush(b64);
	BUF_MEM *pout;
	BIO_get_mem_ptr(b64, &pout);

	strSecret.append(QByteArray(pout->data, secretLength));
	BIO_free_all(b64);

	return strSecret;
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
 @brief　オーバーレイインジケータ表示切替
 @param[in] bIsUse : 表示false,非表示true
*/
//--------------------------------------------------------------------------------//
void ClProcessWidgetBase::setOverlayIndicator(bool bIsUse)
{
	m_pclIndicator->raise();
	m_pclIndicator->setHide(bIsUse);
}
//--------------------------------------------------------------------------------//
