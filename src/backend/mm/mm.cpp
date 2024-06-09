#include "mm_p.h"

Q_LOGGING_CATEGORY(mmModemLog, "cutiemodem.mm")

MMBackend::MMBackend(QObject *parent)
	: Backend(parent)
{
	qDBusRegisterMetaType<MMInterfaceMap>();
	qDBusRegisterMetaType<MMObjectMap>();
	qDBusRegisterMetaType<MMSignalQuality>();
}

MMBackend::~MMBackend()
{
}

void MMBackend::init()
{
	QDBusReply<bool> mmHasOwner =
		QDBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus",
			       "org.freedesktop.DBus",
			       QDBusConnection::systemBus())
			.call("NameHasOwner", "org.freedesktop.ModemManager1");

	QDBusConnection::systemBus().connect(
		"org.freedesktop.DBus", "/org/freedesktop/DBus",
		"org.freedesktop.DBus", "NameOwnerChanged", this,
		SLOT(onNameOwnerChanged(QString, QString, QString)));

	if (!mmHasOwner.isValid() || !mmHasOwner.value())
		return;

	qCInfo(mmModemLog) << "ModemManager is available";

	QDBusReply<MMObjectMap> mmModems =
		QDBusInterface("org.freedesktop.ModemManager1",
			       "/org/freedesktop/ModemManager1",
			       "org.freedesktop.DBus.ObjectManager",
			       QDBusConnection::systemBus())
			.call("GetManagedObjects");
	if (mmModems.isValid()) {
		qCInfo(mmModemLog)
			<< "Found" << mmModems.value().count() << "modems";
		foreach(QDBusObjectPath p, mmModems.value().keys()) {
			MMModem *m = new MMModem();
			m->setPath(p.path());
			QVariantMap props =
				mmModems.value()
					[p]
					["org.freedesktop.ModemManager1.Modem"];
			QVariantMap props3gpp =
				mmModems.value()
					[p]
					["org.freedesktop.ModemManager1.Modem.Modem3gpp"];
			m->readProperties(props, props3gpp);
			m_modems.insert(p.path(), m);
			emit modemAdded(m);
		}
	} else
		qCWarning(mmModemLog) << "Failed to get modems";

	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1",
		"/org/freedesktop/ModemManager/Modems",
		"org.freedesktop.DBus.ObjectManager", "InterfacesAdded", this,
		SLOT(inInterfacesAdded(QDBusObjectPath, QVariantMap)));

	QDBusConnection::systemBus().connect(
		"org.freedesktop.ModemManager1",
		"/org/freedesktop/ModemManager/Modems",
		"org.freedesktop.DBus.ObjectManager", "InterfacesRemoved", this,
		SLOT(onInterfacesRemoved(QDBusObjectPath, QStringList)));
}

void MMBackend::deinit()
{
	for (QString m : m_modems.keys()) {
		emit modemRemoved(m_modems[m]);
		delete m_modems[m];
		m_modems.remove(m);
	}
}

void MMBackend::onNameOwnerChanged(QString name, QString oldOwner,
				   QString newOwner)
{
	if (name != "org.freedesktop.ModemManager1")
		return;

	qCInfo(mmModemLog) << "ModemManager bus owner changed:" << oldOwner
			   << "->" << newOwner;

	if (oldOwner != "")
		deinit();
	if (newOwner != "")
		init();
}

void MMBackend::onInterfacesAdded(QDBusObjectPath path, QVariantMap interfaces)
{
	if (!interfaces.contains("org.freedesktop.ModemManager1.Modem"))
		return;

	QVariantMap props =
		interfaces["org.freedesktop.ModemManager1.Modem"].toMap();
	QVariantMap props3gpp =
		interfaces["org.freedesktop.ModemManager1.Modem.3gpp"].toMap();
	MMModem *m = new MMModem();
	m->setPath(path.path());
	m->readProperties(props, props3gpp);
	m_modems.insert(path.path(), m);

	qCDebug(mmModemLog) << "Modem added:" << path.path();
	emit modemAdded(m);
}

void MMBackend::onInterfacesRemoved(QDBusObjectPath path,
				    QStringList interfaces)
{
	if (!interfaces.contains("org.freedesktop.ModemManager1.Modem"))
		return;

	MMModem *m = m_modems.take(path.path());
	qCDebug(mmModemLog) << "Modem removed:" << path.path();
	emit modemRemoved(m);
	delete m;
}
