#ifndef CLUVCDRIVERMO1_H
#define CLUVCDRIVERMO1_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverMo1 : public ClUvcDriver
{

public:
	explicit ClUvcDriverMo1(QObject *pParent = NULL);	//コンストラクタ
	virtual ~ClUvcDriverMo1();	//デストラクタ

protected:
	virtual bool initialize();	//初期化

	virtual GUID getExtensionNodeGuid(quint32);	//拡張ノードGUID取得

private:
	quint32 m_ulExtensionNodeNumber;	//!< 拡張ノード番号
};

#endif // CLUVCDRIVERMO1_H
