#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "FilterTabView.h"
#include "SegmentTabView.h"
#include "RegisterTabView.h"
#include "FusionTabView.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget * parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  m_caseViewModel = std::make_unique<CaseViewModel>(this);
  m_filterViewModel = std::make_unique<FilterViewModel>(*m_caseViewModel, this);
  m_segmentViewModel = std::make_unique<SegmentViewModel>(*m_caseViewModel, this);
  m_registerViewModel = std::make_unique<RegisterViewModel>(*m_caseViewModel, this);
  m_fusionViewModel = std::make_unique<FusionViewModel>(*m_caseViewModel, *m_registerViewModel, this);

  ui->tabWidget->addTab(new FilterTabView(*m_filterViewModel, this), "1. 滤波 Filter");
  ui->tabWidget->addTab(new SegmentTabView(*m_segmentViewModel, this), "2. 分割 Segment");
  ui->tabWidget->addTab(new RegisterTabView(*m_registerViewModel, this), "3. 配准 Register");
  ui->tabWidget->addTab(new FusionTabView(*m_fusionViewModel, this), "4. 融合 Fusion");

  connect(ui->loadButton, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
  connect(ui->datasetCombo, &QComboBox::currentIndexChanged, this, &MainWindow::onDatasetChanged);
  connect(m_caseViewModel.get(), &CaseViewModel::caseLoaded, this,
          [this]() { ui->infoLabel->setText(m_caseViewModel->infoText()); });
  connect(m_caseViewModel.get(), &CaseViewModel::loadFailed, this, [this](QString message) {
    QMessageBox::warning(this, "读取失败", message);
  });

  onDatasetChanged(ui->datasetCombo->currentIndex());
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::onDatasetChanged(int index)
{
  const bool isProstate = (index == 0);
  ui->caseSpin->setVisible(isProstate);
  m_caseViewModel->setDataset(isProstate ? model::Dataset::Prostate : model::Dataset::BrainBite);
  m_caseViewModel->setCaseIndex(ui->caseSpin->value());
  m_caseViewModel->load();
}

void MainWindow::onLoadClicked()
{
  m_caseViewModel->setCaseIndex(ui->caseSpin->value());
  m_caseViewModel->load();
}
