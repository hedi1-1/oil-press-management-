#include "machine.h"
#include "connexionmachine.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Connection cnx;
    bool test = cnx.createconnect();

    if (test) {
        QMessageBox::information(nullptr, "Connection",
                                 "Connection established successfully!");
    } else {
        QMessageBox::critical(nullptr, "Connection",
                              "Failed to connect to the database.");
    }

    machine w;
    w.show();
    return a.exec();
}
