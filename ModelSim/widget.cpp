#include "widget.h"
#include <QImage>
#include <QPixmap>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug> // For console output

Widget::Widget(QWidget *parent) : QWidget(parent) {
    // 1. Create Widgets
    imageLabel = new ImageLabel(this);
    imageLabel->setText("No image generated yet.");
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("border: 1px solid gray; background-color: lightgray;"); // Visual cue
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); // Allow label to expand

    modelSelector = new QComboBox(this);
    modelSelector->addItem("Model A: Sine Wave");
    modelSelector->addItem("Model B: Checkerboard");
    modelSelector->addItem("Model C: Random Noise");

    generateButton = new QPushButton("Generate Image", this);

    QPushButton *zoomInButton = new QPushButton("Zoom In", this);
    QPushButton *zoomOutButton = new QPushButton("Zoom Out", this);
    QPushButton *resetViewButton = new QPushButton("Reset View", this);

    QPushButton *zoomModeButton = new QPushButton("Zoom Mode", this);
    QPushButton *dragModeButton = new QPushButton("Drag Mode", this);

    // Connect mode buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(zoomModeButton);
    buttonLayout->addWidget(dragModeButton);
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addWidget(resetViewButton);

    // 2. Arrange Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(modelSelector);
    mainLayout->addWidget(generateButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(imageLabel);

    setLayout(mainLayout);
    setWindowTitle("Image");
    setMinimumSize(500, 500);

    // 3. Connect Signals and Slots
    connect(generateButton, &QPushButton::clicked, this, &Widget::updateImage);
    connect(zoomInButton, &QPushButton::clicked, this, &Widget::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &Widget::zoomOut);
    connect(zoomModeButton, &QPushButton::clicked, this, &Widget::setZoomMode);
    connect(dragModeButton, &QPushButton::clicked, this, &Widget::setDragMode);
    connect(resetViewButton, &QPushButton::clicked, this, &Widget::resetView);

    // Initial image generation
    updateImage();
}

Widget::~Widget() {
    // Widgets are children of Widget, so they are automatically deleted when Widget is destroyed.
    // No need for explicit `delete` calls here for the dynamically allocated widgets.
}

void Widget::resetView() {
    if (!originalPixmap.isNull()) {
        // Reset the viewing window to the full image
        viewingWindow = QRect(0, 0, originalPixmap.width(), originalPixmap.height());

        // Display the full image
        imageLabel->setPixmap(originalPixmap);

        // Clear the selection rectangle
        imageLabel->clearSelectionRect();

        qDebug() << "View reset to full image.";
        qDebug() << "New zoomed rectangle after reset:" << zoomedRect;
    }
}

void Widget::setZoomMode() {
    currentMode = ZoomMode;
    qDebug() << "Switched to Zoom Mode";
}

void Widget::setDragMode() {
    currentMode = DragMode;
    qDebug() << "Switched to Drag Mode";
}

void Widget::updateImage() {
    qDebug() << "Updating image with model:" << modelSelector->currentText();

    // Create a QImage to draw into. We'll use RGB32 for simplicity.
    // QImage data is usually row-major, 4 bytes per pixel (BGRA or RGBA depending on system endianness).
    // For direct pixel manipulation and passing to Fortran, it's often easier to work with raw bytes.
    QImage image(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB32);
    image.fill(Qt::black); // Start with a black background

    // Get a pointer to the raw pixel data
    unsigned char *imageData = image.bits();

    // Call the appropriate Fortran placeholder function based on selection
    int selectedIndex = modelSelector->currentIndex();
    switch (selectedIndex) {
        case 0: // Model A
            callFortranModelA(imageData, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        case 1: // Model B
            callFortranModelB(imageData, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        case 2: // Model C
            callFortranModelC(imageData, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        case 3: // reltrans
            callReltrans(imageData, IMAGE_WIDTH, IMAGE_HEIGHT);
            break;
        default:
            qWarning() << "Unknown model selected!";
            break;
    }
    // Scale the image to fit the QLabel while maintaining aspect ratio
    originalPixmap = QPixmap::fromImage(image);
    // Initialize the viewing window to the full image
    viewingWindow = QRect(0, 0, originalPixmap.width(), originalPixmap.height());

    // Display the full image
    QPixmap scaledPixmap = originalPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(scaledPixmap);
    displayedPixmap = scaledPixmap;
}

void Widget::resizeEvent(QResizeEvent *event) {

    // Check if the QLabel has a valid pixmap
    if (!displayedPixmap.isNull()) {
        // Rescale the pixmap to fit the QLabel while maintaining aspect ratio
        QPixmap scaledPixmap = displayedPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
    }
}

void Widget::zoomIn() {
    zoomFactor = 1.2; // Increase zoom factor by 20%
    adjustViewToZoom();
}

void Widget::zoomOut() {
    zoomFactor = 0.8; // Decrease zoom factor by 20%
    adjustViewToZoom();
}

void Widget::adjustViewToZoom() {
    if (!originalPixmap.isNull()) {
        // Scale the viewing window based on the zoom factor
        qDebug() << "Adjusting view to zoom factor:" << zoomFactor;
        qDebug() << "Current viewing window:" << viewingWindow;
        int newWidth = static_cast<int>(viewingWindow.width() / zoomFactor);
        int newHeight = static_cast<int>(viewingWindow.height() / zoomFactor);
        int newX = viewingWindow.center().x();
        int newY = viewingWindow.center().y();

        QRect zoomedRect(newX, newY, newWidth, newHeight);
        qDebug() << "New zoomed rectangle before bounds check:" << zoomedRect;

        zoomedRect = zoomedRect.intersected(QRect(0, 0, originalPixmap.width(), originalPixmap.height()));

        qDebug() << "New zoomed rectangle after bounds check:" << zoomedRect;
        // Update the viewing window
        viewingWindow = zoomedRect;

        // Crop the displayed pixmap to the zoomed rectangle
        QPixmap croppedPixmap = originalPixmap.copy(viewingWindow);

        // Scale the cropped pixmap to fit the QLabel
        QPixmap scaledPixmap = croppedPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
        displayedPixmap = scaledPixmap;
    }
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

void Widget::mousePressEvent(QMouseEvent *event) {
    if (currentMode == DragMode && event->button() == Qt::LeftButton) {
        dragStartPos = event->pos();
        isDragging = true;
    } else if (currentMode == ZoomMode && event->button() == Qt::LeftButton) {
        QPoint relativePos = event->pos() - imageLabel->pos(); // Map to imageLabel coordinates
        QRect rect(relativePos, relativePos); // Initialize the rectangle
        imageLabel->setSelectionRect(rect); // Pass the rectangle to ImageLabel
        isSelecting = true;
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
    if (currentMode == DragMode && isDragging) {
        QPoint dragDelta = event->pos() - dragStartPos;
        dragStartPos = event->pos(); // Update the drag start position
        currentOffset += dragDelta; // Update the current offset

        // Adjust the visible portion of the image based on the drag offset
        QRect visibleRect = imageLabel->rect().translated(currentOffset);
        double xScale = static_cast<double>(originalPixmap.width()) / imageLabel->width();
        double yScale = static_cast<double>(originalPixmap.height()) / imageLabel->height();
        QRect scaledRect(
            static_cast<int>(visibleRect.x() * xScale),
            static_cast<int>(visibleRect.y() * yScale),
            static_cast<int>(visibleRect.width() * xScale),
            static_cast<int>(visibleRect.height() * yScale)
        );

        QPixmap croppedPixmap = originalPixmap.copy(scaledRect);
        QPixmap scaledPixmap = croppedPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
        displayedPixmap = scaledPixmap;
    } else if (currentMode == ZoomMode && isSelecting) {
        QPoint relativePos = event->pos() - imageLabel->pos(); // Map to imageLabel coordinates
        QRect rect(imageLabel->getSelectionRect().topLeft(), relativePos); // Update the rectangle
        imageLabel->setSelectionRect(rect); // Pass the updated rectangle to ImageLabel
    }
}

void Widget::mouseReleaseEvent(QMouseEvent *event) {
    if (currentMode == DragMode && event->button() == Qt::LeftButton) {
        isDragging = false;
    } else if (currentMode == ZoomMode && event->button() == Qt::LeftButton) {
        isSelecting = false;
        zoomToSelection();
        imageLabel->clearSelectionRect(); // Clear the selection rectangle
    }
}

void Widget::zoomToSelection() {
    QRect selectionRect = imageLabel->getSelectionRect(); // Get the rectangle from ImageLabel

    if (!displayedPixmap.isNull() && !selectionRect.isNull()) {
        // Map the selection rectangle to the image coordinates
        QRect imageRect = selectionRect.intersected(imageLabel->rect());
        double xScale = static_cast<double>(viewingWindow.width()) / imageLabel->width();
        double yScale = static_cast<double>(viewingWindow.height()) / imageLabel->height();
        QRect scaledRect(
            viewingWindow.x() + static_cast<int>(selectionRect.x() * xScale),
            viewingWindow.y() + static_cast<int>(selectionRect.y() * yScale),
            static_cast<int>(selectionRect.width() * xScale),
            static_cast<int>(selectionRect.height() * yScale)
        );

        // Update the viewing window to the selected area
        viewingWindow = scaledRect;

        // Crop the original pixmap to the selected area
        QPixmap croppedPixmap = displayedPixmap.copy(viewingWindow);

        // Scale the cropped pixmap to fit the QLabel
        QPixmap scaledPixmap = croppedPixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
        displayedPixmap = scaledPixmap;

        // Clear the selection rectangle
        imageLabel->clearSelectionRect();
    }
}

void Widget::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);

    if (isSelecting && !selectionRect.isNull()) {
        QPainter painter(imageLabel); // Draw directly on the imageLabel

        // Create a QPen with the desired properties
        QPen pen(Qt::DashLine); // Set the pen style
        pen.setWidth(2);        // Set the pen width
        pen.setCapStyle(Qt::RoundCap); // Set the cap style
        pen.setJoinStyle(Qt::RoundJoin); // Set the join style

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush); // No fill for the rectangle
        painter.drawRect(selectionRect);
    }
}

// --- Placeholder Fortran "Subroutines" (C++ implementations for demonstration) ---
// In a real application, these would be `extern "C"` functions linking to your
// compiled Fortran code. The Fortran side would fill the `imageData` array.

void Widget::callFortranModelA(unsigned char* imageData, int width, int height) {
    // Simulates Fortran generating a simple sine wave pattern
    qDebug() << "Fortran Model A (Sine Wave) called.";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char value = static_cast<unsigned char>(127.5 * (1.0 + sin(x * 0.1 + y * 0.05)));
            // QImage::Format_RGB32 is typically ARGB or BGRA depending on system.
            // Let's assume BGRA for simplicity, common on Windows.
            // Fortran would calculate R, G, B bytes and write them to the array.
            imageData[y * width * 4 + x * 4 + 0] = value;       // Blue
            imageData[y * width * 4 + x * 4 + 1] = 255 - value; // Green
            imageData[y * width * 4 + x * 4 + 2] = 128;         // Red
            imageData[y * width * 4 + x * 4 + 3] = 255;         // Alpha (fully opaque)
        }
    }
}

void Widget::callFortranModelB(unsigned char* imageData, int width, int height) {
    // Simulates Fortran generating a checkerboard pattern
    qDebug() << "Fortran Model B (Checkerboard) called.";
    int cellSize = 50;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char r, g, b;
            if (((x / cellSize) % 2 == 0) == ((y / cellSize) % 2 == 0)) {
                r = 200;
                g = 200;
                b = 200;
            } else {
                // Dark color iterates through a color grid
                r = static_cast<unsigned char>((x / cellSize) * 50 % 256); // Red varies by column
                g = static_cast<unsigned char>((y / cellSize) * 50 % 256); // Green varies by row
                b = static_cast<unsigned char>((x / cellSize + y / cellSize) * 25 % 256); // Blue varies by both
            }
            imageData[y * width * 4 + x * 4 + 0] = r;
            imageData[y * width * 4 + x * 4 + 1] = g;
            imageData[y * width * 4 + x * 4 + 2] = b;
            imageData[y * width * 4 + x * 4 + 3] = 255;
        }
    }
}

void Widget::callFortranModelC(unsigned char* imageData, int width, int height) {
    // Simulates Fortran generating random noise
    qDebug() << "Fortran Model C (Random Noise) called.";
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char r = rand() % 256;
            unsigned char g = rand() % 256;
            unsigned char b = rand() % 256;
            imageData[y * width * 4 + x * 4 + 0] = b;
            imageData[y * width * 4 + x * 4 + 1] = g;
            imageData[y * width * 4 + x * 4 + 2] = r;
            imageData[y * width * 4 + x * 4 + 3] = 255;
        }
    }
}

void Widget::generateVectorizedSineWave(const QString &filePath, int width, int height) {
    QSvgGenerator svgGenerator;
    svgGenerator.setFileName(filePath);
    svgGenerator.setSize(QSize(width, height));
    svgGenerator.setViewBox(QRect(0, 0, width, height));
    svgGenerator.setTitle("Sine Wave");
    svgGenerator.setDescription("A vectorized sine wave pattern.");

    QPainter painter(&svgGenerator);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.moveTo(0, height / 2);
    for (int x = 0; x < width; ++x) {
        double y = height / 2 + 50 * sin(x * 0.1); // Sine wave formula
        path.lineTo(x, y);
    }

    painter.setPen(QPen(Qt::blue, 2));
    painter.drawPath(path);
}

void Widget::callReltrans(unsigned char* imageData, int width, int height) {
    // Calls reltrans.
    qDebug() << "Calling reltrans Fortran subroutine.";
    reltrans_(imageData, width, height);
}