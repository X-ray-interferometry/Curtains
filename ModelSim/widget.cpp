#include "widget.h"
#include <QImage>
#include <QPixmap>
#include <QDebug> // For console output

Widget::Widget(QWidget *parent) : QWidget(parent) {
    // 1. Create Widgets
    imageLabel = new QLabel("No image generated yet.", this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setFixedSize(IMAGE_WIDTH, IMAGE_HEIGHT); // Ensure a fixed size for our image area
    imageLabel->setStyleSheet("border: 1px solid gray; background-color: lightgray;"); // Visual cue

    modelSelector = new QComboBox(this);
    modelSelector->addItem("Model A: Sine Wave");
    modelSelector->addItem("Model B: Checkerboard");
    modelSelector->addItem("Model C: Random Noise");

    generateButton = new QPushButton("Generate Image", this);

    // 2. Arrange Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(modelSelector);
    mainLayout->addWidget(generateButton);
    mainLayout->addWidget(imageLabel);
    mainLayout->addStretch(); // Pushes content to the top

    setLayout(mainLayout);
    setWindowTitle("Image");
    setMinimumSize(500, 500);

    // 3. Connect Signals and Slots
    connect(generateButton, &QPushButton::clicked, this, &Widget::updateImage);

    // Initial image generation
    updateImage();
}

Widget::~Widget() {
    // Widgets are children of Widget, so they are automatically deleted when Widget is destroyed.
    // No need for explicit `delete` calls here for the dynamically allocated widgets.
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
        default:
            qWarning() << "Unknown model selected!";
            break;
    }

    // Display the generated image
    imageLabel->setPixmap(QPixmap::fromImage(image));
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
            unsigned char color;
            if (((x / cellSize) % 2 == 0) == ((y / cellSize) % 2 == 0)) {
                color = 200; // Light color
            } else {
                color = 50;  // Dark color
            }
            imageData[y * width * 4 + x * 4 + 0] = color;
            imageData[y * width * 4 + x * 4 + 1] = color;
            imageData[y * width * 4 + x * 4 + 2] = color;
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