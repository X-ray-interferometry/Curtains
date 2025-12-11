#ifndef COLORSCALEWIDGET_H
#define COLORSCALEWIDGET_H

#include <QWidget>
#include <QPainter>

class ColorScaleWidget : public QWidget {
    Q_OBJECT

public:
    explicit ColorScaleWidget(QWidget *parent = nullptr);

    void setBrightnessRange(double minBrightness, double maxBrightness);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QColor mapBrightnessToColor(const QColor &baseColor, double brightness) const;

    QColor baseColor = Qt::white; // Base color for the scale
    double minBrightness = 0.0; // Minimum brightness
    double maxBrightness = 1.0; // Maximum brightness
};

#endif // COLORSCALEWIDGET_H