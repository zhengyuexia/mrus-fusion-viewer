#pragma once

#include "../viewmodel/FusionViewModel.h"

#include <QWidget>

namespace Ui
{
class FusionTabView;
}

class FusionTabView : public QWidget
{
  Q_OBJECT
public:
  explicit FusionTabView(FusionViewModel & viewModel, QWidget * parent = nullptr);
  ~FusionTabView() override;

private:
  Ui::FusionTabView * ui;
  FusionViewModel & m_viewModel;
};
