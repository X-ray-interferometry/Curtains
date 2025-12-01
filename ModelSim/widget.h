#ifndef WIDGET_H
#define WIDGET_H

#include "imagelabel.h"
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>

extern "C" {
    void reltrans(unsigned char* imageData, int width, int height);
}

class Widget : public QWidget {
    Q_OBJECT // Required for Qt's signal/slot mechanism

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void updateImage(); // Slot to handle button click and image update

public slots:
    void zoomIn();  // Zoom in the image
    void zoomOut(); // Zoom out the image

private:
    ImageLabel *imageLabel;
    QComboBox *modelSelector;
    QPushButton *generateButton;

    enum InteractionMode { ZoomMode, DragMode }; // Define interaction modes
    InteractionMode currentMode = ZoomMode;     // Default to ZoomMode

    void setZoomMode(); // Switch to zoom mode
    void setDragMode(); // Switch to drag mode
    void resetView();  // Reset view to full image

    QPixmap originalPixmap;
    QRect viewingWindow; // Tracks the currently visible portion of the image
    QPixmap displayedPixmap;
    double zoomFactor = 1.0; // Zoom factor (1.0 = 100%)

    QPoint dragStartPos; // Track the starting position of the drag
    QPoint currentOffset; // Track the current offset of the image
    bool isDragging = false; // Track whether the user is dragging

    void adjustViewToZoom(); // Adjust the view based on the zoom factor
    void zoomToSelection(); // Zoom into the selected rectangle

    // Variables for box selection
    QRect selectionRect; // Stores the selected rectangle
    bool isSelecting = false; // Tracks whether the user is currently selecting 

    // Placeholder C++ functions representing Fortran subroutines
    // In a real app, these would be `extern "C"` declarations
    // that link to your compiled Fortran code using ISO_C_BINDING.
    void callFortranModelA(unsigned char* imageData, int width, int height);
    void callFortranModelB(unsigned char* imageData, int width, int height);
    void callFortranModelC(unsigned char* imageData, int width, int height);

    static const int IMAGE_WIDTH = 400;
    static const int IMAGE_HEIGHT = 300;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};

#endif // MYWIDGET_H