#include "../../include/tinylang/Basic/Lexer/Lexer.h"
using namespace tinylang;

void KeywordFilter::addKeyword(StringRef Keyword, tok::TokenKind TokenCode) {
  keyword_table.insert(std::make_pair(Keyword, TokenCode));
}

void KeywordFilter::addKeywords() {
#define KEYWORD(NAME, FLAGS) addKeyword(StringRef(#NAME), tok::kw_##NAME);
#include "../../include/tinylang/Basic/TokenKinds.def"
}

namespace charinfo {
LLVM_READNONE inline bool isASCII(char Ch) {
  return static_cast<unsigned char>(Ch) <= 127;
}

LLVM_READNONE inline bool isVerticalWhitespace(char Ch) {
  return isASCII(Ch) && (Ch == '\r' || Ch == '\n');
}

LLVM_READNONE inline bool isHorizontalWhitespace(char Ch) {
  return isASCII(Ch) && (Ch == ' ' || Ch == '\t' || Ch == '\f' || Ch == '\v');
}

LLVM_READNONE inline bool isWhitespace(char Ch) {
  return isHorizontalWhitespace(Ch) || isVerticalWhitespace(Ch);
}

LLVM_READNONE inline bool isDigit(char Ch) {
  return isASCII(Ch) && Ch >= '0' && Ch <= '9';
}

LLVM_READNONE inline bool isHexDigit(char Ch) {
  return isASCII(Ch) && (isDigit(Ch) || (Ch >= 'A' && Ch <= 'F'));
}

LLVM_READNONE inline bool isIdentifierHead(char Ch) {
  return isASCII(Ch) &&
         (Ch == '_' || (Ch >= 'A' && Ch <= 'Z') || (Ch >= 'a' && Ch <= 'z'));
}

LLVM_READNONE inline bool isIdentifierBody(char Ch) {
  return isIdentifierHead(Ch) || isDigit(Ch);
}
} // namespace charinfo

void Lexer ::next(Token &Result) {

  while (*currPtr && charinfo::isWhitespace(*currPtr)) {
    ++currPtr;
  }

  if (!*currPtr) {
    Result.setKind(tok::eof);
    return;
  }

  if (charinfo::isIdentifierHead(*currPtr)) {
    identifier(Result);
    return;
  } else if (charinfo::isDigit(*currPtr)) {
    number(Result);
    return;
  } else if (*currPtr == '"' || *currPtr == '\'') {
    string(Result);
    return;
  }

  else {
    switch (*currPtr) {
#define CASE(ch, tok)                                                          \
  case ch:                                                                     \
    formToken(Result, currPtr + 1, tok);                                       \
    break

      CASE('=', tok::equal);
      CASE('#', tok::hash);
      CASE('+', tok::plus);
      CASE('-', tok::minus);
      CASE('*', tok::star);
      CASE('/', tok::slash);
      CASE(',', tok::comma);
      CASE('.', tok::period);
      CASE(';', tok::semi);
      CASE(')', tok::r_paren);

#undef CASE

    case '(':
      if (*(currPtr + 1) == '*') {
        comment();
        next(Result);
      } else
        formToken(Result, currPtr + 1, tok::l_paren);
      break;

    case ':':
      if (*(currPtr + 1) == '=')
        formToken(Result, currPtr + 2, tok::colonequal);
      else
        formToken(Result, currPtr + 1, tok::colon);
      break;

    case '<':
      if (*(currPtr + 1) == '=')
        formToken(Result, currPtr + 2, tok::lessequal);
      else
        formToken(Result, currPtr + 1, tok::less);
      break;

    case '>':
      if (*(currPtr + 1) == '=')
        formToken(Result, currPtr + 2, tok::greaterequal);
      else
        formToken(Result, currPtr + 1, tok::greater);
      break;

    default:
      Result.setKind(tok::unknown);
    }
    return;
  }
}

void Lexer::string(Token &Result) {
  const char *Start = currPtr;
  const char *End = currPtr + 1;
  while (*End && *End != *Start && !charinfo::isVerticalWhitespace(*End))
    ++End;
  if (charinfo::isVerticalWhitespace(*End)) {
    Diags.report(getLoc(), diag::err_unterminated_char_or_string);
  }
  formToken(Result, End + 1, tok::string_literal);
}

void Lexer::identifier(Token &Result) {
  const char *Start = currPtr;
  const char *End = currPtr + 1;
  while (charinfo::isIdentifierBody(*End))
    ++End;
  StringRef Name(Start, End - Start);
  formToken(Result, End, Keywords.getKeyword(Name, tok::identifier));
}

void Lexer::number(Token &Result) {
  const char *End = currPtr + 1;
  tok::TokenKind Kind = tok::unknown;
  bool IsHex = false;
  while (*End) {
    if (!charinfo::isHexDigit(*End))
      break;
    if (!charinfo::isDigit(*End))
      IsHex = true;
    ++End;
  }
  switch (*End) {
  case 'H': /* hex number */
    Kind = tok::integer_literal;
    ++End;
    break;
  default: /* decimal number */
    if (IsHex)
      Diags.report(getLoc(), diag::err_hex_digit_in_decimal);
    Kind = tok::integer_literal;
    break;
  }
  formToken(Result, End, Kind);
}

void Lexer::comment() {
  const char *End = currPtr + 2;
  unsigned Level = 1;
  while (*End && Level) {
    // Check for nested comment.
    if (*End == '(' && *(End + 1) == '*') {
      End += 2;
      Level++;
    }
    // Check for end of comment
    else if (*End == '*' && *(End + 1) == ')') {
      End += 2;
      Level--;
    } else
      ++End;
  }
  if (!*End) {
    Diags.report(getLoc(), diag::err_unterminated_block_comment);
  }
  currPtr = End;
}

void Lexer::formToken(Token &Result, const char *TokEnd, tok::TokenKind Kind) {
  size_t TokLen = TokEnd - currPtr;
  Result.Ptr = currPtr;
  Result.length = TokLen;
  Result.Kind = Kind;
  currPtr = TokEnd;
}