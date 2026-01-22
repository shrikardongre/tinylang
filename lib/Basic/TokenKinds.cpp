#include "../../include/tinylang/Basic/TokenKinds.h"
#include "llvm/Support/ErrorHandling.h"

using namespace tinylang;
// Making a readonly array.
static const char* const TokNames[]={
    // This #ID converts it to "ID" that is stringification of the ID.
#define TOK(ID) #ID,
#define KEYWORD(ID, FLAG) #ID,
#include "../../include/tinylang/Basic/TokenKinds.def"
  nullptr 
} ;

const char *tok::getTokenName(TokenKind Kind) {
  if (Kind < tok::NUM_TOKENS)
    return TokNames[Kind];
  llvm_unreachable("unknown TokenKind");
  return nullptr;
}

const char *tok::getPunctuatorSpelling(TokenKind Kind) {
  switch (Kind) {
#define PUNCTUATOR(ID, SP) case ID: return SP;
#include "../../include/tinylang/Basic/TokenKinds.def"
    default: break;
  }
  return nullptr;
}

const char *tok::getKeywordSpelling(TokenKind Kind) {
  switch (Kind) {
    // This kw_ ## ID is token pasting that concatenates the token at processing time .
    /*
    Example : #define MAKE(x) foo_##x
              MAKE(bar)
    becomes 
    foo_bar
    
    In .def file 

    #define KEYWORD(ID, FLAG) TOK(kw_ ## ID) 
    so , KEYWORD(IF, KEYALL) --> TOK(kw_IF) --> kw_IF 
    The whole conversion is seen as 
    case kw_IF: return "IF"; 
    */ 
    
#define KEYWORD(ID, FLAG) case kw_ ## ID: return #ID;
#include "../../include/tinylang/Basic/TokenKinds.def"
    default: break;
  }
  return nullptr;
}

