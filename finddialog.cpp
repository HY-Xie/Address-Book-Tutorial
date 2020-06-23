#include "finddialog.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QHBoxLayout>
#include <QMessageBox>
FindDialog::FindDialog(QWidget *parent) : QDialog (parent)
{
    label = new QLabel(tr("Enter the name of a contact: "), this);
    lineEdit = new QLineEdit(this);
    findButton = new QPushButton(tr("&Find"), this);
    findText = "";

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(lineEdit);
    layout->addWidget(findButton);
    setLayout(layout);

    setWindowTitle(tr("Find a contact"));
    connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
    connect(findButton, SIGNAL(clicked()), this, SLOT(accept()));

}

QString FindDialog::getFindText()
{
    return findText;
}

void FindDialog::findClicked()
{
    QString name = lineEdit->text();
    if(name.isEmpty())
    {
        QMessageBox::information(this, tr("Empty Field"),
                                 tr("Please enter a name."));
        return;
    }
    else{
        findText = name;
        lineEdit->clear();
        hide();
    }
}
