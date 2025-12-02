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
    modelSelector->addItem("Sine Wave");
    modelSelector->addItem("Reltrans");

    generateButton = new QPushButton("Generate Image", this);

    QPushButton *zoomInButton = new QPushButton("Zoom In", this);
    QPushButton *zoomOutButton = new QPushButton("Zoom Out", this);
    QPushButton *resetViewButton = new QPushButton("Reset View", this);
    QPushButton *saveButton = new QPushButton("Save Image", this);

    // Connect mode buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addWidget(resetViewButton);

    QHBoxLayout *canvasLayout = new QHBoxLayout();
    canvasLayout->addWidget(graphicsView);
    canvasLayout->addWidget(layersList);

    // 2. Arrange Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(modelSelector);
    mainLayout->addWidget(generateButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addLayout(canvasLayout);
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
        case 0: // Sine Wave
            generateSineWave(modelPath, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        case 1: // Reltrans (placeholder)
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
        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
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

// Image generators

void Widget::generateSineWave(QPainterPath &path, int width, int height) {
    path.moveTo(0, height / 2);
    for (int x = 0; x < width; ++x) {
        double y = height / 2 + 50 * sin(x * 0.1); // Sine wave formula
        path.lineTo(x, y);
    }
}

void Widget::callReltrans(unsigned char* imageData, int width, int height) {
    // Calls reltrans.
    qDebug() << "Calling reltrans Fortran subroutine.";
    //reltrans(imageData, width, height);
}