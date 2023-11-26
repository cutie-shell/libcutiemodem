#include "cutiemodem_p.h"

CutieModem::CutieModem(QObject *parent)
    : QObject(parent)
    , d_ptr(new CutieModemPrivate(this)) { }

CutieModem::CutieModem(CutieModemPrivate *d, QObject *parent)
    : QObject(parent)
    , d_ptr(d) { }

CutieModem::~CutieModem() {}

QString CutieModem::path() {
    Q_D(CutieModem);
	return d->m_path;
}

QVariantMap CutieModem::data() {
    Q_D(CutieModem);
	return d->m_data;
}

QVariantMap CutieModem::simData() {
    Q_D(CutieModem);
	return d->m_simData;
}

QVariantMap CutieModem::netData() {
    Q_D(CutieModem);
	return d->m_netData;
}

uint CutieModem::audioMode() {
    Q_D(CutieModem);
	return d->m_audioMode;
}

QList<CutieCall*> CutieModem::calls() {
    Q_D(CutieModem);
    return d->m_calls.values();
}

void CutieModem::setAudioMode(uint mode) {
    QDBusInterface(
	    "org.mobian_project.CallAudio",
        "/org/mobian_project/CallAudio", 
	    "org.mobian_project.CallAudio",
        QDBusConnection::sessionBus()
    ).call(
        "SelectMode", mode
    );
}

CutieModemPrivate::CutieModemPrivate(CutieModem *q)
:  q_ptr(q) { }

CutieModemPrivate::~CutieModemPrivate() { }

void CutieModemPrivate::init() {
    QDBusReply<QDBusVariant> audioModeReply = QDBusInterface(
	    "org.mobian_project.CallAudio",
        "/org/mobian_project/CallAudio", 
	    "org.freedesktop.DBus.Properties",
        QDBusConnection::sessionBus()
    ).call(
        "Get", "org.mobian_project.CallAudio", "AudioMode"
    );
    m_audioMode = audioModeReply.isValid() 
        ? qdbus_cast<uint>(audioModeReply.value().variant())
        : 0;

    QDBusConnection::sessionBus().connect(
        "org.mobian_project.CallAudio",
        "/org/mobian_project/CallAudio", 
        "org.freedesktop.DBus.Properties", 
        "PropertiesChanged",
        this, SLOT(onCallAudioPropertiesChanged(QString,QMap<QString, QVariant>,QStringList)));
}

void CutieModemPrivate::onCallAudioPropertiesChanged(QString iface, QMap<QString, QVariant> updated, QStringList invalidated) {
    Q_UNUSED(invalidated);
    Q_Q(CutieModem);
    if (iface == "org.mobian_project.CallAudio") {
        if (updated.contains("AudioMode")) {
            m_audioMode = qdbus_cast<uint>(updated.value("AudioMode"));
            emit q->audioModeChanged(m_audioMode);
        }
    };
}