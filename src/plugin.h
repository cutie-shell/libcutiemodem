#pragma once
#include <QDebug>
#include <QtQuick>
#include <QtQml/qqml.h>
#include <QtQml/QQmlExtensionPlugin>

#include "cutiemodem.h"

class CutieModemPlugin : public QQmlExtensionPlugin {
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlEngineExtensionInterface_iid FILE
			  "cutiemodem.json")
    public:
	explicit CutieModemPlugin()
	{
	}

	void registerTypes(const char *uri) override;
};