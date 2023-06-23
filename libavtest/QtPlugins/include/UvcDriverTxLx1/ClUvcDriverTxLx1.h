#ifndef CLUVCDRIVERTXLX1_H
#define CLUVCDRIVERTXLX1_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverTxLx1 : public ClUvcDriver
{

public:
	explicit ClUvcDriverTxLx1(QObject *pParent = NULL);	//コンストラクタ
	virtual ~ClUvcDriverTxLx1();	//デストラクタ

	bool requestExtension(const QString& data);
	bool resultExtension(QString& data);

protected:
	virtual bool initialize();	//初期化

	virtual GUID getExtensionNodeGuid(quint32);	//拡張ノードGUID取得

private:
	quint32 m_ulExtensionNodeNumber;	//!< 拡張ノード番号
};

#endif // CLUVCDRIVERTXLX1_H
