#pragma once

#include "../viewmodel/SegmentViewModel.h"

#include <QWidget>

namespace Ui
{
class SegmentTabView;
}

class SegmentTabView : public QWidget
{
  Q_OBJECT
public:
  explicit SegmentTabView(SegmentViewModel & viewModel, QWidget * parent = nullptr);
  ~SegmentTabView() override;

private:
  Ui::SegmentTabView * ui;
  SegmentViewModel & m_viewModel;
};
