#include "RenderWindow.h"
#include <QFileDialog>
#include <QPixmap>

RenderWindow::RenderWindow(QWidget *parent) : QWidget(parent) {
    setWindowTitle("Rendered Image");

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);

    QPushButton *saveButton = new QPushButton("Save Image", this);
    connect(saveButton, &QPushButton::clicked, this, &RenderWindow::saveImage);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(imageLabel);
    layout->addWidget(saveButton);

    setLayout(layout);
}

void RenderWindow::setImage(const QImage &image) {
    currentImage = image;
    imageLabel->setPixmap(QPixmap::fromImage(image));
}

void RenderWindow::saveImage() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;JPEG Files (*.jpg);;All Files (*)");
    if (!filePath.isEmpty()) {
        if (!currentImage.save(filePath)) {
            qWarning() << "Failed to save image to:" << filePath;
        } else {
            qDebug() << "Image successfully saved to:" << filePath;
        }
    }
}