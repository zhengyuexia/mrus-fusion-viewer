#pragma once

#include "CaseViewModel.h"
#include "../model/Types.h"

#include <QObject>
#include <QImage>
#include <QString>

class FilterViewModel : public QObject
{
  Q_OBJECT
public:
  explicit FilterViewModel(CaseViewModel & caseViewModel, QObject * parent = nullptr);

  enum class Modality
  {
    MR,
    US
  };

public slots:
  void setModality(Modality modality);
  void setSliceZ(int z);
  void setIterations(int iterations);
  void setTimeStep(double timeStep);
  void runFilter();

signals:
  void sliceRangeChanged(int maxZ, int currentZ);
  void beforeImageChanged(QImage image);
  void afterImageChanged(QImage image);
  void statsTextChanged(QString text);
  void errorOccurred(QString message);

private:
  void refreshBeforePreview();
  void onCaseLoaded();
  model::Volume3DType::Pointer currentVolume() const;

  CaseViewModel & m_caseViewModel;
  Modality m_modality = Modality::MR;
  int m_sliceZ = 0;
  int m_iterations = 10;
  double m_timeStep = 0.125;
};
