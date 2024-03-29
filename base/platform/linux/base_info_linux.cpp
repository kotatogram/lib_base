// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "base/platform/linux/base_info_linux.h"

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

#ifdef Q_OS_LINUX
#include <gnu/libc-version.h>
#endif // Q_OS_LINUX

namespace Platform {

QDate WhenSystemBecomesOutdated() {
	return QDate();
}

int AutoUpdateVersion() {
	return 2;
}

QString AutoUpdateKey() {
	return "linux";
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
#endif // DESKTOP_APP_DISABLE_X11_INTEGRATION
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
}

void Finish() {
}

} // namespace Platform
