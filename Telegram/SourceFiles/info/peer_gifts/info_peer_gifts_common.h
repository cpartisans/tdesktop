/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "data/data_star_gift.h"
#include "ui/abstract_button.h"
#include "ui/effects/premium_stars_colored.h"
#include "ui/text/text.h"

class StickerPremiumMark;

namespace Data {
struct UniqueGift;
} // namespace Data

namespace HistoryView {
class StickerPlayer;
} // namespace HistoryView

namespace Main {
class Session;
} // namespace Main

namespace Ui {
class DynamicImage;
} // namespace Ui

namespace Ui::Text {
class CustomEmoji;
} // namespace Ui::Text

namespace Window {
class SessionController;
} // namespace Window

namespace Info::PeerGifts {

struct GiftTypePremium {
	int64 cost = 0;
	QString currency;
	int months = 0;
	int discountPercent = 0;

	[[nodiscard]] friend inline bool operator==(
		const GiftTypePremium &,
		const GiftTypePremium &) = default;
};

struct GiftTypeStars {
	Data::StarGift info;
	PeerData *from = nullptr;
	bool userpic = false;
	bool hidden = false;
	bool mine = false;

	[[nodiscard]] friend inline bool operator==(
		const GiftTypeStars&,
		const GiftTypeStars&) = default;
};

struct GiftDescriptor : std::variant<GiftTypePremium, GiftTypeStars> {
	using variant::variant;

	[[nodiscard]] friend inline bool operator==(
		const GiftDescriptor&,
		const GiftDescriptor&) = default;
};

class GiftButtonDelegate {
public:
	[[nodiscard]] virtual TextWithEntities star() = 0;
	[[nodiscard]] virtual std::any textContext() = 0;
	[[nodiscard]] virtual QSize buttonSize() = 0;
	[[nodiscard]] virtual QMargins buttonExtend() = 0;
	[[nodiscard]] virtual auto buttonPatternEmoji(
		not_null<Data::UniqueGift*> unique,
		Fn<void()> repaint)
	-> std::unique_ptr<Ui::Text::CustomEmoji> = 0;
	[[nodiscard]] virtual QImage background() = 0;
	[[nodiscard]] virtual rpl::producer<not_null<DocumentData*>> sticker(
		const GiftDescriptor &descriptor) = 0;
	[[nodiscard]] virtual not_null<StickerPremiumMark*> hiddenMark() = 0;
};

class GiftButton final : public Ui::AbstractButton {
public:
	GiftButton(QWidget *parent, not_null<GiftButtonDelegate*> delegate);
	~GiftButton();

	void setDescriptor(const GiftDescriptor &descriptor);
	void setGeometry(QRect inner, QMargins extend);

private:
	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

	void cacheUniqueBackground(
		not_null<Data::UniqueGift*> unique,
		int width,
		int height);

	void setDocument(not_null<DocumentData*> document);
	[[nodiscard]] bool documentResolved() const;

	void unsubscribe();

	const not_null<GiftButtonDelegate*> _delegate;
	QImage _hiddenBgCache;
	GiftDescriptor _descriptor;
	Ui::Text::String _text;
	Ui::Text::String _price;
	std::shared_ptr<Ui::DynamicImage> _userpic;
	QImage _uniqueBackgroundCache;
	std::unique_ptr<Ui::Text::CustomEmoji> _uniquePatternEmoji;
	base::flat_map<float64, QImage> _uniquePatternCache;
	Ui::Premium::ColoredMiniStars _stars;
	bool _subscribed = false;
	bool _patterned = false;

	QRect _button;
	QMargins _extend;

	std::unique_ptr<HistoryView::StickerPlayer> _player;
	rpl::lifetime _mediaLifetime;

};

class Delegate final : public GiftButtonDelegate {
public:
	explicit Delegate(not_null<Window::SessionController*> window);
	Delegate(Delegate &&other);
	~Delegate();

	TextWithEntities star() override;
	std::any textContext() override;
	QSize buttonSize() override;
	QMargins buttonExtend() override;
	auto buttonPatternEmoji(
		not_null<Data::UniqueGift*> unique,
		Fn<void()> repaint)
	-> std::unique_ptr<Ui::Text::CustomEmoji> override;
	QImage background() override;
	rpl::producer<not_null<DocumentData*>> sticker(
		const GiftDescriptor &descriptor) override;
	not_null<StickerPremiumMark*> hiddenMark() override;

private:
	const not_null<Window::SessionController*> _window;
	std::unique_ptr<StickerPremiumMark> _hiddenMark;
	QSize _single;
	QImage _bg;

};

[[nodiscard]] DocumentData *LookupGiftSticker(
	not_null<Main::Session*> session,
	const GiftDescriptor &descriptor);

[[nodiscard]] rpl::producer<not_null<DocumentData*>> GiftStickerValue(
	not_null<Main::Session*> session,
	const GiftDescriptor &descriptor);

} // namespace Info::PeerGifts