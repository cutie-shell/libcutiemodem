#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "../../cutiemodem_p.h"

Q_DECLARE_LOGGING_CATEGORY(mmModemLog)

typedef enum {
	MM_MODEM_STATE_FAILED = -1,
	MM_MODEM_STATE_UNKNOWN = 0,
	MM_MODEM_STATE_INITIALIZING = 1,
	MM_MODEM_STATE_LOCKED = 2,
	MM_MODEM_STATE_DISABLED = 3,
	MM_MODEM_STATE_DISABLING = 4,
	MM_MODEM_STATE_ENABLING = 5,
	MM_MODEM_STATE_ENABLED = 6,
	MM_MODEM_STATE_SEARCHING = 7,
	MM_MODEM_STATE_REGISTERED = 8,
	MM_MODEM_STATE_DISCONNECTING = 9,
	MM_MODEM_STATE_CONNECTING = 10,
	MM_MODEM_STATE_CONNECTED = 11
} MMModemState;

typedef enum {
	MM_MODEM_ACCESS_TECHNOLOGY_UNKNOWN = 0,
	MM_MODEM_ACCESS_TECHNOLOGY_POTS = 1 << 0,
	MM_MODEM_ACCESS_TECHNOLOGY_GSM = 1 << 1,
	MM_MODEM_ACCESS_TECHNOLOGY_GSM_COMPACT = 1 << 2,
	MM_MODEM_ACCESS_TECHNOLOGY_GPRS = 1 << 3,
	MM_MODEM_ACCESS_TECHNOLOGY_EDGE = 1 << 4,
	MM_MODEM_ACCESS_TECHNOLOGY_UMTS = 1 << 5,
	MM_MODEM_ACCESS_TECHNOLOGY_HSDPA = 1 << 6,
	MM_MODEM_ACCESS_TECHNOLOGY_HSUPA = 1 << 7,
	MM_MODEM_ACCESS_TECHNOLOGY_HSPA = 1 << 8,
	MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS = 1 << 9,
	MM_MODEM_ACCESS_TECHNOLOGY_1XRTT = 1 << 10,
	MM_MODEM_ACCESS_TECHNOLOGY_EVDO0 = 1 << 11,
	MM_MODEM_ACCESS_TECHNOLOGY_EVDOA = 1 << 12,
	MM_MODEM_ACCESS_TECHNOLOGY_EVDOB = 1 << 13,
	MM_MODEM_ACCESS_TECHNOLOGY_LTE = 1 << 14,
	MM_MODEM_ACCESS_TECHNOLOGY_5GNR = 1 << 15,
	MM_MODEM_ACCESS_TECHNOLOGY_ANY = 0xFFFFFFFF,
} MMModemAccessTechnology;

class MMModemPrivate;

class MMModem : public CutieModem {
	Q_OBJECT
	Q_DECLARE_PRIVATE(MMModem)
    public:
	MMModem(QObject *parent = 0);
	~MMModem();

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
	void readProperties(QVariantMap props, QVariantMap props3gpp);

	void setPowered(bool powered) override;
	void setOnline(bool online) override;

	Q_INVOKABLE void sendMessage(QString to, QString message) override;
	Q_INVOKABLE QString dial(QString to,
				 QString hideID = QString()) override;
};

class MMModemPrivate : public CutieModemPrivate {
	Q_OBJECT
	Q_DECLARE_PUBLIC(MMModem)
    public:
	MMModemPrivate(MMModem *q);
	~MMModemPrivate();

    public slots:
	void onPropertiesChanged(QString interface, QVariantMap changed,
				 QStringList invalidated);
	void onCallAdded(QDBusObjectPath path);
	void onCallRemoved(QDBusObjectPath path);
	void onMessageAdded(QDBusObjectPath path, bool received);

    protected:
	QString m_path;
	QVariantMap m_data;
	QVariantMap m_3gppData;
};

typedef QMap<QString, QVariantMap> MMInterfaceMap;
typedef QMap<QDBusObjectPath, MMInterfaceMap> MMObjectMap;
typedef QPair<uint, bool> MMSignalQuality;

Q_DECLARE_METATYPE(MMInterfaceMap)
Q_DECLARE_METATYPE(MMObjectMap)
Q_DECLARE_METATYPE(MMSignalQuality)
