Example: parsing VAR x := 10; 

Parser::parse()
  └─ consume() → Lexer::next() → produces kw_VAR
Parser::parseModule()
  └─ expects kw_VAR
  └─ consume() → Lexer::next() → identifier
  └─ consume() → Lexer::next() → colonequal
  └─ consume() → Lexer::next() → integer_literal
  └─ consume() → Lexer::next() → semi
