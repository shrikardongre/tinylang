#ifndef TINYLANG_SEMA_SCOPE_H
#define TINYLANG_SEMA_SCOPE_H

#include "../Basic/LLVM.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

namespace tinylang {

// This class refers to the declaration .
class Decl;

class Scope {
  // This thing is allowed in C++ it is nothing but a pointer to Scope that will
  // tell the parent of current Decl.
  Scope *Parent;
  llvm::StringMap<Decl *> Symbols;

public:
  Scope(Scope *Parent = nullptr) : Parent(Parent) {};

  // Pretty obvious general purpose functions .

  bool insert(Decl *Declaration);
  Decl *lookup(StringRef Name);

  Scope *getParent() { return Parent; 

}


};

} // namespace tinylang

#endif