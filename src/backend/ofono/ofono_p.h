#pragma once
#include "../backend.h"
#include "ofonomodem_p.h"

Q_DECLARE_LOGGING_CATEGORY(ofonoModemLog)

class OfonoBackend : public Backend {
	Q_OBJECT
    public:
	OfonoBackend(QObject *parent = 0);
	~OfonoBackend();

	void init() override;
	void deinit() override;

    public slots:
	void onNameOwnerChanged(QString name, QString oldOwner,
				QString newOwner) override;

    private slots:
	void onModemAdded(QDBusObjectPath path, QVariantMap props);
	void onModemRemoved(QDBusObjectPath path);

    private:
	QMap<QString, OfonoModem *> m_modems;
};
