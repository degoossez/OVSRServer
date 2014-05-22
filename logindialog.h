#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>

class LoginDialog : public QDialog
{
    /*!
     *Turns Login Dialog into a QObject
     */
        Q_OBJECT

    private:
        /*!
          * A label for the username component.
          */
        QLabel* labelUsername;

        /*!
          * A label for the password.
          */
        QLabel* labelPassword;

        /*!
          * An editable combo box for allowing the user
          * to enter his username or select it from a list.
          */
        QComboBox* comboUsername;

        /*!
          * A field to let the user enters his password.
          */
        QLineEdit* editPassword;

        /*!
          * The standard dialog button box.
          */
        QDialogButtonBox* buttons;

        /*!
          * A method to set up all dialog components and
          * initialize them.
          */
        void setUpGUI();

    public:
        explicit LoginDialog(QWidget *parent = 0);        

    signals:

        /*!
          * A signal emitted when the login is performed.
          * \param username the username entered in the dialog
          * \param password the password entered in the dialog
          *
          */
        void acceptLogin( QString username, QByteArray password);

    public slots:

        void slotAcceptLogin();

};

#endif // LOGINDIALOG_H
