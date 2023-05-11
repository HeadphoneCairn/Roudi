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

#include "Data.h"
#include "Page.h"

class PageMulti: public Page
{
public:
  PageMulti();
  uint8_t m_last_note;
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t which_multi = 0xFF) override;
  virtual void OnStop() override;
  virtual void OnTimeout() override;
  virtual void Draw(uint8_t from, uint8_t to) override;
private:
  uint8_t m_which; // number of this multi, 0 based 
  MultiValues m_values;
private:
  LineResult ActualLine(LineFunction func, uint8_t line, uint8_t field);
  void SaveIfModified();
  void SaveAs();
  void Remove();
  void MoveLeftOrRight(bool left);
  void New();
private:
  void SetMidiConfiguration();
};
