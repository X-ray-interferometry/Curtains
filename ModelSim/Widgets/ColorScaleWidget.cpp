#include "ColorScaleWidget.h"

ColorScaleWidget::ColorScaleWidget(QWidget *parent)
    : QWidget(parent) {
    setFixedHeight(40); // Set a fixed height for the color scale
}

void ColorScaleWidget::setBrightnessRange(double minBrightness, double maxBrightness) {
    this->minBrightness = minBrightness;
    this->maxBrightness = maxBrightness;

    update(); // Trigger a repaint
}

QColor ColorScaleWidget::mapBrightnessToColor(const QColor &baseColor, double brightness) const {
    // Clamp brightness to the range [0.0, 1.0]
    double clampedBrightness = qBound(0.0, brightness, 1.0);

    // Create a copy of the base color and adjust its brightness
    QColor adjustedColor = baseColor;
    adjustedColor.setHsvF(baseColor.hueF(), baseColor.saturationF(), clampedBrightness);

    return adjustedColor;
}

void ColorScaleWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    // Define the gradient
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0.0, mapBrightnessToColor(baseColor, minBrightness)); // Low brightness color
    gradient.setColorAt(1.0, mapBrightnessToColor(baseColor, maxBrightness)); // High brightness color

    // Draw the gradient bar
    QRect gradientRect(10, 10, width() - 20, 20);
    painter.fillRect(gradientRect, gradient);

    // Draw labels
    painter.setPen(Qt::black);
}