// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "base/platform/win/base_info_win.h"

#include "base/platform/base_platform_info.h"
#include "base/platform/win/base_windows_h.h"

#include <QtCore/QOperatingSystemVersion>
#include <QtCore/QJsonObject>
#include <QtCore/QDate>

#include <VersionHelpers.h>

namespace Platform {

QDate WhenSystemBecomesOutdated() {
	return QDate();
}

int AutoUpdateVersion() {
	return 2;
}

QString AutoUpdateKey() {
	if (IsWindows64Bit()) {
		return "win64";
	} else {
		return "win";
	}
}

bool IsWindows7OrGreater() {
	static const auto result = ::IsWindows7OrGreater();
	return result;
}

bool IsWindows8OrGreater() {
	static const auto result = ::IsWindows8OrGreater();
	return result;
}

bool IsWindows8Point1OrGreater() {
	static const auto result = ::IsWindows8Point1OrGreater();
	return result;
}

bool IsWindows10OrGreater() {
	static const auto result = ::IsWindows10OrGreater();
	return result;
}

bool IsWindows11OrGreater() {
	static const auto result = [&] {
		if (!IsWindows10OrGreater()) {
			return false;
		}
		const auto version = QOperatingSystemVersion::current();
		return (version.majorVersion() > 10)
			|| (version.microVersion() >= 22000);
	}();
	return result;
}

void Start(QJsonObject settings) {
	SetDllDirectory(L"");
}

void Finish() {
}

} // namespace Platform
