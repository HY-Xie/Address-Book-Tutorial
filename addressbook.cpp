#include "addressbook.h"
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QRegularExpression>
#include <QTextStream>
AddressBook::AddressBook(QWidget *parent)
    : QWidget(parent)
{
    QLabel *nameLabel = new QLabel(tr("Name"), this);
    nameLine = new QLineEdit;
    QLabel *addressLabel = new QLabel(tr("Address"), this);
    addressText = new QTextEdit;

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameLine, 0, 1);
    mainLayout->addWidget(addressLabel, 1, 0, Qt::AlignTop);
    mainLayout->addWidget(addressText, 1, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Simple Address Book"));

    nameLine->setReadOnly(true);
    addressText->setReadOnly(true);

    addButton = new QPushButton(tr("&Add"), this);
    addButton->show();
    submitButton = new QPushButton(tr("&Submit"), this);
    submitButton->hide();
    cancelButton = new QPushButton(tr("&Cancel"), this);
    cancelButton->hide();

    connect(addButton, SIGNAL(clicked()), this, SLOT(addContact()));
    connect(submitButton, SIGNAL(clicked()), this, SLOT(submitContact()));
    connect(cancelButton, SIGNAL(clicked()),this, SLOT(cancel()));

    QVBoxLayout *buttonLayout1 = new QVBoxLayout(this);
    buttonLayout1->addWidget(addButton, Qt::AlignTop);
    buttonLayout1->addWidget(submitButton);
    buttonLayout1->addWidget(cancelButton);
    buttonLayout1->addStretch();

    mainLayout->addLayout(buttonLayout1, 1,2);

    previousButton = new QPushButton(tr("&Previous"), this);
    connect(previousButton, SIGNAL(clicked()), this, SLOT(previous()));
    nextButton = new QPushButton(tr("&Next"), this);
    connect(nextButton, SIGNAL(clicked()), this, SLOT(next()));
    QHBoxLayout *buttonLayout2 = new QHBoxLayout(this);
    buttonLayout2->addWidget(previousButton);
    buttonLayout2->addWidget(nextButton);
    mainLayout->addLayout(buttonLayout2, 2, 1);

    editButton = new QPushButton(tr("&Edit"), this);
    editButton->setEnabled(false);
    removeButton = new QPushButton(tr("&Remove"), this);
    removeButton->setEnabled(false);
    connect(editButton, SIGNAL(clicked()), this, SLOT(editContact()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeContact()));

    buttonLayout1->addWidget(editButton);
    buttonLayout1->addWidget(removeButton);

    findButton = new QPushButton(tr("&Find"), this);
    findButton->setEnabled(false);
    connect(findButton, SIGNAL(clicked()), this, SLOT(findContact()));
    buttonLayout1->addWidget(findButton);

    loadButton = new QPushButton(tr("&Load..."), this);
    loadButton->setToolTip(tr("Load contacts from a file"));
    saveButton = new QPushButton(tr("&Save..."), this);
    saveButton->setToolTip(tr("Save contacts to a file"));
    buttonLayout1->addWidget(loadButton);
    buttonLayout1->addWidget(saveButton);
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveToFile()));


    exportButton = new QPushButton(tr("Export"), this);
    exportButton->setEnabled(false);
    buttonLayout1->addWidget(exportButton);
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportToCard()));

    dialog = nullptr;
    dialog = new FindDialog();
    dialog->setWindowModality(Qt::WindowModal);
    //dialog->setWindowFlag(Qt::FramelessWindowHint);

}

AddressBook::~AddressBook()
{
    if(dialog != nullptr){
        delete dialog;
        dialog = nullptr;
    }
}

void AddressBook::addContact()
{
    oldName = nameLine->text();
    oldAddress = addressText->toPlainText();

    nameLine->clear();
    addressText->clear();

    nameLine->setReadOnly(false);
    addressText->setReadOnly(false);
    nameLine->setFocus(Qt::OtherFocusReason);

    //    addButton->setEnabled(false);
    addButton->setDisabled(true);
    submitButton->show();
    cancelButton->show();
    previousButton->setEnabled(false);
    nextButton->setEnabled(false);

    updateInterface(AddingMode);
}

void AddressBook::submitContact()
{
    QString name = nameLine->text();
    QString address = addressText->toPlainText();

    if(name.isEmpty() || address.isEmpty())
    {
        QMessageBox::information(this, tr("Empty field"), tr("Please enter a name and address"));
        return;
    }
    if(currentMode == AddingMode)
    {
        if(!contacts.contains(name))
        {
            contacts.insert(name, address);     // 向QMap添加内容
            QMessageBox::information(this, tr("Add Successful"),
                                     tr("\"%1\" has been added to your address book.").arg(name));
        }
        else
        {
            QMessageBox::information(this, tr("Add Unsuccessful"),
                                     tr("Sorry, \"%1\" is already in your address book.").arg(name));
            return;
        }
    }
    else if (currentMode == EditingMode)
    {
        if(oldName != name)
        {
            if(!contacts.contains(name))
            {
                QMessageBox::information(this, tr("Edit Successful"),
                                         tr("\"%1\" has been edited in your address book.").arg(name));
                contacts.remove(oldName);
                contacts.insert(name, address);
            }
            else{
                QMessageBox::information(this, tr("Edit Unsuccessful"),
                                         tr("Sorry, \"%1\" is alread in your address book").arg(name));
            }
        }
        else if(oldAddress != address)
        {
            QMessageBox::information(this, tr("Edit Successful"),
                                    tr("\"%1\" has been edited in your address book.").arg(name));
            contacts[name] = address;

        }
    }

    updateInterface(NavigationMode);
}

void AddressBook::cancel()
{

}

void AddressBook::next()
{
    QString name = nameLine->text();
    QMap<QString, QString>::const_iterator it = contacts.find(name);
    if( it != contacts.end())
        ++it;
    if( it == contacts.end())
        it = contacts.begin();
    nameLine->setText(it.key());
    addressText->setText(it.value());
}

void AddressBook::previous()
{
    QString name = nameLine->text();
    QMap<QString, QString>::const_iterator it = contacts.find(name);
    if(it == contacts.begin())
        it = contacts.end();
    --it;
    nameLine->setText(it.key());
    addressText->setText(it.value());

}

void AddressBook::editContact()
{
    oldName = nameLine->text();
    oldAddress = addressText->toPlainText();

    updateInterface(EditingMode);
}

void AddressBook::removeContact()
{
    QString name = nameLine->text();
    QString address = addressText->toPlainText();

    if(contacts.contains(name))
    {
        int button = QMessageBox::question(this,
                                           tr("Confirm Remove"),
                                           tr("Are you sure you wang to remove \"%1\"?").arg(name),
                                           QMessageBox::Yes, QMessageBox::No);
        if(button == QMessageBox::Yes)
        {
            previous();
            contacts.remove(name);
            QMessageBox::information(this, tr("Remove Successful"),
                                     tr("\"%1\" has been removed from your address book.").arg(name));
        }
    }
    updateInterface(NavigationMode);
}

void AddressBook::updateInterface(Mode mode)
{
    currentMode = mode;
    switch (currentMode) {
    case AddingMode:

    case EditingMode:
        nameLine->setReadOnly(false);
        nameLine->setFocus(Qt::OtherFocusReason);
        addressText->setReadOnly(false);

        addButton->setEnabled(false);
        editButton->setEnabled(false);
        removeButton->setEnabled(false);

        nextButton->setEnabled(false);
        previousButton->setEnabled(false);

        submitButton->show();
        cancelButton->show();

        findButton->setEnabled(false);
        loadButton->setEnabled(false);
        saveButton->setEnabled(false);

        exportButton->setEnabled(false);
        break;
    case NavigationMode:
        if(contacts.isEmpty()){
            nameLine->clear();
            addressText->clear();
        }
        nameLine->setReadOnly(true);
        addressText->setReadOnly(true);
        addButton->setEnabled(true);

        int num = contacts.size();
        editButton->setEnabled(num >= 1);
        removeButton->setEnabled(num >= 1);
        nextButton->setEnabled(num > 1);
        previousButton->setEnabled(num > 1);

        submitButton->hide();
        cancelButton->hide();

        findButton->setEnabled(num > 0);

        saveButton->setEnabled(num > 0);
        loadButton->setEnabled(true);

        exportButton->setEnabled(true);
        break;
    }
}

void AddressBook::findContact()
{
    //dialog->setWindowModality(Qt::WindowModal);

    //dialog->setAttribute(Qt::WA_DeleteOnClose);

    dialog->show();

    if(dialog->exec() == QDialog::Accepted)
    {
        const QString contactName = dialog->getFindText();
        if(contacts.contains(contactName))
        {
            nameLine->setText(contactName);
            addressText->setText(contacts[contactName]);
        }
        else
        {
            QMessageBox::information(this, tr("Contact Not Found"),
                                     tr("Sorry, \" %1 \" is not in your address book.").arg(contactName));
            return;
        }
    }
    updateInterface(NavigationMode);
}

void AddressBook::loadFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Address Book"), "./",
                                                    tr("Address Book (*.abk) ;; All File(8)"));
    if(fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        else {
            QDataStream in(&file);
            in.setVersion(QDataStream::Qt_5_10);
            contacts.clear();
            in >> contacts;

            if(contacts.isEmpty())
            {
                QMessageBox::information(this, tr("No contact in file."),
                                         tr("The file you are attempting to open contains to contacts."));
            } else {
                QMap<QString, QString>::const_iterator it = contacts.begin();
                nameLine->setText(it.key());
                addressText->setText(it.value());
            }
        }

    }
    updateInterface(NavigationMode);
}

void AddressBook::saveToFile()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Address Book"), "./",
                                                    tr("Address Book (*.abk) ;;All Files (*)"));
    if(fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if(!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        else
        {
            QDataStream out(&file);
            out.setVersion(QDataStream::Qt_5_10);
            out << contacts;
        }
    }

    updateInterface(NavigationMode);
}

void AddressBook::exportToCard()
{
    QString name = nameLine->text();
    QString address = addressText->toPlainText();
    QString firstName, lastName;
    QStringList nameList;

    int index = name.indexOf(" ");
    if(index != -1) {
        nameList = name.split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
        firstName = nameList.first();
        lastName = nameList.last();
    } else {
        firstName = name;
        lastName = "";
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Contact"), "./",
                                                    tr("vCard Files (*.vcf) ;; All Files(*)"));
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("Unable to open file."),
                                 file.errorString());
        return;
    }

    QTextStream out(&file);
    out << "BEGIN:VCARD" << "\n";
    out << "VERSION:2.1" << "\n";
    out << "N:" << lastName << ";" << firstName << "\n";

    if(!nameList.isEmpty())
        out << "FN:" << nameList.join(' ') << '\n';
    else
        out << "FN:" << firstName << '\n';

    address.replace(";", "\\;", Qt::CaseInsensitive);
    address.replace("\n", ";;", Qt::CaseInsensitive);
    address.replace(",", " ", Qt::CaseInsensitive);

    out << "ADR;HOME:;" << address << '\n';
    out << "END;VCARD" << '\n';
    QMessageBox::information(this, tr("Export Successful"),
                             tr("\"%1\" has been exported as a vCard.").arg(name));

    updateInterface(NavigationMode);
}
