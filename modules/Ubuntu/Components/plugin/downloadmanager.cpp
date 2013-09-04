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

#include <QtCore/QList>

class DownloadManagerPrivate {
    Q_DECLARE_PUBLIC(DownloadManager)

public:
    DownloadManagerPrivate(DownloadManager* q) : q_ptr(q) {}
    ~DownloadManagerPrivate() {}

    DownloadManager* q_ptr;

    QList<DownloadRequest*> requests;
};

DownloadManager::DownloadManager(QObject* parent)
    : QAbstractListModel(parent)
    , d_ptr(new DownloadManagerPrivate(this))
{
}

DownloadManager::~DownloadManager()
{
    Q_D(DownloadManager);
    qDeleteAll(d->requests);
    d->requests.clear();
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
    Q_D(const DownloadManager);
    return d->requests.count();
}

QVariant DownloadManager::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Q_D(const DownloadManager);
    DownloadRequest* request = d->requests.at(index.row());
    switch (role) {
    case DownloadId:
        return request->downloadId();
    case Method:
        return request->method();
    case Checksum:
        return request->checksum();
    case Algorithm:
        return request->algorithm();
    case AllowedOverData:
        return request->allowedOverData();
    case AllowedOverWifi:
        return request->allowedOverWifi();
    case AdditionalHeaders:
        return request->additionalHeaders();
    case Uri:
        return request->uri();
    case FileName:
        return request->fileName();
    case DownloadedFilePath:
        return request->downloadedFilePath();
    case MimeType:
        return request->mimeType();
    case Status:
        return request->status();
    case Progress:
        return request->progress();
    default:
        return QVariant();
    }
}

DownloadRequest* DownloadManager::requestDownload(const QUrl& uri)
{
    Q_D(DownloadManager);
    DownloadRequest* request = new DownloadRequest;
    request->setUri(uri);
    beginInsertRows(QModelIndex(), 0, 0);
    d->requests.prepend(request);
    endInsertRows();
    return request;
}

DownloadRequest* DownloadManager::getDownloadRequest(const int id) const
{
    Q_D(const DownloadManager);
    Q_FOREACH(DownloadRequest* request, d->requests) {
        if (request->downloadId() == id) {
            return request;
        }
    }
    return 0;
}

void DownloadManager::start(const int id)
{
    // TODO
}

void DownloadManager::pause(const int id)
{
    // TODO
}

void DownloadManager::resume(const int id)
{
    // TODO
}

void DownloadManager::cancel(const int id)
{
    // TODO
}
