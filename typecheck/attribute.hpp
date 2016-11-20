#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

#include <cstddef>
#include <string>

// SymScope is declared in SymTab.cpp
class SymScope;

enum Basetype
{
    bt_undef,

    bt_integer,
    bt_intptr,
    bt_boolean,
    bt_char,
    bt_charptr,
    bt_procedure,
    bt_ptr,             // Used by nullptr
    bt_string
};


class Attribute
{
  public:
  Basetype m_basetype;  // Type of the subtree
  int m_place;          // Register where this value is stored
  SymScope* m_scope;    // The scope of the current symbol
  int lineno;           // Line number on which that ast node resides

  Attribute() {
    m_basetype = bt_undef;
    lineno = 0;
    m_scope = NULL;
    int m_place = -1;   // Should never actually be negative
  }

  std::string basetype() {
    switch(m_basetype) {
      case bt_integer: return "bt_integer";
      case bt_intptr: return "bt_intptr";
      case bt_boolean: return "bt_boolean";
      case bt_char: return "bt_char";
      case bt_charptr: return "bt_charptr";
      case bt_procedure: return "bt_procedure";
      case bt_ptr: return "bt_ptr";             // Used by nullptr
      case bt_string: return "bt_string";
      default:   return "undefined";
    }
  }

};

#endif //ATTRIBUTE_HPP

