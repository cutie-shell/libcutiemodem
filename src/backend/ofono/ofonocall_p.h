#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "../../cutiecall_p.h"

Q_DECLARE_LOGGING_CATEGORY(ofonoModemLog)

class OfonoCall : public CutieCall {
	Q_OBJECT
	Q_DECLARE_PRIVATE(OfonoCall)
    public:
	OfonoCall(QObject *parent, QString path, QVariantMap props);
	~OfonoCall();

	QString lineIdentification() override;
	CallState state() override;

	Q_INVOKABLE void answer() override;
	Q_INVOKABLE void hangup() override;
};

class OfonoCallPrivate : public CutieCallPrivate {
	Q_OBJECT
	Q_DECLARE_PUBLIC(OfonoCall)
    public:
	OfonoCallPrivate(OfonoCall *q);
	~OfonoCallPrivate();

    public slots:
	void onPropertyChanged(QString name, QDBusVariant value);
	void onDisconnectReason(QString reason);

    protected:
	QString m_path;
	QVariantMap m_data;
};
