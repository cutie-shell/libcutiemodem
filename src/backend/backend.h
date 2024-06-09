#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "../cutiemodem_p.h"

class Backend : public QObject {
	Q_OBJECT
    public:
	Backend(QObject *parent = 0)
		: QObject(parent)
	{
	}
	virtual ~Backend()
	{
	}

	virtual void init() = 0;
	virtual void deinit() = 0;

    signals:
	void modemAdded(CutieModem *modem);
	void modemRemoved(CutieModem *modem);

    public slots:
	virtual void onNameOwnerChanged(QString name, QString oldOwner,
					QString newOwner) = 0;
};
