/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/unique_qptr.h"
#include "ui/text/text_block.h"
#include "ui/widgets/menu/menu_item_base.h"

namespace Ui {

class PopupMenu;

struct WhoReadParticipant {
	QString name;
	QString date;
	bool dateReacted = false;
	QString customEntityData;
	QImage userpicSmall;
	QImage userpicLarge;
	std::pair<uint64, uint64> userpicKey = {};
	uint64 id = 0;

	static constexpr auto kMaxSmallUserpics = 3;
};

bool operator==(const WhoReadParticipant &a, const WhoReadParticipant &b);
bool operator!=(const WhoReadParticipant &a, const WhoReadParticipant &b);

enum class WhoReadType {
	Seen,
	Listened,
	Watched,
	Reacted,
};

struct WhoReadContent {
	std::vector<WhoReadParticipant> participants;
	WhoReadType type = WhoReadType::Seen;
	QString singleCustomEntityData;
	int fullReactionsCount = 0;
	int fullReadCount = 0;
	bool unknown = false;
};

[[nodiscard]] base::unique_qptr<Menu::ItemBase> WhoReactedContextAction(
	not_null<PopupMenu*> menu,
	rpl::producer<WhoReadContent> content,
	Text::CustomEmojiFactory factory,
	Fn<void(uint64)> participantChosen,
	Fn<void()> showAllChosen);

struct WhoReactedEntryData {
	QString text;
	QString date;
	bool dateReacted = false;
	bool preloader = false;
	QString customEntityData;
	QImage userpic;
	Fn<void()> callback;
};

class WhoReactedEntryAction final : public Menu::ItemBase {
public:
	using Data = WhoReactedEntryData;

	WhoReactedEntryAction(
		not_null<RpWidget*> parent,
		Text::CustomEmojiFactory factory,
		const style::Menu &st,
		Data &&data);

	void setData(Data &&data);

	not_null<QAction*> action() const override;
	bool isEnabled() const override;

private:
	int contentHeight() const override;

	void paint(Painter &&p);

	const not_null<QAction*> _dummyAction;
	const Text::CustomEmojiFactory _customEmojiFactory;
	const style::Menu &_st;
	const int _height = 0;

	Text::String _text;
	Text::String _date;
	std::unique_ptr<Ui::Text::CustomEmoji> _custom;
	QImage _userpic;
	int _textWidth = 0;
	int _customSize = 0;
	bool _dateReacted = false;
	bool _preloader = false;

};

class WhoReactedListMenu final {
public:
	WhoReactedListMenu(
		Text::CustomEmojiFactory factory,
		Fn<void(uint64)> participantChosen,
		Fn<void()> showAllChosen);

	void clear();
	void populate(
		not_null<PopupMenu*> menu,
		const WhoReadContent &content,
		Fn<void()> refillTopActions = nullptr,
		int addedToBottom = 0,
		Fn<void()> appendBottomActions = nullptr);

private:
	const Text::CustomEmojiFactory _customEmojiFactory;
	const Fn<void(uint64)> _participantChosen;
	const Fn<void()> _showAllChosen;

	std::vector<not_null<WhoReactedEntryAction*>> _actions;

};

} // namespace Ui
