#include "RegisterTabView.h"
#include "ui_RegisterTabView.h"
#include "DisplayConstants.h"

#include <QPixmap>

RegisterTabView::RegisterTabView(RegisterViewModel & viewModel, QWidget * parent)
    : QWidget(parent), ui(new Ui::RegisterTabView), m_viewModel(viewModel)
{
  ui->setupUi(this);

  connect(ui->runButton, &QPushButton::clicked, &m_viewModel, &RegisterViewModel::runRegistration);
  connect(ui->sliceSlider, &QSlider::valueChanged, &m_viewModel, &RegisterViewModel::setSliceZ);

  connect(&m_viewModel, &RegisterViewModel::sliceRangeChanged, this, [this](int maxZ, int currentZ) {
    ui->sliceSlider->blockSignals(true);
    ui->sliceSlider->setRange(0, maxZ);
    ui->sliceSlider->setValue(currentZ);
    ui->sliceSlider->blockSignals(false);
  });
  connect(&m_viewModel, &RegisterViewModel::statusTextChanged, ui->statusLabel, &QLabel::setText);
  connect(&m_viewModel, &RegisterViewModel::fixedSliceChanged, this, [this](QImage image) {
    if (image.isNull())
    {
      ui->fixedView->clear();
      return;
    }
    ui->fixedView->setPixmap(QPixmap::fromImage(image).scaled(
        view::kImageDisplaySize, view::kImageDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  });
  connect(&m_viewModel, &RegisterViewModel::alignedSliceChanged, this, [this](QImage image) {
    if (image.isNull())
    {
      ui->alignedView->clear();
      return;
    }
    ui->alignedView->setPixmap(QPixmap::fromImage(image).scaled(
        view::kImageDisplaySize, view::kImageDisplaySize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  });
}

RegisterTabView::~RegisterTabView()
{
  delete ui;
}
