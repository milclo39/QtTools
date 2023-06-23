#ifndef CLUVCDRIVEROX1_H
#define CLUVCDRIVEROX1_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverOx1 : public ClUvcDriver
{

public:
    explicit ClUvcDriverOx1(QObject *pParent = NULL);	//コンストラクタ
    virtual ~ClUvcDriverOx1();	//デストラクタ

protected:
	virtual bool initialize();	//初期化
};

#endif // CLUVCDRIVEROX1_H
