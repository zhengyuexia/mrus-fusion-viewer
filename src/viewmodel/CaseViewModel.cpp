#include "CaseViewModel.h"

#include <itkMacro.h>

#include <sstream>

#ifndef MRUS_VIEWER_DATA_DIR
#error "MRUS_VIEWER_DATA_DIR must be defined by CMakeLists.txt"
#endif

CaseViewModel::CaseViewModel(QObject * parent) : QObject(parent) {}

void CaseViewModel::setDataset(model::Dataset dataset)
{
  m_dataset = dataset;
}

void CaseViewModel::setCaseIndex(int index)
{
  m_caseIndex = index;
}

void CaseViewModel::load()
{
  try
  {
    m_data = model::loadCase(m_dataset, m_caseIndex, QString(MRUS_VIEWER_DATA_DIR));
  }
  catch (const itk::ExceptionObject & err)
  {
    m_loaded = false;
    emit loadFailed(QString::fromStdString(err.GetDescription()));
    return;
  }
  m_loaded = true;
  emit caseLoaded();
}

QString CaseViewModel::infoText() const
{
  if (!m_loaded)
  {
    return "Not loaded yet";
  }
  auto mrSize = m_data.mrImage->GetLargestPossibleRegion().GetSize();
  auto usSize = m_data.usImage->GetLargestPossibleRegion().GetSize();
  auto mrSpacing = m_data.mrImage->GetSpacing();
  auto usSpacing = m_data.usImage->GetSpacing();

  std::ostringstream info;
  info << (m_data.hasLabels ? "Prostate / labeled" : "BITE Brain / unlabeled") << "  |  MR: " << mrSize[0] << "x" << mrSize[1]
       << "x" << mrSize[2] << " @ " << mrSpacing[0] << "mm" << "  |  US: " << usSize[0] << "x" << usSize[1] << "x"
       << usSize[2] << " @ " << usSpacing[0] << "mm";
  return QString::fromStdString(info.str());
}
