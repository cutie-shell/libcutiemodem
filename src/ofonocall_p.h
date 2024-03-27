#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"
#include "cutiecall_p.h"

class OfonoCall : public CutieCall {
	Q_OBJECT
	Q_DECLARE_PRIVATE(OfonoCall)
	Q_PROPERTY(QString path READ path() NOTIFY pathChanged);
	Q_PROPERTY(QVariantMap data READ data() NOTIFY dataChanged);

    public:
	OfonoCall(QObject *parent, QString path, QVariantMap props);
	~OfonoCall();

	Q_INVOKABLE void answer() override;
	Q_INVOKABLE void hangup() override;

    Q_SIGNALS:
	void pathChanged(QString);
	void dataChanged(QVariantMap);
	void disconnected(QString);
};

class OfonoCallPrivate : public CutieCallPrivate {
	Q_OBJECT
	Q_DECLARE_PUBLIC(OfonoCall)
    public:
	OfonoCallPrivate(OfonoCall *q);
	~OfonoCallPrivate();

    public slots:
	void onPropertyChanged(QString name, QDBusVariant value);
	void onDisconnectReason(QString reason);
};
