#ifndef MODELDEFINITIONDIALOG_H
#define MODELDEFINITIONDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QComboBox>

class ModelDefinitionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ModelDefinitionDialog(const QString &modelType, QWidget *parent = nullptr);
    
    QMap<QString, QVariant> getParameters() const;

private:
    QFormLayout *formLayout;
    QMap<QString, QWidget*> inputFields;
    QString modelType;

    void setupFields();
};

#endif // MODELDEFINITIONDIALOG_H

#ifndef INNERSHAPEDIALOG_H
#define INNERSHAPEDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QMap>
#include <QVariant>

class InnerShapeDialog : public QDialog {
    Q_OBJECT

public:
    explicit InnerShapeDialog(QWidget *parent = nullptr);

    QMap<QString, QVariant> getParameters() const;
    QString getSelectedShape() const;

private slots:
    void updateFieldsForShape(const QString &shape);

private:
    QFormLayout *formLayout;
    QComboBox *shapeSelector;
    QMap<QString, QWidget*> inputFields;

    void setupFields();
};

#endif // INNERSHAPEDIALOG_H