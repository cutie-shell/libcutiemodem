#include "modemsettings_p.h"
#include "backend/ofono/ofono_p.h"

Q_LOGGING_CATEGORY(modemLog, "cutiemodem")

ModemSettings::ModemSettings(QObject *parent)
	: QObject(parent)
	, d_ptr(new ModemSettingsPrivate(this))
{
	Q_D(ModemSettings);
}

QList<CutieModem *> ModemSettings::modems()
{
	Q_D(ModemSettings);
	return d->m_modems;
}

QObject *ModemSettings::provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
	Q_UNUSED(scriptEngine);
	return new ModemSettings(engine);
}

ModemSettingsPrivate::ModemSettingsPrivate(ModemSettings *q)
	: q_ptr(q)
{
	QByteArray requested = qgetenv("CUTIEMODEM_BACKENDS");
	if (requested.isNull())
		requested = "ofono";

	QStringList backends = QString::fromUtf8(requested).split(';');
	foreach(const QString &b, backends) {
		Backend *backend = nullptr;
		if (b == "ofono")
			backend = new OfonoBackend(this);
		else {
			qCWarning(modemLog)
				<< "Unknown backend requested:" << b;
			continue;
		}

		connect(backend, &Backend::modemAdded, this,
			&ModemSettingsPrivate::onModemAdded);
		connect(backend, &Backend::modemRemoved, this,
			&ModemSettingsPrivate::onModemRemoved);
		backend->init();
		m_backends << backend;
	}
}

void ModemSettingsPrivate::onModemAdded(CutieModem *modem)
{
	Q_Q(ModemSettings);
	m_modems << modem;
	emit q->modemAdded(modem);
	emit q->modemsChanged(m_modems);
}

void ModemSettingsPrivate::onModemRemoved(CutieModem *modem)
{
	Q_Q(ModemSettings);
	m_modems.removeOne(modem);
	emit q->modemRemoved(modem);
	emit q->modemsChanged(m_modems);
}
