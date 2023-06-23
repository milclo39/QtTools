#ifndef CLUVCDRIVERPX1_H
#define CLUVCDRIVERPX1_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverPx1 : public ClUvcDriver
{

public:
	explicit ClUvcDriverPx1(QObject *pParent = NULL);	//コンストラクタ
	virtual ~ClUvcDriverPx1();	//デストラクタ

protected:
	virtual bool initialize();	//初期化
};

#endif // CLUVCDRIVERPX1_H
