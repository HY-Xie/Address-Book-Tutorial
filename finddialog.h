#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE
#include <QDebug>
class FindDialog : public QDialog
{
    Q_OBJECT
public:
    FindDialog(QWidget *parent = nullptr);
    ~FindDialog() { qDebug() << "析构"; }
    QString getFindText();

private:
    QPushButton *findButton;
    QLabel *label;
    QLineEdit *lineEdit;
    QString findText;
public slots:
    void findClicked();

};

#endif // FINDDIALOG_H
