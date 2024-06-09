#pragma once
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "backend/backend.h"

Q_DECLARE_LOGGING_CATEGORY(modemLog)

class ModemSettingsPrivate : public QObject {
	Q_OBJECT
	Q_DECLARE_PUBLIC(ModemSettings)

    public:
	ModemSettingsPrivate(ModemSettings *q);

    protected slots:
	void onModemAdded(CutieModem *modem);
	void onModemRemoved(CutieModem *modem);

    protected:
	QList<CutieModem *> m_modems;
	QList<Backend *> m_backends;
	ModemSettings *q_ptr;
};
