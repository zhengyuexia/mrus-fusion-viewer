#include "FusionViewModel.h"

#include "../model/ImageSlicing.h"
#include "../model/ImageConversion.h"

#include <algorithm>

FusionViewModel::FusionViewModel(CaseViewModel & caseViewModel, RegisterViewModel & registerViewModel,
                                  QObject * parent)
    : QObject(parent), m_caseViewModel(caseViewModel), m_registerViewModel(registerViewModel)
{
  connect(&m_registerViewModel, &RegisterViewModel::resultReady, this, &FusionViewModel::updateFusion);
  connect(&m_caseViewModel, &CaseViewModel::caseLoaded, this,
          [this]() { emit placeholderMessage("请先在'配准'标签页运行配准"); });
}

void FusionViewModel::setMode(Mode mode)
{
  m_mode = mode;
  updateFusion();
}

void FusionViewModel::setParam(int param)
{
  m_param = param;
  updateFusion();
}

void FusionViewModel::setSliceZ(int z)
{
  m_sliceZ = z;
  updateFusion();
}

void FusionViewModel::updateFusion()
{
  auto mr = m_caseViewModel.mrImage();
  auto alignedUs = m_registerViewModel.alignedUsImage();
  if (!mr || !alignedUs)
  {
    emit placeholderMessage("请先在'配准'标签页运行配准");
    return;
  }

  auto mrSize = mr->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(mrSize[2]) - 1;
  m_sliceZ = std::min(m_sliceZ, maxZ);
  emit sliceRangeChanged(maxZ);

  auto mrSlice = model::sliceOf(mr, m_sliceZ);
  auto usSlice = model::sliceOf(alignedUs, m_sliceZ);

  QImage img;
  if (m_mode == Mode::Blend)
  {
    double alpha = m_param / 100.0;
    img = model::blendToQImage(mrSlice, usSlice, alpha);
  }
  else
  {
    int pattern = std::max(2, m_param / 10);
    img = model::checkerToQImage(mrSlice, usSlice, pattern);
  }
  emit fusionImageChanged(img);
}
