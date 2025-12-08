/*
This is the main application entry point.
*/

#include <QApplication>
#include "Widget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Widget w;
    w.show();
    return a.exec();
}