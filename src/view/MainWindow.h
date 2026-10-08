#pragma once

#include "../viewmodel/CaseViewModel.h"
#include "../viewmodel/FilterViewModel.h"
#include "../viewmodel/SegmentViewModel.h"
#include "../viewmodel/RegisterViewModel.h"
#include "../viewmodel/FusionViewModel.h"

#include <QMainWindow>

#include <memory>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindow(QWidget * parent = nullptr);
  ~MainWindow() override;

private:
  void onDatasetChanged(int index);
  void onLoadClicked();

  Ui::MainWindow * ui;

  std::unique_ptr<CaseViewModel> m_caseViewModel;
  std::unique_ptr<FilterViewModel> m_filterViewModel;
  std::unique_ptr<SegmentViewModel> m_segmentViewModel;
  std::unique_ptr<RegisterViewModel> m_registerViewModel;
  std::unique_ptr<FusionViewModel> m_fusionViewModel;
};
