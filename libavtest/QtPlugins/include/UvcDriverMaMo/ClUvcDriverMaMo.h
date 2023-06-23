#ifndef CLUVCDRIVERMAMO_H
#define CLUVCDRIVERMAMO_H

#include "UvcDriver/ClUvcDriver.h"

class ClUvcDriverMaMo : public ClUvcDriver
{

public:
    explicit ClUvcDriverMaMo(QObject *pParent = NULL);	//コンストラクタ
    virtual ~ClUvcDriverMaMo();	//デストラクタ

protected:
	virtual bool initialize();	//初期化
};

#endif // CLUVCDRIVERMAMO_H
