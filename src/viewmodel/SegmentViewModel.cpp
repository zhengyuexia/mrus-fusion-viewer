#include "SegmentViewModel.h"

#include "../model/ImageSlicing.h"
#include "../model/ImageConversion.h"
#include "../model/SegmentationService.h"

#include <itkMacro.h>

#include <algorithm>

SegmentViewModel::SegmentViewModel(CaseViewModel & caseViewModel, QObject * parent)
    : QObject(parent), m_caseViewModel(caseViewModel)
{
  connect(&m_caseViewModel, &CaseViewModel::caseLoaded, this, &SegmentViewModel::onCaseLoaded);
}

model::Volume3DType::Pointer SegmentViewModel::currentVolume() const
{
  return (m_modality == Modality::MR) ? m_caseViewModel.mrImage() : m_caseViewModel.usImage();
}

model::Label3DType::Pointer SegmentViewModel::currentLabelVolume() const
{
  return (m_modality == Modality::MR) ? m_caseViewModel.mrLabel() : m_caseViewModel.usLabel();
}

void SegmentViewModel::setModality(Modality modality)
{
  m_modality = modality;
  refreshPreview();
}

void SegmentViewModel::setSliceZ(int z)
{
  m_sliceZ = z;
  refreshPreview();
}

void SegmentViewModel::setSeedX(int x)
{
  m_seedX = x;
}

void SegmentViewModel::setSeedY(int y)
{
  m_seedY = y;
}

void SegmentViewModel::setLower(int lower)
{
  m_lower = lower;
}

void SegmentViewModel::setUpper(int upper)
{
  m_upper = upper;
}

void SegmentViewModel::setShowGroundTruth(bool show)
{
  m_showGroundTruth = show;
  runSegmentation();
}

void SegmentViewModel::onCaseLoaded()
{
  auto vol = currentVolume();
  if (!vol)
  {
    return;
  }
  emit hasLabelsChanged(m_caseViewModel.hasLabels());
  if (!m_caseViewModel.hasLabels())
  {
    m_showGroundTruth = false;
  }

  auto size = vol->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(size[2]) - 1;
  m_sliceZ = maxZ / 2;
  m_seedX = static_cast<int>(size[0]) / 2;
  m_seedY = static_cast<int>(size[1]) / 2;
  emit sliceRangeChanged(maxZ, m_sliceZ);
  emit seedRangeChanged(static_cast<int>(size[0]) - 1, static_cast<int>(size[1]) - 1);
  refreshPreview();
}

void SegmentViewModel::refreshPreview()
{
  auto vol = currentVolume();
  if (!vol)
  {
    return;
  }
  // Re-clamp on every modality switch: MR/US have different voxel dimensions, so ranges
  // valid for one modality can be out-of-bounds for the other (a real bug this app hit before).
  auto size = vol->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(size[2]) - 1;
  m_sliceZ = std::min(m_sliceZ, maxZ);
  m_seedX = std::min(m_seedX, static_cast<int>(size[0]) - 1);
  m_seedY = std::min(m_seedY, static_cast<int>(size[1]) - 1);
  emit sliceRangeChanged(maxZ, m_sliceZ);
  emit seedRangeChanged(static_cast<int>(size[0]) - 1, static_cast<int>(size[1]) - 1);

  auto slice = model::sliceOf(vol, m_sliceZ);
  emit viewImageChanged(model::grayToQImage(slice));
  emit diceTextChanged("Dice: 尚未运行");
}

void SegmentViewModel::runSegmentation()
{
  auto vol = currentVolume();
  auto labelVol = currentLabelVolume();
  if (!vol)
  {
    return;
  }
  auto volSize = vol->GetLargestPossibleRegion().GetSize();
  int z = std::min(m_sliceZ, static_cast<int>(volSize[2]) - 1);
  auto slice = model::sliceOf(vol, z);

  model::Slice2DType::IndexType seed;
  seed[0] = std::min(m_seedX, static_cast<int>(volSize[0]) - 1);
  seed[1] = std::min(m_seedY, static_cast<int>(volSize[1]) - 1);

  model::LabelSlice2DType::Pointer ourMask;
  try
  {
    ourMask = model::runConnectedThreshold(slice, seed, m_lower, m_upper);
  }
  catch (const itk::ExceptionObject & err)
  {
    emit diceTextChanged(QString("区域生长出错(种子点/阈值可能无效): ") + err.GetDescription());
    return;
  }

  if (m_showGroundTruth && labelVol)
  {
    auto gtSlice = model::labelSliceOf(labelVol, z);

    QString overlayError;
    QImage composite = model::overlaySegmentationVsGroundTruth(slice, ourMask, gtSlice, &overlayError);
    if (!overlayError.isEmpty())
    {
      emit diceTextChanged(overlayError);
      emit viewImageChanged(model::grayToQImage(slice));
      return;
    }
    emit viewImageChanged(composite);

    QString diceError;
    double dice = model::computeDice(ourMask, gtSlice, &diceError);
    if (!diceError.isEmpty())
    {
      emit diceTextChanged(diceError);
    }
    else
    {
      emit diceTextChanged(QString("Dice(本次区域生长 vs 真实标注) = %1  (此切片,2D,仅供参考——真正的 3D Dice 见'配准'标签页)")
                                .arg(dice));
    }
  }
  else
  {
    emit viewImageChanged(model::overlayToQImage(slice, ourMask, 0));
    emit diceTextChanged("Dice: 勾选'叠加真实标注'后显示");
  }
}
