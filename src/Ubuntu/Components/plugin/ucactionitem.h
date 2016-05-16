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
#ifndef UCACTIONITEM_H
#define UCACTIONITEM_H

#include "ucstyleditembase.h"

class UCAction;
class UCActionItemPrivate;
class UCActionMnemonic;
class UCActionItem : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(UCAction *action READ action WRITE setAction NOTIFY actionChanged FINAL)
    Q_PROPERTY(QString text READ text WRITE setText RESET resetText NOTIFY textChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource RESET resetIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName RESET resetIconName NOTIFY iconNameChanged)
    Q_PROPERTY(UCActionMnemonic* mnemonic READ mnemonic CONSTANT FINAL)

    // overrides
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled2 NOTIFY enabledChanged2)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible2 NOTIFY visibleChanged2 FINAL)
public:
    explicit UCActionItem(QQuickItem *parent = 0);

    UCAction *action() const;
    void setAction(UCAction *action);
    QString text();
    void setText(const QString &text);
    void resetText();
    QUrl iconSource();
    void setIconSource(const QUrl &iconSource);
    void resetIconSource();
    QString iconName();
    void setIconName(const QString &iconName);
    void resetIconName();

    void setVisible2(bool visible);
    void setEnabled2(bool enabled);

    UCActionMnemonic* mnemonic();

Q_SIGNALS:
    void actionChanged();
    void textChanged();
    void iconSourceChanged();
    void iconNameChanged();
    void triggered(const QVariant &value);

    void enabledChanged2();
    void visibleChanged2();

public Q_SLOTS:
    void trigger(const QVariant &value = QVariant());

protected:
    UCActionItem(UCActionItemPrivate &, QQuickItem *parent);

    bool event(QEvent *event);

    Q_DECLARE_PRIVATE(UCActionItem)
    Q_PRIVATE_SLOT(d_func(), void _q_visibleBinding())
    Q_PRIVATE_SLOT(d_func(), void _q_enabledBinding())
    Q_PRIVATE_SLOT(d_func(), void _q_invokeActionTrigger(const QVariant &value))
    Q_PRIVATE_SLOT(d_func(), void _q_textBinding())
    Q_PRIVATE_SLOT(d_func(), void _q_onKeyboardAttached())
    Q_PRIVATE_SLOT(d_func(), void _q_updateMnemonic())
};

class UCActionMnemonic : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(int modifier READ modifier WRITE setModifier NOTIFY modifierChanged)
    Q_PROPERTY(QKeySequence sequence READ sequence NOTIFY sequenceChanged)
public:
    UCActionMnemonic(QObject* parent = 0);

    bool visible() const;
    void setVisible(bool visible);

    int modifier() const;
    void setModifier(int modifier);

    const QKeySequence& sequence() const;
    void setSequence(const QKeySequence& sequence);

Q_SIGNALS:
    void visibleChanged();
    void modifierChanged();
    void sequenceChanged();

private:
    bool m_visible;
    int m_modifier;
    QKeySequence m_sequence;
};

#endif // UCACTIONITEM_H
