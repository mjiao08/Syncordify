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
    void onTimeout();
    void initializeSettings();

    void on_adjustedVolumeSlider_valueChanged(int value);
    void on_adjustedVolumeSpinBox_valueChanged(int value);
    void on_activationThresholdSlider_valueChanged(int value);
    void on_activationThresholdDoubleSpinBox_valueChanged(double valueDecimal);

private:
    Ui::MainWindow *ui;
    AudioController *audioController;
    QTimer *timer;
};

#endif // MAINWINDOW_H
