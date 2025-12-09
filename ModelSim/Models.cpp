#include "Models.h"

#include <QPainterPath>
#include <QDebug>

#include <functional>
#include <cmath>

// Image generation functions
void generateSineWave(QPainterPath &path, int width, int height, double amplitude, double frequency) {
    path.setFillRule(Qt::OddEvenFill);
    path.moveTo(0, height / 2);
    for (int x = 0; x < width; ++x) {
        double y = height / 2 + amplitude * sin(x * frequency);
        path.lineTo(x, y);
    }
}

// Basic shapes
void circle(QPainterPath &path, double centerX, double centerY, double radius) {
    path.setFillRule(Qt::WindingFill);
    qDebug() << "Generating circle at (" << centerX << "," << centerY << ") with radius" << radius;
    qDebug() << "Fill rule:" << path.fillRule();
    path.addEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);
}

void rectangle(QPainterPath &path, double centerX, double centerY, double width, double height) {
    path.setFillRule(Qt::WindingFill);
    qDebug() << "Generating rectangle at (" << centerX << "," << centerY << ") with width" << width << " and height" << height;
    qDebug() << "Fill rule:" << path.fillRule();
    path.addRect(centerX - width / 2, centerY - height / 2, width, height);
}

// Disk shapes
void disk(QPainterPath &path, double centerX, double centerY, double innerRadius, double outerRadius, double inclination) {
    QPainterPath diskPath;
    diskPath.setFillRule(Qt::OddEvenFill);
    qDebug() << "Generating disk at (" << centerX << "," << centerY << ") with inner radius" << innerRadius << " and outer radius" << outerRadius;
    qDebug() << "Fill rule:" << diskPath.fillRule();

    double scaleY = std::cos(inclination * M_PI / 180.0);

    // Add outer ellipse
    QPainterPath outerEllipse;
    outerEllipse.addEllipse(centerX - outerRadius, centerY - (outerRadius * scaleY), outerRadius * 2, outerRadius * 2 * scaleY);
    diskPath.addPath(outerEllipse);

    // Subtract inner ellipse
    QPainterPath innerEllipse;
    innerEllipse.addEllipse(centerX - innerRadius, centerY - (innerRadius * scaleY), innerRadius * 2, innerRadius * 2 * scaleY);
    diskPath.addPath(innerEllipse);

    // Combine paths
    path.addPath(diskPath);
    qDebug() << "Disk path generated with inclination" << inclination << "degrees.";
}

void diskInnerShape(QPainterPath &path, double centerX, double centerY, 
                    double innerRadius, double outerRadius, double inclination,
                    QPainterPath &innerShapePath) {
    qDebug() << "Generating disk at (" << centerX << "," << centerY << ") with inner radius" << innerRadius
             << " and outer radius" << outerRadius << " at inclination" << inclination << " degrees";

    // Calculate the vertical scaling factor based on the inclination
    double scaleY = std::cos(inclination * M_PI / 180.0); // Convert inclination to radians
    qDebug() << "Vertical scaling factor:" << scaleY;

    QPainterPath totalModelPath;
    // Generate the back half of the disk (outer arc and inner arc)
    QPainterPath backPath;
    backPath.setFillRule(Qt::OddEvenFill);

    // Outer arc (back half)
    QRectF outerRect(centerX - outerRadius, centerY - outerRadius * scaleY, outerRadius * 2, outerRadius * 2 * scaleY);
    backPath.arcTo(outerRect, 180, 180); // Draw the back half of the outer arc

    // Inner arc (back half)
    QRectF innerRect(centerX - innerRadius, centerY - innerRadius * scaleY, innerRadius * 2, innerRadius * 2 * scaleY);
    backPath.arcTo(innerRect, 0, -180); // Draw the back half of the inner arc (reverse direction)

    backPath.closeSubpath(); // Close the back path
    totalModelPath.addPath(backPath);

    // Combine the back path with the inner shape
    innerShapePath.setFillRule(Qt::WindingFill);
    totalModelPath.addPath(innerShapePath);

    // Generate the front half of the disk (outer arc and inner arc)
    QPainterPath frontPath;
    frontPath.setFillRule(Qt::OddEvenFill);

    // Outer arc (front half)
    frontPath.arcTo(outerRect, 0, 180); // Draw the front half of the outer arc

    // Inner arc (front half)
    frontPath.arcTo(innerRect, 180, -180); // Draw the front half of the inner arc (reverse direction)

    frontPath.closeSubpath(); // Close the front path
    frontPath.setFillRule(Qt::OddEvenFill);

    // Combine the front and back paths into the main path
    totalModelPath.addPath(frontPath);
    path.addPath(totalModelPath);

    qDebug() << "Disk paths generated with inclination" << inclination << " degrees.";
}

// Complex external models

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