#ifndef IMAGEGENERATORS_H
#define IMAGEGENERATORS_H

#include <QPainterPath>
#include <QDebug>

// Image Functions
void generateSineWave(QPainterPath &path, int width, int height, double amplitude, double frequency);
void circle(QPainterPath &path, double centerX, double centerY, double radius);
void rectangle(QPainterPath &path, double centerX, double centerY, double width, double height);
void disk(QPainterPath &path, double centerX, double centerY, double innerRadius, double outerRadius, double inclination);
void diskInnerShape(QPainterPath &path, double centerX, double centerY, 
                    double innerRadius, double outerRadius, double inclination,
                    QPainterPath &innerShapePath);
void callReltrans(unsigned char* imageData, int width, int height);

// Spectrum functions
std::function<double(double)> powerLaw(double normalization, double slope);
std::function<double(double)> bbodyrad(double normalization, double temperature);
std::optional<std::tuple<int, int, int>> getCircleParameters(int IMAGE_WIDTH, int IMAGE_HEIGHT);

#endif // IMAGEGENERATORS_H