/*-------------------------------------------------------------------------*/
//
//                               Harsco Rail
//                             2401 Edmund Road
//                          West Columbia, SC 29171
//                  Copyright 2014-2024 All Rights Reserved
//
//   Date          : September 18,  2024
//   Filename      : mainwindow.cpp
//   Developer(s)  : Nirali Bandaru
//   Purpose       : Darknet Application for YOLO Inferencing
//
//
/*-------------------------------------------------------------------------*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , darknetProcess(new QProcess(this))
{
    ui->setupUi(this);

    // Create and set up the title label
    titleLabel = new QLabel("Darknet Inferencer", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #003366; }");

    // Create and set up folder label
    folderLabel = new QLabel("No folder selected", this);
    folderLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    // Create and set up config, names, data labels
    configLabel = new QLabel("No config file selected", this);
    configLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    namesLabel = new QLabel("No names file selected", this);
    namesLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    dataLabel = new QLabel("No data file selected", this);
    dataLabel->setStyleSheet("QLabel { color: black; background-color: transparent; }");

    // Create the label and button for weights file selection
    weightsLabel = new QLabel("Select Weights File:", this);

    // Create and set up the buttons
    selectFolderButton = new QPushButton("Select Folder", this);
    selectConfigButton = new QPushButton("Select Config", this);
    selectNamesButton = new QPushButton("Select Names", this);
    selectDataButton = new QPushButton("Select Data", this);
    selectWeightsButton = new QPushButton("Select Weights", this);
    runButton = new QPushButton("Run Darknet Inference", this);

    // Set button styles (optional)
    selectFolderButton->setStyleSheet("QPushButton { font-size: 14px; }");
    selectConfigButton->setStyleSheet("QPushButton { font-size: 14px; }");
    selectNamesButton->setStyleSheet("QPushButton { font-size: 14px; }");
    selectDataButton->setStyleSheet("QPushButton { font-size: 14px; }");
    runButton->setStyleSheet("QPushButton { font-size: 14px; background-color: #339966; color: white; }");

    // Connect buttons to the relevant slots
    connect(selectFolderButton, &QPushButton::clicked, this, &MainWindow::selectImageFolder);
    connect(selectConfigButton, &QPushButton::clicked, this, &MainWindow::selectConfigFile);
    connect(selectNamesButton, &QPushButton::clicked, this, &MainWindow::selectNamesFile);
    connect(selectDataButton, &QPushButton::clicked, this, &MainWindow::selectDataFile);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runDarknetInference);
    connect(selectWeightsButton, &QPushButton::clicked, this, &MainWindow::selectWeightsFile);

    // Layout setup
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(titleLabel);
    layout->addWidget(folderLabel);
    layout->addWidget(selectFolderButton);
    layout->addWidget(configLabel);
    layout->addWidget(selectConfigButton);
    layout->addWidget(namesLabel);
    layout->addWidget(selectNamesButton);
    layout->addWidget(dataLabel);
    layout->addWidget(selectDataButton);
    layout->addWidget(weightsLabel);
    layout->addWidget(selectWeightsButton);
    layout->addWidget(runButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Function to select image folder
void MainWindow::selectImageFolder()
{
    imageFolder = QFileDialog::getExistingDirectory(this, tr("Select Image Folder"), "");
    if (!imageFolder.isEmpty()) {
        folderLabel->setText(imageFolder);  // Update folder label
    }
}

// Function to select config file
void MainWindow::selectConfigFile()
{
    configFile = QFileDialog::getOpenFileName(this, tr("Select Config File"), "", tr("Config Files (*.cfg)"));
    if (!configFile.isEmpty()) {
        configLabel->setText(configFile);  // Update config label
    }
}

// Function to select names file
void MainWindow::selectNamesFile()
{
    namesFile = QFileDialog::getOpenFileName(this, tr("Select Names File"), "", tr("Names Files (*.names)"));
    if (!namesFile.isEmpty()) {
        namesLabel->setText(namesFile);  // Update names label
    }
}

// Function to select data file
void MainWindow::selectDataFile()
{
    dataFile = QFileDialog::getOpenFileName(this, tr("Select Data File"), "", tr("Data Files (*.data)"));
    if (!dataFile.isEmpty()) {
        dataLabel->setText(dataFile);  // Update data label
    }
}

// Function to select weights file
void MainWindow::selectWeightsFile()
{
    weightsFile = QFileDialog::getOpenFileName(this, "Select Weights File", "", "Weights Files (*.weights);;All Files (*)");
    if (!weightsFile.isEmpty()) {
        weightsLabel->setText("Weights File: " + weightsFile);
    }
}

// Function to run Darknet inference
void MainWindow::runDarknetInference()
{
    // Ensure all required files are selected
    if (imageFolder.isEmpty() || configFile.isEmpty() || namesFile.isEmpty() || dataFile.isEmpty() || weightsFile.isEmpty()) {
        QMessageBox::warning(this, "Missing Files", "Please select all necessary files before running inference.");
        return;
    }

    // Get the parent directory of the image folder
    QString parentDir = QFileInfo(imageFolder).absolutePath();
    QString linkPath = parentDir + "/darknet";  // Path for symbolic link
    QString darknetExecutable = "/home/nbandaru1/ai/darknet-master/darknet";  // Actual Darknet path

    // Create a symbolic link to the Darknet executable
    if (QFile::exists(linkPath)) {
        QFile::remove(linkPath);  // Remove existing link if it exists
    }
    if (!QFile::link(darknetExecutable, linkPath)) {
        QMessageBox::warning(this, "Error", "Failed to create symbolic link for Darknet.");
        return;
    }

    // Create test.txt file in the parent directory of the image folder
    QString testFilePath = parentDir + "/test.txt";
    QFile testFile(testFilePath);

    if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not create test.txt file.");
        return;
    }

    // Write the paths of all images in the folder to the test.txt file
    QTextStream out(&testFile);
    QDir dir(imageFolder);
    QStringList imageFilters;
    imageFilters << "*.jpg" << "*.JPG";  // Adjust filters as per your image formats
    QStringList imageList = dir.entryList(imageFilters, QDir::Files);

    if (imageList.isEmpty()) {
        QMessageBox::warning(this, "No Images Found", "No image files were found in the selected folder.");
        testFile.close();
        return;
    }

    for (const QString &imageName : imageList) {
        out << dir.absoluteFilePath(imageName) << "\n";  // Write each image's full path
    }
    testFile.close();

    // Check if test.txt exists and is not empty
    if (!QFile::exists(testFilePath)) {
        QMessageBox::warning(this, "Error", "The test.txt file was not created.");
        return;
    }

    // Check the number of lines (images) in test.txt
    QFile testFileCheck(testFilePath);
    if (testFileCheck.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&testFileCheck);
        int imageCount = 0;
        while (!in.atEnd()) {
            in.readLine();
            ++imageCount;
        }
        testFileCheck.close();

        if (imageCount == 0) {
            QMessageBox::warning(this, "Error", "The test.txt file is empty.");
            return;
        } else {
            QMessageBox::information(this, "Images Count", QString("Found %1 images.").arg(imageCount));
        }
    }

    // Construct the Darknet command
    QString darknetCommand = QString("%1 detector test %2 %3 %4 -thresh 0.40 -dont_show -save_labels")
                                 .arg(darknetExecutable)
                                 .arg(dataFile)
                                 .arg(configFile)
                                 .arg(weightsFile);

    // Set working directory for the process
    darknetProcess->setWorkingDirectory(parentDir);

    // Start the Darknet process with shell-like input redirection using bash
    QStringList arguments;
    arguments << "-c" << darknetCommand + " < " + testFilePath;

    darknetProcess->start("/bin/bash", arguments);

    if (!darknetProcess->waitForStarted()) {
        QMessageBox::warning(this, "Error", "Failed to start Darknet process.");
        return;
    }

    // Connect to output and error signals
    connect(darknetProcess, &QProcess::readyReadStandardOutput, this, &MainWindow::handleOutput);
    //connect(darknetProcess, &QProcess::readyReadStandardError, this, &MainWindow::handleError);
}

// Handle the standard output from the Darknet process
void MainWindow::handleOutput()
{
    QString output = darknetProcess->readAllStandardOutput();
    qDebug() << "" << output;  // Display output in debug console
}

// Handle the standard error from the Darknet process
// void MainWindow::handleError()
// {
//     QString errorOutput = darknetProcess->readAllStandardError();
//     qDebug() << "Darknet Error Output: " << errorOutput;  // Display error output in debug console
// }

