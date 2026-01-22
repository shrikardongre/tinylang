#ifndef TINYLANG_LEXER_TOKEN_H
#define TINYLANG_LEXER_TOKEN_H

#include "../LLVM.h"
#include "../TokenKinds.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"

namespace tinylang{
    class Lexer;
    class Token{
        friend class Lexer;
        // This is a pointer to the start of the token .
        const char* Ptr;
        // This is the length of the token.
        size_t length ;

        tok::TokenKind Kind;
    public :
     tok::TokenKind getKind() const { return Kind; }
     void setKind(tok::TokenKind K) { Kind = K; }

     bool is(tok::TokenKind K) const { return Kind == K; }
     bool isNot(tok::TokenKind K) const { return Kind != K; }

      template <typename... Tokens>
      bool isOneOf(Tokens &&... Toks) const {
        return (... || is(Toks));
      }

      const char *getName() const {
    return tok::getTokenName(Kind);
  }

  SMLoc getLocation() const {
    return SMLoc::getFromPointer(Ptr);
  }
  size_t getLength() const { return length; }

  StringRef getIdentifier() {
    assert(is(tok::identifier) &&
           "Cannot get identfier of non-identifier");
    return StringRef(Ptr, length);
  }

  //What assert(expr && "message") really means 
  /*
  if (!(x)) {
  print("Assertion failed: x");
  abort();
  }
  */

    StringRef getLiteralData() {
    assert(isOneOf(tok::integer_literal,
                   tok::string_literal) &&
           "Cannot get literal data of non-literal");
    return StringRef(Ptr, length);
  }
    };

} // namespace tinylang 

#endif