#include "odcrypt.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ODCrypt w;
    w.show();

    return a.exec();
}
