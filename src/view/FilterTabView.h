#pragma once

#include "../viewmodel/FilterViewModel.h"

#include <QWidget>

namespace Ui
{
class FilterTabView;
}

class FilterTabView : public QWidget
{
  Q_OBJECT
public:
  explicit FilterTabView(FilterViewModel & viewModel, QWidget * parent = nullptr);
  ~FilterTabView() override;

private:
  Ui::FilterTabView * ui;
  FilterViewModel & m_viewModel;
};
