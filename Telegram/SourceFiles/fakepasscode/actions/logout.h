#ifndef TELEGRAM_LOGOUT_H
#define TELEGRAM_LOGOUT_H

#include "fakepasscode/action.h"

//#include <unordered_map>

namespace FakePasscode {
    class LogoutAction : public Action {
    public:
        LogoutAction() = default;
        explicit LogoutAction(QByteArray inner_data);
        LogoutAction(base::flat_map<qint32, bool> logout_accounts);

        void Execute() override;

        QByteArray Serialize() const override;

        ActionType GetType() const override;

        void SetLogout(qint32 index, bool logout);

        const base::flat_map<qint32, bool>& GetLogout() const;

        bool IsLogout(qint32 index) const;

        void SubscribeOnLoggingOut();

    private:
        base::flat_map<qint32, bool> index_to_logout_;
    };
}
#endif //TELEGRAM_LOGOUT_H