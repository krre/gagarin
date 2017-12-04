#pragma once
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event);

private:
    enum class Tabs {
        GENERAL,
        VULKAN
    };

    void readSettings();
    void writeSettings();

    Ui::MainWindow* ui;
    QString settingsPath;
};
