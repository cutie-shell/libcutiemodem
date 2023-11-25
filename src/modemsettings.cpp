#include "modemsettings_p.h"
#include "ofonomodem_p.h"

ModemSettings::ModemSettings(QObject *parent)
	: QObject(parent)
	, d_ptr(new ModemSettingsPrivate(this)) {
	Q_D(ModemSettings);
	qDBusRegisterMetaType<OfonoServicePair>();
	qDBusRegisterMetaType<OfonoServiceList>();
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
			d->m_modems.insert(p.first.path(), m);
		}

		emit modemsChanged(d->m_modems.values());
	}
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
