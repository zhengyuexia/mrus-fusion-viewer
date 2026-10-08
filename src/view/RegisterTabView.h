#pragma once

#include "../viewmodel/RegisterViewModel.h"

#include <QWidget>

namespace Ui
{
class RegisterTabView;
}

class RegisterTabView : public QWidget
{
  Q_OBJECT
public:
  explicit RegisterTabView(RegisterViewModel & viewModel, QWidget * parent = nullptr);
  ~RegisterTabView() override;

private:
  Ui::RegisterTabView * ui;
  RegisterViewModel & m_viewModel;
};
