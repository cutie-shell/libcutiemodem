#include "ofonocall_p.h"

OfonoCall::OfonoCall(QObject *parent, QString path, QVariantMap props)
	: CutieCall(new OfonoCallPrivate(this), parent) {
	Q_D(OfonoCall);
	d->m_path = path;
	d->m_data = props;

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCall", 
		"PropertyChanged",
		d, SLOT(onPropertyChanged(QString,QDBusVariant)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCall", 
		"DisconnectReason",
		d, SLOT(onDisconnectReason(QString)));
}

OfonoCall::~OfonoCall() {}

void OfonoCall::answer() {
	Q_D(OfonoCall);
	QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCall",
		QDBusConnection::systemBus()
	).call("Answer");
}

void OfonoCall::hangup() {
	Q_D(OfonoCall);
	QDBusInterface(
		"org.ofono",
		d->m_path, 
		"org.ofono.VoiceCall",
		QDBusConnection::systemBus()
	).call("Hangup");
}

OfonoCallPrivate::OfonoCallPrivate(OfonoCall *q)
	: CutieCallPrivate(q) { }

OfonoCallPrivate::~OfonoCallPrivate() { }

void OfonoCallPrivate::onPropertyChanged(QString name, QDBusVariant value) {
	Q_Q(OfonoCall);
	m_data.insert(name, value.variant());
	emit q->dataChanged(m_data);
}

void OfonoCallPrivate::onDisconnectReason(QString reason) {
	Q_Q(OfonoCall);
	emit q->disconnected(reason);
}
