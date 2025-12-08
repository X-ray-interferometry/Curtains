#include "SpectrumDialog.h"
#include "Models.h"
#include <QVBoxLayout>
#include <QLabel>

SpectrumDialog::SpectrumDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Select Spectrum Function");

    spectrumSelector = new QComboBox(this);
    spectrumSelector->addItem("Power Law");
    spectrumSelector->addItem("Black Body");

    normalizationSpinBox = new QDoubleSpinBox(this);
    normalizationSpinBox->setRange(0.0, 100.0);
    normalizationSpinBox->setValue(1.0);

    slopeSpinBox = new QDoubleSpinBox(this);
    slopeSpinBox->setRange(0, 3.5);
    slopeSpinBox->setValue(2.0);

    temperatureSpinBox = new QDoubleSpinBox(this);
    temperatureSpinBox->setRange(0.1, 100.0);
    temperatureSpinBox->setValue(10.0);

    QFormLayout *formLayout = new QFormLayout();
    formLayout->addRow("Spectrum Type:", spectrumSelector);
    formLayout->addRow("Normalization:", normalizationSpinBox);
    formLayout->addRow("Slope (Power Law):", slopeSpinBox);
    formLayout->addRow("Temperature (Black Body):", temperatureSpinBox);

    QPushButton *okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &SpectrumDialog::accept);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(okButton);

    setLayout(mainLayout);

    // Update the UI based on the selected spectrum type
    connect(spectrumSelector, &QComboBox::currentIndexChanged, [this](int index) {
        slopeSpinBox->setEnabled(index == 0); // Enable slope for power law
        temperatureSpinBox->setEnabled(index == 1); // Enable temperature for black body
    });
}

std::function<double(double)> SpectrumDialog::getSelectedSpectrum() const {
    double normalization = normalizationSpinBox->value();

    if (spectrumSelector->currentIndex() == 0) { // Power Law
        double slope = slopeSpinBox->value();
        return powerLawSpectrum(normalization, slope);
    } else { // Black Body
        double temperature = temperatureSpinBox->value();
        return blackBodySpectrum(normalization, temperature);
    }
}