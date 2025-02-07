/*********************
 * leveling_menu.cpp *
 *********************/

/****************************************************************************
 *   Written By Mark Pelletier  2017 - Aleph Objects, Inc.                  *
 *   Written By Marcio Teixeira 2018 - Aleph Objects, Inc.                  *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation, either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   To view a copy of the GNU General Public License, go to the following  *
 *   location: <https://www.gnu.org/licenses/>.                              *
 ****************************************************************************/

#include "../config.h"

#if BOTH(TOUCH_UI_FTDI_EVE,HAS_LEVELING)

#include "screens.h"

#if BOTH(HAS_BED_PROBE,BLTOUCH)
  #include "../../../../../feature/bltouch.h"
#endif

using namespace FTDI;
using namespace ExtUI;
using namespace Theme;

#ifdef TOUCH_UI_PORTRAIT
  #define GRID_ROWS 9
  #define GRID_COLS 2
  #define TITLE_POS          BTN_POS(1,1), BTN_SIZE(2,1)
  #define LEVEL_BED_POS      BTN_POS(1,2), BTN_SIZE(2,1)
  #define LEVEL_AXIS_POS     BTN_POS(1,3), BTN_SIZE(2,1)
  #define SHOW_MESH_POS      BTN_POS(1,4), BTN_SIZE(2,1)
  #define BLTOUCH_TITLE_POS  BTN_POS(1,6), BTN_SIZE(2,1)
  #define BLTOUCH_RESET_POS  BTN_POS(1,7), BTN_SIZE(1,1)
  #define BLTOUCH_TEST_POS   BTN_POS(2,7), BTN_SIZE(1,1)
  #define BACK_POS           BTN_POS(1,9), BTN_SIZE(2,1)
#else
  #define GRID_ROWS 7
  #define GRID_COLS 2
  #define TITLE_POS          BTN_POS(1,1), BTN_SIZE(2,1)
  #define LEVEL_BED_POS      BTN_POS(1,2), BTN_SIZE(2,1)
  #define LEVEL_AXIS_POS     BTN_POS(1,3), BTN_SIZE(2,1)
  #define SHOW_MESH_POS      BTN_POS(1,4), BTN_SIZE(2,1)
  #define BLTOUCH_TITLE_POS  BTN_POS(1,5), BTN_SIZE(2,1)
  #define BLTOUCH_RESET_POS  BTN_POS(1,6), BTN_SIZE(1,1)
  #define BLTOUCH_TEST_POS   BTN_POS(2,6), BTN_SIZE(1,1)
  #define BACK_POS           BTN_POS(1,7), BTN_SIZE(2,1)
#endif

void LevelingMenu::onRedraw(draw_mode_t what) {
  if (what & BACKGROUND) {
    CommandProcessor cmd;
    cmd.cmd(CLEAR_COLOR_RGB(Theme::bg_color))
       .cmd(CLEAR(true,true,true))
       .tag(0);
  }

  if (what & FOREGROUND) {
    CommandProcessor cmd;
    cmd.font(font_large)
       .text(TITLE_POS, GET_TEXT_F(MSG_LEVELING))
       .font(font_medium).colors(normal_btn)
       .tag(2).button(LEVEL_BED_POS, GET_TEXT_F(MSG_LEVEL_BED))
    #if ENABLED(Z_STEPPER_AUTO_ALIGN) || defined(AXIS_LEVELING_COMMANDS)
       .tag(3).button(LEVEL_AXIS_POS, GET_TEXT_F(MSG_AUTOLEVEL_X_AXIS))
    #endif
       .enabled(ENABLED(HAS_MESH))
       .tag(4).button(SHOW_MESH_POS, GET_TEXT_F(MSG_SHOW_MESH));
    #if ENABLED(BLTOUCH)
      cmd.text(BLTOUCH_TITLE_POS, GET_TEXT_F(MSG_BLTOUCH))
         .tag(5).button(BLTOUCH_RESET_POS, GET_TEXT_F(MSG_BLTOUCH_RESET))
         .tag(6).button(BLTOUCH_TEST_POS,  GET_TEXT_F(MSG_BLTOUCH_SELFTEST));
    #endif
    cmd.colors(action_btn)
       .tag(1).button(BACK_POS, GET_TEXT_F(MSG_BACK));
  }
}

bool LevelingMenu::onTouchEnd(uint8_t tag) {
  switch (tag) {
    case 1: GOTO_PREVIOUS();                   break;
    case 2:
    #ifndef BED_LEVELING_COMMANDS
      #define BED_LEVELING_COMMANDS "G29"
    #endif
    #if HAS_MESH
      BedMeshScreen::startMeshProbe();
    #else
      SpinnerDialogBox::enqueueAndWait_P(F(BED_LEVELING_COMMANDS));
    #endif
    break;
    case 3:
        #if ENABLED(Z2_PRESENCE_CHECK)
          if(has_z2_jumper()) {
            GOTO_SCREEN(StatusScreen);
            ExtUI::injectCommands_P(PSTR("G34 A2 I20 T0.01"));
          } else {
            SpinnerDialogBox::enqueueAndWait_P(F(AXIS_LEVELING_COMMANDS));
          }
        #elif defined(AXIS_LEVELING_COMMANDS)
          SpinnerDialogBox::enqueueAndWait_P(F(AXIS_LEVELING_COMMANDS));
        #elif ENABLED(Z_STEPPER_AUTO_ALIGN)
          SpinnerDialogBox::enqueueAndWait_P(F("G34 A2 I20 T0.01"));
        #endif
      break;
    #if HAS_MESH
    case 4: GOTO_SCREEN(BedMeshScreen); break;
    #endif
    #if ENABLED(BLTOUCH)
    case 5: injectCommands_P(PSTR("M280 P0 S60")); break;
    case 6: SpinnerDialogBox::enqueueAndWait_P(F("M280 P0 S90\nG4 P100\nM280 P0 S120")); break;
    #endif
    default: return false;
  }
  return true;
}

#endif // BOTH(TOUCH_UI_FTDI_EVE,HAS_LEVELING)
