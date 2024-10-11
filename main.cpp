/*-------------------------------------------------------------------------*/
//
//                               Harsco Rail
//                             2401 Edmund Road
//                          West Columbia, SC 29171
//                  Copyright 2014-2024 All Rights Reserved
//
//   Date          : September 18,  2024
//   Filename      : main.cpp
//   Developer(s)  : Nirali Bandaru
//   Purpose       : Darknet Application for YOLO Inferencing
//
//
//
/*-------------------------------------------------------------------------*/

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
