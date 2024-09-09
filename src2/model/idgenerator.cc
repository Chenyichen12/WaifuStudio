#include "idgenerator.h"

#include <QMutexLocker>
IdGenerator::IdGenerator() { this->mutex = new QMutex(); }

IdGenerator& IdGenerator::getInstance() {
  static auto instance = IdGenerator();
  return instance;
}

int IdGenerator::generate() {
  QMutexLocker(this->mutex);
  return currentId++;
}
