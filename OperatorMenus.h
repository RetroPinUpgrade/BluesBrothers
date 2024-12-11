/**************************************************************************
 *     This file is part of the RPU OS for Arduino Project.

    I, Dick Hamill, the author of this program disclaim all copyright
    in order to make this program freely available in perpetuity to
    anyone who would like to use it. Dick Hamill, 12/7/2024

    RPU OS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RPU OS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.
 */


#ifndef OPERATOR_MENUS_H
#define OPERATOR_MENUS_H

#define OPERATOR_MENU_ADJ_TYPE_LIST                 1
#define OPERATOR_MENU_ADJ_TYPE_MIN_MAX              2
#define OPERATOR_MENU_ADJ_TYPE_MIN_MAX_DEFAULT      3
#define OPERATOR_MENU_ADJ_TYPE_SCORE                4
#define OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT   5
#define OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT     6
#define OPERATOR_MENU_ADJ_TYPE_CPC                  7

#define OPERATOR_MENU_BUTTON_UNDEFINED   0xF0

#define OPERATOR_MENU_RETURN_TO_GAME      0
#define OPERATOR_MENU_SELF_TEST_MENU      1
#define OPERATOR_MENU_AUDITS_MENU         2
#define OPERATOR_MENU_BASIC_ADJ_MENU      3
#define OPERATOR_MENU_GAME_ADJ_MENU       4
#define OPERATOR_MENU_NOT_ACTIVE          0xFF

#define OPERATOR_MENU_TEST_LAMPS          0
#define OPERATOR_MENU_TEST_DISPLAYS       1
#define OPERATOR_MENU_TEST_SOLENOIDS      2
#define OPERATOR_MENU_TEST_SWITCHES       3
#define OPERATOR_MENU_TEST_SOUNDS         4
#define OPERATOR_MENU_TEST_EJECT_BALLS    5


class OperatorMenus
{
  public:
    OperatorMenus();
    ~OperatorMenus();
    
    void    SetLastChangedTime(unsigned long setSelfTestChange);
    void    SetNavigationButtons(byte forwardButtonNum, byte backButtonNum, byte enterButtonNum, byte menuButtonNum);

    unsigned long GetLastChangedTime();
    
    void    EnterOperatorMenu();
    void    ExitOperatorMenu();
    boolean OperatorMenusActive();
    boolean HasTopLevelChanged();
    byte    GetTopLevel();
    boolean HasSubLevelChanged();
    byte    GetSubLevel();
    void    SetNumSubLevels(byte numSubLevels);
    boolean HasParameterChanged();
    byte    GetParameterID();
    unsigned short GetParameterCallout();

    void    SetParameterControls(   byte adjustmentType, byte numAdjustmentValues, byte *adjustmentValues,
                                    byte parameterCallout, byte currentAdjustmentStorageByte, 
                                    byte *currentAdjustmentByte, unsigned long *currentAdjustmentUL );
    
    int     UpdateMenu(unsigned long currentTime);

  private:
    byte TopLevel;
    byte SubLevel;
    byte ForwardButton;
    byte BackButton;
    byte EnterButton;
    byte MenuButton;
    byte NumSubLevels;
    boolean TopLevelChanged;
    boolean SubLevelChanged;
    boolean ParameterChanged;
  
    byte AdjustmentType;
    byte NumAdjustmentValues;
    byte AdjustmentValues[8];    
    byte CurrentAdjustmentStorageByte;
    byte TempValue;
    byte *CurrentAdjustmentByte;
    byte ParameterID;
    unsigned short ParameterCallout;    
    unsigned long *CurrentAdjustmentUL;
    unsigned long SoundSettingTimeout;
    unsigned long AdjustmentScore;

    byte CurValue;
    byte CurSound;
    byte SoundPlaying;
    byte SoundToPlay;
    byte CurrentSwitches[8];
    byte LastTestValue;
    boolean SolenoidCycle;
    boolean SolenoidStackStateOnEntry;
    unsigned long LastTestTime;
//    unsigned long LastSelfTestChange;
    unsigned long SavedValue;
    unsigned long ResetHold;
    unsigned long NextSpeedyValueChange;
    unsigned long NumSpeedyChanges;
    unsigned long LastResetPress;

    void RunSelfTest(byte curSwitch, boolean testChanged, unsigned long currentTime);
    void UpdateSelfTest(unsigned long currentTime);
    void ReadCurrentSwitches();
    byte GetDisplayMaskForSwitches(byte switch1, byte switch2);
    void HandleEnterButton();
    void ShowParameterValue();
};





#endif
