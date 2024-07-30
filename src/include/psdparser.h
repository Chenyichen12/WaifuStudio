//
// Created by chen_yichen on 2024/7/30.
//

#pragma once
#include <QString>
#include <QObject>
namespace Parser {
class PsdParser:QObject {
  Q_OBJECT
private:
  QString path;
  int parseHeight = -1;
  int parseWidth = -1;

public:
  explicit PsdParser(const QString &path);
  void Parse();

  inline int height() const { return parseHeight; }
  inline int width() const { return parseWidth; }
};

} // namespace Parser
