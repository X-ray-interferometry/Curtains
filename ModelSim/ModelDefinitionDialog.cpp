#include "ModelDefinitionDialog.h"

ModelDefinitionDialog::ModelDefinitionDialog(const QString &modelType, QWidget *parent)
    : QDialog(parent), modelType(modelType) {
    setWindowTitle("Define " + modelType + " Parameters");
    formLayout = new QFormLayout(this);

    setupFields();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    formLayout->addWidget(buttonBox);
    setLayout(formLayout);
}

void ModelDefinitionDialog::setupFields() {
    if (modelType == "Sine Wave") {
        QDoubleSpinBox *amplitudeField = new QDoubleSpinBox(this);
        amplitudeField->setRange(0.1, 100.0);
        amplitudeField->setValue(1.0);
        formLayout->addRow("Amplitude:", amplitudeField);
        inputFields["Amplitude"] = amplitudeField;

        QDoubleSpinBox *frequencyField = new QDoubleSpinBox(this);
        frequencyField->setRange(0.1, 100.0);
        frequencyField->setValue(1.0);
        formLayout->addRow("Frequency:", frequencyField);
        inputFields["Frequency"] = frequencyField;
    } else if (modelType == "Circle") {
        QDoubleSpinBox *radiusField = new QDoubleSpinBox(this);
        radiusField->setRange(1.0, 500.0);
        radiusField->setValue(50.0);
        formLayout->addRow("Radius:", radiusField);
        inputFields["Radius"] = radiusField;
        QDoubleSpinBox *centerXField = new QDoubleSpinBox(this);
        centerXField->setRange(0.0, 400.0);
        centerXField->setValue(200.0);
        formLayout->addRow("Center X:", centerXField);
        inputFields["CenterX"] = centerXField;
        QDoubleSpinBox *centerYField = new QDoubleSpinBox(this);
        centerYField->setRange(0.0, 400.0);
        centerYField->setValue(200.0);
        formLayout->addRow("Center Y:", centerYField);
        inputFields["CenterY"] = centerYField;
    }
    // Add more fields for other models as needed
}

QMap<QString, QVariant> ModelDefinitionDialog::getParameters() const {
    QMap<QString, QVariant> parameters;
    qDebug() << "Retrieving parameters from ModelDefinitionDialog.";
    qDebug() << "Input fields:" << inputFields.keys();
    for (auto it = inputFields.begin(); it != inputFields.end(); ++it) {
        if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(it.value())) {
            parameters[it.key()] = spinBox->value();
        }
    }
    return parameters;
}