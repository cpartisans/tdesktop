/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

namespace ChatHelpers {
class Show;
} // namespace ChatHelpers

namespace Data {
struct SponsoredMessageDetails;
struct SponsoredReportAction;
} // namespace Data

namespace Ui {
class RpWidget;
namespace Menu {
struct MenuCallback;
} // namespace Menu
} // namespace Ui

class HistoryItem;

namespace Menu {

enum class SponsoredPhrases {
	Channel,
	Bot,
	Search,
};

void FillSponsored(
	not_null<Ui::RpWidget*> parent,
	const Ui::Menu::MenuCallback &addAction,
	std::shared_ptr<ChatHelpers::Show> show,
	SponsoredPhrases phrases,
	const Data::SponsoredMessageDetails &details,
	Data::SponsoredReportAction report,
	bool mediaViewer,
	bool skipAbout);

void FillSponsored(
	not_null<Ui::RpWidget*> parent,
	const Ui::Menu::MenuCallback &addAction,
	std::shared_ptr<ChatHelpers::Show> show,
	const FullMsgId &fullId,
	bool mediaViewer,
	bool skipAbout = false);

void ShowSponsored(
	not_null<Ui::RpWidget*> parent,
	std::shared_ptr<ChatHelpers::Show> show,
	const FullMsgId &fullId);

void ShowSponsoredAbout(
	std::shared_ptr<ChatHelpers::Show> show,
	const FullMsgId &fullId);

} // namespace Menu
