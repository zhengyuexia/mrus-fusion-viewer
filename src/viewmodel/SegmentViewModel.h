#pragma once

#include "CaseViewModel.h"
#include "../model/Types.h"

#include <QObject>
#include <QImage>
#include <QString>

class SegmentViewModel : public QObject
{
  Q_OBJECT
public:
  explicit SegmentViewModel(CaseViewModel & caseViewModel, QObject * parent = nullptr);

  enum class Modality
  {
    MR,
    US
  };

public slots:
  void setModality(Modality modality);
  void setSliceZ(int z);
  void setSeedX(int x);
  void setSeedY(int y);
  void setLower(int lower);
  void setUpper(int upper);
  void setShowGroundTruth(bool show);
  void runSegmentation();

signals:
  void sliceRangeChanged(int maxZ, int currentZ);
  void seedRangeChanged(int maxX, int maxY);
  void viewImageChanged(QImage image);
  void diceTextChanged(QString text);
  void hasLabelsChanged(bool hasLabels);

private:
  void onCaseLoaded();
  void refreshPreview();
  model::Volume3DType::Pointer currentVolume() const;
  model::Label3DType::Pointer currentLabelVolume() const;

  CaseViewModel & m_caseViewModel;
  Modality m_modality = Modality::MR;
  int m_sliceZ = 0;
  int m_seedX = 0;
  int m_seedY = 0;
  int m_lower = 100;
  int m_upper = 255;
  bool m_showGroundTruth = false;
};
