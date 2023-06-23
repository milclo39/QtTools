#ifndef CLDOCCAMFILTER_H
#define CLDOCCAMFILTER_H
#include "ClCamFilter.h"

class ClDocCamFilter : public ClCamFilter
{
public:
    bool filter(const QString &desc);
};

#endif // CLDOCCAMFILTER_H
