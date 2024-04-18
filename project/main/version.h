#ifndef VERSION_H
#define VERSION_H

#include <QString>

#define PG_VERSION_MAJOR                2               /**< Major version number (X.x.x) */
#define PG_VERSION_MINOR                0               /**< Minor version number (x.X.x) */
#define PG_VERSION_PATCH                3               /**< Patch version number (x.x.X) */

#define PG_VERSION_TYPE                 "Release"       /**< [Alpha] [Beta] [Release] >*/

#define GITEE_URL   "https://gitee.com/rtduino/pinout-generator/raw/master/software/updates-gitee.json"
#define GITHUB_URL  "https://raw.githubusercontent.com/RTduino/pinout-generator/master/software/updates-github.json"

QString get_version_string(void);
QString get_version_type(void);

#endif // VERSION_H
