#pragma once

#include "Types.h"

#include <QString>

namespace model
{
enum class Dataset
{
  Prostate,
  BrainBite
};

struct CasePaths
{
  QString mrImage;
  QString usImage;
  QString mrLabel; // empty string means no segmentation available
  QString usLabel;
};

struct CaseData
{
  Volume3DType::Pointer mrImage;
  Volume3DType::Pointer usImage;
  Label3DType::Pointer mrLabel;
  Label3DType::Pointer usLabel;
  bool hasLabels = false;
};

// dataRoot is the project's own bundled data/ directory (see QTVIEWER_DATA_DIR in CMakeLists.txt) —
// never a hardcoded absolute path, so the repo still runs after a fresh clone elsewhere.
CasePaths resolveCasePaths(Dataset dataset, int caseIndex, const QString & dataRoot);

Volume3DType::Pointer readVolume(const QString & path);
Label3DType::Pointer readLabelVolume(const QString & path);

// Throws itk::ExceptionObject on read failure — caller decides how to surface it.
CaseData loadCase(Dataset dataset, int caseIndex, const QString & dataRoot);

// Only the Prostate dataset has multiple bundled cases.
constexpr int kProstateCaseCount = 3; // case000000..case000002
}
