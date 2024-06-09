#include "ofono_p.h"

Q_LOGGING_CATEGORY(ofonoModemLog, "cutiemodem.ofono")

OfonoBackend::OfonoBackend(QObject *parent)
	: Backend(parent)
{
	qDBusRegisterMetaType<OfonoServicePair>();
	qDBusRegisterMetaType<OfonoServiceList>();
}

OfonoBackend::~OfonoBackend()
{
}

void OfonoBackend::init()
{
	QDBusReply<bool> ofonoHasOwner =
		QDBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus",
			       "org.freedesktop.DBus",
			       QDBusConnection::systemBus())
			.call("NameHasOwner", "org.ofono");

	QDBusConnection::systemBus().connect(
		"org.freedesktop.DBus", "/org/freedesktop/DBus",
		"org.freedesktop.DBus", "NameOwnerChanged", this,
		SLOT(onNameOwnerChanged(QString, QString, QString)));

	if (!ofonoHasOwner.isValid() || !ofonoHasOwner.value())
		return;

	qCInfo(ofonoModemLog) << "Ofono is available";

	QDBusReply<OfonoServiceList> ofonoModems =
		QDBusInterface("org.ofono", "/", "org.ofono.Manager",
			       QDBusConnection::systemBus())
			.call("GetModems");
	if (ofonoModems.isValid()) {
		qCInfo(ofonoModemLog) << "Found" << ofonoModems.value().count() << "modems";
		foreach(OfonoServicePair p, ofonoModems.value()) {
			OfonoModem *m = new OfonoModem();
			m->setPath(p.first.path());
			m_modems.insert(p.first.path(), m);
			emit modemAdded(m);
		}
	} else
		qCWarning(ofonoModemLog) << "Failed to get modems";

	QDBusConnection::systemBus().connect(
		"org.ofono", "/", "org.ofono.Manager", "ModemAdded", this,
		SLOT(onModemAdded(QDBusObjectPath, QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.ofono", "/", "org.ofono.Manager", "ModemRemoved", this,
		SLOT(onModemRemoved(QDBusObjectPath)));
}

void OfonoBackend::deinit()
{
	for (QString m : m_modems.keys()) {
		emit modemRemoved(m_modems[m]);
		delete m_modems[m];
		m_modems.remove(m);
	}
}

void OfonoBackend::onNameOwnerChanged(QString name, QString oldOwner,
				      QString newOwner)
{
	if (name != "org.ofono")
		return;

	qCInfo(ofonoModemLog) << "Ofono bus owner changed:" << oldOwner << "->" << newOwner;

	if (oldOwner != "")
		deinit();
	if (newOwner != "")
		init();
}

void OfonoBackend::onModemAdded(QDBusObjectPath path, QVariantMap props)
{
	OfonoModem *m = new OfonoModem();
	m->setPath(path.path());
	m_modems.insert(path.path(), m);

	qCDebug(ofonoModemLog) << "Modem added:" << path.path();
	emit modemAdded(m);
}

void OfonoBackend::onModemRemoved(QDBusObjectPath path)
{
	qCDebug(ofonoModemLog) << "Modem removed:" << path.path();
	emit modemRemoved(m_modems[path.path()]);
	m_modems.remove(path.path());
	delete m_modems[path.path()];
}