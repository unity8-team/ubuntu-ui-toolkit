/*
 * Copyright 2015 Canonical Ltd.
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
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCPLATFORMTHEME_H
#define UCPLATFORMTHEME_H

#include <qpa/qplatformtheme.h>
#include <QtPlatformSupport/private/qgenericunixthemes_p.h>

class UCPlatformTheme : public QGenericUnixTheme
{
public:
    UCPlatformTheme();
    virtual ~UCPlatformTheme();

    QVariant themeHint(ThemeHint hint) const override;

    QPlatformMenuItem* createPlatformMenuItem() const override;
    QPlatformMenu* createPlatformMenu() const override;
//    QPlatformMenuBar* createPlatformMenuBar() const  override;
//    void showPlatformMenuBar() override {}

};

#endif // UCPLATFORMTHEME_H
