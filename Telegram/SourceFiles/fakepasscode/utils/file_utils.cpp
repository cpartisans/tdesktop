#include "file_utils.h"
#include "core/file_utilities.h"
#include "fakepasscode/log/fake_log.h"
#include <random>
#include "core/application.h"
#include "main/main_domain.h"
#include "storage/storage_domain.h"
#include "main/main_account.h"
#include "main/main_session.h"
#include "storage/storage_account.h"
#include "data/data_session.h"

constexpr qint64 kBufferSize = 1024 * 1024; // 1 Mb

namespace FakePasscode::FileUtils {
    FileResult DeleteFile(QString path) {
        QFile file(path);
        ushort result = FileResult::Success;
        if (Core::App().domain().local().IsErasingEnabled()) {
            result |= (file.open(QIODevice::OpenModeFlag::ReadWrite) ? FileResult::Success : FileResult::NotOpened);
            qint64 fileSize = file.size();

            std::random_device rd;
            std::mt19937 gen(rd());
            std::vector<uint8_t> buffer(kBufferSize);

            for (size_t step = 1; step <= 2; step++) {
                if (step == 1) {
                    buffer.assign(kBufferSize, 0);
                } else {
                    buffer.assign(kBufferSize, 255);
                }
                file.seek(0);
                for (qint64 i = 0; i < fileSize; i += kBufferSize) {
                    file.write(reinterpret_cast<char *>(buffer.data()), std::min(kBufferSize, fileSize - i));
                }
            }

            std::uniform_int_distribution<int> byteRange(0, 255);
            file.seek(0);
            for (qint64 j = 0; j < fileSize; j += kBufferSize) {
                for (qint64 i = 0; i < kBufferSize; i++) {
                    buffer[i] = byteRange(gen);
                }

                file.write(reinterpret_cast<char *>(buffer.data()), std::min(kBufferSize, fileSize - j));
            }

            std::uniform_int_distribution<int> hourRange(0, 23), minsecRange(0, 59),
                    yearRange(0, 10), monthRange(1, 12), dayRange(1, 28);
            for (size_t i = 0; i < 4; i++) {
                if (i != 2) {
                    if (!file.setFileTime(QDateTime(QDate(yearRange(gen) + 2010, monthRange(gen), dayRange(gen)),
                                                    QTime(hourRange(gen), minsecRange(gen), minsecRange(gen))),
                                          (QFileDevice::FileTime) i)) {
                        result |= FileResult::MetadataNotChanged;
                    }
                }
            }
            file.close();

            QFileInfo fileInfo(path);
            QDir newDir = fileInfo.dir();
            QString newName = GetRandomName(newDir);

            result |= (file.rename(newName) ?
                       FileResult::Success : FileResult::NotRenamed);

        }
        if (!file.remove()) {
            result |= FileResult::NotDeleted;
        }

        FAKE_LOG(qsl("%2 file cleared %1").arg(path, QString::number(result)));
        return (FileResult) result;
    }

    bool DeleteFolderRecursively(QString path, bool deleteRoot) {
        QDir dir(path);
        bool isOk = true;
        for (auto &entry: dir.entryList(QDir::Dirs | QDir::Filter::NoDotAndDotDot | QDir::Filter::Hidden)) {
            if (!DeleteFolderRecursively(dir.path() + "/" + entry, true)) {
                isOk = false;
            }
        }
        for (auto &entry: dir.entryList(QDir::Filter::Files | QDir::Filter::Hidden)) {
            if (DeleteFile(dir.path() + "/" + entry) != FileResult::Success) {
                isOk = false;
            }
        }
        if (deleteRoot) {
            if (Core::App().domain().local().IsErasingEnabled()) {
                QDir newDir = GetRandomDir();
                QString newName = GetRandomName(newDir);
                if (!(dir.rename(dir.absolutePath(), newDir.absolutePath() + "/" + newName) && newDir.rmdir(newName))) {
                    isOk = false;
                }
            } else {
                if (!dir.rmdir(path)) {
                    isOk = false;
                }
            }
        }
        return isOk;
    }

    QDir GetRandomDir() {
        QDir dir(cWorkingDir());
        const int kDepth = 5;
        QStringList entries;
        std::random_device rd;
        std::mt19937 gen(rd());
        for (int i = 0; i < kDepth; i++) {
            entries = dir.entryList(QDir::Dirs);
            int ind = std::uniform_int_distribution<int>(0, entries.size() - 1)(gen);
            if (QFileInfo(dir.absolutePath() + "/" + entries[ind]).isWritable())
                dir.cd(entries[ind]);
        }
        return dir;
    }

    QString GetRandomName(QDir dir) {
        QString name;
        std::random_device rd;
        std::mt19937 gen(rd());
        do {
            name = QString::number(gen());
        } while (QFileInfo(dir.absolutePath() + "/" + name).exists());
        return name;
    }

    void ClearCaches() {
        const auto& domain = Core::App().domain();
        for (const auto &[index, account] : domain.accounts()) {
            if (account->sessionExists()) {
                FAKE_LOG(qsl("Clear cache for account %1").arg(index));
                if (!domain.local().IsErasingEnabled()) {
                    account->session().data().cache().clear();
                    account->session().data().cacheBigFile().clear();
                } else {
                    account->session().data().cache().close(
                    crl::guard(
                        account.get(),
                        [account = account.get()] {
                           account->session().data().cacheBigFile().close(crl::guard(account, [=] {
                               for (const auto& cache_path : {
                                       account->session().local().cachePath(),
                                       account->session().local().cacheBigFilePath()
                               }) {
                                   const auto cache_entries = QDir(cache_path).entryList(
                                           QDir::Dirs | QDir::NoDotAndDotDot);
                                   for (const auto &entry: cache_entries) {
                                       DeleteFolderRecursively(entry, true);
                                   }
                               }
                               account->session().data().resetCaches();
                           }));
                    }));
                }
            }
        }
    }
}  // namespace FakePasscode::FileUtils
