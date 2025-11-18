#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>

class Widget : public QWidget {
    Q_OBJECT // Required for Qt's signal/slot mechanism

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void updateImage(); // Slot to handle button click and image update

private:
    QLabel *imageLabel;
    QComboBox *modelSelector;
    QPushButton *generateButton;

    // Placeholder C++ functions representing Fortran subroutines
    // In a real app, these would be `extern "C"` declarations
    // that link to your compiled Fortran code using ISO_C_BINDING.
    void callFortranModelA(unsigned char* imageData, int width, int height);
    void callFortranModelB(unsigned char* imageData, int width, int height);
    void callFortranModelC(unsigned char* imageData, int width, int height);

    static const int IMAGE_WIDTH = 400;
    static const int IMAGE_HEIGHT = 300;
};

#endif // MYWIDGET_H