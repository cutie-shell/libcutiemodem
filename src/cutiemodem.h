#pragma once
#include <QtQuick>

class CutieModemPrivate;
class CutieCallPrivate;
class ModemSettingsPrivate;
class OfonoCallPrivate;
class OfonoModemPrivate;

class CutieModem;

class CutieCall : public QObject {
	Q_OBJECT
	Q_DECLARE_PRIVATE(CutieCall);
	Q_PROPERTY(QVariantMap data READ data() NOTIFY dataChanged);
	Q_PROPERTY(CutieModem *modem READ modem());
	Q_PROPERTY(QString lineIdentification READ lineIdentification() NOTIFY lineIdentificationChanged)
	Q_PROPERTY(CallState state READ state() NOTIFY stateChanged);

    public:
	enum class CallState {
		Active,
		Held,
		Dialing,
		Alerting,
		Incoming,
		Waiting,
		Disconnected,
		Invalid
	};
	Q_ENUM(CallState)

	CutieCall(QObject *parent = 0);
	~CutieCall();

	virtual QVariantMap data() = 0;
	CutieModem *modem();
	virtual QString lineIdentification() = 0;
	virtual CallState state() = 0;

	virtual Q_INVOKABLE void answer() = 0;
	virtual Q_INVOKABLE void hangup() = 0;

    signals:
	void dataChanged(QVariantMap);
	void lineIdentificationChanged();
	void stateChanged();
	void disconnected(QString);

    protected:
	CutieCall(CutieCallPrivate *d, QObject *parent = 0);
	CutieCallPrivate *d_ptr;
};

class CutieModem : public QObject {
	Q_OBJECT
	Q_DECLARE_PRIVATE(CutieModem)
	Q_PROPERTY(QVariantMap data READ data() NOTIFY dataChanged);
	Q_PROPERTY(QVariantMap simData READ simData() NOTIFY simDataChanged);
	Q_PROPERTY(QVariantMap netData READ netData() NOTIFY netDataChanged);
	Q_PROPERTY(uint audioMode READ audioMode()
			   WRITE setAudioMode NOTIFY audioModeChanged);
	Q_PROPERTY(QList<CutieCall *> calls READ calls() NOTIFY callsChanged);
	Q_PROPERTY(bool powered READ powered() WRITE setPowered NOTIFY poweredChanged);
	Q_PROPERTY(bool online READ online() WRITE setOnline NOTIFY onlineChanged);
	Q_PROPERTY(QString name READ name());
	Q_PROPERTY(QString manufacturer READ manufacturer());
	Q_PROPERTY(QString model READ model());
	Q_PROPERTY(QString serial READ serial());
	Q_PROPERTY(QString networkCode READ networkCode() NOTIFY networkCodeChanged);
	Q_PROPERTY(QString networkCountryCode READ networkCountryCode() NOTIFY networkCountryCodeChanged);
	Q_PROPERTY(QString networkName READ networkName() NOTIFY networkNameChanged);
	Q_PROPERTY(uint networkStrength READ networkStrength() NOTIFY networkStrengthChanged);
	Q_PROPERTY(NetworkStatus networkStatus READ networkStatus() NOTIFY networkStatusChanged);
	Q_PROPERTY(NetworkTechnology networkTechnology READ networkTechnology() NOTIFY networkTechnologyChanged);

    public:
	enum class NetworkStatus {
		Unregistered,
		Registered,
		Searching,
		Denied,
		Unknown,
		Roaming,
		Invalid
	};
	Q_ENUM(NetworkStatus)
	enum class NetworkTechnology {
		GSM,
		EDGE,
		UMTS,
		HSPA,
		LTE,
		Invalid
	};
	Q_ENUM(NetworkTechnology)

	CutieModem(QObject *parent = 0);
	~CutieModem();

	virtual QVariantMap data() = 0;
	virtual QVariantMap simData() = 0;
	virtual QVariantMap netData() = 0;

	uint audioMode();
	void setAudioMode(uint mode);
	QList<CutieCall *> calls();

	virtual bool powered() = 0;
	virtual bool online() = 0;
	virtual QString name() = 0;
	virtual QString manufacturer() = 0;
	virtual QString model() = 0;
	virtual QString serial() = 0;

	virtual QString networkCode() = 0;
	virtual QString networkCountryCode() = 0;
	virtual QString networkName() = 0;
	virtual uint networkStrength() = 0;
	virtual NetworkStatus networkStatus() = 0;
	virtual NetworkTechnology networkTechnology() = 0;

	virtual void setPowered(bool powered) = 0;
	virtual void setOnline(bool online) = 0;

	virtual Q_INVOKABLE void sendMessage(QString to, QString message) = 0;
	virtual Q_INVOKABLE QString dial(QString to,
					 QString hideID = QString()) = 0;
	virtual Q_INVOKABLE void setProp(QString key, QVariant value) = 0;
	virtual Q_INVOKABLE void setSimProp(QString key, QVariant value) = 0;
	virtual Q_INVOKABLE void setNetProp(QString key, QVariant value) = 0;

    signals:
	void dataChanged();
	void simDataChanged();
	void netDataChanged();

	void poweredChanged();
	void onlineChanged();

	void networkCodeChanged();
	void networkCountryCodeChanged();
	void networkNameChanged();
	void networkStrengthChanged();
	void networkStatusChanged();
	void networkTechnologyChanged();

	void audioModeChanged(uint mode);
	void incomingMessage(QString message, QVariantMap props);
	void newCall(CutieCall *call);
	void callsChanged(QList<CutieCall *> calls);

    protected:
	CutieModem(CutieModemPrivate *d, QObject *parent = 0);
	CutieModemPrivate *d_ptr;
};

class ModemSettings : public QObject {
	Q_OBJECT
	Q_DECLARE_PRIVATE(ModemSettings)
	Q_PROPERTY(QList<CutieModem *> modems READ modems NOTIFY modemsChanged)
    public:
	ModemSettings(QObject *parent = 0);
	QList<CutieModem *> modems();

	static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine);

    signals:
	void modemsChanged(QList<CutieModem *> modems);

    protected:
	ModemSettingsPrivate *d_ptr;
};

