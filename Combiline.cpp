#include "Combiline.h"

#include "Data.h"
#include "Debug.h"
#include "Utils.h"

#include <string.h>
#include <stdlib.h>


NewCombiline::NewCombiline():
  m_par_function(nullptr),
  m_selected_value(nullptr)
{   
  // TODO vul iets onnozel in om crash te voorkomen
}

void NewCombiline::Init(NewCombilineParameters par_function, uint8_t* selected_value)
{
  m_par_function = par_function;
  m_selected_value = selected_value;
}


PSTRING(PSTR_combiline_error, "ERROR"); 

void NewCombiline::GetText(char* text, uint8_t text_len, Screen::Inversion& inversion, uint8_t start, uint8_t len, uint8_t extra_padding, bool right_align) 
/*
- text: this function will write to this string
- text_len: this should be the length of the text* buffer (not including \0), it is a safety feature 
- inversion: this will be filled by this function
- start: we will start writing at text[start] .. 
- len: ... for length len, if there are not enough chars to be written, spaces are written
- extra_padding: number of extra space to be added at the end
- right_align: if enabled, value is aligned to the right

NOTE: Wanted to use sprintf with "%*.*s", but the variable width specifiers don't appear 
      to be supported by the standard library. You can however use constant width 
      specificiers such as "%5.8s".
*/
{
  inversion = Screen::inversion_all;

  // Do some checks
  if (!m_par_function || start + len + extra_padding > text_len ) { 
      strncpy(text, GetPString(PSTR_combiline_error), text_len);
      return;
    }

  text += start; // we start writing to text at the start position

  // ----
  NewParsPars m;
  m_par_function(m);
  // ----

  // Key:
  const char* key = "";  // TODO this "" does not take extra memory, so could remove PSTRING_EMPTY
  if (m.types & TypeCString) {
    key = static_cast<const char*>(m.name);
  } else if (m.types & TypePString) {
    key = GetPString(static_cast<const char*>(m.name));
  }
  strncpy(text, key, len);
  text[len] = 0;

  // Value
  if (m.number_of_values != 0) {
    const char* value = "";
    if (m.types & TypePTable) {
      const char *const * ptable = static_cast<const char *const *>(m.values);
      value = GetPStringFromPTable(ptable, *m_selected_value);
    } else if (m.types & TypeFunction) {
      NewCombiLineFunction function = (NewCombiLineFunction)m.values;
      value = function(*m_selected_value);
    }
    uint8_t inv_start = 0, inv_stop = 0;
    if (right_align) {
      PadRight(text, len - strlen(text)); // Pad end with spaces
      const uint8_t pos = len - strlen(value); // We expect the length of value to be < 24
      strcpy(text + pos, value);
      inv_start = pos;
      inv_stop = len - 1;
    } else {
      inv_start = strlen(text);
      const uint8_t freechar = len - strlen(text); // can never be negative
      strncat(text, value, freechar); // no need to terminate with zero, because already done above
      inv_stop = strlen(text) - 1;
      PadRight(text, len - strlen(text)); // Pad end with spaces
    }
    inversion = {Screen::InvertGiven, inv_start + start, inv_stop + start};
  } 

  // Add extra_padding 
  PadRight(text, extra_padding);

  return;
}


uint8_t* NewCombiline::GetSelectedValue()
{
  return m_selected_value;
}

bool NewCombiline::OnLeft() 
{
  if (*m_selected_value > 0) {
    (*m_selected_value)--;
    return true;
  } else {
    return false; 
  }
}

bool NewCombiline::OnRight() 
{
  // ----
  NewParsPars m;
  m_par_function(m);
  // ---

  if (*m_selected_value < m.number_of_values - 1) {
    (*m_selected_value)++;
    return true;
  } else {
    return false;
  }
}

void NewCombiline::UnitTest()
{
  // TODO: check all possibilities
}