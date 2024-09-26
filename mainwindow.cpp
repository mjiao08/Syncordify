#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioController(new AudioController()) {
    ui->setupUi(this);

    connect(ui->adjustedVolumeSlider,
            &QSlider::valueChanged,
            this,
            &MainWindow::OnAdjustedVolumeSliderValueChanged);
    connect(ui->adjustedVolumeSpinBox,
            &QSpinBox::valueChanged,
            this,
            &MainWindow::OnAdjustedVolumeSpinBoxValueChanged);
    connect(ui->activationThresholdSlider,
            &QSlider::valueChanged,
            this,
            &MainWindow::OnActivationThresholdSliderValueChanged);
    connect(ui->activationThresholdDoubleSpinBox,
            &QDoubleSpinBox::valueChanged,
            this,
            &MainWindow::OnActivationThresholdDoubleSpinBoxValueChanged);

    InitializeSettings();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::OnTimeout);
    timer->start(100);
}

MainWindow::~MainWindow() {
    delete audioController;
    delete ui;
}

void MainWindow::InitializeSettings() {
    QSettings settings("settings.ini", QSettings::IniFormat);

    // Set variables
    audioController->adjustedVolume = settings.value("adjustedVolume", 0.5f).toFloat();
    audioController->activationThreshold = settings.value("activationThreshold", 0.05f).toFloat();

    // Update spin box values (this triggers the valueChanged functions to also update the slider positions)
    ui->adjustedVolumeSpinBox->setValue((int)(audioController->adjustedVolume * 100));
    ui->activationThresholdDoubleSpinBox->setValue(audioController->activationThreshold);
}

void MainWindow::OnTimeout() {
    audioController->CheckAndAdjustAppVolume();
    printf("Adjusted Volume: %f\n", audioController->adjustedVolume);
    printf("Activation Threshold: %f\n", audioController->activationThreshold);
    printf("\n");
}

float MainWindow::IntToDecimal(int value) {
    return (float)value / 100;
}

void MainWindow::OnAdjustedVolumeSliderValueChanged(int value) {
    ui->adjustedVolumeSpinBox->setValue(value);
    audioController->adjustedVolume = IntToDecimal(value);
}

void MainWindow::OnAdjustedVolumeSpinBoxValueChanged(int value) {
    ui->adjustedVolumeSlider->setValue(value);
    audioController->adjustedVolume = IntToDecimal(value);
}

void MainWindow::OnActivationThresholdSliderValueChanged(int value) {
    float valueDecimal = IntToDecimal(value);

    ui->activationThresholdDoubleSpinBox->setValue(valueDecimal);
    audioController->activationThreshold = valueDecimal;
}

void MainWindow::OnActivationThresholdDoubleSpinBoxValueChanged(double valueDecimal) {
    int valueInteger = valueDecimal * 100;

    ui->activationThresholdSlider->setValue(valueInteger);
    audioController->activationThreshold = valueDecimal;
}
