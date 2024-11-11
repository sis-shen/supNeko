#include "widget.h"

#include <QApplication>
#include "model/data.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LOG()<<"hello";
    Widget w;
    w.show();
    return a.exec();
}
