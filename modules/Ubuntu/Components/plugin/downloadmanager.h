/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Olivier Tilloy <olivier.tilloy@canonical.com>
 */

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QtCore/QAbstractListModel>

#include "downloadrequest.h"

class QUrl;

class DownloadManagerPrivate;
class DownloadManager : public QAbstractListModel
{
    Q_OBJECT

    Q_ENUMS(Roles)

public:
    DownloadManager(QObject* parent=0);
    ~DownloadManager();

    enum Roles {
        DownloadId = Qt::UserRole + 1,
        Method,
        Checksum,
        Algorithm,
        AllowedOverData,
        AllowedOverWifi,
        AdditionalHeaders,
        Uri,
        FileName,
        DownloadedFilePath,
        MimeType,
        Status,
        Progress
    };

    // reimplemented from QAbstractListModel
    QHash<int, QByteArray> roleNames() const;
    int rowCount(const QModelIndex& parent=QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    DownloadRequest* requestDownload(const QUrl& uri);
    DownloadRequest* getDownloadRequest(const int id) const;
    void start(const int id);
    void pause(const int id);
    void resume(const int id);
    void cancel(const int id);

Q_SIGNALS:
    void finished(const int id, DownloadRequest::Status status);

private:
    Q_DISABLE_COPY(DownloadManager)
    Q_DECLARE_PRIVATE(DownloadManager)
    QScopedPointer<DownloadManagerPrivate> d_ptr;
};

#endif // DOWNLOADMANAGER_H
