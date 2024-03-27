#include "cutiecall_p.h"

CutieCall::CutieCall(QObject *parent)
	: QObject(parent)
	, d_ptr(new CutieCallPrivate(this))
{
}

CutieCall::CutieCall(CutieCallPrivate *d, QObject *parent)
	: QObject(parent)
	, d_ptr(d)
{
}

CutieCall::~CutieCall()
{
}

CutieModem *CutieCall::modem()
{
	Q_D(CutieCall);
	return qobject_cast<CutieModem *>(d->parent());
}

CutieCallPrivate::CutieCallPrivate(CutieCall *q)
	: q_ptr(q)
{
}

CutieCallPrivate::~CutieCallPrivate()
{
}
