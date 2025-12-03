#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QListWidget>
#include <QPainterPath>

//extern "C" {
//    void reltrans(unsigned char* imageData, int width, int height);
//}

class Widget : public QWidget {
    Q_OBJECT // Required for Qt's signal/slot mechanism

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void updateImage(); // Slot to handle button click and image update
    void handleSingleSliderValueChanged(int value); // Slot to handle slider value changes
    void handleDoubleSliderRangeChanged(double low, double high); // Slot to handle double slider range changes

public slots:
    void zoomIn();  // Zoom in the image
    void zoomOut(); // Zoom out the image
    void toggleLayerVisibility(QListWidgetItem *item); // Toggle layer visibility
    void saveImage(); // Save the current image

private:
    QComboBox *modelSelector;
    QPushButton *generateButton;
    QPushButton *saveButton;
    QSlider *slider;
    QSlider *lowRangeSlider;
    QSlider *highRangeSlider;
    QComboBox *sliderModeSelector;
    QGraphicsView *graphicsView; // Replace QLabel with QGraphicsView
    QGraphicsScene *graphicsScene; // Scene to hold vectorized content
    QListWidget *layersList; // List widget to display layers
    QPixmap currentPixmap; // Stores the current image being drawn on
    QPoint lastPoint;      // Tracks the last point for drawing
    QPainterPath drawingPath; // Stores the vectorized lines
    QGraphicsPathItem *drawingPathItem; // Item to represent the drawn paths
    bool isDrawing = false; // Tracks whether the user is currently drawing

    void resetView();  // Reset view to full image

    // Placeholder C++ functions representing Fortran subroutines
    // In a real app, these would be `extern "C"` declarations
    // that link to your compiled Fortran code using ISO_C_BINDING.
    void generateSineWave(QPainterPath &path, int width, int height);
    void callReltrans(unsigned char* imageData, int width, int height);
    void blankImage(QPainterPath &path, int width, int height);
    void circle(QPainterPath &path, int centerX, int centerY, int radius);

    static const int IMAGE_WIDTH = 400;
    static const int IMAGE_HEIGHT = 300;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // MYWIDGET_H

#ifndef SINGLESLIDERWIDGET_H
#define SINGLESLIDERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

class SingleSliderWidget : public QWidget {
    Q_OBJECT

public:
    explicit SingleSliderWidget(QWidget *parent = nullptr);

signals:
    void valueChanged(double value);

private:
    QSlider *slider;
    QDoubleSpinBox *sliderValueBox;
};

#endif // SINGLESLIDERWIDGET_H

#ifndef DOUBLESLIDERWIDGET_H
#define DOUBLESLIDERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QVBoxLayout>

class DoubleSliderWidget : public QWidget {
    Q_OBJECT

public:
    explicit DoubleSliderWidget(QWidget *parent = nullptr);

signals:
    void valueChanged(double value);
    void rangeChanged(double low, double high);

private:
    QSlider *lowSlider;
    QSlider *highSlider;
    QDoubleSpinBox *lowSliderValueBox;
    QDoubleSpinBox *highSliderValueBox;
};

#endif // DOUBLESLIDERWIDGET_H
