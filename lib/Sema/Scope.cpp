#include "../../include/tinylang/Sema/Scope.h"
#include "../../include/tinylang/AST/AST.h"

using namespace tinylang;

bool Scope::insert(Decl *Declaration) {
  return Symbols
      .insert(std::pair<StringRef, Decl *>(Declaration->getName(), Declaration))
      .second;
}

// This is some elite level C++ I wanna write on my own someday :)

Decl *Scope::lookup(StringRef Name) {
  Scope *S = this;
  // this here means the current scope object on which the scope what called .
  // We iterate to find the name in the current scope if not found we go to the
  // parent scope .

  while (S) {
    StringMap<Decl *>::const_iterator I = S->Symbols.find(Name);
    if (I != S->Symbols.end())
      return I->second;
    S = S->getParent();
  }
  return nullptr;

  // example :
  // Scope *Global = new Scope(nullptr);
  // Scope *Function = new Scope(Global);
  // Scope *Block = new Scope(Function)

  // Block.Parent    → Function
  // Function.Parent → Global
  // Global.Parent   → nullptr

}
