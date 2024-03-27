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
    public:
	OfonoModem(QObject *parent = 0);
	~OfonoModem();

	QVariantMap data() override;
	QVariantMap simData() override;
	QVariantMap netData() override;

	bool powered() override;
	bool online() override;
	QString name() override;
	QString manufacturer() override;
	QString model() override;
	QString serial() override;

	QString networkCode() override;
	QString networkCountryCode() override;
	QString networkName() override;
	uint networkStrength() override;
	NetworkStatus networkStatus() override;
	NetworkTechnology networkTechnology() override;

	void setPath(QString path);

	void setPowered(bool powered) override;
	void setOnline(bool online) override;
	
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

	protected:
	QString m_path;
	QVariantMap m_data;
	QVariantMap m_simData;
	QVariantMap m_netData;
};

typedef QPair<QDBusObjectPath, QVariantMap> OfonoServicePair;
typedef QList<OfonoServicePair> OfonoServiceList;

Q_DECLARE_METATYPE(OfonoServicePair)
Q_DECLARE_METATYPE(OfonoServiceList)
