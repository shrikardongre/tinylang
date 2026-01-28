#ifndef TINYLANG_LEXER_LEXER_H
#define TINYLANG_LEXER_LEXER_H

#include "../Basic/Diagnostic.h"
#include "../Basic/LLVM.h"
#include "./Token.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h" 

namespace tinylang {
/*
There are far more keywords now. How can we handle this? A simple
and fast solution is to populate a hash table with the keywords, which are all stored in the
TokenKinds.def file.
*/
class KeywordFilter {
    llvm::StringMap <tok::TokenKind> keyword_table;
    void addKeyword (llvm::StringRef Keyword, tok::TokenKind TokenCode) ;

public:
    void addKeywords();
    tok::TokenKind getKeyword(llvm::StringRef Name, tok::TokenKind DefaultTokenCode= tok::unknown) {
        auto Result = keyword_table.find(Name);
        if(Result!=keyword_table.end()){
            return Result->second;
        }
        return DefaultTokenCode;
    }
};

class Lexer {
    llvm::SourceMgr &SrcMgr ;
    DiagnosticsEngine &Diags;

    const char* currPtr;
    llvm::StringRef CurBuf;

    unsigned CurBuffer = 0;

    KeywordFilter Keywords;

public:
  Lexer(SourceMgr &SrcMgr, DiagnosticsEngine &Diags)
      : SrcMgr(SrcMgr), Diags(Diags) {
    CurBuffer = SrcMgr.getMainFileID();
    CurBuf = SrcMgr.getMemoryBuffer(CurBuffer)->getBuffer();
    currPtr = CurBuf.begin();
    Keywords.addKeywords();
  }

  DiagnosticsEngine &getDiagnostics() const {
    return Diags;
  }

  /// Returns the next token from the input.
  void next(Token &Result);

  /// Gets source code buffer.
  StringRef getBuffer() const { return CurBuf; }

private:
  void identifier(Token &Result);
  void number(Token &Result);
  void string(Token &Result);
  void comment();

  SMLoc getLoc() { return SMLoc::getFromPointer(currPtr); }

  void formToken(Token &Result, const char *TokEnd,
                 tok::TokenKind Kind);
};
} // namespace tinylang
#endif
