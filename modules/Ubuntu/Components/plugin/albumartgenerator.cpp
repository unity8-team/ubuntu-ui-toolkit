/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Jussi Pakkanen <jussi.pakkanen@canonical.com>
*/

#include "albumartgenerator.h"
#include <stdexcept>
#include <QDebug>
#include <QMimeDatabase>
#include <QUrl>

static const char *DEFAULT_ALBUM_ART = "/usr/share/unity/icons/album_missing.png";

AlbumArtGenerator::AlbumArtGenerator() : QQuickImageProvider(QQuickImageProvider::Image,
        QQmlImageProviderBase::ForceAsynchronousImageLoading) {

}

QImage AlbumArtGenerator::requestImage(const QString &id, QSize *realSize,
        const QSize &/*requestedSize*/) {
    /* Allow appending a query string (e.g. ?something=timestamp)
     * to the id and then ignore it.
     * This is workaround to force reloading a thumbnail when it has
     * the same file name on disk but we know the content has changed.
     * It is necessary because in such a situation the QML image cache
     * will kick in and this ImageProvider will never get called.
     * The only "solution" is setting Image.cache = false, but in some
     * cases we don't want to do that for performance reasons, so this
     * is the only way around the issue for now. */
    QUrl url(id);
    std::string artist(url.host().toUtf8().data());
    std::string album(url.path().toUtf8().data());
    try {
        ThumbnailSize desiredSize = TN_SIZE_ORIGINAL;
        ThumbnailPolicy policy = TN_REMOTE;
        std::string tgt_path = tn.get_album_art(artist, album, desiredSize, policy);
        if(!tgt_path.empty()) {
            QString tgt(tgt_path.c_str());
            QImage image;
            image.load(tgt);
            *realSize = image.size();
            return image;
        }
    } catch(std::runtime_error &e) {
        qDebug() << "AlbumArt generator failed: " << e.what();
    }
    QImage fallback;
    fallback.load(DEFAULT_ALBUM_ART);
    *realSize = fallback.size();
    return fallback;
}
/*
QImage AlbumArtGenerator::getFallbackImage(const QString &id, QSize *size,
        const QSize &requestedSize) {
    Q_UNUSED(requestedSize);
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForFile(id);
    QImage result;
    if(mime.name().contains("audio")) {
        result.load(DEFAULT_ALBUM_ART);
    } else if(mime.name().contains("video")) {
        result.load(DEFAULT_VIDEO_ART);
    }
    *size = result.size();
    return result;
}
*/

