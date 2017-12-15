#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QApplication>
#include "getkeycode.h"

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    getKeyCode window;
    window.show();

    return app.exec();
}
