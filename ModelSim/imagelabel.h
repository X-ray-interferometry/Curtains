#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QRect>
#include <QPainter>

class ImageLabel : public QLabel {
    Q_OBJECT

public:
    explicit ImageLabel(QWidget *parent = nullptr);

    void setSelectionRect(const QRect &rect); // Set the selection rectangle
    void clearSelectionRect();                // Clear the selection rectangle
    QRect getSelectionRect() const;           // Public getter for selectionRect

protected:
    void paintEvent(QPaintEvent *event) override; // Override paintEvent to draw the rectangle

private:
    QRect selectionRect; // Stores the selection rectangle
    bool isSelecting = false; // Tracks whether the user is selecting
};

#endif // IMAGELABEL_H