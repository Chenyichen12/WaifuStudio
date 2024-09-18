#include "mophercommand.h"

#include <QUndoCommand>

namespace WaifuL2d {
MopherCommand::MopherCommand(AbstractMopher* mopher) : targetMopher(mopher) {}

}  // namespace WaifuL2d
