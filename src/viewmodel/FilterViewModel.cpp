#include "FilterViewModel.h"

#include "../model/ImageSlicing.h"
#include "../model/ImageConversion.h"
#include "../model/DenoiseService.h"

#include <itkMacro.h>

#include <algorithm>
#include <sstream>

FilterViewModel::FilterViewModel(CaseViewModel & caseViewModel, QObject * parent)
    : QObject(parent), m_caseViewModel(caseViewModel)
{
  connect(&m_caseViewModel, &CaseViewModel::caseLoaded, this, &FilterViewModel::onCaseLoaded);
}

model::Volume3DType::Pointer FilterViewModel::currentVolume() const
{
  return (m_modality == Modality::MR) ? m_caseViewModel.mrImage() : m_caseViewModel.usImage();
}

void FilterViewModel::setModality(Modality modality)
{
  m_modality = modality;
  refreshBeforePreview();
}

void FilterViewModel::setSliceZ(int z)
{
  m_sliceZ = z;
  refreshBeforePreview();
}

void FilterViewModel::setIterations(int iterations)
{
  m_iterations = iterations;
}

void FilterViewModel::setTimeStep(double timeStep)
{
  m_timeStep = timeStep;
}

void FilterViewModel::onCaseLoaded()
{
  auto vol = currentVolume();
  if (!vol)
  {
    return;
  }
  auto size = vol->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(size[2]) - 1;
  m_sliceZ = maxZ / 2;
  emit sliceRangeChanged(maxZ, m_sliceZ);
  refreshBeforePreview();
  emit statsTextChanged("差值统计:尚未运行");
}

void FilterViewModel::refreshBeforePreview()
{
  auto vol = currentVolume();
  if (!vol)
  {
    return;
  }
  // MR and US have different voxel dimensions (e.g. 128 vs 88 slices) — clamp on every
  // modality switch, not just at load time, or an out-of-range Z reaches ExtractImageFilter.
  auto size = vol->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(size[2]) - 1;
  m_sliceZ = std::min(m_sliceZ, maxZ);

  auto slice = model::sliceOf(vol, m_sliceZ);
  emit beforeImageChanged(model::grayToQImage(slice));
  emit afterImageChanged(QImage());
  emit statsTextChanged("差值统计:尚未运行");
}

void FilterViewModel::runFilter()
{
  auto vol = currentVolume();
  if (!vol)
  {
    return;
  }
  auto size = vol->GetLargestPossibleRegion().GetSize();
  int z = std::min(m_sliceZ, static_cast<int>(size[2]) - 1);
  auto slice = model::sliceOf(vol, z);

  try
  {
    auto result = model::runCurvatureFlow(slice, static_cast<unsigned int>(m_iterations), m_timeStep);
    emit afterImageChanged(model::grayToQImage(result.denoised));

    std::ostringstream ss;
    ss << "差值统计(|原图-去噪后|): mean=" << result.meanAbsDiff << "  sigma=" << result.sigmaAbsDiff
       << "  max=" << result.maxAbsDiff << "  (调用 itk::CurvatureFlowImageFilter,PDE 保边去噪,见 01_denoise.md)";
    emit statsTextChanged(QString::fromStdString(ss.str()));
  }
  catch (const itk::ExceptionObject & err)
  {
    emit statsTextChanged(QString("去噪出错: ") + err.GetDescription());
  }
}
