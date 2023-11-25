#pragma once
#include <QDebug>
#include <QtQuick>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusMetaType>
#include "cutiemodem.h"

class CutieModemPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CutieModem)
public:
    CutieModemPrivate(CutieModem *q);
    ~CutieModemPrivate();

public slots:
    void onCallAudioPropertiesChanged(QString iface, QMap<QString, QVariant> updated, QStringList invalidated);

protected:
    void init();

    QString m_path;
    QVariantMap m_data;
    QVariantMap m_simData;
    QVariantMap m_netData;
    QMap<QString, CutieCall*> m_calls;
    uint m_audioMode;
    
    CutieModem *q_ptr;
};