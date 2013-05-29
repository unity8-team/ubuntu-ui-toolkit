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
 * Author: Renato Araujo Oliveira Filho <renato.filho@canonical.com>
 */

#include "giconprovider.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QIcon>
#include <QPainter>

extern "C" {
#include <gio/gio.h>
#include <gtk/gtk.h>
}

/*
  GIconProvider is a implementation of QQuickImageProvider that handles GIcon sources.
  GIcons sources can be loaded in QML using the uri: "image://gicon/<icon_name>"

  Example:
  \qml
    Image {
      source: "image://gicon/my_icon_name"
    }
  \endqml
*/

static QIcon getThemedIcon(GThemedIcon *icon)
{
    const gchar* const* iconNames = g_themed_icon_get_names(icon);
    guint index = 0;
    while(iconNames[index] != NULL) {
        if (QIcon::hasThemeIcon(iconNames[index])) {
            return QIcon::fromTheme(iconNames[index]);
        }
        index++;
    }
    return QIcon();
}

GIconProvider::GIconProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{
    g_type_init();
    gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(),
                                      "/usr/share/notify-osd/icons");    
}

QImage GIconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QPixmap result;
    QByteArray utf8Name = QUrl::fromPercentEncoding(id.toUtf8()).toUtf8();
    GError *error = NULL;
    GIcon *icon = g_icon_new_for_string(utf8Name.data(), &error);

    if (error) {
        qWarning() << "Fail to load icon: " << id << error->message;
        g_error_free(error);
        return QImage();
    }

    if (G_IS_THEMED_ICON(icon)) {
        QIcon themedIcon = getThemedIcon(reinterpret_cast<GThemedIcon*>(icon));
        if (!themedIcon.isNull()) {
            if (requestedSize.isValid()) {
                result = themedIcon.pixmap(requestedSize);
            } else {
                result = themedIcon.pixmap(themedIcon.availableSizes().last());
            }
        } else {
            GtkIconInfo* info = NULL;
            info = gtk_icon_theme_lookup_by_gicon(gtk_icon_theme_get_default(),
                                                  icon,
                                                  requestedSize.width(),
                                                  GTK_ICON_LOOKUP_FORCE_SVG);
            if (info) {
                // obtain filename and clean-up GObject/gtk+ pieces
                const gchar* iconName = NULL;
                iconName = gtk_icon_info_get_filename(info); // not ref'ed
                QImage image = QImage(iconName);
                gtk_icon_info_free(info);
                g_object_unref(icon);

                // create paint-device
                QPixmap target = QPixmap(requestedSize);
                target.fill(Qt::transparent);
                QPainter painter(&target);

                // scale original image preserving aspect-ratio
                int width = image.width();
                int height = image.height();
                QImage scaledImage;
                if (width > height) {
                    scaledImage = image.scaledToWidth(requestedSize.width(),
                                                      Qt::SmoothTransformation);
                } else {
                    scaledImage = image.scaledToHeight(requestedSize.height(),
                                                       Qt::SmoothTransformation);
                }

                // determine offset and render scaled image
                width = scaledImage.width();
                height = scaledImage.height();
                QPoint offset(abs(width - requestedSize.width()) / 2,
                              abs(height - requestedSize.height()) / 2);
                painter.drawImage(offset, scaledImage);

                return target.toImage();
            } else {
                qDebug() << "Fail to load themed icon for:" << id;                
            }
        }
    } else if (G_IS_FILE_ICON(icon)) {
        gchar *iconName = g_icon_to_string(icon);
        if (QFile::exists(iconName)) {
            result = QPixmap(iconName);
            if (requestedSize.isValid()) {
                result = result.scaled(requestedSize);
            }
        } else {
            qWarning() << "File does not exists:" << iconName;
        }

        g_free(iconName);
    } else {
        qWarning() << "Invalid icon format:" << id;
    }

    g_object_unref(icon);
    *size = result.size();
    return result.toImage();
}

