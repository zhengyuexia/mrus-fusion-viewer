#include "SegmentTabView.h"
#include "ui_SegmentTabView.h"
#include "DisplayConstants.h"

#include <QPixmap>

#include <algorithm>

SegmentTabView::SegmentTabView(SegmentViewModel & viewModel, QWidget * parent)
    : QWidget(parent), ui(new Ui::SegmentTabView), m_viewModel(viewModel)
{
  ui->setupUi(this);

  connect(ui->modalityCombo, &QComboBox::currentIndexChanged, &m_viewModel, [this](int index) {
    m_viewModel.setModality(index == 0 ? SegmentViewModel::Modality::MR : SegmentViewModel::Modality::US);
  });
  connect(ui->sliceSlider, &QSlider::valueChanged, &m_viewModel, &SegmentViewModel::setSliceZ);
  connect(ui->seedXSpin, &QSpinBox::valueChanged, &m_viewModel, &SegmentViewModel::setSeedX);
  connect(ui->seedYSpin, &QSpinBox::valueChanged, &m_viewModel, &SegmentViewModel::setSeedY);
  connect(ui->lowerSpin, &QSpinBox::valueChanged, &m_viewModel, &SegmentViewModel::setLower);
  connect(ui->upperSpin, &QSpinBox::valueChanged, &m_viewModel, &SegmentViewModel::setUpper);
  connect(ui->runButton, &QPushButton::clicked, &m_viewModel, &SegmentViewModel::runSegmentation);
  connect(ui->showGroundTruthCheck, &QCheckBox::toggled, &m_viewModel, &SegmentViewModel::setShowGroundTruth);

  connect(&m_viewModel, &SegmentViewModel::sliceRangeChanged, this, [this](int maxZ, int currentZ) {
    ui->sliceSlider->blockSignals(true);
    ui->sliceSlider->setRange(0, maxZ);
    ui->sliceSlider->setValue(currentZ);
    ui->sliceSlider->blockSignals(false);
  });
  connect(&m_viewModel, &SegmentViewModel::seedRangeChanged, this, [this](int maxX, int maxY) {
    ui->seedXSpin->blockSignals(true);
    ui->seedXSpin->setRange(0, maxX);
    ui->seedXSpin->setValue(std::min(ui->seedXSpin->value(), maxX));
    ui->seedXSpin->blockSignals(false);
    ui->seedYSpin->blockSignals(true);
    ui->seedYSpin->setRange(0, maxY);
    ui->seedYSpin->setValue(std::min(ui->seedYSpin->value(), maxY));
    ui->seedYSpin->blockSignals(false);
  });
  connect(&m_viewModel, &SegmentViewModel::viewImageChanged, this, [this](QImage image) {
    ui->segView->setPixmap(QPixmap::fromImage(image).scaled(
        view::kImageDisplaySize, view::kImageDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  });
  connect(&m_viewModel, &SegmentViewModel::diceTextChanged, ui->diceLabel, &QLabel::setText);
  connect(&m_viewModel, &SegmentViewModel::hasLabelsChanged, this, [this](bool hasLabels) {
    ui->showGroundTruthCheck->setEnabled(hasLabels);
    if (!hasLabels)
    {
      ui->showGroundTruthCheck->setChecked(false);
    }
  });
}

SegmentTabView::~SegmentTabView()
{
  delete ui;
}
