#ifndef ADDRESSBOOK_H
#define ADDRESSBOOK_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
class QPushButton;
QT_END_NAMESPACE
#include <QMap>
#include <QString>
#include "finddialog.h"
enum Mode { NavigationMode, AddingMode, EditingMode };

class AddressBook : public QWidget
{
    Q_OBJECT

public:
    AddressBook(QWidget *parent = nullptr);
    ~AddressBook();
private:
    QLineEdit *nameLine;
    QTextEdit *addressText;
    QPushButton *addButton;
    QPushButton *submitButton;
    QPushButton *cancelButton;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QPushButton *editButton;
    QPushButton *removeButton;
    QPushButton *loadButton;
    QPushButton *saveButton;
    QPushButton *exportButton;

    Mode currentMode;

    QMap<QString, QString> contacts;
    QString oldName;
    QString oldAddress;

    FindDialog *dialog;
    QPushButton *findButton;

public slots:
    void addContact();
    void submitContact();
    void cancel();
    void next();
    void previous();
    void editContact();
    void removeContact();
    void updateInterface(Mode mode);
    void findContact();
    void loadFromFile();
    void saveToFile();
    void exportToCard();
};

#endif // ADDRESSBOOK_H
