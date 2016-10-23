#include "Properties.h"
#include <QtWidgets>

Properties::Properties(Octree* octree, Viewport* viewport) :
    octree(octree),
    viewport(viewport) {
    QFormLayout* formlayout = new QFormLayout;
    setLayout(formlayout);

    levelLabel = new QLabel;
    formlayout->addRow(tr("Level:"), levelLabel);

    indexLabel = new QLabel;
    formlayout->addRow(tr("Index:"), indexLabel);

    colorButton = new QPushButton;
    colorButton->setAutoFillBackground(true);
    colorButton->setFlat(true);
    setNodeColor(QColor(Qt::blue));
    formlayout->addRow(tr("Color:"), colorButton);

    QCheckBox* shadelessCheckBox = new QCheckBox(tr("Shadeless"));
    formlayout->addRow(shadelessCheckBox);

    connect(colorButton, &QPushButton::clicked, this, &Properties::changeNodeColor);
    connect(shadelessCheckBox, &QCheckBox::stateChanged, viewport, &Viewport::setShadeless);
    connect(octree, &Octree::nodeSelected, this, &Properties::onNodeSelected);
    connect(octree, &Octree::nodeDeselected, this, &Properties::onNodeDeselected);

    onNodeDeselected();
}

void Properties::setNodeLevel(int level) {
    if (level >= 0) {
        levelLabel->setText(QString::number(23 - level));
    } else {
        levelLabel->setText(QString());
    }
}

void Properties::setNodeIndex(int index) {
    if (index >= 0) {
        indexLabel->setText(QString::number(index));
    } else {
        indexLabel->setText(QString());
    }
}

void Properties::setNodeColor(const QColor& color) {

    QPalette colorButtonPal = colorButton->palette();
    colorButtonPal.setColor(QPalette::Button, color.isValid() ? color : QColor(Qt::transparent));
    colorButton->setPalette(colorButtonPal);
    nodeColor = color;

    colorButton->setEnabled(color.isValid());
}

void Properties::changeNodeColor() {
    QColor color = QColorDialog::getColor(nodeColor);
    if (color.isValid()) {
        setNodeColor(color);
        octree->changeNodeColor(color);
    }
}

void Properties::onNodeSelected(int level, int index, const QColor& color) {
    setNodeLevel(level);
    setNodeIndex(index);
    setNodeColor(color);
}

void Properties::onNodeDeselected() {
    setNodeLevel(-1);
    setNodeIndex(-1);
    setNodeColor(QColor());
}
