#pragma once
#include <QtQuick>

class CutieModemPrivate;
class CutieCallPrivate;
class ModemSettingsPrivate;
class OfonoCallPrivate;
class OfonoModemPrivate;

class CutieModem;

class CutieCall : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE (CutieCall);
    Q_PROPERTY (QString path READ path() NOTIFY pathChanged);
    Q_PROPERTY (QVariantMap data READ data() NOTIFY dataChanged);
    Q_PROPERTY (CutieModem *modem READ modem());

public:
    CutieCall(QObject *parent=0);
    ~CutieCall();

    QString path();
    QVariantMap data();
    CutieModem *modem();

    virtual Q_INVOKABLE void answer() = 0;
    virtual Q_INVOKABLE void hangup() = 0;

signals:
    void pathChanged(QString);
    void dataChanged(QVariantMap);
    void disconnected(QString);

protected:
    CutieCall(CutieCallPrivate *d, QObject *parent = 0);

protected:
    CutieCallPrivate *d_ptr;
};

class CutieModem : public QObject {
    Q_OBJECT
    Q_DECLARE_PRIVATE (CutieModem)
    Q_PROPERTY (QString path READ path() NOTIFY pathChanged);
    Q_PROPERTY (QVariantMap data READ data() NOTIFY dataChanged);
    Q_PROPERTY (QVariantMap simData READ simData() NOTIFY simDataChanged);
    Q_PROPERTY (QVariantMap netData READ netData() NOTIFY netDataChanged);
    Q_PROPERTY (uint audioMode READ audioMode() WRITE setAudioMode NOTIFY audioModeChanged);
    Q_PROPERTY (QList<CutieCall*> calls READ calls() NOTIFY callsChanged);

public:
    CutieModem(QObject *parent=0);
    ~CutieModem();

    QString path();
    QVariantMap data();
    QVariantMap simData();
    QVariantMap netData();
    uint audioMode();
    void setAudioMode(uint mode);
    QList<CutieCall*> calls();

    virtual void setPath(QString path) = 0;
    virtual Q_INVOKABLE void sendMessage(QString to, QString message) = 0;
    virtual Q_INVOKABLE QString dial(QString to, QString hideID = QString()) = 0;
    virtual Q_INVOKABLE void setProp(QString key, QVariant value) = 0;
    virtual Q_INVOKABLE void setSimProp(QString key, QVariant value) = 0;
    virtual Q_INVOKABLE void setNetProp(QString key, QVariant value) = 0;

signals:
    void pathChanged();
    void dataChanged();
    void simDataChanged();
    void netDataChanged();
    void audioModeChanged(uint mode);
	void incomingMessage(QString message, QVariantMap props);
	void newCall(CutieCall* call);
    void callsChanged(QList<CutieCall*> calls);

protected:
    CutieModem(CutieModemPrivate *d, QObject *parent = 0);

protected:
    CutieModemPrivate *d_ptr;
};

class ModemSettings : public QObject {
	Q_OBJECT
    Q_DECLARE_PRIVATE(ModemSettings)
	Q_PROPERTY(QList<CutieModem *> modems READ modems NOTIFY modemsChanged)
public:
	ModemSettings(QObject *parent = 0);
	QList<CutieModem *> modems();

	static QObject *provider(QQmlEngine *engine, QJSEngine *scriptEngine);

signals:
	void modemsChanged(QList<CutieModem *> modems);

protected:
    ModemSettingsPrivate *d_ptr;
};

class OfonoCall : public CutieCall {
    Q_OBJECT
    Q_DECLARE_PRIVATE (OfonoCall)
    Q_PROPERTY (QString path READ path() NOTIFY pathChanged);
    Q_PROPERTY (QVariantMap data READ data() NOTIFY dataChanged);
public:
    OfonoCall(QObject *parent, QString path, QVariantMap props);
    ~OfonoCall();

    Q_INVOKABLE void answer() override;
    Q_INVOKABLE void hangup() override;
    
Q_SIGNALS:
    void pathChanged(QString);
    void dataChanged(QVariantMap);
    void disconnected(QString);
};

class OfonoModem : public CutieModem {
    Q_OBJECT
    Q_DECLARE_PRIVATE (OfonoModem)
    Q_PROPERTY (QString path READ path() NOTIFY pathChanged);
    Q_PROPERTY (QVariantMap data READ data() NOTIFY dataChanged);
    Q_PROPERTY (QVariantMap simData READ simData() NOTIFY simDataChanged);
    Q_PROPERTY (QVariantMap netData READ netData() NOTIFY netDataChanged);
public:
    OfonoModem(QObject *parent=0);
    ~OfonoModem();

    void setPath(QString path) override;
    Q_INVOKABLE void sendMessage(QString to, QString message) override;
    Q_INVOKABLE QString dial(QString to, QString hideID = QString()) override;
    Q_INVOKABLE void setProp(QString key, QVariant value) override;
    Q_INVOKABLE void setSimProp(QString key, QVariant value) override;
    Q_INVOKABLE void setNetProp(QString key, QVariant value) override;
};
