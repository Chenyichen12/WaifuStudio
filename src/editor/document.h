#ifndef EDITOR_DOCUMENT_H_
#define EDITOR_DOCUMENT_H_
#include <memory>
#include <string>
namespace editor {
class Document {
  std::string _file_path;

public:
  static std::unique_ptr<Document> LoadFromPath(const std::string &path);
};
} // namespace editor

#endif // EDITOR_DOCUMENT_H_