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

#include "downloadrequest.h"

class DownloadRequestPrivate {
    Q_DECLARE_PUBLIC(DownloadRequest)

public:
    DownloadRequestPrivate(DownloadRequest* q)
        : q_ptr(q)
        , downloadId(0)
        , method(DownloadRequest::GET)
        , algorithm(DownloadRequest::Md5)
        , allowedOverData(true)
        , allowedOverWifi(true)
        , status(DownloadRequest::Pending)
        , progress(0.0)
    {
    }

    ~DownloadRequestPrivate() {}

    DownloadRequest* q_ptr;

    int downloadId;
    DownloadRequest::Method method;
    QString checksum;
    DownloadRequest::Algorithm algorithm;
    bool allowedOverData;
    bool allowedOverWifi;
    QVariantMap additionalHeaders;
    QUrl uri;
    QString fileName;
    QUrl downloadedFilePath; // TODO: query from the service
    QString mimeType; // TODO: query from the service
    DownloadRequest::Status status; // TODO: query from the service and keep the value updated
    float progress; // TODO: keep the value updated
};

DownloadRequest::DownloadRequest(QObject* parent)
    : QObject(parent)
    , d_ptr(new DownloadRequestPrivate(this))
{
}

DownloadRequest::~DownloadRequest()
{
}

int DownloadRequest::downloadId() const
{
    Q_D(const DownloadRequest);
    return d->downloadId;
}

DownloadRequest::Method DownloadRequest::method() const
{
    Q_D(const DownloadRequest);
    return d->method;
}

void DownloadRequest::setMethod(Method method)
{
    Q_D(DownloadRequest);
    if (d->method != method) {
        d->method = method;
        Q_EMIT methodChanged();
    }
}

const QString& DownloadRequest::checksum() const
{
    Q_D(const DownloadRequest);
    return d->checksum;
}

void DownloadRequest::setChecksum(const QString& checksum)
{
    Q_D(DownloadRequest);
    if (d->checksum != checksum) {
        d->checksum = checksum;
        Q_EMIT checksumChanged();
    }
}

DownloadRequest::Algorithm DownloadRequest::algorithm() const
{
    Q_D(const DownloadRequest);
    return d->algorithm;
}

void DownloadRequest::setAlgorithm(Algorithm algorithm)
{
    Q_D(DownloadRequest);
    if (d->algorithm != algorithm) {
        d->algorithm = algorithm;
        Q_EMIT algorithmChanged();
    }
}

bool DownloadRequest::allowedOverData() const
{
    Q_D(const DownloadRequest);
    return d->allowedOverData;
}

void DownloadRequest::setAllowedOverData(bool allowedOverData)
{
    Q_D(DownloadRequest);
    if (d->allowedOverData != allowedOverData) {
        d->allowedOverData = allowedOverData;
        Q_EMIT allowedOverDataChanged();
    }
}

bool DownloadRequest::allowedOverWifi() const
{
    Q_D(const DownloadRequest);
    return d->allowedOverWifi;
}

void DownloadRequest::setAllowedOverWifi(bool allowedOverWifi)
{
    Q_D(DownloadRequest);
    if (d->allowedOverWifi != allowedOverWifi) {
        d->allowedOverWifi = allowedOverWifi;
        Q_EMIT allowedOverWifiChanged();
    }
}

const QVariantMap& DownloadRequest::additionalHeaders() const
{
    Q_D(const DownloadRequest);
    return d->additionalHeaders;
}

void DownloadRequest::setAdditionalHeaders(const QVariantMap& additionalHeaders)
{
    Q_D(DownloadRequest);
    if (d->additionalHeaders != additionalHeaders) {
        d->additionalHeaders = additionalHeaders;
        Q_EMIT additionalHeadersChanged();
    }
}

const QUrl& DownloadRequest::uri() const
{
    Q_D(const DownloadRequest);
    return d->uri;
}

void DownloadRequest::setUri(const QUrl& uri)
{
    Q_D(DownloadRequest);
    if (d->uri != uri) {
        d->uri = uri;
        Q_EMIT uriChanged();
    }
}

const QString& DownloadRequest::fileName() const
{
    Q_D(const DownloadRequest);
    return d->fileName;
}

void DownloadRequest::setFileName(const QString& fileName)
{
    Q_D(DownloadRequest);
    if (d->fileName != fileName) {
        d->fileName = fileName;
        Q_EMIT fileNameChanged();
    }
}

const QUrl& DownloadRequest::downloadedFilePath() const
{
    Q_D(const DownloadRequest);
    return d->downloadedFilePath;
}

const QString& DownloadRequest::mimeType() const
{
    Q_D(const DownloadRequest);
    return d->mimeType;
}

DownloadRequest::Status DownloadRequest::status() const
{
    Q_D(const DownloadRequest);
    return d->status;
}

float DownloadRequest::progress() const
{
    Q_D(const DownloadRequest);
    return d->progress;
}
