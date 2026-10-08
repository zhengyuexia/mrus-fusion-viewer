#include "CaseRepository.h"

#include <itkImageFileReader.h>

namespace model
{
CasePaths resolveCasePaths(Dataset dataset, int caseIndex, const QString & dataRoot)
{
  CasePaths p;
  if (dataset == Dataset::Prostate)
  {
    const QString root = dataRoot + "/prostate";
    const QString id = QString("case%1").arg(caseIndex, 6, 10, QChar('0'));
    p.mrImage = root + "/mr_images/" + id + ".nii.gz";
    p.usImage = root + "/us_images/" + id + ".nii.gz";
    p.mrLabel = root + "/mr_labels/" + id + ".nii.gz";
    p.usLabel = root + "/us_labels/" + id + ".nii.gz";
  }
  else
  {
    const QString root = dataRoot + "/bite_nifti";
    p.mrImage = root + "/05_mr.nii.gz";
    p.usImage = root + "/05_us.nii.gz";
    // BITE only has sparse expert landmarks, not per-voxel masks — mrLabel/usLabel stay empty.
  }
  return p;
}

Volume3DType::Pointer readVolume(const QString & path)
{
  using ReaderType = itk::ImageFileReader<Volume3DType>;
  auto reader = ReaderType::New();
  reader->SetFileName(path.toStdString());
  reader->Update();
  return reader->GetOutput();
}

Label3DType::Pointer readLabelVolume(const QString & path)
{
  using ReaderType = itk::ImageFileReader<Label3DType>;
  auto reader = ReaderType::New();
  reader->SetFileName(path.toStdString());
  reader->Update();
  return reader->GetOutput();
}

CaseData loadCase(Dataset dataset, int caseIndex, const QString & dataRoot)
{
  const CasePaths paths = resolveCasePaths(dataset, caseIndex, dataRoot);
  CaseData data;
  data.hasLabels = !paths.mrLabel.isEmpty() && !paths.usLabel.isEmpty();

  data.mrImage = readVolume(paths.mrImage);
  data.usImage = readVolume(paths.usImage);
  data.mrLabel = data.hasLabels ? readLabelVolume(paths.mrLabel) : nullptr;
  data.usLabel = data.hasLabels ? readLabelVolume(paths.usLabel) : nullptr;
  return data;
}
}
