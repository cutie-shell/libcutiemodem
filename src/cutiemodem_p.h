#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"

Q_DECLARE_LOGGING_CATEGORY(modemLog)

class CutieModemPrivate : public QObject {
	Q_OBJECT
	Q_DECLARE_PUBLIC(CutieModem)
    public:
	CutieModemPrivate(CutieModem *q);
	~CutieModemPrivate();

    public slots:
	void onCallAudioPropertiesChanged(QString iface,
					  QMap<QString, QVariant> updated,
					  QStringList invalidated);

    protected:
	void init();

	QMap<QString, CutieCall *> m_calls;
	uint m_audioMode;

	CutieModem *q_ptr;
};