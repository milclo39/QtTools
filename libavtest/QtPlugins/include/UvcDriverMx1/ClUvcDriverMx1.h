#ifndef CLUVCDRIVERMX1_H
#define CLUVCDRIVERMX1_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverMx1 : public ClUvcDriver
{

public:
	explicit ClUvcDriverMx1(QObject *pParent = NULL);	//コンストラクタ
	virtual ~ClUvcDriverMx1();	//デストラクタ

	bool requestExtension(const QString& data);
	bool resultExtension(QString& data);
	bool keyStatus(QString& data);

protected:
	virtual bool initialize();	//初期化

	virtual GUID getExtensionNodeGuid(quint32);	//拡張ノードGUID取得

private:
	quint32 m_ulExtensionNodeNumber;	//!< 拡張ノード番号
};

#endif // CLUVCDRIVERMX1_H
