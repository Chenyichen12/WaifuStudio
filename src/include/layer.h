//
// Created by chen_yichen on 2024/7/30.
//

#pragma once
#include <QString>
class Layer {
protected:
  QString name;
  int id;
  bool isvisible;

public:
  explicit Layer(const QString &name, bool isVisible = true);
  Layer(const Layer& l) = delete;

  ~Layer();
  const QString &getName() const;
  int getId() const;
  bool isVisible() const;
  virtual bool isContainer() = 0;
};
