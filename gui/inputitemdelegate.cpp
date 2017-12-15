#include "inputitemdelegate.h"
#include <QLineEdit>

InputItemDelegate::InputItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *InputItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setMaxLength(1);
    lineEdit->setValidator(new InputItemValidator(this->parent()));
    return lineEdit;
}

QValidator::State InputItemValidator::validate (QString &input, int &pos) const
{
    if (input.isEmpty()) {
        return QValidator::Acceptable;
    }
    char c = input.at(0).toLatin1();
    if (('0' <= c && c <= '9') || ('a' <= c && c <= 'j') || ('A' <= c && c <= 'J')) {
        return QValidator::Acceptable;
    }
    if (c == '+' || c == '-' || c == '*' || c == '_') {
        return QValidator::Acceptable;
    }
    return QValidator::Invalid;
}
