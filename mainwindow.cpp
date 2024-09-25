#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    audioController = new AudioController();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimeout);
    timer->start(100);
}

MainWindow::~MainWindow() {
    delete audioController;
    delete ui;
}

void MainWindow::onTimeout() {
    audioController->CheckAndAdjustAppVolume();
    printf("Adjusted Volume: %f\n", audioController->adjustedVolume);
    printf("Activation Threshold: %f\n", audioController->activationThreshold);
    printf("\n");
}

void MainWindow::on_adjustedVolumeSlider_valueChanged(int value) {
    float valueDecimal = (float)value / 100;

    ui->adjustedVolumeSpinBox->setValue(value);
    audioController->adjustedVolume = valueDecimal;
    qDebug() << "adjustedVolume after adjustedVolumeSlider function: " << valueDecimal << "\n";
}

void MainWindow::on_adjustedVolumeSpinBox_valueChanged(int value) {
    float valueDecimal = (float)value / 100;

    ui->adjustedVolumeSlider->setValue(value);
    audioController->adjustedVolume = valueDecimal;
}

void MainWindow::on_activationThresholdSlider_valueChanged(int value) {
    float valueDecimal = (float)value / 100;

    ui->activationThresholdDoubleSpinBox->setValue(valueDecimal);
    audioController->activationThreshold = valueDecimal;
}

void MainWindow::on_activationThresholdDoubleSpinBox_valueChanged(double valueDecimal) {
    int valueInteger = valueDecimal * 100;

    ui->activationThresholdSlider->setValue(valueInteger);
    audioController->activationThreshold = valueDecimal;
}
