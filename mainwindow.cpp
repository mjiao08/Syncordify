#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QTimer* timer = new QTimer(this);
    // connect(timer, &QTimer::timeout, this, &MainWindow::onAdjustVolumeClicked);
    timer->start(100);
}

MainWindow::~MainWindow() {
    delete ui;
    delete audioController;
}
