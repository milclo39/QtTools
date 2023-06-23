#include "ClDocCamFilter.h"
//#include <QRegExp>

bool ClDocCamFilter::filter(const QString& desc)
{
#if 0
#if defined(Q_OS_WIN)
    static QRegExp l12z(".*vid.09a1.*pid.002C.*", Qt::CaseInsensitive);
    static QRegExp mxpa(".*vid.09a1.*pid.0030.*", Qt::CaseInsensitive);
    static QRegExp mxp3(".*vid.09a1.*pid.002A.*", Qt::CaseInsensitive);
    static QRegExp l12w(".*vid.09a1.*pid.0029.*", Qt::CaseInsensitive);
    static QRegExp l12f(".*vid.09a1.*pid.0028.*", Qt::CaseInsensitive);
    static QRegExp ox1(".*vid.09a1.*pid.0027.*", Qt::CaseInsensitive);
    static QRegExp mxp2(".*vid.09a1.*pid.0026.*", Qt::CaseInsensitive);
    static QRegExp mo2(".*vid.09a1.*pid.0025.*", Qt::CaseInsensitive);
    static QRegExp px1(".*vid.09a1.*pid.0022.*", Qt::CaseInsensitive);
#if 1 //MX-1対応検討
    static QRegExp mx1(".*vid.09a1.*pid.0021.*", Qt::CaseInsensitive);
#else
    static QRegExp mx1(".*vid.04b4.*pid.00c3.*", Qt::CaseInsensitive);
#endif
    static QRegExp tx1(".*vid.09a1.*pid.0020.*", Qt::CaseInsensitive);
    static QRegExp mo1(".*vid.09a1.*pid.001f.*", Qt::CaseInsensitive);
    static QRegExp l12(".*vid.09a1.*pid.001d.*", Qt::CaseInsensitive);
    static QRegExp mc1(".*vid.09a1.*pid.001b.*", Qt::CaseInsensitive);
    static QRegExp p10(".*vid.09a1.*pid.0019.*", Qt::CaseInsensitive);
    static QRegExp l1ex(".*vid.09a1.*pid.0015.*", Qt::CaseInsensitive);
    static QRegExp p30s(".*vid.09a1.*pid.0013.*", Qt::CaseInsensitive);
    static QRegExp ff1(".*vid.09a1.*pid.0012.*", Qt::CaseInsensitive);
    static QRegExp p100(".*vid.09a1.*pid.000f.*", Qt::CaseInsensitive);
    static QRegExp l1n(".*vid.09a1.*pid.000e.*", Qt::CaseInsensitive);
    static QRegExp p10s(".*vid.09a1.*pid.0008.*", Qt::CaseInsensitive);
#elif defined(Q_OS_MAC)
    static QRegExp l12z(".*09a1002C.*", Qt::CaseInsensitive);
    static QRegExp mxpa(".*09a10030.*", Qt::CaseInsensitive);
    static QRegExp mxp3(".*09a1002A.*", Qt::CaseInsensitive);
    static QRegExp l12w(".*09a10029.*", Qt::CaseInsensitive);
    static QRegExp l12f(".*09a10028.*", Qt::CaseInsensitive);
    static QRegExp ox1(".*09a10027.*", Qt::CaseInsensitive);
    static QRegExp mxp2(".*09a10026.*", Qt::CaseInsensitive);
    static QRegExp mo2(".*09a10025.*", Qt::CaseInsensitive);
    static QRegExp px1(".*09a10022.*", Qt::CaseInsensitive);
    static QRegExp mx1(".*09a10021.*", Qt::CaseInsensitive);
    static QRegExp tx1(".*09a10020.*", Qt::CaseInsensitive);
    static QRegExp mo1(".*09a1001f.*", Qt::CaseInsensitive);
    static QRegExp l12(".*09a1001d.*", Qt::CaseInsensitive);
    static QRegExp mc1(".*09a1001b.*", Qt::CaseInsensitive);
    static QRegExp p10(".*09a10019.*", Qt::CaseInsensitive);
    static QRegExp l1ex(".*09a10015.*", Qt::CaseInsensitive);
    static QRegExp p30s(".*09a10013.*", Qt::CaseInsensitive);
    static QRegExp ff1(".*09a10012.*", Qt::CaseInsensitive);
    static QRegExp p100(".*09a1000f.*", Qt::CaseInsensitive);
    static QRegExp l1n(".*09a1000e.*", Qt::CaseInsensitive);
    static QRegExp p10s(".*09a10008.*", Qt::CaseInsensitive);
#endif
    if(l12z.indexIn(desc) >= 0) return true;
    if(mxpa.indexIn(desc) >= 0) return true;
    if(mxp3.indexIn(desc) >= 0) return true;
    if(l12w.indexIn(desc) >= 0) return true;
    if(l12f.indexIn(desc) >= 0) return true;
    if(ox1.indexIn(desc) >= 0) return true;
    if(mxp2.indexIn(desc) >= 0) return true;
    if(mo2.indexIn(desc) >= 0) return true;
    if(px1.indexIn(desc) >= 0) return true;
    if(mx1.indexIn(desc) >= 0) return true;
    if(tx1.indexIn(desc) >= 0) return true;
    if(mo1.indexIn(desc) >= 0) return true;
    if(l12.indexIn(desc) >= 0) return true;
    if(mc1.indexIn(desc) >= 0) return true;
    if(p10.indexIn(desc) >= 0) return true;
    if(l1ex.indexIn(desc) >= 0) return true;
    if(p30s.indexIn(desc) >= 0) return true;
    if(ff1.indexIn(desc) >= 0) return true;
    if(p100.indexIn(desc) >= 0) return true;
    if(l1n.indexIn(desc) >= 0) return true;
    if(p10s.indexIn(desc) >= 0) return true;

    return false;
#endif
    return true;
}
