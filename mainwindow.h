#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "audiocontroller.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnTimeout();
    void InitializeSettings();

    float IntToDecimal(int value);
    void OnAdjustedVolumeSliderValueChanged(int value);
    void OnAdjustedVolumeSpinBoxValueChanged(int value);
    void OnActivationThresholdSliderValueChanged(int value);
    void OnActivationThresholdDoubleSpinBoxValueChanged(double valueDecimal);

private:
    Ui::MainWindow *ui;
    AudioController *audioController;
    QTimer *timer;
};

#endif // MAINWINDOW_H
