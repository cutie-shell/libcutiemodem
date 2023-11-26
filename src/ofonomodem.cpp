#include "ofonomodem_p.h"

OfonoModem::OfonoModem(QObject *parent)
	: CutieModem(new OfonoModemPrivate(this), parent) { }

OfonoModem::~OfonoModem() { }

void OfonoModem::setPath(QString path) {
	Q_D(OfonoModem);
	d->m_path = path;
	
	QDBusReply<QVariantMap> props = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.Modem",
		QDBusConnection::systemBus()
	).call("GetProperties");
	if (props.isValid())
		d->m_data = props.value();
	else d->m_data = QVariantMap();
	emit dataChanged(d->m_data);
	
	QDBusReply<QVariantMap> simProps = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.SimManager",
		QDBusConnection::systemBus()
	).call("GetProperties");
	if (simProps.isValid())
		d->m_simData = simProps.value();
	else d->m_simData = QVariantMap();
	emit simDataChanged(d->m_simData);
	
	QDBusReply<QVariantMap> netProps = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.NetworkRegistration",
		QDBusConnection::systemBus()
	).call("GetProperties");
	if (netProps.isValid())
		d->m_netData = netProps.value();
	else d->m_netData = QVariantMap();
	emit netDataChanged(d->m_netData);

	QDBusReply<OfonoServiceList> ofonoCalls = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCallManager",
		QDBusConnection::systemBus()
	).call("GetCalls");
	if (ofonoCalls.isValid()) {
		foreach (OfonoServicePair p, ofonoCalls.value()) {
			OfonoCall *c = new OfonoCall(this, p.first.path(), p.second);
			d->m_calls.insert(p.first.path(), c);
		}

		emit callsChanged(d->m_calls.values());
	}

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.Modem", 
		"PropertyChanged",
		d, SLOT(onPropertyChanged(QString,QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.SimManager", 
		"PropertyChanged",
		d, SLOT(onSimPropertyChanged(QString,QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.NetworkRegistration", 
		"PropertyChanged",
		d, SLOT(onNetPropertyChanged(QString,QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.MessageManager", 
		"IncomingMessage",
		d, SLOT(onIncomingMessage(QString,QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCallManager", 
		"CallAdded",
		d, SLOT(onCallAdded(QDBusObjectPath,QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCallManager", 
		"CallRemoved",
		d, SLOT(onCallRemoved(QDBusObjectPath)));
}

void OfonoModem::setProp(QString key, QVariant value) {
	Q_D(OfonoModem);
	QDBusReply<QVariantMap> props = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.Modem",
		QDBusConnection::systemBus()
	).call("SetProperty", key, value);
}

void OfonoModem::setSimProp(QString key, QVariant value) {
	Q_D(OfonoModem);
	QDBusReply<QVariantMap> props = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.SimManager",
		QDBusConnection::systemBus()
	).call("SetProperty", key, value);
}

void OfonoModem::setNetProp(QString key, QVariant value) {
	Q_D(OfonoModem);
	QDBusReply<QVariantMap> props = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.NetworkRegistration",
		QDBusConnection::systemBus()
	).call("SetProperty", key, value);
}

void OfonoModem::sendMessage(QString to, QString message) {
	Q_D(OfonoModem);
	QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.MessageManager",
		QDBusConnection::systemBus()
	).call("SendMessage", to, message);
}

QString OfonoModem::dial(QString to, QString hideID) {
	Q_D(OfonoModem);
	QDBusReply<QDBusObjectPath> path = QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCallManager",
		QDBusConnection::systemBus()
	).call("Dial", to, hideID);
	if (path.isValid()) 
		return path.value().path();
	return QString();
}

OfonoModemPrivate::OfonoModemPrivate(OfonoModem *q)
	: CutieModemPrivate(q) { }

OfonoModemPrivate::~OfonoModemPrivate() { }

void OfonoModemPrivate::onPropertyChanged(QString name, QDBusVariant value) {
	Q_Q(OfonoModem);
	m_data.insert(name, value.variant());
	emit q->dataChanged(m_data);
}

void OfonoModemPrivate::onSimPropertyChanged(QString name, QDBusVariant value) {
	Q_Q(OfonoModem);
	m_simData.insert(name, value.variant());
	emit q->simDataChanged(m_simData);
}

void OfonoModemPrivate::onNetPropertyChanged(QString name, QDBusVariant value) {
	Q_Q(OfonoModem);
	m_netData.insert(name, value.variant());
	emit q->netDataChanged(m_netData);
}

void OfonoModemPrivate::onIncomingMessage(QString message, QVariantMap props) {
	Q_Q(OfonoModem);
	emit q->incomingMessage(message, props);
}

void OfonoModemPrivate::onCallAdded(QDBusObjectPath path, QVariantMap props) {
	Q_Q(OfonoModem);
	OfonoCall *call = new OfonoCall(this, path.path(), props);
	m_calls.insert(path.path(), call);
	emit q->newCall(call);
	emit q->callsChanged(m_calls.values());
}

void OfonoModemPrivate::onCallRemoved(QDBusObjectPath path) {
	Q_Q(OfonoModem);
	m_calls.remove(path.path());
	emit q->callsChanged(m_calls.values());
}
