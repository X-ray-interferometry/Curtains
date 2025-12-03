#include "widget.h"
#include <QImage>
#include <QPixmap>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QDebug> 
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QStackedLayout>

SingleSliderWidget::SingleSliderWidget(QWidget *parent) : QWidget(parent) {
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 100);
    slider->setValue(50);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksBelow);

    sliderValueBox = new QDoubleSpinBox(this);
    sliderValueBox->setRange(0.0, 10.0);
    sliderValueBox->setValue(5.0);
    sliderValueBox->setSingleStep(0.1);

    // Connect slider and spin box
    connect(slider, &QSlider::valueChanged, [this](int value) {
        sliderValueBox->setValue(value * 0.1);
        emit valueChanged(value * 0.1);
    });
    connect(sliderValueBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        slider->setValue(static_cast<int>(value * 10));
    });

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(slider);
    layout->addWidget(sliderValueBox);
    setLayout(layout);
}

DoubleSliderWidget::DoubleSliderWidget(QWidget *parent) : QWidget(parent) {
    lowSlider = new QSlider(Qt::Horizontal, this);
    lowSlider->setRange(0, 100);
    lowSlider->setValue(20);
    lowSlider->setTickInterval(1);
    lowSlider->setTickPosition(QSlider::TicksBelow);

    highSlider = new QSlider(Qt::Horizontal, this);
    highSlider->setRange(0, 100);
    highSlider->setValue(80);
    highSlider->setTickInterval(1);
    highSlider->setTickPosition(QSlider::TicksBelow);

    lowSliderValueBox = new QDoubleSpinBox(this);
    lowSliderValueBox->setRange(0.0, 10.0);
    lowSliderValueBox->setValue(2.0);
    lowSliderValueBox->setSingleStep(0.1);

    highSliderValueBox = new QDoubleSpinBox(this);
    highSliderValueBox->setRange(0.0, 10.0);
    highSliderValueBox->setValue(8.0);
    highSliderValueBox->setSingleStep(0.1);

    // Connect sliders and spin boxes
    connect(lowSlider, &QSlider::valueChanged, [this](int value) {
        lowSliderValueBox->setValue(value * 0.1);
        emit rangeChanged(lowSliderValueBox->value(), highSliderValueBox->value());
    });
    connect(lowSliderValueBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        lowSlider->setValue(static_cast<int>(value * 10));
        emit rangeChanged(value, highSliderValueBox->value());
    });

    connect(highSlider, &QSlider::valueChanged, [this](int value) {
        highSliderValueBox->setValue(value * 0.1);
        emit rangeChanged(lowSliderValueBox->value(), highSliderValueBox->value());
    });
    connect(highSliderValueBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        highSlider->setValue(static_cast<int>(value * 10));
        emit rangeChanged(lowSliderValueBox->value(), value);
    });

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *lowLayout = new QHBoxLayout();
    lowLayout->addWidget(lowSlider);
    lowLayout->addWidget(lowSliderValueBox);

    QHBoxLayout *highLayout = new QHBoxLayout();
    highLayout->addWidget(highSlider);
    highLayout->addWidget(highSliderValueBox);

    layout->addLayout(lowLayout);
    layout->addLayout(highLayout);
    setLayout(layout);
}

Widget::Widget(QWidget *parent) : QWidget(parent) {
    // Create Widgets
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
    modelSelector->addItem("Circle");
    modelSelector->addItem("Reltrans");
    modelSelector->setToolTip("Select Model");

    // Buttons
    generateButton = new QPushButton("Base Model", this);
    QPushButton *zoomInButton = new QPushButton("Zoom In", this);
    zoomInButton->setToolTip("Zoom In (Ctrl + '+')");
    QPushButton *zoomOutButton = new QPushButton("Zoom Out", this);
    zoomOutButton->setToolTip("Zoom Out (Ctrl + '-')");
    QPushButton *resetViewButton = new QPushButton("Reset View", this);
    QPushButton *saveButton = new QPushButton("Save Image", this);

    // Connect scene  buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addWidget(resetViewButton);

    // Slider Mode Selector
    sliderModeSelector = new QComboBox(this);
    sliderModeSelector->addItem("Energy bin");
    sliderModeSelector->addItem("Energy range");
    sliderModeSelector->setToolTip("Select Slider Mode");

    // Sliders
    SingleSliderWidget *singleSliderWidget = new SingleSliderWidget(this); // For energy bin
    DoubleSliderWidget *doubleSliderWidget = new DoubleSliderWidget(this); // For energy range
    // Stacked layout for the slider widgets
    QStackedLayout *sliderLayout = new QStackedLayout();
    sliderLayout->addWidget(singleSliderWidget);
    sliderLayout->addWidget(doubleSliderWidget);

    connect(singleSliderWidget, &SingleSliderWidget::valueChanged, this, &Widget::handleSingleSliderValueChanged);
    connect(doubleSliderWidget, &DoubleSliderWidget::rangeChanged, this, &Widget::handleDoubleSliderRangeChanged);

    // Combine slider button and sliders
    QHBoxLayout *energySliderLayout = new QHBoxLayout();
    energySliderLayout->addWidget(sliderModeSelector);
    energySliderLayout->addLayout(sliderLayout);

    connect(sliderModeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), [sliderLayout](int index) {
        sliderLayout->setCurrentIndex(index); // Switch between slider layouts
    });

    // Horizontal layout for graphics view and layers list
    QHBoxLayout *canvasLayout = new QHBoxLayout();
    canvasLayout->addWidget(graphicsView);
    canvasLayout->addWidget(layersList);

    // Arrange Main Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(modelSelector);
    mainLayout->addWidget(generateButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(canvasLayout,5);
    mainLayout->addLayout(energySliderLayout,1);
    mainLayout->addWidget(saveButton);

    setLayout(mainLayout);
    setWindowTitle("Image");
    setMinimumSize(500, 500);

    // Connect Signals and Slots of main widgets
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
        case 2: // Circle
            circle(modelPath, IMAGE_WIDTH / 2, IMAGE_HEIGHT / 2, 50);
            break;
        case 3: // Reltrans (placeholder)
            qWarning() << "Reltrans model not implemented for vectorized graphics.";
            return;
        default:
            qWarning() << "Unknown model selected!";
            return;
    }
    qDebug() << "Model path generated.";
    QGraphicsPathItem *modelPathItem = new QGraphicsPathItem(modelPath);
    modelPathItem->setPen(QPen(Qt::blue, 2));
    modelPathItem->setBrush(QBrush(Qt::blue)); // Set the fill color
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

void Widget::handleSingleSliderValueChanged(int value) {
    qDebug() << "Slider value changed to:" << value;

    // Example: Adjust the opacity of the graphicsScene
    qreal opacity = value / 10.0; // Convert slider value to a range of 0.0 to 1.0
    for (auto item : graphicsScene->items()) {
        item->setOpacity(opacity); // Set the opacity of each item in the scene
    }
}

void Widget::handleDoubleSliderRangeChanged(double low, double high) {
    qDebug() << "Double slider range changed to:" << low << " - " << high;
    // Handle the double slider range (e.g., update a model or UI)
    qreal opacity = (high - low) / 10.0; // Example calculation
    for (auto item : graphicsScene->items()) {
        item->setOpacity(opacity); // Set the opacity of each item in the scene
    }
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

void Widget::circle(QPainterPath &path, int centerX, int centerY, int radius) {
    path.setFillRule(Qt::WindingFill);
    qDebug() << "Generating circle at (" << centerX << "," << centerY << ") with radius" << radius;
    qDebug() << "Fill rule:" << path.fillRule();
    path.addEllipse(QPointF(centerX, centerY), radius, radius);
}

void Widget::callReltrans(unsigned char* imageData, int width, int height) {
    // Calls reltrans.
    qDebug() << "Calling reltrans Fortran subroutine.";
    //reltrans(imageData, width, height);
}