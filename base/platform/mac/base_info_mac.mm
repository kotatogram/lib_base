// This file is part of Desktop App Toolkit,
// a set of libraries for developing nice desktop applications.
//
// For license and copyright information please follow this link:
// https://github.com/desktop-app/legal/blob/master/LEGAL
//
#include "base/platform/mac/base_info_mac.h"

#include "base/timer.h"
#include "base/platform/base_platform_info.h"
#include "base/platform/mac/base_utilities_mac.h"

#include <sys/sysctl.h>
#include <Cocoa/Cocoa.h>

#include <QtCore/QDate>
#include <QtCore/QJsonObject>

@interface WakeUpObserver : NSObject {
}

- (void) receiveWakeNote:(NSNotification*)note;

@end // @interface WakeUpObserver

@implementation WakeUpObserver {
}

- (void) receiveWakeNote:(NSNotification*)aNotification {
	base::CheckLocalTime();
}

@end // @implementation WakeUpObserver

namespace Platform {
namespace {

WakeUpObserver *GlobalWakeUpObserver = nil;

int MinorVersion() {
	static const int version = QSysInfo::macVersion();
	constexpr int kShift = 2;
	if (version == QSysInfo::MV_Unknown || version < kShift + 6) {
		return 0;
	}
	return version - kShift;
}

template <int Minor>
bool IsMacThatOrGreater() {
	static const auto result = (MinorVersion() >= Minor);
	return result;
}

} // namespace

QDate WhenSystemBecomesOutdated() {
	if (!IsMac10_10OrGreater()) {
		return QDate(2019, 9, 1);
	} else if (!IsMac10_12OrGreater()) {
		return QDate(2020, 9, 1);
	}
	return QDate();
}

int AutoUpdateVersion() {
	if (!IsMac10_10OrGreater()) {
		return 1;
	}
	return 2;
}

QString AutoUpdateKey() {
	if (!IsMac10_12OrGreater()) {
		return "osx";
	} else {
		return "mac";
	}
}

bool IsMac10_6OrGreater() {
	return IsMacThatOrGreater<6>();
}

bool IsMac10_7OrGreater() {
	return IsMacThatOrGreater<7>();
}

bool IsMac10_8OrGreater() {
	return IsMacThatOrGreater<8>();
}

bool IsMac10_9OrGreater() {
	return IsMacThatOrGreater<9>();
}

bool IsMac10_10OrGreater() {
	return IsMacThatOrGreater<10>();
}

bool IsMac10_11OrGreater() {
	return IsMacThatOrGreater<11>();
}

bool IsMac10_12OrGreater() {
	return IsMacThatOrGreater<12>();
}

bool IsMac10_13OrGreater() {
	return IsMacThatOrGreater<13>();
}

bool IsMac10_14OrGreater() {
	return IsMacThatOrGreater<14>();
}

void Start(QJsonObject settings) {
	Expects(GlobalWakeUpObserver == nil);

	GlobalWakeUpObserver = [[WakeUpObserver alloc] init];

	NSNotificationCenter *center = [[NSWorkspace sharedWorkspace] notificationCenter];
	Assert(center != nil);

	[center
		addObserver: GlobalWakeUpObserver
		selector: @selector(receiveWakeNote:)
		name: NSWorkspaceDidWakeNotification
		object: nil];

	Ensures(GlobalWakeUpObserver != nil);
}

void Finish() {
	Expects(GlobalWakeUpObserver != nil);

	[GlobalWakeUpObserver release];
	GlobalWakeUpObserver = nil;
}

} // namespace Platform
