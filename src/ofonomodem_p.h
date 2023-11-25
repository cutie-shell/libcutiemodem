#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"
#include "cutiemodem_p.h"

class OfonoModemPrivate : public CutieModemPrivate {
    Q_OBJECT
    Q_DECLARE_PUBLIC (OfonoModem)
public:
    OfonoModemPrivate(OfonoModem *q);
    ~OfonoModemPrivate();

public slots:
    void onPropertyChanged(QString name, QDBusVariant value);
    void onSimPropertyChanged(QString name, QDBusVariant value);
    void onNetPropertyChanged(QString name, QDBusVariant value);
	void onIncomingMessage(QString message, QVariantMap props);
	void onCallAdded(QDBusObjectPath path, QVariantMap props);
	void onCallRemoved(QDBusObjectPath path);
};

typedef QPair<QDBusObjectPath, QVariantMap> OfonoServicePair;
typedef QList<OfonoServicePair> OfonoServiceList;

Q_DECLARE_METATYPE(OfonoServicePair)
Q_DECLARE_METATYPE(OfonoServiceList)