#pragma once
#include "../backend.h"
#include "mmmodem_p.h"

Q_DECLARE_LOGGING_CATEGORY(mmModemLog)

class MMBackend : public Backend {
	Q_OBJECT
    public:
	MMBackend(QObject *parent = 0);
	~MMBackend();

	void init() override;
	void deinit() override;

    public slots:
	void onNameOwnerChanged(QString name, QString oldOwner,
				QString newOwner) override;

    private slots:
	void onInterfacesAdded(QDBusObjectPath path, QVariantMap interfaces);
	void onInterfacesRemoved(QDBusObjectPath path, QStringList interfaces);

    private:
	QMap<QString, MMModem *> m_modems;
};
