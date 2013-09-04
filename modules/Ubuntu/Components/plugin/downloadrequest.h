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

#ifndef DOWNLOADREQUEST_H
#define DOWNLOADREQUEST_H

#include <QtCore/QCryptographicHash>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QVariantMap>

class DownloadManager;

class DownloadRequestPrivate;
class DownloadRequest : public QObject
{
    Q_OBJECT

    Q_ENUMS(Method Algorithm DownloadStatus)

    Q_PROPERTY(int downloadId READ downloadId)
    Q_PROPERTY(Method method READ method WRITE setMethod NOTIFY methodChanged)
    Q_PROPERTY(QString checksum READ checksum WRITE setChecksum NOTIFY checksumChanged)
    Q_PROPERTY(Algorithm algorithm READ algorithm WRITE setAlgorithm NOTIFY algorithmChanged)
    Q_PROPERTY(bool allowedOverData READ allowedOverData WRITE setAllowedOverData NOTIFY allowedOverDataChanged)
    Q_PROPERTY(bool allowedOverWifi READ allowedOverWifi WRITE setAllowedOverWifi NOTIFY allowedOverWifiChanged)
    Q_PROPERTY(QVariantMap additionalHeaders READ additionalHeaders WRITE setAdditionalHeaders NOTIFY additionalHeadersChanged)
    Q_PROPERTY(QUrl uri READ uri WRITE setUri NOTIFY uriChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QUrl downloadedFilePath READ downloadedFilePath)
    Q_PROPERTY(QString mimeType READ mimeType)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(float progress READ progress NOTIFY progressChanged)

public:
    friend class DownloadManager;

    ~DownloadRequest();

    enum Method {
        GET,
        HEAD,
        POST
    };

    enum Algorithm {
        Md4 = QCryptographicHash::Md4,
        Md5 = QCryptographicHash::Md5,
        Sha1 = QCryptographicHash::Sha1,
        Sha224 = QCryptographicHash::Sha224,
        Sha256 = QCryptographicHash::Sha256,
        Sha384 = QCryptographicHash::Sha384,
        Sha512 = QCryptographicHash::Sha512
    };

    enum Status {
        Pending,
        Active,
        Paused,
        Canceled,
        Complete,
        Failed
    };

    int downloadId() const;

    Method method() const;
    void setMethod(Method method);

    const QString& checksum() const;
    void setChecksum(const QString& checksum);

    Algorithm algorithm() const;
    void setAlgorithm(Algorithm algorithm);

    bool allowedOverData() const;
    void setAllowedOverData(bool allowedOverData);

    bool allowedOverWifi() const;
    void setAllowedOverWifi(bool allowedOverWifi);

    const QVariantMap& additionalHeaders() const;
    void setAdditionalHeaders(const QVariantMap& additionalHeaders);

    const QUrl& uri() const;
    void setUri(const QUrl& uri);

    const QString& fileName() const;
    void setFileName(const QString& fileName);

    const QUrl& downloadedFilePath() const;

    const QString& mimeType() const;

    Status status() const;

    float progress() const;

Q_SIGNALS:
    void methodChanged() const;
    void checksumChanged() const;
    void algorithmChanged() const;
    void allowedOverDataChanged() const;
    void allowedOverWifiChanged() const;
    void additionalHeadersChanged() const;
    void uriChanged() const;
    void fileNameChanged() const;
    void statusChanged() const;
    void progressChanged() const;

private:
    explicit DownloadRequest(QObject* parent=0);
    Q_DISABLE_COPY(DownloadRequest)
    Q_DECLARE_PRIVATE(DownloadRequest)
    QScopedPointer<DownloadRequestPrivate> d_ptr;
};

#endif // DOWNLOADREQUEST_H
