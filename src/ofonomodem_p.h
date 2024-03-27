#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"
#include "cutiemodem_p.h"

class OfonoModem : public CutieModem {
	Q_OBJECT
	Q_DECLARE_PRIVATE(OfonoModem)
	Q_PROPERTY(QString path READ path() NOTIFY pathChanged);
	Q_PROPERTY(QVariantMap data READ data() NOTIFY dataChanged);
	Q_PROPERTY(QVariantMap simData READ simData() NOTIFY simDataChanged);
	Q_PROPERTY(QVariantMap netData READ netData() NOTIFY netDataChanged);

    public:
	OfonoModem(QObject *parent = 0);
	~OfonoModem();

	void setPath(QString path) override;
	Q_INVOKABLE void sendMessage(QString to, QString message) override;
	Q_INVOKABLE QString dial(QString to,
				 QString hideID = QString()) override;
	Q_INVOKABLE void setProp(QString key, QVariant value) override;
	Q_INVOKABLE void setSimProp(QString key, QVariant value) override;
	Q_INVOKABLE void setNetProp(QString key, QVariant value) override;
};

class OfonoModemPrivate : public CutieModemPrivate {
	Q_OBJECT
	Q_DECLARE_PUBLIC(OfonoModem)
    public:
	OfonoModemPrivate(OfonoModem *q);
	~OfonoModemPrivate();

    public slots:
	void onPropertyChanged(QString name, QDBusVariant value);
	void onSimPropertyChanged(QString name, QDBusVariant value);
	void onNetPropertyChanged(QString name, QDBusVariant value);
	void onIncomingMessage(QString message, QVariantMap props);
	void onCallAdded(QDBusObjectPath path, QVariantMap props);
	void onCallRemoved(QDBusObjectPath path);
};

typedef QPair<QDBusObjectPath, QVariantMap> OfonoServicePair;
typedef QList<OfonoServicePair> OfonoServiceList;

Q_DECLARE_METATYPE(OfonoServicePair)
Q_DECLARE_METATYPE(OfonoServiceList)
