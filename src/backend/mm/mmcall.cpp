#include "mmcall_p.h"

MMCall::MMCall(QObject *parent, QString path, QVariantMap props)
	: CutieCall(new MMCallPrivate(this), parent)
{
	Q_D(MMCall);
	d->m_path = path;
	d->m_data = props;

	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1", d->m_path,
		"org.freedesktop.DBus.Properties", "PropertiesChanged", d,
		SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));

	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1", d->m_path,
		"org.freedesktop.ModemManager1.Call", "StateChanged", d,
		SLOT(onStateChanged(int, int, uint)));
}

MMCall::~MMCall()
{
}

QString MMCall::lineIdentification()
{
	Q_D(MMCall);
	return qdbus_cast<QString>(d->m_data["Number"]);
}

CutieCall::CallState MMCall::state()
{
	Q_D(MMCall);
	MMCallState state = (MMCallState)qdbus_cast<int>(d->m_data["State"]);

	if (state == MM_CALL_STATE_UNKNOWN)
		return CallState::InvalidState;
	else if (state == MM_CALL_STATE_DIALING)
		return CallState::Dialing;
	else if (state == MM_CALL_STATE_RINGING_OUT)
		return CallState::Alerting;
	else if (state == MM_CALL_STATE_RINGING_IN)
		return CallState::Incoming;
	else if (state == MM_CALL_STATE_ACTIVE)
		return CallState::Active;
	else if (state == MM_CALL_STATE_HELD)
		return CallState::Held;
	else if (state == MM_CALL_STATE_WAITING)
		return CallState::Waiting;
	else if (state == MM_CALL_STATE_TERMINATED)
		return CallState::Disconnected;

	return CallState::InvalidState;
}

void MMCall::answer()
{
	Q_D(MMCall);
	qCDebug(mmModemLog) << "Answering call" << d->m_path;
	QDBusInterface("org.freedesktop.ModemManager1", d->m_path,
		       "org.freedesktop.ModemManager1.Call",
		       QDBusConnection::systemBus())
		.call("Accept");
}

void MMCall::hangup()
{
	Q_D(MMCall);
	qCDebug(mmModemLog) << "Hanging up call" << d->m_path;
	QDBusInterface("org.freedesktop.ModemManager1.Call", d->m_path,
		       "org.freedesktop.ModemManager1.Call",
		       QDBusConnection::systemBus())
		.call("Hangup");
	d->m_hangupRequested = true;
}

MMCallPrivate::MMCallPrivate(MMCall *q)
	: CutieCallPrivate(q)
{
}

MMCallPrivate::~MMCallPrivate()
{
}

void MMCallPrivate::onPropertiesChanged(QString interface, QVariantMap values,
					QStringList invalidated)
{
	Q_Q(MMCall);
	if (interface != "org.freedesktop.ModemManager1.Call")
		return;

	for (QString name : values.keys()) {
		qCDebug(mmModemLog)
			<< "Call property changed:" << name << values[name];
		m_data.insert(name, values[name]);
		if ("Number" == name)
			emit q->lineIdentificationChanged();
		else if ("State" == name)
			emit q->stateChanged();
	}

	for (QString name : invalidated) {
		QDBusReply<QVariant> reply =
			QDBusInterface("org.freedesktop.ModemManager1", m_path,
				       "org.freedesktop.DBus.Properties",
				       QDBusConnection::systemBus())
				.call("Get",
				      "org.freedesktop.ModemManager1.Call",
				      name);

		if (reply.isValid()) {
			qCDebug(mmModemLog) << "Call property changed:" << name
					    << reply.value();
			m_data.insert(name, reply.value());
			if ("Number" == name)
				emit q->lineIdentificationChanged();
			else if ("State" == name)
				emit q->stateChanged();
		} else {
			qCWarning(mmModemLog)
				<< "Failed to get call property:" << name;
			m_data.remove(name);
		}
	}
}

void MMCallPrivate::onStateChanged(int oldState, int newState, uint reason)
{
	Q_Q(MMCall);
	qCDebug(mmModemLog) << "Call state changed:" << oldState << "->"
			    << newState << reason;
	if (newState != MM_CALL_STATE_TERMINATED)
		return;

	if (m_hangupRequested)
		emit q->disconnected("local");
	else
		emit q->disconnected("remote");
}
