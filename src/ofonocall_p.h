#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"
#include "cutiecall_p.h"

class OfonoCallPrivate : public CutieCallPrivate {
    Q_OBJECT
    Q_DECLARE_PUBLIC (OfonoCall)
public:
    OfonoCallPrivate(OfonoCall *q);
    ~OfonoCallPrivate();

public slots:
    void onPropertyChanged(QString name, QDBusVariant value);
    void onDisconnectReason(QString reason);
};