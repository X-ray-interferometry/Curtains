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
    void handleSliderValueChanged(int value); // Slot to handle slider value changes

public slots:
    void zoomIn();  // Zoom in the image
    void zoomOut(); // Zoom out the image
    void toggleLayerVisibility(QListWidgetItem *item); // Toggle layer visibility
    void saveImage(); // Save the current image
    void sliderModeChange(); // Change slider mode between energy range and single energy bin

private:
    QComboBox *modelSelector;
    QPushButton *generateButton;
    QPushButton *saveButton;
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

    static const int IMAGE_WIDTH = 400;
    static const int IMAGE_HEIGHT = 300;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // MYWIDGET_H