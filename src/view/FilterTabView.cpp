#include "FilterTabView.h"
#include "ui_FilterTabView.h"
#include "DisplayConstants.h"

#include <QPixmap>

FilterTabView::FilterTabView(FilterViewModel & viewModel, QWidget * parent)
    : QWidget(parent), ui(new Ui::FilterTabView), m_viewModel(viewModel)
{
  ui->setupUi(this);

  connect(ui->modalityCombo, &QComboBox::currentIndexChanged, &m_viewModel, [this](int index) {
    m_viewModel.setModality(index == 0 ? FilterViewModel::Modality::MR : FilterViewModel::Modality::US);
  });
  connect(ui->sliceSlider, &QSlider::valueChanged, &m_viewModel, &FilterViewModel::setSliceZ);
  connect(ui->iterationsSpin, &QSpinBox::valueChanged, &m_viewModel, &FilterViewModel::setIterations);
  connect(ui->timeStepSpin, &QDoubleSpinBox::valueChanged, &m_viewModel, &FilterViewModel::setTimeStep);
  connect(ui->applyButton, &QPushButton::clicked, &m_viewModel, &FilterViewModel::runFilter);

  connect(&m_viewModel, &FilterViewModel::sliceRangeChanged, this, [this](int maxZ, int currentZ) {
    ui->sliceSlider->blockSignals(true);
    ui->sliceSlider->setRange(0, maxZ);
    ui->sliceSlider->setValue(currentZ);
    ui->sliceSlider->blockSignals(false);
  });
  connect(&m_viewModel, &FilterViewModel::beforeImageChanged, this, [this](QImage image) {
    ui->beforeView->setPixmap(QPixmap::fromImage(image).scaled(
        view::kImageDisplaySize, view::kImageDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  });
  connect(&m_viewModel, &FilterViewModel::afterImageChanged, this, [this](QImage image) {
    if (image.isNull())
    {
      ui->afterView->clear();
      return;
    }
    ui->afterView->setPixmap(QPixmap::fromImage(image).scaled(
        view::kImageDisplaySize, view::kImageDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  });
  connect(&m_viewModel, &FilterViewModel::statsTextChanged, ui->statsLabel, &QLabel::setText);
}

FilterTabView::~FilterTabView()
{
  delete ui;
}
