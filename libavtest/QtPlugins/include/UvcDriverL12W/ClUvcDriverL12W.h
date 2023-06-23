#ifndef CLUVCDRIVERL12W_H
#define CLUVCDRIVERL12W_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverL12W : public ClUvcDriver
{

public:
    explicit ClUvcDriverL12W(QObject *pParent = NULL);	//コンストラクタ
    virtual ~ClUvcDriverL12W();	//デストラクタ

protected:
	virtual bool initialize();	//初期化
};

#endif // CLUVCDRIVERL12W_H
