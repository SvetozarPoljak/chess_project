#include <QApplication>
#include "dialog.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qRegisterMetaType<std::array<int, 64>>("std::array<int, 64>");
    Dialog w;
    w.show();
    return a.exec();
}
