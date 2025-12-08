#include "Models.h"

#include <QPainterPath>
#include <QDebug>

#include <functional>
#include <cmath>

// Image generation functions
void generateSineWave(QPainterPath &path, int width, int height) {
    path.setFillRule(Qt::OddEvenFill);
    path.moveTo(0, height / 2);
    for (int x = 0; x < width; ++x) {
        double y = height / 2 + 50 * sin(x * 0.1); // Sine wave formula
        path.lineTo(x, y);
    }
}

void blankImage(QPainterPath &path, int width, int height) {
    // Generates a blank image (no drawing)
    qDebug() << "Generating blank image.";
}

void circle(QPainterPath &path, int centerX, int centerY, int radius) {
    path.setFillRule(Qt::WindingFill);
    qDebug() << "Generating circle at (" << centerX << "," << centerY << ") with radius" << radius;
    qDebug() << "Fill rule:" << path.fillRule();
    path.addEllipse(QPointF(centerX, centerY), radius, radius);
}

void callReltrans(unsigned char* imageData, int width, int height) {
    // Calls reltrans.
    qDebug() << "Calling reltrans Fortran subroutine.";
    //reltrans(imageData, width, height);
}

// Spectrum functions

// Power law spectrum: brightness = normalization * energy^(-slope)
std::function<double(double)> powerLaw(double normalization, double slope) {
    return [normalization, slope](double energy) {
        return normalization * std::pow(energy, -slope);
    };
}

// Black body spectrum: brightness = normalization * (energy^3) / (exp(energy / temperature) - 1)
std::function<double(double)> bbodyrad(double normalization, double temperature) {
    return [normalization, temperature](double energy) {
        if (energy <= 0.0) return 0.0; // Avoid division by zero
        return normalization * 1.0344e-3 * (std::pow(energy, 2) * 0.1 / (std::exp(energy / temperature) - 1));
    };
}