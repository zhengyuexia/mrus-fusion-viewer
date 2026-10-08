#include "FusionTabView.h"
#include "ui_FusionTabView.h"
#include "DisplayConstants.h"

#include <QPixmap>

FusionTabView::FusionTabView(FusionViewModel & viewModel, QWidget * parent)
    : QWidget(parent), ui(new Ui::FusionTabView), m_viewModel(viewModel)
{
  ui->setupUi(this);

  connect(ui->modeCombo, &QComboBox::currentIndexChanged, &m_viewModel, [this](int index) {
    m_viewModel.setMode(index == 0 ? FusionViewModel::Mode::Blend : FusionViewModel::Mode::Checker);
  });
  connect(ui->paramSlider, &QSlider::valueChanged, &m_viewModel, &FusionViewModel::setParam);
  connect(ui->sliceSlider, &QSlider::valueChanged, &m_viewModel, &FusionViewModel::setSliceZ);

  connect(&m_viewModel, &FusionViewModel::sliceRangeChanged, this, [this](int maxZ) {
    ui->sliceSlider->blockSignals(true);
    ui->sliceSlider->setRange(0, maxZ);
    ui->sliceSlider->blockSignals(false);
  });
  connect(&m_viewModel, &FusionViewModel::fusionImageChanged, this, [this](QImage image) {
    ui->fusionView->setPixmap(QPixmap::fromImage(image).scaled(
        view::kImageDisplaySize, view::kImageDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  });
  connect(&m_viewModel, &FusionViewModel::placeholderMessage, ui->fusionView, &QLabel::setText);
}

FusionTabView::~FusionTabView()
{
  delete ui;
}
