#ifndef CLCAMFILTER_H
#define CLCAMFILTER_H
class QString;

class ClCamFilter
{
public:
    virtual bool filter(const QString& desc);
};

#endif // CLCAMFILTER_H
