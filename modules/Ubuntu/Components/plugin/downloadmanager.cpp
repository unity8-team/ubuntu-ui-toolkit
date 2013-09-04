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

#include "downloadmanager.h"

class DownloadManagerPrivate {
    Q_DECLARE_PUBLIC(DownloadManager)

public:
    DownloadManagerPrivate(DownloadManager* q) : q_ptr(q) {}
    ~DownloadManagerPrivate() {}

    DownloadManager* q_ptr;
};

DownloadManager::DownloadManager(QObject* parent)
    : QAbstractListModel(parent)
    , d_ptr(new DownloadManagerPrivate(this))
{
    // TODO
}

DownloadManager::~DownloadManager()
{
    // TODO
}

QHash<int, QByteArray> DownloadManager::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[DownloadId] = "downloadId";
        roles[Method] = "method";
        roles[Checksum] = "checksum";
        roles[Algorithm] = "algorithm";
        roles[AllowedOverData] = "allowedOverData";
        roles[AllowedOverWifi] = "allowedOverWifi";
        roles[AdditionalHeaders] = "additionalHeaders";
        roles[Uri] = "uri";
        roles[FileName] = "filename";
        roles[DownloadedFilePath] = "downloadedFilePath";
        roles[MimeType] = "mimeType";
        roles[Status] = "status";
        roles[Progress] = "progress";
    }
    return roles;
}

int DownloadManager::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 0; // TODO: implement me
}

QVariant DownloadManager::data(const QModelIndex& index, int role) const
{
    // TODO: implement me
    return QVariant();
}
