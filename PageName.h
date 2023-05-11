/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#pragma once

#include "Page.h"
#include "Utils.h"

class PageName: public Page
{
public:
  PageName(bool complete_char_set = true);
protected:
  virtual const char* GetTitle() = 0;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual bool OnUpDown(UpDownAction action) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
protected:
  enum Result { NONE, ACCEPT, REJECT };
  void SetValues(const char* name, uint8_t number, uint8_t number_max, const char* number_prefix);
  Result GetResult();
  void GetNameAndValue(char* name, uint8_t& number); // copies current name into name string, and sets the current number to number
private:
  uint8_t GetMaxPositions();
  uint8_t GetPositionOfNumber();
  uint8_t GetPositionOfAccept();
  uint8_t GetPositionOfCancel();
private:
  uint8_t m_number;
  uint8_t m_number_max;
  uint8_t m_number_prefix_len;
private:
  NameEditor m_name;
  Result m_result;
  uint8_t m_position;
};


