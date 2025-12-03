#include "widget.h"
#include "QRangeSlider.hpp"
#include <QImage>
#include <QPixmap>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QDebug> 
#include <QFileDialog>

Widget::Widget(QWidget *parent) : QWidget(parent) {
    // 1. Create Widgets
    graphicsScene = new QGraphicsScene(this);
    graphicsView = new QGraphicsView(graphicsScene, this);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag); // Enable dragging
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // Zoom relative to mouse position

    layersList = new QListWidget(this);
    layersList->setFixedWidth(150); // Set a fixed width for the layers list
    layersList->setSelectionMode(QAbstractItemView::SingleSelection);
    layersList->setAlternatingRowColors(true);

    modelSelector = new QComboBox(this);
    modelSelector->addItem("Blank");
    modelSelector->addItem("Sine Wave");
    modelSelector->addItem("Reltrans");

    //Buttons
    generateButton = new QPushButton("Base Model", this);
    QPushButton *zoomInButton = new QPushButton("Zoom In", this);
    zoomInButton->setToolTip("Zoom In (Ctrl + '+')");
    QPushButton *zoomOutButton = new QPushButton("Zoom Out", this);
    zoomOutButton->setToolTip("Zoom Out (Ctrl + '-')");
    QPushButton *resetViewButton = new QPushButton("Reset View", this);
    QPushButton *saveButton = new QPushButton("Save Image", this);
    QPushButton *sliderModeButton = new QPushButton("Energy slider mode", this);

    // Sliders

    QSlider *slider = new QSlider(Qt::Horizontal, this);
    slider->setToolTip("Adjust Energy range");
    slider->setRange(0, 10); // Set the range of the slider
    slider->setValue(5);     // Set the initial value of the slider
    rangeSlider = new QRangeSlider(this); // Initialize the range slider
    rangeSlider->setToolTip("Adjust Energy Range");
    rangeSlider->setMinimum(0); // Set the minimum value
    rangeSlider->setMaximum(10); // Set the maximum value
    rangeSlider->setLowValue(0); // Set the initial lower value
    rangeSlider->setHighValue(10); // Set the initial upper value
    //rangeSlider->hide(); // Initially hide the range slider

    // Connect signals for range slider value changes
    connect(rangeSlider, &QRangeSlider::lowValueChange, this, &Widget::handleLowerValueChanged);
    connect(rangeSlider, &QRangeSlider::highValueChange, this, &Widget::handleUpperValueChanged);

    // Connect slider value changes to the appropriate slot
    connect(slider, &QSlider::valueChanged, this, &Widget::handleSliderValueChanged);
    connect(sliderModeButton, &QPushButton::clicked, this, &Widget::sliderModeChange);

    // Connect scene  buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addWidget(resetViewButton);

    // Horizontal layout for energy slider and its button
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(sliderModeButton);
    sliderLayout->addWidget(slider);
    sliderLayout->addWidget(rangeSlider);

    // Horizontal layout for graphics view and layers list
    QHBoxLayout *canvasLayout = new QHBoxLayout();
    canvasLayout->addWidget(graphicsView);
    canvasLayout->addWidget(layersList);

    // 2. Arrange Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(modelSelector);
    mainLayout->addWidget(generateButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(canvasLayout);
    mainLayout->addLayout(sliderLayout);
    mainLayout->addWidget(saveButton);

    setLayout(mainLayout);
    setWindowTitle("Image");
    setMinimumSize(500, 500);

    // 3. Connect Signals and Slots
    connect(generateButton, &QPushButton::clicked, this, &Widget::updateImage);
    connect(zoomInButton, &QPushButton::clicked, this, &Widget::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &Widget::zoomOut);
    connect(resetViewButton, &QPushButton::clicked, this, &Widget::resetView);
    connect(layersList, &QListWidget::itemClicked, this, &Widget::toggleLayerVisibility);
    connect(saveButton, &QPushButton::clicked, this, &Widget::saveImage);

    qDebug() << "Widget initialized.";
    // Initial image generation
    updateImage();
}

Widget::~Widget() {
    // Widgets are children of Widget, so they are automatically deleted when Widget is destroyed.
    // No need for explicit `delete` calls here for the dynamically allocated widgets.
}

void Widget::resetView() {
    graphicsView->resetTransform(); // Reset zoom and transformations
    graphicsView->fitInView(graphicsScene->itemsBoundingRect(), Qt::KeepAspectRatio); // Fit the view to the content
}

void Widget::updateImage() {
    qDebug() << "Updating image with model:" << modelSelector->currentText();
    graphicsScene->clear(); // Clear the scene before rendering new content
    layersList->clear(); // Clear the layers list

    QPainterPath modelPath;
    int selectedIndex = modelSelector->currentIndex();
    switch (selectedIndex) {
        case 0: // Blank
            blankImage(modelPath, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        case 1: // Sine Wave
            generateSineWave(modelPath, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        case 2: // Reltrans (placeholder)
            qWarning() << "Reltrans model not implemented for vectorized graphics.";
            return;
        default:
            qWarning() << "Unknown model selected!";
            return;
    }
    qDebug() << "Model path generated.";
    QGraphicsPathItem *modelPathItem = new QGraphicsPathItem(modelPath);
    modelPathItem->setPen(QPen(Qt::blue, 2));
    graphicsScene->addItem(modelPathItem); // Add the model path to the scene
    qDebug() << "Model path item added to graphics scene.";
    // Add the model layer to the layers list
    QListWidgetItem *modelLayerItem = new QListWidgetItem("[Visible] Base Model");
    modelLayerItem->setData(Qt::UserRole, QVariant::fromValue(modelPathItem)); // Store the graphics item
    layersList->addItem(modelLayerItem);
    qDebug() << "Layers list updated.";

    // Re-add the drawingPathItem to ensure it remains visible
    if (!drawingPath.isEmpty()) {
        graphicsScene->addItem(drawingPathItem); // Add the path item to the scene
        qDebug() << "Re-added drawing path item to graphics scene.";

        // Add the drawing layer to the layers list
        QListWidgetItem *drawingLayerItem = new QListWidgetItem("Drawing Layer [Visible]");
        drawingLayerItem->setData(Qt::UserRole, QVariant::fromValue(drawingPathItem)); // Store the graphics item
        layersList->addItem(drawingLayerItem);
    }
    graphicsView->fitInView(graphicsScene->itemsBoundingRect(), Qt::KeepAspectRatio); // Fit the view to the content
}

void Widget::zoomIn() {
    graphicsView->scale(1.2, 1.2); // Zoom in by 20%
}

void Widget::zoomOut() {
    graphicsView->scale(0.8, 0.8); // Zoom out by 20%
}

void Widget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus) {
            zoomIn();
        } else if (event->key() == Qt::Key_Minus) {
            zoomOut();
        }
    }
}

// Drawing functionality
void Widget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDrawing = true;
        lastPoint = event->pos(); // Store the starting point
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (isDrawing) {
        QPainterPath path;
        path.moveTo(lastPoint);
        path.lineTo(event->pos());
        QGraphicsPathItem *modelPathItem = new QGraphicsPathItem(path);
        modelPathItem->setPen(QPen(Qt::blue, 2));
        graphicsScene->addItem(modelPathItem); // Add the model path to the scene
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isDrawing) {
        isDrawing = false;
    }
}

// layer functionality

void Widget::toggleLayerVisibility(QListWidgetItem *item) {
    QGraphicsPathItem *pathItem = item->data(Qt::UserRole).value<QGraphicsPathItem*>();
    if (!pathItem) {
        qWarning() << "No graphics item associated with this layer!";
        return;
    }
    qDebug() << "Toggling layer visibility for item:" << item->text();
    qDebug() << "Current visibility:" << pathItem->isVisible();
    if (pathItem->isVisible()) {
        pathItem->hide();
        item->setText("[Hidden] " + item->text().remove("[Visible] "));
        qDebug() << "Layer hidden.";
    } else {
        pathItem->show();
        item->setText("[Visible] " + item->text().remove("[Hidden] "));
        qDebug() << "Layer shown.";
    }
}

// Save image functionality

void Widget::saveImage() {
    // Prompt the user for a file location and name
    QString filePath = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;JPEG Files (*.jpg);;BMP Files (*.bmp);;All Files (*)");

    // Check if the user canceled the dialog
    if (filePath.isEmpty()) {
        qWarning() << "Save operation canceled.";
        return;
    }

    // Create a pixmap to render the scene
    QRectF sceneRect = graphicsScene->sceneRect(); // Get the scene's bounding rectangle
    QPixmap pixmap(sceneRect.width(), sceneRect.height());
    pixmap.fill(Qt::black); // Fill the pixmap with a black background (0 photons)

    // Render the scene onto the pixmap
    QPainter painter(&pixmap);
    graphicsScene->render(&painter);
    painter.end();

    // Save the current image to the specified file
    if (!pixmap.save(filePath)) {
        qWarning() << "Failed to save image to:" << filePath;
    } else {
        qDebug() << "Image successfully saved to:" << filePath;
    }
}

// Slider functionality

void Widget::handleSliderValueChanged(int value) {
    qDebug() << "Slider value changed to:" << value;

    // Example: Adjust the opacity of the graphicsScene
    qreal opacity = 10*value / 100.0; // Convert slider value to a range of 0.0 to 1.0
    for (auto item : graphicsScene->items()) {
        item->setOpacity(opacity); // Set the opacity of each item in the scene
    }
}

void Widget::sliderModeChange() {
    qDebug() << "Slider mode change button clicked.";
    if (!slider || !rangeSlider) {
        qWarning() << "One of the sliders is not initialized!";
        return; // Prevent segmentation fault if either slider is null
    }

    if (sliderMode == 0) {
        qDebug() << "Switching to energy range mode.";
        slider->setVisible(false);
        rangeSlider->setVisible(true);
        sliderMode = 1; // Toggle mode
    } else {
        qDebug() << "Switched to single energy bin mode.";
        rangeSlider->setVisible(false);
        slider->setVisible(true);
        sliderMode = 0; // Toggle mode
    }
}

void Widget::handleLowerValueChanged(int value) {
    qDebug() << "Range slider lower value changed to:" << value;
    // Handle the lower value (e.g., update energy range)
}

void Widget::handleUpperValueChanged(int value) {
    qDebug() << "Range slider upper value changed to:" << value;
    // Handle the upper value (e.g., update energy range)
}

// Image generators

void Widget::generateSineWave(QPainterPath &path, int width, int height) {
    path.moveTo(0, height / 2);
    for (int x = 0; x < width; ++x) {
        double y = height / 2 + 50 * sin(x * 0.1); // Sine wave formula
        path.lineTo(x, y);
    }
}

void Widget::blankImage(QPainterPath &path, int width, int height) {
    // Generates a blank image (no drawing)
    qDebug() << "Generating blank image.";
}

void Widget::callReltrans(unsigned char* imageData, int width, int height) {
    // Calls reltrans.
    qDebug() << "Calling reltrans Fortran subroutine.";
    //reltrans(imageData, width, height);
}