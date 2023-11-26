#include "modemsettings_p.h"
#include "ofonomodem_p.h"

ModemSettings::ModemSettings(QObject *parent)
	: QObject(parent)
	, d_ptr(new ModemSettingsPrivate(this)) {
	Q_D(ModemSettings);
	qDBusRegisterMetaType<OfonoServicePair>();
	qDBusRegisterMetaType<OfonoServiceList>();

	QDBusReply<bool> ofonoHasOwner = QDBusInterface(
		"org.freedesktop.DBus",
		"/org/freedesktop/DBus",
		"org.freedesktop.DBus",
		QDBusConnection::systemBus()
	).call("NameHasOwner", "org.ofono");
	if (ofonoHasOwner.isValid() && ofonoHasOwner.value())
		d->initOfonoBackend();

	QDBusConnection::systemBus().connect(
		"org.freedesktop.DBus",
		"/org/freedesktop/DBus",
		"org.freedesktop.DBus", 
		"NameOwnerChanged",
		d, SLOT(onNameOwnerChanged(QString, QString, QString)));
}

QList<CutieModem *> ModemSettings::modems() {
	Q_D(ModemSettings);
	return d->m_modems.values();
}

QObject *ModemSettings::provider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(scriptEngine);
    return new ModemSettings(engine);
}

ModemSettingsPrivate::ModemSettingsPrivate(ModemSettings *q)
	: q_ptr(q) { }

void ModemSettingsPrivate::initOfonoBackend() {
	Q_Q(ModemSettings);
	QDBusReply<OfonoServiceList> ofonoModems = QDBusInterface(
		"org.ofono",
		"/", 
		"org.ofono.Manager",
		QDBusConnection::systemBus()
	).call("GetModems");
	if (ofonoModems.isValid()) {
		foreach (OfonoServicePair p, ofonoModems.value()) {
			OfonoModem *m = new OfonoModem();
			m->setPath(p.first.path());
			m_ofono_modems.insert(p.first.path(), m);
			m_modems.insert(p.first.path(), m);
		}

		emit q->modemsChanged(m_modems.values());
	}

	QDBusConnection::systemBus().connect(
		"org.ofono",
		"/",
		"org.ofono.Manager", 
		"ModemAdded",
		this, SLOT(onOfonoModemAdded(QDBusObjectPath,QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.ofono",
		"/",
		"org.ofono.Manager", 
		"ModemRemoved",
		this, SLOT(onOfonoModemRemoved(QDBusObjectPath)));
}

void ModemSettingsPrivate::deinitOfonoBackend() {
	for (QString m : m_ofono_modems.keys()) {
		delete m_ofono_modems[m];
		m_ofono_modems.remove(m);
		m_modems.remove(m);
	}
}

void ModemSettingsPrivate::onNameOwnerChanged(QString name, QString oldOwner, QString newOwner) {
	if (name == "org.ofono") {
		if (newOwner != "" && oldOwner != "") {
			deinitOfonoBackend();
			initOfonoBackend();
		} else if (oldOwner != "") {
			deinitOfonoBackend();
		} else {
			initOfonoBackend();
		}
	}
}

void ModemSettingsPrivate::onOfonoModemAdded(QDBusObjectPath path, QVariantMap props) {
	Q_Q(ModemSettings);
	OfonoModem *m = new OfonoModem();
	m->setPath(path.path());
	m_ofono_modems.insert(path.path(), m);
	m_modems.insert(path.path(), m);
	emit q->modemsChanged(m_modems.values());
}

void ModemSettingsPrivate::onOfonoModemRemoved(QDBusObjectPath path) {
	Q_Q(ModemSettings);
	m_ofono_modems.remove(path.path());
	m_modems.remove(path.path());
	delete m_modems[path.path()];
	emit q->modemsChanged(m_modems.values());
}
