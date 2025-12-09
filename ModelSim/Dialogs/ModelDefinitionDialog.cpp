#include "ModelDefinitionDialog.h"

#include <Qlabel>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>

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
    } else if (modelType == "Disk") {
        QDoubleSpinBox *innerRadiusField = new QDoubleSpinBox(this);
        innerRadiusField->setRange(0.0, 500.0);
        innerRadiusField->setValue(20.0);
        formLayout->addRow("Inner Radius:", innerRadiusField);
        inputFields["InnerRadius"] = innerRadiusField;

        QDoubleSpinBox *outerRadiusField = new QDoubleSpinBox(this);
        outerRadiusField->setRange(1.0, 500.0);
        outerRadiusField->setValue(50.0);
        formLayout->addRow("Outer Radius:", outerRadiusField);
        inputFields["OuterRadius"] = outerRadiusField;

        QDoubleSpinBox *inclinationField = new QDoubleSpinBox(this);
        inclinationField->setRange(0.0, 90.0);
        inclinationField->setValue(60.0);
        formLayout->addRow("Inclination:", inclinationField);
        inputFields["Inclination"] = inclinationField;

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
    } else if (modelType == "Disk with inner shape") {
        QDoubleSpinBox *innerRadiusField = new QDoubleSpinBox(this);
        innerRadiusField->setRange(0.0, 500.0);
        innerRadiusField->setValue(20.0);
        formLayout->addRow("Inner Radius:", innerRadiusField);
        inputFields["InnerRadius"] = innerRadiusField;

        QDoubleSpinBox *outerRadiusField = new QDoubleSpinBox(this);
        outerRadiusField->setRange(1.0, 500.0);
        outerRadiusField->setValue(50.0);
        formLayout->addRow("Outer Radius:", outerRadiusField);
        inputFields["OuterRadius"] = outerRadiusField;

        QDoubleSpinBox *inclinationField = new QDoubleSpinBox(this);
        inclinationField->setRange(0.0, 90.0);
        inclinationField->setValue(60.0);
        formLayout->addRow("Inclination:", inclinationField);
        inputFields["Inclination"] = inclinationField;

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

        QPushButton *innerShapeButton = new QPushButton("Configure Inner Shape", this);
        formLayout->addRow(innerShapeButton);

        connect(innerShapeButton, &QPushButton::clicked, this, [this]() {
            InnerShapeDialog innerShapeDialog(this);
            if (innerShapeDialog.exec() == QDialog::Accepted) {
                QMap<QString, QVariant> innerShapeParams = innerShapeDialog.getParameters();
                QString selectedShape = innerShapeDialog.getSelectedShape();
                inputFields["InnerShape"] = new QLabel(selectedShape, this); // Store the shape type
                QString innerShapeParamsString;
                for (auto it = innerShapeParams.begin(); it != innerShapeParams.end(); ++it) {
                    innerShapeParamsString += it.key() + ": " + it.value().toString() + "\n";
                }
                inputFields["InnerShapeParams"] = new QLabel(innerShapeParamsString, this); // Store the parameters
                qDebug() << "Inner shape configured:" << selectedShape << innerShapeParams;
            }
        });
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

InnerShapeDialog::InnerShapeDialog(QWidget *parent)
    : QDialog(parent) {
    setWindowTitle("Select Inner Shape");

    formLayout = new QFormLayout(this);

    // Shape selector
    shapeSelector = new QComboBox(this);
    shapeSelector->addItems({"Circle", "Rectangle"});
    formLayout->addRow("Shape:", shapeSelector);

    connect(shapeSelector, &QComboBox::currentTextChanged, this, &InnerShapeDialog::updateFieldsForShape);

    // Add initial fields for the default shape
    setupFields();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    formLayout->addWidget(buttonBox);
    setLayout(formLayout);
}

void InnerShapeDialog::setupFields() {
    updateFieldsForShape(shapeSelector->currentText());
}

void InnerShapeDialog::updateFieldsForShape(const QString &shape) {
    // Clear existing fields
    for (auto field : inputFields) {
        formLayout->removeWidget(field);
        delete field;
    }
    inputFields.clear();

    // Add fields based on the selected shape
    if (shape == "Circle") {
        QDoubleSpinBox *radiusField = new QDoubleSpinBox(this);
        radiusField->setRange(1.0, 500.0);
        radiusField->setValue(50.0);
        formLayout->addRow("Radius:", radiusField);
        inputFields["Radius"] = radiusField;
    } else if (shape == "Rectangle") {
        QDoubleSpinBox *widthField = new QDoubleSpinBox(this);
        widthField->setRange(1.0, 500.0);
        widthField->setValue(100.0);
        formLayout->addRow("Width:", widthField);
        inputFields["Width"] = widthField;

        QDoubleSpinBox *heightField = new QDoubleSpinBox(this);
        heightField->setRange(1.0, 500.0);
        heightField->setValue(50.0);
        formLayout->addRow("Height:", heightField);
        inputFields["Height"] = heightField;
    }
}

QMap<QString, QVariant> InnerShapeDialog::getParameters() const {
    QMap<QString, QVariant> parameters;
    for (auto it = inputFields.begin(); it != inputFields.end(); ++it) {
        if (QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox*>(it.value())) {
            parameters[it.key()] = spinBox->value();
        }
    }
    return parameters;
}

QString InnerShapeDialog::getSelectedShape() const {
    return shapeSelector->currentText();
}