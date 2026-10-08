#include "CaseViewModel.h"

#include <itkMacro.h>

#include <sstream>

#ifndef QTVIEWER_DATA_DIR
#error "QTVIEWER_DATA_DIR must be defined by CMakeLists.txt"
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
    m_data = model::loadCase(m_dataset, m_caseIndex, QString(QTVIEWER_DATA_DIR));
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
    return "尚未加载";
  }
  auto mrSize = m_data.mrImage->GetLargestPossibleRegion().GetSize();
  auto usSize = m_data.usImage->GetLargestPossibleRegion().GetSize();
  auto mrSpacing = m_data.mrImage->GetSpacing();
  auto usSpacing = m_data.usImage->GetSpacing();

  std::ostringstream info;
  info << (m_data.hasLabels ? "前列腺/有标注" : "BITE脑部/无标注") << "  |  MR: " << mrSize[0] << "x" << mrSize[1]
       << "x" << mrSize[2] << " @ " << mrSpacing[0] << "mm" << "  |  US: " << usSize[0] << "x" << usSize[1] << "x"
       << usSize[2] << " @ " << usSpacing[0] << "mm";
  return QString::fromStdString(info.str());
}
