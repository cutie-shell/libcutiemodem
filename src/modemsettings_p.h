#pragma once
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"

class ModemSettingsPrivate : public QObject {
	Q_OBJECT
	Q_DECLARE_PUBLIC (ModemSettings)

public:
	ModemSettingsPrivate(ModemSettings *q);

public slots:
	void onNameOwnerChanged(QString name, QString oldOwner, QString newOwner);
	void onOfonoModemAdded(QDBusObjectPath path, QVariantMap props);
	void onOfonoModemRemoved(QDBusObjectPath path);

protected:
	void initOfonoBackend();
	void deinitOfonoBackend();

	QMap<QString,CutieModem *> m_modems;
	QMap<QString,OfonoModem *> m_ofono_modems;

	ModemSettings *q_ptr;
};
