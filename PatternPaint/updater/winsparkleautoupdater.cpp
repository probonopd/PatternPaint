#include "winsparkleautoupdater.h"
// TODO: Configure project correctly.
//#include <winsparkle.h>
#include "../../thirdparty/WinSparkle-0.4/include/winsparkle.h"

class WinSparkleAutoUpdater::Private {
public:
    QString url;
};

WinSparkleAutoUpdater::WinSparkleAutoUpdater(const QString& url)
{
    d = new Private();
    d->url = url;

    // Setup updates feed. This must be done before win_sparkle_init(), but
    // could be also, often more conveniently, done using a VERSIONINFO Windows
    // resource. See the "psdk" example and its .rc file for an example of that
    // (these calls wouldn't be needed then).
    // TODO: Better string conversion here?
    win_sparkle_set_appcast_url(d->url.toStdString().c_str());

    win_sparkle_set_app_details(
                L"A",
                L"A",
                L"A");
}

void WinSparkleAutoUpdater::checkForUpdates()
{
    // Initialize the updater and start auto-updating
    win_sparkle_init();
}

WinSparkleAutoUpdater::~WinSparkleAutoUpdater()
{
    win_sparkle_cleanup();
    delete d;
}

