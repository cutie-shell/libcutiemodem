#include "ofonomodem_p.h"
#include "ofonocall_p.h"

OfonoModem::OfonoModem(QObject *parent)
	: CutieModem(new OfonoModemPrivate(this), parent)
{
}

OfonoModem::~OfonoModem()
{
}

bool OfonoModem::powered()
{
	Q_D(OfonoModem);
	return qdbus_cast<bool>(d->m_data["Powered"]);
}

bool OfonoModem::online()
{
	Q_D(OfonoModem);
	return qdbus_cast<bool>(d->m_data["Online"]);
}

QString OfonoModem::name()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_data["Name"]);
}

QString OfonoModem::manufacturer()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_data["Manufacturer"]);
}

QString OfonoModem::model()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_data["Model"]);
}

QString OfonoModem::serial()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_data["Serial"]);
}

QString OfonoModem::networkCode()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_netData["MobileNetworkCode"]);
}

QString OfonoModem::networkCountryCode()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_netData["MobileCountryCode"]);
}

QString OfonoModem::networkName()
{
	Q_D(OfonoModem);
	return qdbus_cast<QString>(d->m_netData["Name"]);
}

uint OfonoModem::networkStrength()
{
	Q_D(OfonoModem);
	return qdbus_cast<uint>(d->m_netData["Strength"]);
}

CutieModem::NetworkStatus OfonoModem::networkStatus()
{
	Q_D(OfonoModem);
	QString statusString = qdbus_cast<QString>(d->m_netData["Status"]);

	if ("unregistered" == statusString)
		return NetworkStatus::Unregistered;
	else if ("registered" == statusString)
		return NetworkStatus::Registered;
	else if ("searching" == statusString)
		return NetworkStatus::Searching;
	else if ("denied" == statusString)
		return NetworkStatus::Denied;
	else if ("unknown" == statusString)
		return NetworkStatus::Unknown;
	else if ("roaming" == statusString)
		return NetworkStatus::Roaming;

	return NetworkStatus::InvalidStatus;
}

CutieModem::NetworkTechnology OfonoModem::networkTechnology()
{
	Q_D(OfonoModem);
	QString technologyString =
		qdbus_cast<QString>(d->m_netData["Technology"]);

	if ("gsm" == technologyString)
		return NetworkTechnology::GSM;
	else if ("edge" == technologyString)
		return NetworkTechnology::EDGE;
	else if ("umts" == technologyString)
		return NetworkTechnology::UMTS;
	else if ("hspa" == technologyString)
		return NetworkTechnology::HSPA;
	else if ("lte" == technologyString)
		return NetworkTechnology::LTE;

	return NetworkTechnology::InvalidTechnology;
}

void OfonoModem::setPowered(bool powered)
{
	setProp("Powered", QVariant(powered));
}

void OfonoModem::setOnline(bool online)
{
	setProp("Online", QVariant(online));
}

void OfonoModem::setPath(QString path)
{
	Q_D(OfonoModem);
	d->m_path = path;

	QDBusReply<QVariantMap> props =
		QDBusInterface("org.ofono", d->m_path, "org.ofono.Modem",
			       QDBusConnection::systemBus())
			.call("GetProperties");
	if (props.isValid())
		d->m_data = props.value();
	else
		d->m_data = QVariantMap();

	QDBusReply<QVariantMap> simProps =
		QDBusInterface("org.ofono", d->m_path, "org.ofono.SimManager",
			       QDBusConnection::systemBus())
			.call("GetProperties");
	if (simProps.isValid())
		d->m_simData = simProps.value();
	else
		d->m_simData = QVariantMap();

	QDBusReply<QVariantMap> netProps =
		QDBusInterface("org.ofono", d->m_path,
			       "org.ofono.NetworkRegistration",
			       QDBusConnection::systemBus())
			.call("GetProperties");
	if (netProps.isValid())
		d->m_netData = netProps.value();
	else
		d->m_netData = QVariantMap();

	QDBusReply<OfonoServiceList> ofonoCalls =
		QDBusInterface("org.ofono", d->m_path,
			       "org.ofono.VoiceCallManager",
			       QDBusConnection::systemBus())
			.call("GetCalls");
	if (ofonoCalls.isValid()) {
		foreach(OfonoServicePair p, ofonoCalls.value()) {
			OfonoCall *c =
				new OfonoCall(this, p.first.path(), p.second);
			d->m_calls.insert(p.first.path(), c);
		}

		emit callsChanged(d->m_calls.values());
	}

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.Modem", "PropertyChanged", d,
		SLOT(onPropertyChanged(QString, QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.SimManager",
		"PropertyChanged", d,
		SLOT(onSimPropertyChanged(QString, QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.NetworkRegistration",
		"PropertyChanged", d,
		SLOT(onNetPropertyChanged(QString, QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.MessageManager",
		"IncomingMessage", d,
		SLOT(onIncomingMessage(QString, QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.VoiceCallManager",
		"CallAdded", d,
		SLOT(onCallAdded(QDBusObjectPath, QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.VoiceCallManager",
		"CallRemoved", d, SLOT(onCallRemoved(QDBusObjectPath)));
}

void OfonoModem::setProp(QString key, QVariant value)
{
	Q_D(OfonoModem);
	QDBusReply<QVariantMap> props =
		QDBusInterface("org.ofono", d->m_path, "org.ofono.Modem",
			       QDBusConnection::systemBus())
			.call("SetProperty", key,
			      QVariant::fromValue(QDBusVariant(value)));
}

void OfonoModem::setSimProp(QString key, QVariant value)
{
	Q_D(OfonoModem);
	QDBusReply<QVariantMap> props =
		QDBusInterface("org.ofono", d->m_path, "org.ofono.SimManager",
			       QDBusConnection::systemBus())
			.call("SetProperty", key,
			      QVariant::fromValue(QDBusVariant(value)));
}

void OfonoModem::setNetProp(QString key, QVariant value)
{
	Q_D(OfonoModem);
	QDBusReply<QVariantMap> props =
		QDBusInterface("org.ofono", d->m_path,
			       "org.ofono.NetworkRegistration",
			       QDBusConnection::systemBus())
			.call("SetProperty", key,
			      QVariant::fromValue(QDBusVariant(value)));
}

void OfonoModem::sendMessage(QString to, QString message)
{
	Q_D(OfonoModem);
	QDBusInterface("org.ofono", d->m_path, "org.ofono.MessageManager",
		       QDBusConnection::systemBus())
		.call("SendMessage", to, message);
}

QString OfonoModem::dial(QString to, QString hideID)
{
	Q_D(OfonoModem);
	QDBusReply<QDBusObjectPath> path =
		QDBusInterface("org.ofono", d->m_path,
			       "org.ofono.VoiceCallManager",
			       QDBusConnection::systemBus())
			.call("Dial", to, hideID);
	if (path.isValid())
		return path.value().path();
	return QString();
}

OfonoModemPrivate::OfonoModemPrivate(OfonoModem *q)
	: CutieModemPrivate(q)
{
}

OfonoModemPrivate::~OfonoModemPrivate()
{
}

void OfonoModemPrivate::onPropertyChanged(QString name, QDBusVariant value)
{
	Q_Q(OfonoModem);
	m_data.insert(name, value.variant());

	if ("Powered" == name)
		emit q->poweredChanged();
	else if ("Online" == name)
		emit q->onlineChanged();
}

void OfonoModemPrivate::onSimPropertyChanged(QString name, QDBusVariant value)
{
	Q_Q(OfonoModem);
	m_simData.insert(name, value.variant());
}

void OfonoModemPrivate::onNetPropertyChanged(QString name, QDBusVariant value)
{
	Q_Q(OfonoModem);
	m_netData.insert(name, value.variant());

	if ("Status" == name)
		emit q->networkStatus();
	else if ("MobileCountryCode" == name)
		emit q->networkCountryCodeChanged();
	else if ("MobileNetworkCode" == name)
		emit q->networkCodeChanged();
	else if ("Technology" == name)
		emit q->networkTechnologyChanged();
	else if ("Name" == name)
		emit q->networkNameChanged();
	else if ("Strength" == name)
		emit q->networkStrengthChanged();
}

void OfonoModemPrivate::onIncomingMessage(QString message, QVariantMap props)
{
	Q_Q(OfonoModem);
	emit q->incomingMessage(message, props);
}

void OfonoModemPrivate::onCallAdded(QDBusObjectPath path, QVariantMap props)
{
	Q_Q(OfonoModem);
	OfonoCall *call = new OfonoCall(this, path.path(), props);
	m_calls.insert(path.path(), call);
	emit q->newCall(call);
	emit q->callsChanged(m_calls.values());
}

void OfonoModemPrivate::onCallRemoved(QDBusObjectPath path)
{
	Q_Q(OfonoModem);
	m_calls.remove(path.path());
	emit q->callsChanged(m_calls.values());
}
