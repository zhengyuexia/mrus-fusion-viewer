#pragma once

#include "../model/CaseRepository.h"

#include <QObject>
#include <QString>

class CaseViewModel : public QObject
{
  Q_OBJECT
public:
  explicit CaseViewModel(QObject * parent = nullptr);

  void setDataset(model::Dataset dataset);
  void setCaseIndex(int index);
  model::Dataset dataset() const { return m_dataset; }
  int caseIndex() const { return m_caseIndex; }

  model::Volume3DType::Pointer mrImage() const { return m_data.mrImage; }
  model::Volume3DType::Pointer usImage() const { return m_data.usImage; }
  model::Label3DType::Pointer mrLabel() const { return m_data.mrLabel; }
  model::Label3DType::Pointer usLabel() const { return m_data.usLabel; }
  bool hasLabels() const { return m_data.hasLabels; }
  bool isLoaded() const { return m_loaded; }
  const model::CaseData & caseData() const { return m_data; }
  QString infoText() const;

public slots:
  void load();

signals:
  void caseLoaded();
  void loadFailed(QString message);

private:
  model::Dataset m_dataset = model::Dataset::Prostate;
  int m_caseIndex = 0;
  model::CaseData m_data;
  bool m_loaded = false;
};
