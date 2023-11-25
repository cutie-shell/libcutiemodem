#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"

class CutieCallPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC (CutieCall)

public:
    CutieCallPrivate(CutieCall *q);
    ~CutieCallPrivate();

protected:
    QString m_path;
    QVariantMap m_data;
    
    CutieCall *q_ptr;
};