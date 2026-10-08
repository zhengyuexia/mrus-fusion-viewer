#include "RegisterViewModel.h"

#include "../model/ImageSlicing.h"
#include "../model/ImageConversion.h"
#include "../model/RegistrationService.h"

#include <itkMacro.h>

#include <sstream>

RegisterViewModel::RegisterViewModel(CaseViewModel & caseViewModel, QObject * parent)
    : QObject(parent), m_caseViewModel(caseViewModel)
{
  connect(&m_caseViewModel, &CaseViewModel::caseLoaded, this, &RegisterViewModel::onCaseLoaded);
}

void RegisterViewModel::onCaseLoaded()
{
  m_transform = nullptr;
  m_alignedUsImage = nullptr;
  m_alignedUsLabel = nullptr;
  emit statusTextChanged("尚未运行配准");
  emit fixedSliceChanged(QImage());
  emit alignedSliceChanged(QImage());

  auto mr = m_caseViewModel.mrImage();
  if (mr)
  {
    auto size = mr->GetLargestPossibleRegion().GetSize();
    int maxZ = static_cast<int>(size[2]) - 1;
    m_sliceZ = maxZ / 2;
    emit sliceRangeChanged(maxZ, m_sliceZ);
  }
}

void RegisterViewModel::setSliceZ(int z)
{
  m_sliceZ = z;
  refreshSliceViews();
}

void RegisterViewModel::refreshSliceViews()
{
  auto mr = m_caseViewModel.mrImage();
  if (!mr || !m_alignedUsImage)
  {
    return;
  }
  emit fixedSliceChanged(model::grayToQImage(model::sliceOf(mr, m_sliceZ)));
  emit alignedSliceChanged(model::grayToQImage(model::sliceOf(m_alignedUsImage, m_sliceZ)));
}

void RegisterViewModel::runRegistration()
{
  auto mr = m_caseViewModel.mrImage();
  auto us = m_caseViewModel.usImage();
  if (!mr || !us)
  {
    return;
  }
  emit statusTextChanged("配准中,请稍候(3D 刚性 + 互信息 + 多分辨率金字塔,可能需要几十秒)...");

  model::RegistrationInput input;
  input.mrImage = mr;
  input.usImage = us;
  input.mrLabel = m_caseViewModel.mrLabel();
  input.usLabel = m_caseViewModel.usLabel();
  input.hasLabels = m_caseViewModel.hasLabels();

  model::RegistrationResult result;
  try
  {
    result = model::runRigidRegistration(input);
  }
  catch (const itk::ExceptionObject & err)
  {
    emit statusTextChanged(QString("配准出错: ") + err.GetDescription());
    return;
  }

  m_transform = result.transform;
  m_alignedUsImage = result.alignedUsImage;
  m_alignedUsLabel = result.alignedUsLabel;

  std::ostringstream ss;
  ss << "完成:iterations=" << result.iterations << ", final MI=" << result.finalMetricValue << "\n旋转(versor)=("
     << result.versor[0] << "," << result.versor[1] << "," << result.versor[2] << ")  平移(mm)=("
     << result.translationMm[0] << "," << result.translationMm[1] << "," << result.translationMm[2] << ")";

  if (result.diceAfter.has_value())
  {
    ss << "\nDice(配准后) = " << *result.diceAfter << "   Dice(配准前基线,Identity) = " << *result.diceBefore
       << "\n(诚实提示:这批真实前列腺数据上,刚性配准常常不如不配准——探头压迫导致的组织形变,"
          "刚性变换表达不了,详见 notes/10_prostate_registration_3d.md)";
  }
  else
  {
    ss << "\n(此数据集没有分割标注,无法算 Dice——只能看 MI 分数和肉眼在'融合'标签页核对对齐质量)";
  }
  emit statusTextChanged(QString::fromStdString(ss.str()));

  auto mrSize = mr->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(mrSize[2]) - 1;
  m_sliceZ = maxZ / 2;
  emit sliceRangeChanged(maxZ, m_sliceZ);
  refreshSliceViews();
  emit resultReady();
}
