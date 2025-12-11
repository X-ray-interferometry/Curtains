#include "Widget.h"
#include "Models.h"
#include "Dialogs/SpectrumDialog.h"
#include "Dialogs/ModelDefinitionDialog.h"
#include "RenderWindow.h"
#include "Widgets/ColorScaleWidget.h"

#include <QImage>
#include <QPixmap>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QDebug> 
#include <QFileDialog>
#include <QDoubleSpinBox>
#include <QStackedLayout>
#include <QList>
#include <QMenu>

#include <functional>
#include <ranges>
#include <vector>

SingleSliderWidget::SingleSliderWidget(QWidget *parent) : QWidget(parent) {
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(5, 100);
    slider->setValue(50);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksBelow);

    sliderValueBox = new QDoubleSpinBox(this);
    sliderValueBox->setRange(0.5, 10.0);
    sliderValueBox->setValue(5.0);
    sliderValueBox->setSingleStep(0.1);

    // Connect slider and spin box
    connect(slider, &QSlider::valueChanged, [this](int value) {
        qDebug() << "Slider changed to:" << value*0.1;
        double scaledValue = value * 0.1;
        sliderValueBox->blockSignals(true);
        sliderValueBox->setValue(scaledValue); // Update the spin box
        sliderValueBox->blockSignals(false);

        emit valueChanged(scaledValue);
    });
    connect(sliderValueBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        qDebug() << "Spin box changed to:" << value;
        slider->blockSignals(true);
        slider->setValue(static_cast<int>(value * 10)); // Scale back to integer for the slider
        slider->blockSignals(false);
    });

    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(slider);
    layout->addWidget(sliderValueBox);
    setLayout(layout);
}

DoubleSliderWidget::DoubleSliderWidget(QWidget *parent) : QWidget(parent) {
    lowSlider = new QSlider(Qt::Horizontal, this);
    lowSlider->setRange(5, 100);
    lowSlider->setValue(20);
    lowSlider->setTickInterval(1);
    lowSlider->setTickPosition(QSlider::TicksBelow);

    highSlider = new QSlider(Qt::Horizontal, this);
    highSlider->setRange(5, 100);
    highSlider->setValue(80);
    highSlider->setTickInterval(1);
    highSlider->setTickPosition(QSlider::TicksBelow);

    lowSliderValueBox = new QDoubleSpinBox(this);
    lowSliderValueBox->setRange(0.5, 10.0);
    lowSliderValueBox->setValue(2.0);
    lowSliderValueBox->setSingleStep(0.1);

    highSliderValueBox = new QDoubleSpinBox(this);
    highSliderValueBox->setRange(0.5, 10.0);
    highSliderValueBox->setValue(8.0);
    highSliderValueBox->setSingleStep(0.1);

    // Connect sliders and spin boxes
    connect(lowSlider, &QSlider::valueChanged, [this](int value) {
        double scaledValue = value * 0.1;
        lowSliderValueBox->setValue(scaledValue);
        emit rangeChanged(lowSliderValueBox->value(), highSliderValueBox->value());
    });
    connect(lowSliderValueBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        lowSlider->setValue(static_cast<int>(value * 10));
        emit rangeChanged(value, highSliderValueBox->value());
    });

    connect(highSlider, &QSlider::valueChanged, [this](int value) {
        double scaledValue = value * 0.1;
        qDebug() << "High slider changed to:" << scaledValue;
        highSliderValueBox->setValue(scaledValue);
        emit rangeChanged(lowSliderValueBox->value(), scaledValue);
    });
    connect(highSliderValueBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        highSlider->setValue(static_cast<int>(value * 10));
        emit rangeChanged(lowSliderValueBox->value(), value);
    });

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *lowLayout = new QHBoxLayout();
    lowLayout->addWidget(lowSlider);
    lowLayout->addWidget(lowSliderValueBox);

    QHBoxLayout *highLayout = new QHBoxLayout();
    highLayout->addWidget(highSlider);
    highLayout->addWidget(highSliderValueBox);

    layout->addLayout(lowLayout);
    layout->addLayout(highLayout);
    setLayout(layout);
}

Widget::Widget(QWidget *parent) : QWidget(parent) {
    // Create Widgets
    graphicsScene = new QGraphicsScene(this);
    graphicsView = new QGraphicsView(graphicsScene, this);
    graphicsView->setRenderHint(QPainter::Antialiasing);
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag); // Enable dragging
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // Zoom relative to mouse position

    layersList = new QListWidget(this);
    layersList->setFixedWidth(150); // Set a fixed width for the layers list
    layersList->setSelectionMode(QAbstractItemView::SingleSelection);
    layersList->setAlternatingRowColors(true);

    modelSelector = new QComboBox(this);
    modelSelector->addItem("Disk");
    modelSelector->addItem("Disk with inner shape");
    modelSelector->addItem("Circle");
    modelSelector->addItem("Ellipse");
    modelSelector->addItem("Reltrans");
    modelSelector->setToolTip("Select Model");

    // Buttons
    generateButton = new QPushButton("Add Model", this);
    QPushButton *zoomInButton = new QPushButton("Zoom In", this);
    zoomInButton->setToolTip("Zoom In (Ctrl + '+')");
    QPushButton *zoomOutButton = new QPushButton("Zoom Out", this);
    zoomOutButton->setToolTip("Zoom Out (Ctrl + '-')");
    QPushButton *resetViewButton = new QPushButton("Reset View", this);
    QPushButton *clearButton = new QPushButton("Clear Model", this);
    QPushButton *logButton = new QPushButton("Toggle Log Scale", this);
    logButton->setCheckable(true);
    logButton->setChecked(logScale);

    // Connect log button
    connect(logButton, &QPushButton::toggled, [this](bool checked) {
        logScale = checked ? 1 : 0;
    });

    // Connect scene  buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addWidget(resetViewButton);

    // Slider Mode Selector
    sliderModeSelector = new QComboBox(this);
    sliderModeSelector->addItem("Energy bin");
    sliderModeSelector->addItem("Energy range");
    sliderModeSelector->setToolTip("Select Slider Mode");

    // Sliders
    SingleSliderWidget *singleSliderWidget = new SingleSliderWidget(this); // For energy bin
    DoubleSliderWidget *doubleSliderWidget = new DoubleSliderWidget(this); // For energy range
    // Stacked layout for the slider widgets
    QStackedLayout *sliderLayout = new QStackedLayout();
    sliderLayout->addWidget(singleSliderWidget);
    sliderLayout->addWidget(doubleSliderWidget);

    connect(singleSliderWidget, &SingleSliderWidget::valueChanged, this, &Widget::handleSingleSliderValueChanged);
    connect(doubleSliderWidget, &DoubleSliderWidget::rangeChanged, this, &Widget::handleDoubleSliderRangeChanged);

    // Combine slider button and sliders
    QHBoxLayout *energySliderLayout = new QHBoxLayout();
    energySliderLayout->addWidget(sliderModeSelector);
    energySliderLayout->addLayout(sliderLayout);

    connect(sliderModeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), [sliderLayout](int index) {
        sliderLayout->setCurrentIndex(index); // Switch between slider layouts
    });

    colorScaleWidget = new ColorScaleWidget(this);

    double minBrightness = 0.0; // Replace with actual minimum brightness
    double maxBrightness = 1.0; // Replace with actual maximum brightness
    colorScaleWidget->setBrightnessRange(minBrightness, maxBrightness);

    // Horizontal layout for graphics view and layers list
    QVBoxLayout *totalViewLayout = new QVBoxLayout();
    QHBoxLayout *canvasLayout = new QHBoxLayout();
    canvasLayout->addWidget(graphicsView);
    QVBoxLayout *layersLayout = new QVBoxLayout();
    layersLayout->addWidget(new QLabel("Layers:"));
    layersLayout->addWidget(layersList,10);
    layersLayout->addWidget(logButton,1);
    canvasLayout->addLayout(layersLayout);
    totalViewLayout->addLayout(canvasLayout,9);
    totalViewLayout->addWidget(colorScaleWidget,1);

    // Spectrum Dialog
    SpectrumDialog *spectrumDialog = new SpectrumDialog(this);

    // Model selection layout
    QHBoxLayout *modelLayout = new QHBoxLayout();
    modelLayout->addWidget(modelSelector);
    modelLayout->addWidget(generateButton);
    modelLayout->addWidget(clearButton);

    // Render Window
    QPushButton *renderImageButton = new QPushButton("Render Image", this);

    connect(renderImageButton, &QPushButton::clicked, [this]() {
        qDebug() << "Rendering stacked image.";
        // Render the total stacked image
        QImage stackedImage = renderStackedImage();

        // Check if the RenderWindow already exists
        if (!renderWindow) {
            renderWindow = std::make_unique<RenderWindow>();
        }

        renderWindow->setImage(stackedImage);
        renderWindow->show();
    });

    // Arrange Main Layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(modelLayout, 1);
    mainLayout->addLayout(buttonLayout, 1);
    mainLayout->addLayout(totalViewLayout, 8);
    mainLayout->addLayout(energySliderLayout, 1);
    mainLayout->addWidget(renderImageButton, 1);

    setLayout(mainLayout);
    setWindowTitle("Image");
    setMinimumSize(500, 500);

    // Connect Signals and Slots of main widgets
    
    connect(clearButton, &QPushButton::clicked, this, &Widget::clearModel);
    connect(zoomInButton, &QPushButton::clicked, this, &Widget::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &Widget::zoomOut);
    connect(resetViewButton, &QPushButton::clicked, this, &Widget::resetView);
    connect(layersList, &QListWidget::itemClicked, this, &Widget::toggleLayerVisibility);
    connect(layersList, &QListWidget::customContextMenuRequested, this, &Widget::showLayerContextMenu);
    layersList->setContextMenuPolicy(Qt::CustomContextMenu);

    // Generate button connections
    connect(generateButton, &QPushButton::clicked, [this]() {
        QString modelType = modelSelector->currentText();
        ModelDefinitionDialog modelDefDialog(modelType, this);

        if (modelDefDialog.exec() == QDialog::Accepted) {
            // Retrieve model parameters from the dialog
            parameters = modelDefDialog.getParameters();
            if (modelType == "Disk with inner shape") {
                qDebug() << "Disk with inner shape parameters retrieved.";
                shapeType = parameters["InnerShape"].toString();
                if (shapeType == "Circle") {
                    qDebug() << "Inner shape is Circle with radius:" << parameters["Radius"].toDouble();
                    radius = parameters["Radius"].toDouble();
                } else if (shapeType == "Rectangle") {
                    qDebug() << "Inner shape is Rectangle with width:" << parameters["Width"].toDouble()
                             << "and height:" << parameters["Height"].toDouble();
                    width = parameters["Width"].toDouble();
                    height = parameters["Height"].toDouble();
                }
                centerX = parameters["CenterX"].toDouble();
                centerY = parameters["CenterY"].toDouble();
                innerRadius = parameters["InnerRadius"].toDouble();
                outerRadius = parameters["OuterRadius"].toDouble();
                inclination = parameters["Inclination"].toDouble();
                qDebug() << "Disk with inner shape parameters - Amplitude:" << amplitude << ", Frequency:" << frequency;
            } else if (modelType == "Circle") {
                radius = parameters["Radius"].toDouble();
                centerX = parameters["CenterX"].toDouble();
                centerY = parameters["CenterY"].toDouble();
                qDebug() << "Circle parameters - Radius:" << radius << ", CenterX:" << centerX << ", CenterY:" << centerY;
            } else if (modelType == "Disk") {
                centerX = parameters["CenterX"].toDouble();
                centerY = parameters["CenterY"].toDouble();
                innerRadius = parameters["InnerRadius"].toDouble();
                outerRadius = parameters["OuterRadius"].toDouble();
                inclination = parameters["Inclination"].toDouble();
                qDebug() << "Disk parameters - InnerRadius:" << innerRadius << ", OuterRadius:" << outerRadius << ", Inclination:" << inclination;
            } else if (modelType == "Ellipse") {
                radiusX = parameters["RadiusX"].toDouble();
                radiusY = parameters["RadiusY"].toDouble();
                centerX = parameters["CenterX"].toDouble();
                centerY = parameters["CenterY"].toDouble();
                qDebug() << "Ellipse parameters - RadiusX:" << radiusX << ", RadiusY:" << radiusY << ", CenterX:" << centerX << ", CenterY:" << centerY;
        }
    }});
    connect(generateButton, &QPushButton::clicked, [this, spectrumDialog]() {
        if (spectrumDialog->exec() == QDialog::Accepted) {
            currentSpectrumFunction = spectrumDialog->getSelectedSpectrum();
            qDebug() << "Spectrum function updated.";
            // Use the selected spectrum function
        }
        else 
        {
            currentSpectrumFunction = [](double energy) { return 1.0; }; // Default to constant brightness
            qDebug() << "No spectrum function provided, using default constant brightness.";
        }
        
    });
    connect(generateButton, &QPushButton::clicked, this, &Widget::updateImage);

    qDebug() << "Widget initialized.";
}



Widget::~Widget() {
    // Widgets are children of Widget, so they are automatically deleted when Widget is destroyed.
    // No need for explicit `delete` calls here for the dynamically allocated widgets.
}

void Widget::resetView() {
    graphicsView->resetTransform(); // Reset zoom and transformations
    graphicsView->fitInView(graphicsScene->itemsBoundingRect(), Qt::KeepAspectRatio); // Fit the view to the content
}

void Widget::clearModel() {
    graphicsScene->clear();
    layersList->clear();
    pathSpectra.clear();
    qDebug() << "Cleared model and layers.";
}

void Widget::updateImage() {
    qDebug() << "Updating image with model:" << modelSelector->currentText();

    QPainterPath modelPath;
    QPainterPath innerShapePath;
    QColor fillColor;

    int selectedIndex = modelSelector->currentIndex();
    switch (selectedIndex) {
        case 0: // Disk
            disk(modelPath, centerX, centerY, innerRadius, outerRadius, inclination);
            break;
        case 1: // disk with inner shape
            // Define the inner shape function based on parameters
            if (shapeType == "Circle") {
                qDebug() << "Inner shape is Circle with radius:" << radius;
                circle(innerShapePath, centerX, centerY, radius);
            } else if (shapeType == "Rectangle") {
                qDebug() << "Inner shape is Rectangle with width:" << width << "and height:" << height;
                rectangle(innerShapePath, centerX, centerY, width, height);
            }
            diskInnerShape(modelPath, centerX, centerY, innerRadius, outerRadius, inclination, innerShapePath);
            break;
        case 2: // Circle
            circle(modelPath, centerX, centerY, radius);
            break;
        case 3: // Ellipse
            ellipse(modelPath, centerX, centerY, radiusX, radiusY);
            break;
        case 4: // Reltrans (placeholder)
            qWarning() << "Reltrans model not implemented for vectorized graphics.";
            return;
        default:
            qWarning() << "Unknown model selected!";
            return;
    }

    qDebug() << "Model path generated.";
    addModelPathItem(modelPath, fillColor, currentSpectrumFunction); // Use the helper function
    graphicsView->fitInView(graphicsScene->itemsBoundingRect(), Qt::KeepAspectRatio); // Fit the view to the content
}


// Model Path Functions
void Widget::addModelPathItem(const QPainterPath &path, const QColor &fillColor, std::function<double(double)> spectrumFunction) {
    QGraphicsPathItem *modelPathItem = new QGraphicsPathItem(path);
    modelPathItem->setPen(Qt::NoPen); // Outline
    modelPathItem->setBrush(QBrush(fillColor)); // Set the fill color
    graphicsScene->addItem(modelPathItem); // Add the model path to the scene
    qDebug() << "Model path item added to graphics scene.";

    // Dynamically adjust the scene rectangle to fit all paths
    QRectF combinedBoundingRect;
    for (const auto &item : graphicsScene->items()) {
        if (auto pathItem = qgraphicsitem_cast<QGraphicsPathItem *>(item)) {
            combinedBoundingRect = combinedBoundingRect.united(pathItem->path().boundingRect());
        }
    }
    graphicsScene->setSceneRect(combinedBoundingRect);

    // Add the model layer to the layers list
    QListWidgetItem *modelLayerItem = new QListWidgetItem("[Visible] " + modelSelector->currentText());
    modelLayerItem->setData(Qt::UserRole, QVariant::fromValue(modelPathItem)); // Store the graphics item
    layersList->addItem(modelLayerItem);
    qDebug() << "Layers list updated.";
    addPathWithSpectrum(modelPathItem, spectrumFunction);
}

void Widget::addPathWithSpectrum(QGraphicsPathItem *pathItem, std::function<double(double)> spectrumFunction) {
    if (!pathItem) {
        qWarning() << "Attempted to add a null path item!";
        return;
    }

    // Calculate the maximum brightness of the spectrum
    double maxBrightness = 0.0;
    double brightness = 0.0;
    double low = 0.1; // Start of the energy range
    double high = 10.0; // End of the energy range
    double step = 0.1; // Step size for evaluation
    double totalBrightness = 0.0;

    for (double energy = low; energy <= high; energy += step) {
        brightness = spectrumFunction(energy);
        maxBrightness = std::max(maxBrightness, brightness);
        totalBrightness += brightness;
    }

    pathSpectra.append({pathItem, spectrumFunction, maxBrightness, totalBrightness});
    qDebug() << "Added path with spectrum. Max brightness:" << maxBrightness << ". Total brightness:" << totalBrightness << ". Total paths:" << pathSpectra.size();
    globalMaxBrightness = std::max(maxBrightness, globalMaxBrightness);
    globalTotalMaxBrightness = std::max(totalBrightness, globalTotalMaxBrightness);
    qDebug() << "Updated global max brightness to:" << globalMaxBrightness;
    qDebug() << "Updated global total max brightness to:" << globalTotalMaxBrightness;
}

void Widget::setPathSpectrumFunction(QGraphicsPathItem *pathItem, std::function<double(double)> spectrumFunction) {
    for (PathSpectrum &pathSpectrum : pathSpectra) {
        if (pathSpectrum.pathItem == pathItem) {
            pathSpectrum.spectrumFunction = spectrumFunction;
            return;
        }
    }
    qWarning() << "Path item not found in spectrum list!";
}

// Render stacked image from all paths

QImage Widget::renderStackedImage() {
    QRectF sceneRect = graphicsScene->sceneRect();
    qDebug() << "Scene Rect:" << sceneRect;
    
    if (sceneRect.isEmpty()) {
        sceneRect = graphicsScene->itemsBoundingRect();
        graphicsScene->setSceneRect(sceneRect);
        qDebug() << "Updated Scene Rect:" << sceneRect;
    }
    int width = static_cast<int>(sceneRect.width());
    int height = static_cast<int>(sceneRect.height());
    qDebug() << "Rendering stacked image of size:" << width << "x" << height;

    // Create an accumulation buffer for brightness values
    std::vector<std::vector<double>> brightnessBuffer(height, std::vector<double>(width, 0.0));

    // Iterate over all paths and accumulate brightness
    for (const PathSpectrum &pathSpectrum : pathSpectra) {
        QImage pathImage(width, height, QImage::Format_RGB32);
        pathImage.fill(Qt::black); // Initialize the image with black (0 brightness)
        QPainter pathPainter(&pathImage);
        pathPainter.setRenderHint(QPainter::Antialiasing);

        // Render the path to the image
        QGraphicsPathItem *pathItem = pathSpectrum.pathItem;
        if (pathItem) {
            QPainterPath path = pathItem->path();
            qDebug() << "Path bounding rect:" << path.boundingRect();
            QBrush brush = pathItem->brush();
            QColor color = brush.color();
            double brightness = color.valueF(); // Get the brightness of the path
            qDebug() << "Rendering path with brightness:" << brightness;

            QColor brushColor = QColor::fromRgbF(brightness, brightness, brightness);
            qDebug() << "Brush color for rendering:" << brushColor;
            pathPainter.setBrush(QBrush(brushColor));
            pathPainter.setPen(Qt::NoPen);
            pathPainter.translate(-sceneRect.topLeft()); // Align scene coordinates with image coordinates
            pathPainter.drawPath(path);
        }
        pathPainter.end();

        // Accumulate brightness values into the buffer
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                QColor pixelColor = pathImage.pixelColor(x, y);
                double pixelBrightness = pixelColor.valueF();
                brightnessBuffer[y][x] += pixelBrightness;
                if (pixelBrightness > 0.0) {
                    qDebug() << "Accumulating brightness at (" << x << "," << y << "):" << pixelBrightness;
                }
            }
        }
    }

    // Normalize the brightness buffer and render the final image
    QImage finalImage(width, height, QImage::Format_Grayscale8);
    double maxBrightness = 0.0;
    for (const auto &row : brightnessBuffer) {
        for (double value : row) {
            maxBrightness = std::max(maxBrightness, value);
        }
    }
    qDebug() << "Max brightness in buffer:" << maxBrightness;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double normalizedBrightness = qBound(0.0, brightnessBuffer[y][x] / maxBrightness, 1.0);
            int pixelValue = static_cast<int>(normalizedBrightness * 255);
            finalImage.setPixelColor(x, y, QColor(pixelValue, pixelValue, pixelValue));
        }
    }

    return finalImage;
}

void Widget::zoomIn() {
    graphicsView->scale(1.2, 1.2); // Zoom in by 20%
}

void Widget::zoomOut() {
    graphicsView->scale(0.8, 0.8); // Zoom out by 20%
}

void Widget::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus) {
            zoomIn();
        } else if (event->key() == Qt::Key_Minus) {
            zoomOut();
        }
    }
}

// layer functionality

void Widget::toggleLayerVisibility(QListWidgetItem *item) {
    QGraphicsPathItem *pathItem = item->data(Qt::UserRole).value<QGraphicsPathItem*>();
    if (!pathItem) {
        qWarning() << "No graphics item associated with this layer!";
        return;
    }
    qDebug() << "Toggling layer visibility for item:" << item->text();
    qDebug() << "Current visibility:" << pathItem->isVisible();
    if (pathItem->isVisible()) {
        pathItem->hide();
        item->setText("[Hidden] " + item->text().remove("[Visible] "));
        qDebug() << "Layer hidden.";
    } else {
        pathItem->show();
        item->setText("[Visible] " + item->text().remove("[Hidden] "));
        qDebug() << "Layer shown.";
    }
}

void Widget::showLayerContextMenu(const QPoint &pos) {
    QListWidgetItem *item = layersList->itemAt(pos);
    if (!item) return;

    QMenu contextMenu(this);
    QAction *editPathAction = contextMenu.addAction("Edit Path");
    QAction *editSpectrumAction = contextMenu.addAction("Edit Spectrum");
    QAction *deleteLayerAction = contextMenu.addAction("Delete Layer");

    QAction *selectedAction = contextMenu.exec(layersList->mapToGlobal(pos));
    if (selectedAction == editPathAction) {
        editLayerPath(item);
    } else if (selectedAction == editSpectrumAction) {
        editLayerSpectrum(item);
    } else if (selectedAction == deleteLayerAction) {
        deleteLayer(item);
    }
}

// Layer context menu actions

void Widget::deleteLayer(QListWidgetItem *item) {
    QGraphicsPathItem *pathItem = item->data(Qt::UserRole).value<QGraphicsPathItem*>();
    if (!pathItem) return;

    graphicsScene->removeItem(pathItem);
    delete pathItem;
    delete item;

    pathSpectra.erase(std::remove_if(pathSpectra.begin(), pathSpectra.end(), [pathItem](const PathSpectrum &ps) {
        return ps.pathItem == pathItem;
    }), pathSpectra.end());

    qDebug() << "Layer deleted.";
}

void Widget::editLayerPath(QListWidgetItem *item) {
    QGraphicsPathItem *pathItem = item->data(Qt::UserRole).value<QGraphicsPathItem*>();
    if (!pathItem) return;

    // Open the ModelDefinitionDialog with the current model type
    QString modelType = modelSelector->currentText();
    ModelDefinitionDialog modelDefDialog(modelType, this);

    if (modelDefDialog.exec() == QDialog::Accepted) {
        // Retrieve updated model parameters
        QMap<QString, QVariant> parameters = modelDefDialog.getParameters();

        QPainterPath updatedPath;
        if (modelType == "Sine Wave") {
            double amplitude = parameters["Amplitude"].toDouble();
            double frequency = parameters["Frequency"].toDouble();
            generateSineWave(updatedPath, IMAGE_WIDTH, IMAGE_HEIGHT, amplitude, frequency);
        } else if (modelType == "Circle") {
            double radius = parameters["Radius"].toDouble();
            int centerX = parameters["CenterX"].toInt();
            int centerY = parameters["CenterY"].toInt();
            circle(updatedPath, centerX, centerY, radius);
        }

        // Update the path item with the new path
        pathItem->setPath(updatedPath);
        qDebug() << "Path updated for layer.";
    }
}

void Widget::editLayerSpectrum(QListWidgetItem *item) {
    QGraphicsPathItem *pathItem = item->data(Qt::UserRole).value<QGraphicsPathItem*>();
    if (!pathItem) return;

    // Find the spectrum associated with the path
    auto it = std::find_if(pathSpectra.begin(), pathSpectra.end(), [pathItem](const PathSpectrum &ps) {
        return ps.pathItem == pathItem;
    });

    if (it != pathSpectra.end()) {
        // Open the SpectrumDialog to edit the spectrum
        SpectrumDialog spectrumDialog(this);
        if (spectrumDialog.exec() == QDialog::Accepted) {
            it->spectrumFunction = spectrumDialog.getSelectedSpectrum();
            qDebug() << "Spectrum updated for layer.";
        }
    }
}

// Slider functionality

void Widget::handleSingleSliderValueChanged(double value) {
    qDebug() << "Slider value changed to:" << value;

    double energy = value * 0.1; // Convert slider value to energy (0.0 to 10.0)

    // Now, normalize and update the brightness of each path
    for (const PathSpectrum &pathSpectrum : pathSpectra) {
        double brightness = pathSpectrum.spectrumFunction(energy);
        if (logScale) {
            brightness = log10(1e-11 + brightness); // Apply log scale if enabled
        }

        // Normalize brightness using the maximum brightness
        double normalizedBrightness = brightness / globalMaxBrightness;
        normalizedBrightness = qBound(0.0, normalizedBrightness, 1.0); // Clamp brightness to [0.0, 1.0]

        qDebug() << "Normalized brightness:" << normalizedBrightness;

        // Update the color based on the normalized brightness
        QColor originalColor = pathSpectrum.pathItem->brush().color();
        QColor modifiedColor = originalColor;
        modifiedColor.setHsvF(originalColor.hueF(), originalColor.saturationF(), normalizedBrightness);
        pathSpectrum.pathItem->setBrush(QBrush(modifiedColor)); // Update the path's color
        qDebug() << "Updated path item brush with color:" << modifiedColor;
    }

    colorScaleWidget->setBrightnessRange(0.0, globalMaxBrightness);
    qDebug() << "Updated color scale with min brightness:" << 0.0 << "and max brightness:" << globalMaxBrightness;
}

void Widget::handleDoubleSliderRangeChanged(double low, double high) {
    qDebug() << "Double slider range changed to:" << low << " - " << high;
    // Handle the double slider range (e.g., update a model or UI)
    double energy_low = low * 0.1; // Convert slider value to energy (0.0 to 10.0)
    double energy_high = high * 0.1;
    double step = 0.1; // Define the step size for the energy range
    double maxNormalizedBrightness = 0.0;

    // Now, normalize and update the brightness of each path
    for (const PathSpectrum &pathSpectrum : pathSpectra) {
        double brightnessSum = evaluateSpectrumOverRange(pathSpectrum, energy_low, energy_high, step);
        if (logScale) {
            brightnessSum = log10(1.0 + brightnessSum); // Apply log scale if enabled
        }

        // Normalize brightness using the maximum brightness
        double normalizedBrightness = brightnessSum / globalTotalMaxBrightness;
        normalizedBrightness = qBound(0.0, normalizedBrightness, 1.0); // Clamp brightness to [0.0, 1.0]
        maxNormalizedBrightness = std::max(maxNormalizedBrightness, normalizedBrightness);
        qDebug() << "Normalized brightness:" << normalizedBrightness;

        // Update the color based on the normalized brightness
        QColor originalColor = pathSpectrum.pathItem->brush().color();
        QColor modifiedColor = originalColor;
        modifiedColor.setHsvF(originalColor.hueF(), originalColor.saturationF(), normalizedBrightness);
        pathSpectrum.pathItem->setBrush(QBrush(modifiedColor)); // Update the path's color
        qDebug() << "Updated path item brush with color:" << modifiedColor;
    }

    // Update the color scale widget
    colorScaleWidget->setBrightnessRange(0.0, maxNormalizedBrightness);
    qDebug() << "Updated color scale with range: 0.0 to" << maxNormalizedBrightness;
}

std::vector<double> generateEnergyRange(double low, double high, double step) {
    std::vector<double> energyRange;
    for (double energy = low; energy <= high; energy += step) {
        energyRange.push_back(energy);
    }
    return energyRange;
}

double Widget::evaluateSpectrumOverRange(const PathSpectrum &pathSpectrum, double low, double high, double step) {
    double sum = 0.0;
    std::vector<double> energyRange = generateEnergyRange(low, high, step);

    for (double energy : energyRange) {
        sum += pathSpectrum.spectrumFunction(energy); // Evaluate the spectrum function
    }

    return sum;
}

// Visual aids