/*-------------------------------------------------------------------------*/
//
//                               Harsco Rail
//                             2401 Edmund Road
//                          West Columbia, SC 29171
//                  Copyright 2014-2024 All Rights Reserved
//
//   Date          : September 18,  2024
//   Filename      : mainwindow.h
//   Developer(s)  : Nirali Bandaru
//   Purpose       : Darknet Application for YOLO Inferencing
//
//
//
/*-------------------------------------------------------------------------*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QVector>
#include <QRect>
#include <QImage>
#include <QGroupBox>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void selectImageFolder();
    void selectConfigFile();
    void selectNamesFile();
    void selectDataFile();
    void runDarknetInference();
    void selectWeightsFile();
    void handleOutput();  // Handle Darknet output
    void handleError();   // Handle Darknet error
private:
    Ui::MainWindow *ui;

    // Define the buttons
    QPushButton *selectFolderButton;
    QPushButton *selectConfigButton;
    QPushButton *selectNamesButton;
    QPushButton *selectDataButton;
    QPushButton *runButton;
    QPushButton *selectWeightsButton;


    // Define the labels
    QLabel *titleLabel;
    QLabel *folderLabel;
    QLabel *configLabel;
    QLabel *namesLabel;
    QLabel *dataLabel;
    QLabel *weightsLabel;

    // Variables to hold the selected paths
    QString imageFolder;
    QString configFile;
    QString namesFile;
    QString dataFile;
    QString weightsFile;

    // For running Darknet
    QProcess *darknetProcess;
};

#endif // MAINWINDOW_H
