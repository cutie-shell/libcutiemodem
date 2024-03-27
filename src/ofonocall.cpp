#include "ofonocall_p.h"

OfonoCall::OfonoCall(QObject *parent, QString path, QVariantMap props)
	: CutieCall(new OfonoCallPrivate(this), parent)
{
	Q_D(OfonoCall);
	d->m_path = path;
	d->m_data = props;

	QDBusConnection::systemBus().connect(
		"org.ofono", d->m_path, "org.ofono.VoiceCall",
		"PropertyChanged", d,
		SLOT(onPropertyChanged(QString, QDBusVariant)));

	QDBusConnection::systemBus().connect("org.ofono", d->m_path,
					     "org.ofono.VoiceCall",
					     "DisconnectReason", d,
					     SLOT(onDisconnectReason(QString)));
}

OfonoCall::~OfonoCall()
{
}

QString OfonoCall::lineIdentification()
{
	Q_D(OfonoCall);
	return qdbus_cast<QString>(d->m_data["LineIdentification"]);
}

CutieCall::CallState OfonoCall::state()
{
	Q_D(OfonoCall);
	QString stateString = qdbus_cast<QString>(d->m_data["State"]);

	if ("active" == stateString)
		return CallState::Active;
	else if ("held" == stateString)
		return CallState::Held;
	else if ("dialing" == stateString)
		return CallState::Dialing;
	else if ("alerting" == stateString)
		return CallState::Alerting;
	else if ("incoming" == stateString)
		return CallState::Incoming;
	else if ("waiting" == stateString)
		return CallState::Waiting;
	else if ("disconnected" == stateString)
		return CallState::Disconnected;

	return CallState::Invalid;
}

void OfonoCall::answer()
{
	Q_D(OfonoCall);
	QDBusInterface("org.ofono", d->m_path, "org.ofono.VoiceCall",
		       QDBusConnection::systemBus())
		.call("Answer");
}

void OfonoCall::hangup()
{
	Q_D(OfonoCall);
	QDBusInterface("org.ofono", d->m_path, "org.ofono.VoiceCall",
		       QDBusConnection::systemBus())
		.call("Hangup");
}

OfonoCallPrivate::OfonoCallPrivate(OfonoCall *q)
	: CutieCallPrivate(q)
{
}

OfonoCallPrivate::~OfonoCallPrivate()
{
}

void OfonoCallPrivate::onPropertyChanged(QString name, QDBusVariant value)
{
	Q_Q(OfonoCall);
	m_data.insert(name, value.variant());

	if ("LineIdentification" == name)
		emit q->lineIdentificationChanged();
	else if ("State" == name)
		emit q->stateChanged();
}

void OfonoCallPrivate::onDisconnectReason(QString reason)
{
	Q_Q(OfonoCall);
	emit q->disconnected(reason);
}
