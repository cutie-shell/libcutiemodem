#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"

Q_DECLARE_LOGGING_CATEGORY(modemLog)

class CutieCallPrivate : public QObject {
	Q_OBJECT
	Q_DECLARE_PUBLIC(CutieCall)

    public:
	CutieCallPrivate(CutieCall *q);
	~CutieCallPrivate();

    protected:
	CutieCall *q_ptr;
};