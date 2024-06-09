#include "mmmodem_p.h"
#include "mmcall_p.h"

MMModem::MMModem(QObject *parent)
	: CutieModem(new MMModemPrivate(this), parent)
{
}

MMModem::~MMModem()
{
}

bool MMModem::powered()
{
	Q_D(MMModem);
	if (d->m_data["State"].isValid()) {
		MMModemState state =
			(MMModemState)(d->m_data["State"].value<int>());
		return !(state == MM_MODEM_STATE_DISABLED ||
			 state == MM_MODEM_STATE_DISABLING);
	}

	return false;
}

bool MMModem::online()
{
	Q_D(MMModem);
	return powered();
}

QString MMModem::name()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_data["DeviceIdentifier"]);
}

QString MMModem::manufacturer()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_data["Manufacturer"]);
}

QString MMModem::model()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_data["Model"]);
}

QString MMModem::serial()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_data["EquipmentIdentifier"]);
}

QString MMModem::networkCode()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_3gppData["OperatorCode"]).mid(4);
}

QString MMModem::networkCountryCode()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_3gppData["OperatorCode"]).mid(0, 3);
}

QString MMModem::networkName()
{
	Q_D(MMModem);
	return qdbus_cast<QString>(d->m_3gppData["OperatorName"]);
}

uint MMModem::networkStrength()
{
	Q_D(MMModem);
	return qdbus_cast<MMSignalQuality>(d->m_data["SignalQuality"]).first;
}

CutieModem::NetworkStatus MMModem::networkStatus()
{
	Q_D(MMModem);
	int state = qdbus_cast<int>(d->m_data["State"]);

	switch (state) {
	case MM_MODEM_STATE_FAILED:
	case MM_MODEM_STATE_UNKNOWN:
	case MM_MODEM_STATE_INITIALIZING:
	case MM_MODEM_STATE_LOCKED:
	case MM_MODEM_STATE_DISABLED:
	case MM_MODEM_STATE_DISABLING:
	case MM_MODEM_STATE_ENABLING:
	case MM_MODEM_STATE_ENABLED:
		return NetworkStatus::Unregistered;
	case MM_MODEM_STATE_SEARCHING:
		return NetworkStatus::Searching;
	case MM_MODEM_STATE_REGISTERED:
	case MM_MODEM_STATE_DISCONNECTING:
	case MM_MODEM_STATE_CONNECTING:
	case MM_MODEM_STATE_CONNECTED:
		return NetworkStatus::Registered;
	default:
		return NetworkStatus::Unknown;
	}
}

CutieModem::NetworkTechnology MMModem::networkTechnology()
{
	Q_D(MMModem);
	MMModemAccessTechnology tech =
		(MMModemAccessTechnology)qdbus_cast<uint>(
			d->m_data["AccessTechnologies"]);

	if (tech & MM_MODEM_ACCESS_TECHNOLOGY_LTE)
		return NetworkTechnology::LTE;
	if (tech & MM_MODEM_ACCESS_TECHNOLOGY_HSPA_PLUS)
		return NetworkTechnology::HSPA;
	if (tech & MM_MODEM_ACCESS_TECHNOLOGY_HSPA)
		return NetworkTechnology::HSPA;
	if (tech & MM_MODEM_ACCESS_TECHNOLOGY_UMTS)
		return NetworkTechnology::UMTS;
	if (tech & MM_MODEM_ACCESS_TECHNOLOGY_EDGE)
		return NetworkTechnology::EDGE;
	if (tech & MM_MODEM_ACCESS_TECHNOLOGY_GPRS)
		return NetworkTechnology::GSM;

	return NetworkTechnology::InvalidTechnology;
}

void MMModem::setPowered(bool _powered)
{
	Q_D(MMModem);
	if (powered() == _powered)
		return;

	QDBusInterface("org.freedesktop.ModemManager1", d->m_path,
		       "org.freedesktop.ModemManager1.Modem",
		       QDBusConnection::systemBus())
		.call("Enable", _powered);
}

void MMModem::setOnline(bool online)
{
	setPowered(online);
}

void MMModem::setPath(QString path)
{
	Q_D(MMModem);
	d->m_path = path;

	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1", d->m_path,
		"org.freedesktop.DBus.Properties", "PropertiesChanged", d,
		SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1", d->m_path,
		"org.freedesktop.ModemManager1.Modem.Voice", "CallAdded", d,
		SLOT(onCallAdded(QDBusObjectPath)));
	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1", d->m_path,
		"org.freedesktop.ModemManager1.Modem.Voice", "CallRemoved", d,
		SLOT(onCallRemoved(QDBusObjectPath)));
	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1", d->m_path,
		"org.freedesktop.ModemManager1.Modem.Messaging", "Added", d,
		SLOT(onMessageAdded(QDBusObjectPath, bool)));
}

void MMModem::readProperties(QVariantMap props, QVariantMap props3gpp)
{
	Q_D(MMModem);
	d->m_data = props;
	d->m_3gppData = props3gpp;
}

void MMModem::sendMessage(QString to, QString message)
{
	Q_D(MMModem);
	qCDebug(mmModemLog) << "Sending message to" << to << ":" << message;

	QVariantMap props;
	props.insert("Number", to);
	props.insert("Text", message);

	QDBusInterface("org.freedesktop.ModemManager1", d->m_path,
		       "org.freedesktop.ModemManager1.Messaging",
		       QDBusConnection::systemBus())
		.call("Create", props);
}

QString MMModem::dial(QString to, QString hideID)
{
	Q_D(MMModem);
	qCDebug(mmModemLog) << "Dialing" << to;

	QVariantMap props;
	props.insert("Number", to);

	QDBusReply<QDBusObjectPath> reply =
		QDBusInterface("org.freedesktop.ModemManager1", d->m_path,
			       "org.freedesktop.ModemManager1.Voice",
			       QDBusConnection::systemBus())
			.call("CreateCall", props);

	if (reply.isValid()) {
		QDBusInterface("org.freedesktop.ModemManager1",
			       reply.value().path(),
			       "org.freedesktop.ModemManager1.Call",
			       QDBusConnection::systemBus())
			.call("Start");
		return reply.value().path();
	}

	return QString();
}

MMModemPrivate::MMModemPrivate(MMModem *q)
	: CutieModemPrivate(q)
{
}

MMModemPrivate::~MMModemPrivate()
{
}

void MMModemPrivate::onPropertiesChanged(QString interface, QVariantMap values,
					 QStringList invalidated)
{
	Q_Q(MMModem);
	if (interface == "org.freedesktop.ModemManager1.Modem") {
		for (QString name : values.keys()) {
			qCDebug(mmModemLog) << "Modem property changed:" << name
					    << values[name];
			m_data.insert(name, values[name]);
			if ("State" == name) {
				emit q->poweredChanged();
				emit q->onlineChanged();
				emit q->networkStatusChanged();
			} else if ("SignalQuality" == name)
				emit q->networkStrengthChanged();
			else if ("AccessTechnologies" == name)
				emit q->networkTechnologyChanged();
		}
	} else if (interface ==
		   "org.freedesktop.ModemManager1.Modem.Modem3gpp") {
		for (QString name : values.keys()) {
			qCDebug(mmModemLog) << "3GPP property changed:" << name
					    << values[name];
			m_3gppData.insert(name, values[name]);
			if ("OperatorCode" == name) {
				emit q->networkCountryCodeChanged();
				emit q->networkCodeChanged();
			} else if ("OperatorName" == name)
				emit q->networkNameChanged();
		}
	}
}

void MMModemPrivate::onCallAdded(QDBusObjectPath path)
{
	Q_Q(MMModem);
	QDBusReply<QVariantMap> reply =
		QDBusInterface("org.freedesktop.ModemManager1", path.path(),
			       "org.freedesktop.DBus.Properties",
			       QDBusConnection::systemBus())
			.call("GetProperties");
	if (!reply.isValid()) {
		qCWarning(mmModemLog) << "Failed to get call properties";
		return;
	}

	QVariantMap props =
		reply.value()["org.freedesktop.ModemManager1.Call"].toMap();
	MMCall *call = new MMCall(this, path.path(), props);
	m_calls.insert(path.path(), call);
	emit q->newCall(call);
	emit q->callsChanged(m_calls.values());
}

void MMModemPrivate::onCallRemoved(QDBusObjectPath path)
{
	Q_Q(MMModem);
	if (!m_calls.contains(path.path())) {
		qCWarning(mmModemLog) << "Call not found:" << path.path();
		return;
	}

	delete m_calls.take(path.path());
	emit q->callsChanged(m_calls.values());
}

void MMModemPrivate::onMessageAdded(QDBusObjectPath path, bool received)
{
	Q_Q(MMModem);
	if (!received)
		return;

	QDBusReply<QVariantMap> reply =
		QDBusInterface("org.freedesktop.ModemManager1", path.path(),
			       "org.freedesktop.DBus.Properties",
			       QDBusConnection::systemBus())
			.call("GetProperties");
	if (!reply.isValid()) {
		qCWarning(mmModemLog) << "Failed to get message properties";
		return;
	}

	QString message = reply.value()["org.freedesktop.ModemManager1.Sms"]
				  .toMap()["Text"]
				  .toString();
	QVariantMap props;
	props.insert("Sender",
		     reply.value()["org.freedesktop.ModemManager1.Sms"]
			     .toMap()["Number"]);
	props.insert("LocalSentTime",
		     reply.value()["org.freedesktop.ModemManager1.Sms"]
			     .toMap()["Timestamp"]);
	emit q->incomingMessage(props["Text"].toString(), props);
}
