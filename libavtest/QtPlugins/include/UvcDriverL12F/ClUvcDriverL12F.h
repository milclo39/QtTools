#ifndef CLUVCDRIVERL12F_H
#define CLUVCDRIVERL12F_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverL12F : public ClUvcDriver
{

public:
    explicit ClUvcDriverL12F(QObject *pParent = NULL);	//コンストラクタ
    virtual ~ClUvcDriverL12F();	//デストラクタ

protected:
	virtual bool initialize();	//初期化
};

#endif // CLUVCDRIVERL12F_H
