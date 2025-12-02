#include "imagelabel.h"

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent) {}

void ImageLabel::setSelectionRect(const QRect &rect) {
    selectionRect = rect;
    isSelecting = true;
    update(); // Trigger a repaint
}

void ImageLabel::clearSelectionRect() {
    selectionRect = QRect();
    isSelecting = false;
    update(); // Trigger a repaint
}

QRect ImageLabel::getSelectionRect() const {
    return selectionRect; // Return the current selection rectangle
}

void ImageLabel::paintEvent(QPaintEvent *event) {
    QLabel::paintEvent(event); // Call the base class paintEvent

    if (isSelecting && !selectionRect.isNull()) {
        QPainter painter(this);

        // Ensure the painter is active before setting properties
        if (!painter.isActive()) {
            qWarning() << "Painter not active!";
            return;
        }

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