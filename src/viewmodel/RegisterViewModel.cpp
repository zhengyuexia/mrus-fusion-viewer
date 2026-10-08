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
  emit statusTextChanged("Registration not run yet");
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
  emit statusTextChanged("Registering, please wait (3D rigid + mutual information + multi-resolution pyramid, may take tens of seconds)...");

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
    emit statusTextChanged(QString("Registration failed: ") + err.GetDescription());
    return;
  }

  m_transform = result.transform;
  m_alignedUsImage = result.alignedUsImage;
  m_alignedUsLabel = result.alignedUsLabel;

  std::ostringstream ss;
  ss << "Done: iterations=" << result.iterations << ", final MI=" << result.finalMetricValue << "\nRotation (versor)=("
     << result.versor[0] << "," << result.versor[1] << "," << result.versor[2] << ")  Translation (mm)=("
     << result.translationMm[0] << "," << result.translationMm[1] << "," << result.translationMm[2] << ")";

  if (result.diceAfter.has_value())
  {
    ss << "\nDice (after registration) = " << *result.diceAfter << "   Dice (before registration, Identity baseline) = " << *result.diceBefore
       << "\n(Honest caveat: on this real prostate dataset, rigid registration often does no better than no "
          "registration at all -- probe-induced tissue deformation isn't something a rigid transform can "
          "express; see notes/10_prostate_registration_3d.md)";
  }
  else
  {
    ss << "\n(This dataset has no segmentation labels, so Dice can't be computed -- only the MI score and a "
          "visual check in the 'Fusion' tab are available)";
  }
  emit statusTextChanged(QString::fromStdString(ss.str()));

  auto mrSize = mr->GetLargestPossibleRegion().GetSize();
  int maxZ = static_cast<int>(mrSize[2]) - 1;
  m_sliceZ = maxZ / 2;
  emit sliceRangeChanged(maxZ, m_sliceZ);
  refreshSliceViews();
  emit resultReady();
}
