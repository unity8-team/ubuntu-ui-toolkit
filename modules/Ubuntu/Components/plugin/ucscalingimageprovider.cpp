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
#include <QtCore/QDebug>

/*!
    \internal

    The UCScalingImageProvider class loads and scales images.
    It responds to URLs of the form "image://scaling/scale/path" where:
    - 'scale' is the scaling factor applied to the image
    - 'path' is the full path of the image on the filesystem

    If the scaling factor is bigger than 1.0 then do not scale: UCScalingImageProvider
    never upscales but instead set the devicePixelRatio appropriately so that upscaling
    is done in the GPU.

    Example:
     * image://scaling/0.5/arrow.png
*/
UCScalingImageProvider::UCScalingImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{
}

QImage UCScalingImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int separatorPosition = id.indexOf("/");
    float scaleFactor = id.left(separatorPosition).toFloat();
    QString path = id.mid(separatorPosition+1);
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        QImage image;
        QImageReader imageReader(&file);
        QSize realSize = imageReader.size();
        QSize scaledSize = realSize;
        QSize constrainedSize;

        if (!qFuzzyCompare(scaleFactor, (float)1.0) && scaleFactor < 1.0) {
            scaledSize = realSize * scaleFactor;
        }
        if (requestedSize.isValid() && (requestedSize.width() < realSize.width() || requestedSize.height() < realSize.height())) {
            constrainedSize = scaledSize.scaled(requestedSize, Qt::KeepAspectRatio);
        }

        if (constrainedSize.isValid()) {
            imageReader.setScaledSize(constrainedSize);
        } else if (scaledSize != realSize) {
            imageReader.setScaledSize(scaledSize);
        }

        imageReader.read(&image);
        *size = scaledSize;
        float windowPixelRatio = UCUnits::instance().gridUnit() / DEFAULT_GRID_UNIT_PX;
        if (scaleFactor < 1.0) {
            image.setDevicePixelRatio(windowPixelRatio);
        } else {
            /* When image needs to be upscaled, do not upscale it here but set its
               devicePixelRatio appropriately so that upscaling is done when rendering. */
            image.setDevicePixelRatio(windowPixelRatio / scaleFactor);
        }
        return image;
    } else {
        return QImage();
    }
}
