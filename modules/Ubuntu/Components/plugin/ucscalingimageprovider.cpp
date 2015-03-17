/*
 * Copyright 2012 Canonical Ltd.
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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "ucscalingimageprovider.h"
#include "ucunits.h"

#include <QtCore/QFile>
#include <QtGui/QImageReader>

/*!
    \internal

    The UCScalingImageProvider class loads and scales images.
    It responds to URLs of the form "image://scaling/scale/path" where:
    - 'scale' is the scaling factor applied to the image
    - 'path' is the full path of the image on the filesystem

    Example:
     * image://scaling/0.5/arrow.png
*/
UCScalingImageProvider::UCScalingImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
}
#include <QDebug>
QImage UCScalingImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int separatorPosition = id.indexOf("/");
    float scaleFactor = id.left(separatorPosition).toFloat();
    QString path = id.mid(separatorPosition+1);
    QFile file(path);
    QSize scaledRequestedSize = requestedSize * scaleFactor;

    qDebug() << "ImageProvider requested image" << id << requestedSize << scaleFactor << scaledRequestedSize;

    if (file.open(QIODevice::ReadOnly)) {
        QImage image;
        QImageReader imageReader(&file);
        QSize realSize = imageReader.size();
        QSize constrainedSize;

        qDebug() << "requestedSize" << requestedSize << "realSize" << realSize << "scaledRequestedSize" << scaledRequestedSize;
        if (scaledRequestedSize.isValid() && (scaledRequestedSize.width() < realSize.width() || scaledRequestedSize.height() < realSize.height())) {
            if (scaledRequestedSize.width() > 0 && scaledRequestedSize.height() == 0 && realSize.width() > 0) {
                constrainedSize.setWidth(scaledRequestedSize.width());
                constrainedSize.setHeight(realSize.height() * scaledRequestedSize.width() / realSize.width());
            } else if (scaledRequestedSize.height() > 0 && scaledRequestedSize.width() == 0 && realSize.height() > 0) {
                constrainedSize.setHeight(scaledRequestedSize.height());
                constrainedSize.setWidth(realSize.width() * scaledRequestedSize.height() / realSize.height());
            } else {
                constrainedSize = realSize.scaled(scaledRequestedSize, Qt::KeepAspectRatio);
            }
        }

        if (!constrainedSize.isEmpty()) {
            imageReader.setScaledSize(constrainedSize);
        } else if (scaledRequestedSize != realSize) {
            imageReader.setScaledSize(scaledRequestedSize);
        }

        imageReader.read(&image);
        image.setDevicePixelRatio(UCUnits::instance().devicePixelRatio());
        *size = image.size();
        qDebug() << "ImageProvider: returning" << *size;
        return image;
    } else {
        return QImage();
    }
}
