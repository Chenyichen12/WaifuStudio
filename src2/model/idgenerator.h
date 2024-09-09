#pragma once
class QMutex;
class IdGenerator {
 private:
  int currentId = 0;
  IdGenerator();
  QMutex* mutex;
 public:
  static IdGenerator& getInstance();
   int generate();
};