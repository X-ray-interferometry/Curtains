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