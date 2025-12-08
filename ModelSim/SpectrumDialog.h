#ifndef SPECTRUMDIALOG_H
#define SPECTRUMDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <functional>

class SpectrumDialog : public QDialog {
    Q_OBJECT

public:
    explicit SpectrumDialog(QWidget *parent = nullptr);

    std::function<double(double)> getSelectedSpectrum() const;

private:
    QComboBox *spectrumSelector;
    QDoubleSpinBox *normalizationSpinBox;
    QDoubleSpinBox *slopeSpinBox; // For power law
    QDoubleSpinBox *temperatureSpinBox; // For black body

    std::function<double(double)> selectedSpectrum;
};

#endif // SPECTRUMDIALOG_H