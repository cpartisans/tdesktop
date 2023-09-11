/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/object_ptr.h"

namespace style {
struct ComposeIcons;
} // namespace style

namespace Api {
struct SendOptions;
} // namespace Api

namespace Ui {
class PopupMenu;
class RpWidget;
} // namespace Ui

namespace Data {
class Thread;
} // namespace Data

namespace SendMenu {

enum class Type {
	Disabled,
	SilentOnly,
	Scheduled,
	ScheduledToUser, // For "Send when online".
	Reminder,
};

enum class FillMenuResult {
	Success,
	None,
};

Fn<void()> DefaultSilentCallback(Fn<void(Api::SendOptions)> send);
Fn<void()> DefaultScheduleCallback(
	not_null<Ui::RpWidget*> parent,
	Type type,
	Fn<void(Api::SendOptions)> send);
Fn<void()> DefaultWhenOnlineCallback(Fn<void(Api::SendOptions)> send);

FillMenuResult FillSendMenu(
	not_null<Ui::PopupMenu*> menu,
	Type type,
	Fn<void()> silent,
	Fn<void()> schedule,
	Fn<void()> whenOnline,
	Fn<void()> autoDelete,
	const style::ComposeIcons *iconsOverride = nullptr);

void SetupMenuAndShortcuts(
	not_null<Ui::RpWidget*> button,
	Fn<Type()> type,
	Fn<void()> silent,
	Fn<void()> schedule,
	Fn<void()> whenOnline,
	Fn<void()> autoDelete);

void SetupUnreadMentionsMenu(
	not_null<Ui::RpWidget*> button,
	Fn<Data::Thread*()> currentThread);

void SetupUnreadReactionsMenu(
	not_null<Ui::RpWidget*> button,
	Fn<Data::Thread*()> currentThread);
} // namespace SendMenu

namespace Ui {
class BoxContent;
}

namespace SendMenu {

Fn<void()> DefaultAutoDeleteCallback(
	not_null<Ui::RpWidget*> parent,
	Fn<void(object_ptr<Ui::BoxContent>)> show,
	Fn<void(Api::SendOptions)> send);

Fn<void()> DefaultAutoDeleteCallback(
	not_null<Ui::RpWidget*> parent,
	Fn<void(Api::SendOptions)> send);

Fn<void()> NoAutoDeleteCallback();

} // namespace SendMenu
