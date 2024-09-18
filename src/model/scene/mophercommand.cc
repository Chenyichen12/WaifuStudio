#include "mophercommand.h"

#include <QUndoCommand>

namespace WaifuL2d {
MopherCommand::MopherCommand(AbstractDeformer* mopher) : targetMopher(mopher) {}

}  // namespace WaifuL2d
