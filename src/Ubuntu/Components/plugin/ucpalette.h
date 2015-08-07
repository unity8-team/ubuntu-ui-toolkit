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
 */

#ifndef UCPALETTE_H
#define UCPALETTE_H

#include <QtCore/QObject>
#include <QtGui/QColor>

class UCPaletteValues : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor background MEMBER m_background NOTIFY backgroundChanged FINAL)
    Q_PROPERTY(QColor backgroundText MEMBER m_backgroundText NOTIFY backgroundTextChanged FINAL)
    Q_PROPERTY(QColor base MEMBER m_base NOTIFY baseChanged FINAL)
    Q_PROPERTY(QColor baseText MEMBER m_baseText NOTIFY baseTextChanged FINAL)
    Q_PROPERTY(QColor foreground MEMBER m_foreground NOTIFY foregroundChanged FINAL)
    Q_PROPERTY(QColor foregroundText MEMBER m_foregroundText NOTIFY foregroundTextChanged FINAL)
    Q_PROPERTY(QColor overlay MEMBER m_overlay NOTIFY overlayChanged FINAL)
    Q_PROPERTY(QColor overlayText MEMBER m_overlayText NOTIFY overlayTextChanged FINAL)
    Q_PROPERTY(QColor field MEMBER m_field NOTIFY fieldChanged FINAL)
    Q_PROPERTY(QColor fieldText MEMBER m_fieldText NOTIFY fieldTextChanged FINAL)
    Q_PROPERTY(QColor selection MEMBER m_selection NOTIFY selectionChanged FINAL)
public:
    explicit UCPaletteValues(QObject *parent = 0);

Q_SIGNALS:
    void backgroundChanged();
    void backgroundTextChanged();
    void baseChanged();
    void baseTextChanged();
    void foregroundChanged();
    void foregroundTextChanged();
    void overlayChanged();
    void overlayTextChanged();
    void fieldChanged();
    void fieldTextChanged();
    void selectionChanged();

private: // property members
    QColor m_background;
    QColor m_backgroundText;
    QColor m_base;
    QColor m_baseText;
    QColor m_foreground;
    QColor m_foregroundText;
    QColor m_overlay;
    QColor m_overlayText;
    QColor m_field;
    QColor m_fieldText;
    QColor m_selection;

    friend class UCTheme;
    friend class UCListItem;
    friend class UCListItemDivider;
};

class UCPalette : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCPaletteValues *normal MEMBER m_normal NOTIFY normalChanged FINAL)
    Q_PROPERTY(UCPaletteValues *selected MEMBER m_selected NOTIFY selectedChanged FINAL)
public:
    explicit UCPalette(QObject *parent = 0);

Q_SIGNALS:
    void normalChanged();
    void selectedChanged();

private: // property members
    UCPaletteValues *m_normal;
    UCPaletteValues *m_selected;

    friend class UCTheme;
    friend class UCListItem;
    friend class UCListItemDivider;
};

#endif // UCPALETTE_H
