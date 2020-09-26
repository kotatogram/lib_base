// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "base/platform/base_platform_info.h"

#include <QtCore/QLocale>

namespace Platform {

QString DeviceModelPretty() {
	return QSysInfo::machineHostName();
}

QString SystemVersionPretty() {
	return QSysInfo::prettyProductName();
}

QString SystemCountry() {
	return QLocale::system().name().split('_').last();
}

QString SystemLanguage() {
	const auto system = QLocale::system();
	const auto languages = system.uiLanguages();
	return languages.isEmpty()
		? system.name().split('_').first()
		: languages.front();
}

} // namespace Platform
