#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "../../cutiecall_p.h"

Q_DECLARE_LOGGING_CATEGORY(mmModemLog)

typedef enum {
	MM_CALL_STATE_UNKNOWN = 0,
	MM_CALL_STATE_DIALING = 1,
	MM_CALL_STATE_RINGING_OUT = 2,
	MM_CALL_STATE_RINGING_IN = 3,
	MM_CALL_STATE_ACTIVE = 4,
	MM_CALL_STATE_HELD = 5,
	MM_CALL_STATE_WAITING = 6,
	MM_CALL_STATE_TERMINATED = 7
} MMCallState;

typedef enum {
	MM_CALL_STATE_REASON_UNKNOWN = 0,
	MM_CALL_STATE_REASON_OUTGOING_STARTED = 1,
	MM_CALL_STATE_REASON_INCOMING_NEW = 2,
	MM_CALL_STATE_REASON_ACCEPTED = 3,
	MM_CALL_STATE_REASON_TERMINATED = 4,
	MM_CALL_STATE_REASON_REFUSED_OR_BUSY = 5,
	MM_CALL_STATE_REASON_ERROR = 6,
	MM_CALL_STATE_REASON_AUDIO_SETUP_FAILED = 7,
	MM_CALL_STATE_REASON_TRANSFERRED = 8,
	MM_CALL_STATE_REASON_DEFLECTED = 9,
} MMCallStateReason;

class MMCallPrivate;

class MMCall : public CutieCall {
	Q_OBJECT
	Q_DECLARE_PRIVATE(MMCall)
    public:
	MMCall(QObject *parent, QString path, QVariantMap props);
	~MMCall();

	QString lineIdentification() override;
	CallState state() override;

	Q_INVOKABLE void answer() override;
	Q_INVOKABLE void hangup() override;
};

class MMCallPrivate : public CutieCallPrivate {
	Q_OBJECT
	Q_DECLARE_PUBLIC(MMCall)
    public:
	MMCallPrivate(MMCall *q);
	~MMCallPrivate();

    public slots:
	void onPropertiesChanged(QString interface, QVariantMap values,
				 QStringList invalidated);
	void onStateChanged(int oldState, int newState, uint reason);

    protected:
	QString m_path;
	QVariantMap m_data;
	bool m_hangupRequested;
};
