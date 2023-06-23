#ifndef CLUVCDRIVERMXP2_H
#define CLUVCDRIVERMXP2_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverMxP2 : public ClUvcDriver
{

public:
    explicit ClUvcDriverMxP2(QObject *pParent = NULL);	//コンストラクタ
    virtual ~ClUvcDriverMxP2();	//デストラクタ

protected:
	virtual bool initialize();	//初期化
};

#endif // CLUVCDRIVERMXP2_H
