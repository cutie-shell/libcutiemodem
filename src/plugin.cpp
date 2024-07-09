#include "plugin.h"

void CutieModemPlugin::registerTypes(const char *uri)
{
	qmlRegisterSingletonType<ModemSettings>(uri, 1, 0, "CutieModemSettings",
						&ModemSettings::provider);

	qmlRegisterType<CutieModem::NetworkStatus>(uri, 1, 0, "NetworkStatus");
	qmlRegisterType<CutieModem::NetworkTechnology>(uri, 1, 0,
						       "NetworkTechnology");

	qmlRegisterUncreatableType<CutieModem>(
		uri, 1, 0, "CutieModem",
		"Cannot be crated directly, use CutieModemSettings to get available modems");
	qmlRegisterUncreatableType<CutieCall>(
		uri, 1, 0, "CutieCall",
		"Cannot be crated directly, use dial() on the modem object to call");
}
