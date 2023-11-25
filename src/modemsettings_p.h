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

protected:
	QMap<QString,CutieModem *> m_modems;

	ModemSettings *q_ptr;
};
