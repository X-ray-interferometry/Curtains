#include "SpectrumDialog.h"
#include "../Models.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QStackedWidget>

SpectrumDialog::SpectrumDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Select Spectrum Function");

    spectrumSelector = new QComboBox(this);
    spectrumSelector->addItem("Power Law");
    spectrumSelector->addItem("Black Body (Bbodyrad)");
    spectrumSelector->setCurrentIndex(0);

    // Create the Power Law form
    QWidget *powerLawForm = new QWidget(this);
    QFormLayout *powerLawLayout = new QFormLayout(powerLawForm);

    slopeNormalizationSpinBox = new QDoubleSpinBox(this);
    slopeNormalizationSpinBox->setRange(0.0, 100.0);
    slopeNormalizationSpinBox->setValue(1.0);
    slopeNormalizationSpinBox->setSingleStep(0.1);

    slopeSpinBox = new QDoubleSpinBox(this);
    slopeSpinBox->setRange(0, 3.5);
    slopeSpinBox->setValue(2.0);
    slopeSpinBox->setSingleStep(0.1);

    powerLawLayout->addRow("Normalization:", slopeNormalizationSpinBox);
    powerLawLayout->addRow("Slope:", slopeSpinBox);

    // Create the Black Body form
    QWidget *blackBodyForm = new QWidget(this);
    QFormLayout *blackBodyLayout = new QFormLayout(blackBodyForm);

    bbodyradNormalizationSpinBox = new QDoubleSpinBox(this);
    bbodyradNormalizationSpinBox->setRange(0.0, 1e6);
    bbodyradNormalizationSpinBox->setValue(1.0);
    bbodyradNormalizationSpinBox->setSingleStep(0.1);

    temperatureSpinBox = new QDoubleSpinBox(this);
    temperatureSpinBox->setRange(0.1, 100.0);
    temperatureSpinBox->setValue(1.0);
    temperatureSpinBox->setSingleStep(0.1);

    blackBodyLayout->addRow("Normalization:", bbodyradNormalizationSpinBox);
    blackBodyLayout->addRow("Temperature:", temperatureSpinBox);

    // Create the stacked widget
    QStackedWidget *stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(powerLawForm); // Index 0
    stackedWidget->addWidget(blackBodyForm); // Index 1

    // Connect the spectrum selector to the stacked widget
    connect(spectrumSelector, &QComboBox::currentIndexChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(new QLabel("Spectrum Type:", this));
    mainLayout->addWidget(spectrumSelector);
    mainLayout->addWidget(stackedWidget);

    QPushButton *okButton = new QPushButton("OK", this);
    connect(okButton, &QPushButton::clicked, this, &SpectrumDialog::accept);
    mainLayout->addWidget(okButton);

    setLayout(mainLayout);

    // Update the UI based on the selected spectrum type
    connect(spectrumSelector, &QComboBox::currentIndexChanged, [this](int index) {
        slopeSpinBox->setEnabled(index == 0); // Enable slope for power law
        temperatureSpinBox->setEnabled(index == 1); // Enable temperature for black body
    });
}

std::function<double(double)> SpectrumDialog::getSelectedSpectrum() const {
    if (spectrumSelector->currentIndex() == 0) { // Power Law
        double normalization = slopeNormalizationSpinBox->value();
        double slope = slopeSpinBox->value();
        return powerLaw(normalization, slope);
    } else { // Black Body
        double normalization = bbodyradNormalizationSpinBox->value();
        double temperature = temperatureSpinBox->value();
        return bbodyrad(normalization, temperature);
    }
}