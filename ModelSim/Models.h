#ifndef IMAGEGENERATORS_H
#define IMAGEGENERATORS_H

#include <QPainterPath>
#include <QDebug>

// Image Functions
void generateSineWave(QPainterPath &path, int width, int height);
void blankImage(QPainterPath &path, int width, int height);
void circle(QPainterPath &path, int centerX, int centerY, int radius);
void callReltrans(unsigned char* imageData, int width, int height);

// Spectrum functions
std::function<double(double)> powerLaw(double normalization, double slope);
std::function<double(double)> bbodyrad(double normalization, double temperature);

#endif // IMAGEGENERATORS_H