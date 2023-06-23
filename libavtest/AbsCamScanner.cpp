#include "AbsCamScanner.h"

AbsCamScanner::AbsCamScanner(QObject *parent) : QObject(parent)
{
}

QString AbsCamScanner::name() const
{
    return m_name;
}

void AbsCamScanner::setName(const QString &name)
{
    m_name = name;
}

bool AbsCamScanner::exist(ClCamDevice *pCamDev)
{
	Q_UNUSED(pCamDev)
	return false;
}
