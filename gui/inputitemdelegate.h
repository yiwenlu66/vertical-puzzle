#ifndef INPUTITEMDELEGATE_H
#define INPUTITEMDELEGATE_H

#include <QItemDelegate>
#include <QValidator>

class InputItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit InputItemDelegate(QObject *parent = __null);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class InputItemValidator : public QValidator
{
    Q_OBJECT
public:
    explicit InputItemValidator(QObject *parent = __null) : QValidator(parent) {}
    QValidator::State validate (QString &, int &) const override;
};

#endif // INPUTITEMDELEGATE_H
