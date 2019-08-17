#pragma once

#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>

namespace QtAutoUpdater {

	class AdminAuthoriser {
		public:
			virtual inline ~AdminAuthoriser() {}
			//! Tests whether this program already has elevated rights or not
			virtual bool hasAdminRights() = 0;
			//! Runs a program with the given arguments with elevated rights
			virtual bool executeAsAdmin(const QString &program, const QStringList &arguments) = 0;
	};

}
