#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QImage>

class RenderWindow : public QWidget {
    Q_OBJECT

public:
    explicit RenderWindow(QWidget *parent = nullptr);

    void setImage(const QImage &image); // Set the image to display

private slots:
    void saveImage(); // Save the displayed image

private:
    QLabel *imageLabel; // Label to display the image
    QImage currentImage; // The currently displayed image
};

#endif // RENDERWINDOW_H