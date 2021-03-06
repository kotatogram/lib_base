// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "base/platform/linux/base_info_linux.h"

#include "base/platform/linux/base_linux_gtk_integration.h"

#ifndef DESKTOP_APP_DISABLE_X11_INTEGRATION
#include "base/platform/linux/base_linux_xcb_utilities.h"
#endif // !DESKTOP_APP_DISABLE_X11_INTEGRATION

#include <QtCore/QJsonObject>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QLocale>
#include <QtCore/QVersionNumber>
#include <QtCore/QDate>
#include <QtGui/QGuiApplication>

#include <glib.h>
#include <glibmm.h>
#include <giomm.h>

// this file is used on both Linux & BSD
#ifdef Q_OS_LINUX
#include <gnu/libc-version.h>
#endif // Q_OS_LINUX

namespace Platform {

QDate WhenSystemBecomesOutdated() {
	const auto libcName = GetLibcName();
	const auto libcVersion = GetLibcVersion();

	if (IsLinux32Bit()) {
		return QDate(2020, 9, 1);
	} else if (libcName == qstr("glibc") && !libcVersion.isEmpty()) {
		if (QVersionNumber::fromString(libcVersion) < QVersionNumber(2, 23)) {
			return QDate(2020, 9, 1); // Older than Ubuntu 16.04.
		}
	}

	return QDate();
}

OutdateReason WhySystemBecomesOutdated() {
	return IsLinux32Bit() ? OutdateReason::Is32Bit : OutdateReason::IsOld;
}

int AutoUpdateVersion() {
	return 2;
}

QString AutoUpdateKey() {
	if (IsLinux32Bit()) {
		return "linux32";
	} else if (IsLinux64Bit()) {
		return "linux";
	} else {
		Unexpected("Platform in AutoUpdateKey.");
	}
}

QString GetLibcName() {
#ifdef Q_OS_LINUX
	return "glibc";
#endif // Q_OS_LINUX

	return QString();
}

QString GetLibcVersion() {
#ifdef Q_OS_LINUX
	static const auto result = [&] {
		const auto version = QString::fromLatin1(gnu_get_libc_version());
		return QVersionNumber::fromString(version).isNull() ? QString() : version;
	}();
	return result;
#endif // Q_OS_LINUX

	return QString();
}

QString GetWindowManager() {
#ifndef DESKTOP_APP_DISABLE_X11_INTEGRATION
	base::Platform::XCB::CustomConnection connection;
	if (xcb_connection_has_error(connection)) {
		return {};
	}

	const auto root = base::Platform::XCB::GetRootWindow(connection);
	if (!root.has_value()) {
		return {};
	}

	const auto nameAtom = base::Platform::XCB::GetAtom(
		connection,
		"_NET_WM_NAME");

	const auto utf8Atom = base::Platform::XCB::GetAtom(
		connection,
		"UTF8_STRING");

	const auto supportingWindow = base::Platform::XCB::GetSupportingWMCheck(
		connection,
		*root);

	if (!nameAtom.has_value()
		|| !utf8Atom.has_value()
		|| !supportingWindow.has_value()
		|| *supportingWindow == XCB_WINDOW_NONE) {
		return {};
	}

	const auto cookie = xcb_get_property(
		connection,
		false,
		*supportingWindow,
		*nameAtom,
		*utf8Atom,
		0,
		1024);

	const auto reply = base::Platform::XCB::MakeReplyPointer(
		xcb_get_property_reply(
			connection,
			cookie,
			nullptr));

	if (!reply) {
		return {};
	}

	return (reply->format == 8 && reply->type == *utf8Atom)
		? QString::fromUtf8(
			reinterpret_cast<const char*>(
				xcb_get_property_value(reply.get())),
			xcb_get_property_value_length(reply.get()))
		: QString();
#else // !DESKTOP_APP_DISABLE_X11_INTEGRATION
	return QString();
#endif // !DESKTOP_APP_DISABLE_X11_INTEGRATION
}

bool IsX11() {
	return QGuiApplication::instance()
		? QGuiApplication::platformName() == "xcb"
		: qEnvironmentVariableIsSet("DISPLAY");
}

bool IsWayland() {
	return QGuiApplication::instance()
		? QGuiApplication::platformName().startsWith(
			"wayland",
			Qt::CaseInsensitive)
		: qEnvironmentVariableIsSet("WAYLAND_DISPLAY");
}

void Start(QJsonObject options) {
	Glib::init();
	Gio::init();

	using base::Platform::GtkIntegration;
	if (const auto integration = GtkIntegration::Instance()) {
		integration->prepareEnvironment();
		integration->load();
		integration->initializeSettings();
	} else {
		g_warning("GTK integration is disabled, some features unavailable.");
	}
}

void Finish() {
}

} // namespace Platform
