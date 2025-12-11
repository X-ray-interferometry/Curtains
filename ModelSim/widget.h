#ifndef WIDGET_H
#define WIDGET_H

#include "RenderWindow.h"
#include "Widgets/ColorScaleWidget.h"

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QListWidget>
#include <QPainterPath>
#include <QImage>
#include <memory>

//extern "C" {
//    void reltrans(unsigned char* imageData, int width, int height);
//}

struct PathSpectrum {
    QGraphicsPathItem *pathItem;
    std::function<double(double)> spectrumFunction;
    double maxBrightness; // Maximum brightness of the spectrum
    double totalBrightness; // Total brightness over the evaluated range
};

class Widget : public QWidget {
    Q_OBJECT // Required for Qt's signal/slot mechanism

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void updateImage(); // Slot to handle button click and image update
    void handleSingleSliderValueChanged(double value); // Slot to handle slider value changes
    void handleDoubleSliderRangeChanged(double low, double high); // Slot to handle double slider range changes

public slots:
    void zoomIn();  // Zoom in the image
    void zoomOut(); // Zoom out the image
    void toggleLayerVisibility(QListWidgetItem *item); // Toggle layer visibility
    void clearModel();

private:
    QComboBox *modelSelector;
    QPushButton *generateButton;
    QPushButton *clearButton;
    QSlider *slider;
    QSlider *lowRangeSlider;
    QSlider *highRangeSlider;
    QComboBox *sliderModeSelector;
    QGraphicsView *graphicsView; // Replace QLabel with QGraphicsView
    QGraphicsScene *graphicsScene; // Scene to hold vectorized content
    QListWidget *layersList; // List widget to display layers
    QPixmap currentPixmap; // Stores the current image being drawn on
    QPoint lastPoint;      // Tracks the last point for drawing
    QList<PathSpectrum> pathSpectra; // List of paths and their spectrum functions
    ColorScaleWidget *colorScaleWidget; // Declare the color scale widget

    // Rendering functionality
    std::function<double(double)> currentSpectrumFunction; // Store the currently selected spectrum function
    double amplitude; // Amplitude
    double frequency; // Frequency
    double radius;    // Radius for circle
    double innerRadius; // Inner radius for disk
    double outerRadius; // Outer radius for disk
    double inclination; // Inclination for disk
    double centerX;   // Center X for circle
    double centerY;   // Center Y for circle
    double radiusX;  // Radius X for ellipse
    double radiusY;  // Radius Y for ellipse
    double width;   // Width for rectangle
    double height;   // Height for rectangle
    int logScale = 0;   // Log scale flag 0 == linear, 1 == log
    QString shapeType; // Inner shape type for disk

    QMap<QString, QVariant> parameters; // Store model parameters   

    std::unique_ptr<RenderWindow> renderWindow; // Use a smart pointer

    void resetView();  // Reset view to full image
    void addModelPathItem(const QPainterPath &path, const QColor &fillColor, std::function<double(double)> spectrumFunction = nullptr);
    void addPathWithSpectrum(QGraphicsPathItem *pathItem, std::function<double(double)> spectrumFunction);
    void setPathSpectrumFunction(QGraphicsPathItem *pathItem, std::function<double(double)> spectrumFunction);
    double evaluateSpectrumOverRange(const PathSpectrum &pathSpectrum, double low, double high, double step);
    void editLayerPath(QListWidgetItem *item);
    void editLayerSpectrum(QListWidgetItem *item);
    void deleteLayer(QListWidgetItem *item);
    void showLayerContextMenu(const QPoint &pos);

    QImage renderStackedImage(); // Declare the function here

    // Placeholder C++ functions representing Fortran subroutines
    // In a real app, these would be `extern "C"` declarations
    // that link to your compiled Fortran code using ISO_C_BINDING.

    static const int IMAGE_WIDTH = 400;
    static const int IMAGE_HEIGHT = 400;
    QColor fillColor = Qt::white; // Default fill color
    double globalMinBrightness = 0.0; // Minimum brightness across all paths
    double globalMaxBrightness = 0.0; // Maximum brightness across all paths
    double globalTotalMinBrightness = 0.0; // Total minimum brightness across all paths
    double globalTotalMaxBrightness = 0.0; // Total maximum brightness across all paths

protected:
    void keyPressEvent(QKeyEvent *event) override;
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
