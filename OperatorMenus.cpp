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

#include <Arduino.h>
#include "RPU_Config.h"
#include "RPU.h"
#include "AudioHandler.h"
#include "OperatorMenus.h"


#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
#ifdef RPU_OS_USE_6_DIGIT_CREDIT_DISPLAY_WITH_7_DIGIT_DISPLAYS
#define TOTAL_DISPLAY_DIGITS 34
#else
#define TOTAL_DISPLAY_DIGITS 35
#endif
#else
#define TOTAL_DISPLAY_DIGITS 30
#endif



unsigned long OM_GetLastChangedTime();
void OM_SetLastChangedTime(unsigned long setSelfTestChange);


OperatorMenus::OperatorMenus() {
  TopLevel = OPERATOR_MENU_NOT_ACTIVE;
  ForwardButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  BackButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  EnterButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  MenuButton = OPERATOR_MENU_BUTTON_UNDEFINED;

  AdjustmentType = 0;
  NumAdjustmentValues = 0;
  for (byte count=0; count<8; count++) {
    AdjustmentValues[count] = 0;
  }
  CurrentAdjustmentStorageByte = 0;
  TempValue = 0;
  CurrentAdjustmentByte = NULL;

  LastTestTime = 0;
//  LastSelfTestChange = 0;
  SavedValue = 0;
  ResetHold = 0;
  NextSpeedyValueChange = 0;
  NumSpeedyChanges = 0;
  LastResetPress = 0;
  CurValue = 0;
  LastTestValue = 0;
  CurSound = 0x01;
  SoundPlaying = 0;
  SoundToPlay = 0;
  SolenoidCycle = true;
    
  CurrentAdjustmentUL = NULL;
  SoundSettingTimeout = 0;
  AdjustmentScore = 0;
  SolenoidStackStateOnEntry = false;
}


boolean OperatorMenus::OperatorMenusActive() {
  if (TopLevel==OPERATOR_MENU_NOT_ACTIVE) return false;
  return true;
}


void OperatorMenus::SetNavigationButtons(byte forwardButtonNum, byte backButtonNum, byte enterButtonNum, byte menuButtonNum)
{
  ForwardButton = forwardButtonNum;
  BackButton = backButtonNum;
  EnterButton = enterButtonNum;
  MenuButton = menuButtonNum;
}

boolean OperatorMenus::HasTopLevelChanged() {
  if (TopLevelChanged) {
    TopLevelChanged = false;
    return true;
  }
  return false;
}

boolean OperatorMenus::HasSubLevelChanged() {
  if (SubLevelChanged) {
    SubLevelChanged = false;
    return true;
  }
  return false;
}

boolean OperatorMenus::HasParameterChanged() {
  if (ParameterChanged) {
    ParameterChanged = false;
    return true;
  }
  return false;
}

byte OperatorMenus::GetParameterID() {
  return ParameterID;
}


unsigned short OperatorMenus::GetParameterCallout() {
  return ParameterCallout;
}


byte OperatorMenus::GetTopLevel() {
  return TopLevel;
}

byte OperatorMenus::GetSubLevel() {
  return SubLevel;
}

void OperatorMenus::SetNumSubLevels(byte numSubLevels) {
  NumSubLevels = numSubLevels;
}


void OperatorMenus::SetParameterControls(   byte adjustmentType, byte numAdjustmentValues, byte *adjustmentValues,
                                            byte parameterCallout, byte currentAdjustmentStorageByte, 
                                            byte *currentAdjustmentByte, unsigned long *currentAdjustmentUL ) {
  AdjustmentType = adjustmentType;
  NumAdjustmentValues = numAdjustmentValues;
  if (NumAdjustmentValues>8) NumAdjustmentValues = 8;
  for (byte count=0; (count<numAdjustmentValues); count++) {
    AdjustmentValues[count] = adjustmentValues[count];
  }
  ParameterCallout = parameterCallout;
  CurrentAdjustmentStorageByte = currentAdjustmentStorageByte;
  CurrentAdjustmentByte = currentAdjustmentByte;
  CurrentAdjustmentUL = currentAdjustmentUL;
  ParameterID = 0;

  if (AdjustmentType==OPERATOR_MENU_ADJ_TYPE_LIST && CurrentAdjustmentByte!=NULL) {
    for (byte count=0; count<NumAdjustmentValues; count++) {
      if (AdjustmentValues[count]==*CurrentAdjustmentByte) {
        ParameterID = count;
        break;
      }
    }
  }
  ShowParameterValue(); 
}



void OperatorMenus::EnterOperatorMenu() {
  // make note of the game over status (to restore later)
  TopLevel = OPERATOR_MENU_RETURN_TO_GAME;  
  TopLevelChanged = true;
  SubLevelChanged = false;
  ParameterChanged = false;
  CurrentAdjustmentByte = NULL;
  CurrentAdjustmentUL = NULL;

  RPU_SetDisplayCredits(0, false);
  RPU_SetDisplayBallInPlay(0, false);
  for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
    RPU_SetDisplayBlank(count, 0);
  }
  
  // Remember credits & BIP display
  SolenoidStackStateOnEntry = RPU_IsSolenoidStackEnabled();
}

void OperatorMenus::ExitOperatorMenu() {
  TopLevel = OPERATOR_MENU_NOT_ACTIVE;
  SubLevel = OPERATOR_MENU_NOT_ACTIVE;
  // Restore credits & BIP display
  
  if (SolenoidStackStateOnEntry) RPU_EnableSolenoidStack();
  else RPU_DisableSolenoidStack();
}


void OperatorMenus::ShowParameterValue() {

  if (AdjustmentType==OPERATOR_MENU_ADJ_TYPE_CPC) {
  } else {
    if (CurrentAdjustmentByte) {
      RPU_SetDisplay(0, *CurrentAdjustmentByte, true, 1);
      RPU_SetDisplayBlank(1, 0);
    } else if (CurrentAdjustmentUL) {
      unsigned long value0, value1;
      value0 = (*CurrentAdjustmentUL) / 1000000;
      value1 = (*CurrentAdjustmentUL) % 1000000;
      if (value0) RPU_SetDisplay(0, value0, true, 1);
      else RPU_SetDisplayBlank(0, 0);
      RPU_SetDisplay(1, value1, true, 2);
    }
  }

}


int OperatorMenus::UpdateMenu(unsigned long currentTime) {

  byte curSwitch;

  while ( (curSwitch = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    byte beginningTopLevel = TopLevel;
    byte beginningSubLevel = SubLevel;
    
  //  ForwardButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  //  BackButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  //  EnterButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  //  MenuButton = OPERATOR_MENU_BUTTON_UNDEFINED;
  
    if (curSwitch==MenuButton) {
      RPU_TurnOffAllLamps();
      TopLevel += 1;
      TopLevelChanged = true;
      SubLevel = OPERATOR_MENU_NOT_ACTIVE;
      SubLevelChanged = false;
      ParameterChanged = false;
      if (TopLevel>OPERATOR_MENU_GAME_ADJ_MENU) {
        RPU_SetDisplayCredits(0, false);
        RPU_SetDisplayBallInPlay(0, false);
        for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
          RPU_SetDisplayBlank(count, 0);
        }        
        TopLevel = OPERATOR_MENU_RETURN_TO_GAME;      
        SubLevel = OPERATOR_MENU_NOT_ACTIVE;
      } else {
        for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
          RPU_SetDisplayBlank(count, 0);
        }
        RPU_SetDisplayCredits(TopLevel, true);
        RPU_SetDisplayBallInPlay(0, false);
      }
      NumSubLevels = 0;
      if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) NumSubLevels = 6;
      if (TopLevel==OPERATOR_MENU_AUDITS_MENU) NumSubLevels = 9;
    } else if (curSwitch==EnterButton) {
      if (TopLevel==OPERATOR_MENU_RETURN_TO_GAME) {
        ExitOperatorMenu();
        return 0;
      } else {
        if (SubLevel==OPERATOR_MENU_NOT_ACTIVE) {
          SubLevel = 0;
          SubLevelChanged = true;
        } else {
          // Handle enter button for test/audit/adjustment
          HandleEnterButton();   
        }
      }
    } else if (curSwitch==ForwardButton) {
      if (TopLevel!=OPERATOR_MENU_RETURN_TO_GAME) {
        if (SubLevel==OPERATOR_MENU_NOT_ACTIVE) {
          SubLevel = 0;
          SubLevelChanged = true;
          CurrentAdjustmentByte = NULL;
          CurrentAdjustmentUL = NULL;
        } else {
          SubLevel += 1;
          if (SubLevel>=NumSubLevels) SubLevel = 0;
          SubLevelChanged = true;
        }
        //ShowParameterValue();
        RPU_SetDisplayCredits(TopLevel);
        RPU_SetDisplayBallInPlay(SubLevel + 1);
      }
    } else if (curSwitch==BackButton) {
      if (TopLevel!=OPERATOR_MENU_RETURN_TO_GAME) {
        if (SubLevel==OPERATOR_MENU_NOT_ACTIVE) {
          SubLevel = 0;
          SubLevelChanged = true;
          CurrentAdjustmentByte = NULL;
          CurrentAdjustmentUL = NULL;
        } else {
          if (SubLevel==0) {
            if (NumSubLevels) SubLevel = NumSubLevels - 1;
            else SubLevel = 0;
          } else {
            SubLevel -= 1;
          }
          SubLevelChanged = true;
        }
        //ShowParameterValue();
        RPU_SetDisplayCredits(TopLevel);
        RPU_SetDisplayBallInPlay(SubLevel + 1);
      }
    }
  
    // Test modes are handled above
    if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) {
      if (SubLevel!=OPERATOR_MENU_NOT_ACTIVE) RunSelfTest(curSwitch, (SubLevel==beginningSubLevel)?false:true, currentTime);
    }

    (void)beginningTopLevel;
  }

  // See if any modes need to be updated
  if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) {
    if (SubLevel!=OPERATOR_MENU_NOT_ACTIVE) UpdateSelfTest(currentTime);
  }

  return 1;
}


void OperatorMenus::HandleEnterButton() {

  if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) {
    
  } else if (TopLevel==OPERATOR_MENU_AUDITS_MENU) {
    
  } else if (TopLevel==OPERATOR_MENU_BASIC_ADJ_MENU || TopLevel==OPERATOR_MENU_GAME_ADJ_MENU) {
    
    if (CurrentAdjustmentByte && (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX || AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX_DEFAULT)) {
      byte curVal = *CurrentAdjustmentByte;

      if (RPU_GetUpDownSwitchState()) {
        curVal += 1;
        if (curVal > AdjustmentValues[1]) {
          if (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX) curVal = AdjustmentValues[0];
          else {
            if (curVal > 99) curVal = AdjustmentValues[0];
            else curVal = 99;
          }
        }
      } else {
        if (curVal == AdjustmentValues[0]) {
          if (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX_DEFAULT) curVal = 99;
          else curVal = AdjustmentValues[1];
        } else {
          curVal -= 1;
        }
      }

      *CurrentAdjustmentByte = curVal;
      if (CurrentAdjustmentStorageByte) RPU_WriteByteToEEProm(CurrentAdjustmentStorageByte, curVal);
/*
      if (curState == MACHINE_STATE_ADJUST_SOUND_SELECTOR) {
        Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_SELF_TEST_AUDIO_OPTIONS_START + curVal, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
      } else if (curState == MACHINE_STATE_ADJUST_MUSIC_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_BACKGROUND_SONG_1, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
        Audio.SetMusicVolume(curVal);
        SoundSettingTimeout = CurrentTime + 5000;
      } else if (curState == MACHINE_STATE_ADJUST_SFX_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_BONUS_COUNT, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
        Audio.SetSoundFXVolume(curVal);
        SoundSettingTimeout = CurrentTime + 5000;
      } else if (curState == MACHINE_STATE_ADJUST_CALLOUTS_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_VP_EXTRA_BALL, AUDIO_PLAY_TYPE_WAV_TRIGGER, curVal);
        Audio.SetNotificationsVolume(curVal);
        SoundSettingTimeout = CurrentTime + 3000;
      }
*/
    } else if (CurrentAdjustmentByte && AdjustmentType == OPERATOR_MENU_ADJ_TYPE_LIST) {
      byte valCount = 0;
      byte curVal = *CurrentAdjustmentByte;
      byte newIndex = 0;
      boolean upDownState = RPU_GetUpDownSwitchState();
      for (valCount = 0; valCount < (NumAdjustmentValues); valCount++) {
        if (curVal == AdjustmentValues[valCount]) {
          if (upDownState) {
            if (valCount < (NumAdjustmentValues - 1)) newIndex = valCount + 1;
          } else {
            if (valCount > 0) newIndex = valCount - 1;
          }
        }
      }
      *CurrentAdjustmentByte = AdjustmentValues[newIndex];
      if (CurrentAdjustmentStorageByte) RPU_WriteByteToEEProm(CurrentAdjustmentStorageByte, AdjustmentValues[newIndex]);
    } else if (CurrentAdjustmentUL && (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT || AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT)) {
      unsigned long curVal = *CurrentAdjustmentUL;

      if (RPU_GetUpDownSwitchState()) curVal += 5000;
      else if (curVal >= 5000) curVal -= 5000;
      if (curVal > 100000) curVal = 0;
      if (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT && curVal == 0) curVal = 5000;
      *CurrentAdjustmentUL = curVal;
      if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, curVal);
    }

    ShowParameterValue();
  }
  
}



void OperatorMenus::ReadCurrentSwitches() {
  byte numSwitchesToShow = RPU_NUMBER_OF_PLAYER_DISPLAYS * 2;
  byte count, switchCount;
  byte firstSwitchFound = SWITCH_STACK_EMPTY;
  for (count=0; count<numSwitchesToShow; count++) {
    for (switchCount=0; switchCount<64; switchCount++) {
      if (firstSwitchFound==((switchCount+LastTestValue)%64)) break;
      if (RPU_ReadSingleSwitchState( (switchCount+LastTestValue)%64 )) {
        // We found a switch, so we can record it and move on
        CurrentSwitches[count] = (switchCount+LastTestValue)%64;
        LastTestValue = CurrentSwitches[count] + 1;
        if (firstSwitchFound==SWITCH_STACK_EMPTY) firstSwitchFound = CurrentSwitches[count];
        break;
      }      
    }
    if (switchCount==64) {
      // can't find anymore switches, so break
      break;
    }
  }
  if (count<numSwitchesToShow) {
    for (byte remainderCount=count; remainderCount<numSwitchesToShow; remainderCount++) {
      CurrentSwitches[remainderCount] = SWITCH_STACK_EMPTY;
    }
  }
}

byte OperatorMenus::GetDisplayMaskForSwitches(byte switch1, byte switch2) {
  byte displayMask = 0;
  
  for (byte count=0; count<2; count++) {
    byte curSwitch = switch1;
    if (count==1) curSwitch = switch2;
      displayMask /= 8;

#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
    if (curSwitch!=SWITCH_STACK_EMPTY) {
      if (curSwitch>99) {
        displayMask |= 0x70;
      } else if (curSwitch>9) {
        displayMask |= 0x60;
      } else {
        displayMask |= 0x40;
      }      
    }
#else 
    if (curSwitch!=SWITCH_STACK_EMPTY) {
      if (curSwitch>99) {
        displayMask |= 0x38;
      } else if (curSwitch>9) {
        displayMask |= 0x30;
      } else {
        displayMask |= 0x20;
      }      
    }
#endif  
  }  
  return displayMask;  
}

void OperatorMenus::UpdateSelfTest(unsigned long currentTime) {
  if (SubLevel==OPERATOR_MENU_TEST_SWITCHES) {
    if (LastTestTime==0 || currentTime>(LastTestTime+1500)) {
      ReadCurrentSwitches();
      unsigned long displayValue = 0;
      byte switchToShow = 0;
      byte displayMask = 0;
      for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
        // Switches have a "1" starting index now
        displayValue = (unsigned long)(CurrentSwitches[switchToShow]+1) * 1000 + (unsigned long)(CurrentSwitches[switchToShow+1]+1);
        displayMask = GetDisplayMaskForSwitches(CurrentSwitches[switchToShow]+1, CurrentSwitches[switchToShow+1]+1);
        switchToShow += 2;
        RPU_SetDisplay(count, displayValue, false);
        RPU_SetDisplayBlank(count, displayMask);
      }
      LastTestTime = currentTime;
    }
  } else if (SubLevel==OPERATOR_MENU_TEST_DISPLAYS) {
    RPU_CycleAllDisplays(currentTime, CurValue);
  } else if (SubLevel==OPERATOR_MENU_TEST_SOLENOIDS) {
    if (currentTime>(LastTestTime+1000)) {
      RPU_SetDisplay(0, CurValue, true);      
      LastTestTime = currentTime;
      if (CurValue<15) {
        RPU_PushToSolenoidStack(CurValue, 10);
      } else {
        RPU_FireContinuousSolenoid(0x10<<(CurValue-15), 5);
      }
      CurValue += 1;
      if (CurValue>18) CurValue = 0;
    }
  }
}


void OperatorMenus::RunSelfTest(byte curSwitch, boolean testChanged, unsigned long currentTime) {

  boolean resetDoubleClick = false;
  unsigned short savedScoreStartByte = 0;
  unsigned short auditNumStartByte = 0;

  if (curSwitch==EnterButton) {
    ResetHold = currentTime;
    if ((currentTime-LastResetPress)<400) {
      resetDoubleClick = true;
      curSwitch = SWITCH_STACK_EMPTY;
    }
    LastResetPress = currentTime;
    SoundToPlay += 1;
    if (SoundToPlay>31) SoundToPlay = 0;
  }

  if (ResetHold!=0 && !RPU_ReadSingleSwitchState(EnterButton)) {
    ResetHold = 0;
    NextSpeedyValueChange = 0;
  }

  boolean resetBeingHeld = false;
  if (ResetHold!=0 && (currentTime-ResetHold)>1300) {
    resetBeingHeld = true;
    if (NextSpeedyValueChange==0) {
      NextSpeedyValueChange = currentTime;
      NumSpeedyChanges = 0;
    }
  }

  if (testChanged) {
    RPU_SetDisplayCredits(1);
    RPU_SetDisplayBallInPlay(1+SubLevel);
    if (SubLevel==OPERATOR_MENU_TEST_LAMPS) {
      RPU_SetDisplay(0, 99, true);
      for (byte count=1; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
        RPU_SetDisplayBlank(count, 0x00);
      }
      RPU_TurnOffAllLamps();
      for (int count=0; count<RPU_MAX_LAMPS; count++) {
        RPU_SetLampState(count, 1, 0, 500);
      }
      CurValue = 99;
    } else if (SubLevel==OPERATOR_MENU_TEST_DISPLAYS) {
      RPU_TurnOffAllLamps();
      for (int count=0; count<4; count++) {
        RPU_SetDisplayBlank(count, RPU_OS_ALL_DIGITS_MASK);        
      }
      CurValue = 0;
      RPU_CycleAllDisplays(currentTime, CurValue);
    } else if (SubLevel==OPERATOR_MENU_TEST_SOLENOIDS) {
      for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
        RPU_SetDisplayBlank(count, 0);
      }
      RPU_TurnOffAllLamps();
      LastTestTime = currentTime;
      RPU_EnableSolenoidStack();
      SolenoidCycle = true;
      CurValue = 0;      
    } else if (SubLevel==OPERATOR_MENU_TEST_SWITCHES) {
      RPU_TurnOffAllLamps();
      LastTestTime = 0;
      LastTestValue = 0;
    } else if (SubLevel==OPERATOR_MENU_TEST_SOUNDS) {
    } else if (SubLevel==OPERATOR_MENU_TEST_EJECT_BALLS) {
      
    }
  }

  (void)resetDoubleClick;
  (void)savedScoreStartByte;
  (void)auditNumStartByte;
  (void)resetBeingHeld;
}


OperatorMenus::~OperatorMenus() {
}


/*
int RunBaseSelfTest(int curState, boolean curStateChanged, unsigned long CurrentTime, byte resetSwitch, byte slamSwitch, byte altSelfTestSwitch) {
  byte curSwitch = RPU_PullFirstFromSwitchStack();
  int returnState = curState;
  boolean resetDoubleClick = false;
  unsigned short savedScoreStartByte = 0;
  unsigned short auditNumStartByte = 0;
#ifndef RPU_OS_DISABLE_CPC_FOR_SPACE  
  unsigned short cpcSelectorStartByte = 0;
#endif

  if (curSwitch==resetSwitch) {
    ResetHold = CurrentTime;
    if ((CurrentTime-LastResetPress)<400) {
      resetDoubleClick = true;
      curSwitch = SWITCH_STACK_EMPTY;
    }
    LastResetPress = CurrentTime;
    SoundToPlay += 1;
    if (SoundToPlay>31) SoundToPlay = 0;
  }

  if (ResetHold!=0 && !RPU_ReadSingleSwitchState(resetSwitch)) {
    ResetHold = 0;
    NextSpeedyValueChange = 0;
  }

  boolean resetBeingHeld = false;
  if (ResetHold!=0 && (CurrentTime-ResetHold)>1300) {
    resetBeingHeld = true;
    if (NextSpeedyValueChange==0) {
      NextSpeedyValueChange = CurrentTime;
      NumSpeedyChanges = 0;
    }
  }

  if (slamSwitch!=0xFF && curSwitch==slamSwitch) {
    returnState = MACHINE_STATE_ATTRACT;
  }
  
  if ((curSwitch==SW_SELF_TEST_SWITCH || curSwitch==altSelfTestSwitch) && (CurrentTime-LastSelfTestChange)>250) {
    if (RPU_GetUpDownSwitchState()) returnState -= 1;
    else returnState += 1;
//    if (returnState==MACHINE_STATE_TEST_DONE) returnState = MACHINE_STATE_ATTRACT;
    LastSelfTestChange = CurrentTime;
  }

  if (curStateChanged) {
    RPU_SetCoinLockout(false);
    
    for (int count=0; count<4; count++) {
      RPU_SetDisplay(count, 0);
      RPU_SetDisplayBlank(count, 0x00);        
    }

#if (RPU_MPU_ARCHITECTURE<10)
    if (curState<=MACHINE_STATE_TEST_SCORE_LEVEL_1) {
      RPU_SetDisplayCredits(0, false);
      RPU_SetDisplayBallInPlay(MACHINE_STATE_TEST_SOUNDS-curState);
    } else {
      RPU_SetDisplayCredits(0 - curState, true);
      RPU_SetDisplayBallInPlay(0, false);      
    }
#else
    if (curState<=MACHINE_STATE_TEST_HISCR) {
      RPU_SetDisplayCredits(0, false);
      RPU_SetDisplayBallInPlay(MACHINE_STATE_TEST_BOOT-curState, true);
    } else {
      RPU_SetDisplayCredits(curState - MACHINE_STATE_TEST_BOOT, true);
      RPU_SetDisplayBallInPlay(0, false);      
    }
#endif      
  }

  if (curState==MACHINE_STATE_TEST_LAMPS) {
    if (curStateChanged) {
      RPU_DisableSolenoidStack();        
      RPU_SetDisableFlippers(true);
      RPU_TurnOffAllLamps();
      for (int count=0; count<RPU_MAX_LAMPS; count++) {
        RPU_SetLampState(count, 1, 0, 500);
      }
      CurValue = 99;
      RPU_SetDisplay(0, CurValue, true);  
    }
    if (curSwitch==resetSwitch || resetDoubleClick) {
      if (RPU_GetUpDownSwitchState()) {
        CurValue += 1;
        if (CurValue==RPU_MAX_LAMPS) CurValue = 99;
        else if (CurValue>99) CurValue = 0;    
      } else {
        if (CurValue>0) CurValue -= 1;
        else CurValue = 99;
        if (CurValue==98) CurValue = RPU_MAX_LAMPS - 1;
      }
      if (CurValue==99) {
        for (int count=0; count<RPU_MAX_LAMPS; count++) {
          RPU_SetLampState(count, 1, 0, 500);
        }
      } else {
        RPU_TurnOffAllLamps();
        RPU_SetLampState(CurValue, 1);
      }      
      RPU_SetDisplay(0, CurValue, true);  
    }    
  } else if (curState==MACHINE_STATE_TEST_DISPLAYS) {
    if (curStateChanged) {
      RPU_TurnOffAllLamps();
      for (int count=0; count<4; count++) {
        RPU_SetDisplayBlank(count, RPU_OS_ALL_DIGITS_MASK);        
      }
      CurValue = 0;
    }
    if (curSwitch==resetSwitch || resetDoubleClick) {
      if (RPU_GetUpDownSwitchState()) {
        CurValue += 1;
        if (CurValue>TOTAL_DISPLAY_DIGITS) {
          for (int count=0; count<4; count++) {
            RPU_SetDisplayBlank(count, RPU_OS_ALL_DIGITS_MASK);        
          }
          CurValue = 0;
        }
      } else {
        if (CurValue>0) CurValue -= 1;
        else CurValue = TOTAL_DISPLAY_DIGITS;
      }
    }    
    RPU_CycleAllDisplays(CurrentTime, CurValue);
  } else if (curState==MACHINE_STATE_TEST_SOLENOIDS) {
    if (curStateChanged) {
      RPU_TurnOffAllLamps();
      LastSolTestTime = CurrentTime;
      RPU_EnableSolenoidStack(); 
      RPU_SetDisableFlippers(false);
      //RPU_SetDisplayBlank(4, 0);
      SolenoidCycle = true;
      SavedValue = 0;
      RPU_PushToSolenoidStack(SavedValue, 10);
    } 
    if (curSwitch==resetSwitch || resetDoubleClick) {
      SolenoidCycle = (SolenoidCycle) ? false : true;
    }

    if ((CurrentTime-LastSolTestTime)>1000) {
      if (SolenoidCycle) {
        SavedValue += 1;
#if (RPU_MPU_ARCHITECTURE<10)
        if (SavedValue>14) SavedValue = 0;
#else        
        if (SavedValue>21) SavedValue = 0;
#endif        
      }
      RPU_PushToSolenoidStack(SavedValue, 10);
      RPU_SetDisplay(0, SavedValue, true);
      LastSolTestTime = CurrentTime;
    }
    
  } else if (curState==MACHINE_STATE_TEST_SWITCHES) {
    if (curStateChanged) {
      RPU_TurnOffAllLamps();
      RPU_DisableSolenoidStack(); 
      RPU_SetDisableFlippers(true);
    }

    byte displayOutput = 0;
    for (byte switchCount=0; switchCount<64 && displayOutput<4; switchCount++) {
      if (RPU_ReadSingleSwitchState(switchCount)) {
        RPU_SetDisplay(displayOutput, switchCount, true);
        displayOutput += 1;
      }
    }

    if (displayOutput<4) {
      for (int count=displayOutput; count<4; count++) {
        RPU_SetDisplayBlank(count, 0x00);
      }
    }

  } else if (curState==MACHINE_STATE_TEST_SOUNDS) {
#ifdef RPU_OS_USE_SB100    
    byte soundToPlay = 0x01 << (((CurrentTime-LastSelfTestChange)/750)%8);
    if (SoundPlaying!=soundToPlay) {
      RPU_PlaySB100(soundToPlay);
      SoundPlaying = soundToPlay;
      RPU_SetDisplay(0, (unsigned long)soundToPlay, true);
      LastSolTestTime = CurrentTime; // Time the sound started to play
    }
    // If the sound play call was more than 300ms ago, turn it off
//    if ((CurrentTime-LastSolTestTime)>300) RPU_PlaySB100(128);
#elif defined (RPU_OS_USE_S_AND_T)
    byte soundToPlay = ((CurrentTime-LastSelfTestChange)/2000)%256;
    if (SoundPlaying!=soundToPlay) {
      RPU_PlaySoundSAndT(soundToPlay);
      SoundPlaying = soundToPlay;
      RPU_SetDisplay(0, (unsigned long)soundToPlay, true);
      LastSolTestTime = CurrentTime; // Time the sound started to play
    }
#elif defined (RPU_OS_USE_DASH51) 
    byte soundToPlay = ((CurrentTime-LastSelfTestChange)/2000)%32;
    if (SoundPlaying!=soundToPlay) {
      if (soundToPlay==17) soundToPlay = 0;
      RPU_PlaySoundDash51(soundToPlay);
      SoundPlaying = soundToPlay;
      RPU_SetDisplay(0, (unsigned long)soundToPlay, true);
      LastSolTestTime = CurrentTime; // Time the sound started to play
    }
#elif defined (RPU_OS_USE_WTYPE_1_SOUND)
    byte soundToPlay = (((CurrentTime-LastSelfTestChange)/3000)%31)+1;
    if (SoundPlaying!=soundToPlay) {
      RPU_PushToSoundStack(soundToPlay*256, 8);
      SoundPlaying = soundToPlay;
      RPU_SetDisplay(0, (unsigned long)soundToPlay, true);
      LastSolTestTime = CurrentTime; // Time the sound started to play
    }
#elif defined (RPU_OS_USE_WTYPE_2_SOUND) 
//    byte soundToPlay = (((CurrentTime-LastSelfTestChange)/1000)%32);
    if (SoundPlaying!=SoundToPlay) {
      RPU_PushToSoundStack(SoundToPlay, 8);
      SoundPlaying = SoundToPlay  ;
      RPU_SetDisplay(0, (unsigned long)SoundToPlay, true);
      LastSolTestTime = CurrentTime; // Time the sound started to play
    }
#endif
  } else if (curState==MACHINE_STATE_TEST_BOOT) {
    if (curStateChanged) {
      for (int count=0; count<4; count++) {
        RPU_SetDisplay(count, 8007, true);
      }
    }
    if (curSwitch==resetSwitch || resetDoubleClick) {
      returnState = MACHINE_STATE_ATTRACT;
    }
    for (int count=0; count<4; count++) {
#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
      RPU_SetDisplayBlank(count, ((CurrentTime/500)%2)?0x78:0x00);
#else      
      RPU_SetDisplayBlank(count, ((CurrentTime/500)%2)?0x3C:0x00);
#endif      
    }
  } else if (curState==MACHINE_STATE_TEST_SCORE_LEVEL_1) {
#ifdef RPU_OS_USE_SB100    
    if (curStateChanged) RPU_PlaySB100(0);
#endif
    savedScoreStartByte = RPU_AWARD_SCORE_1_EEPROM_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_SCORE_LEVEL_2) {
    savedScoreStartByte = RPU_AWARD_SCORE_2_EEPROM_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_SCORE_LEVEL_3) {
    savedScoreStartByte = RPU_AWARD_SCORE_3_EEPROM_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_HISCR) {
    savedScoreStartByte = RPU_HIGHSCORE_EEPROM_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_CREDITS) {
    if (curStateChanged) {
      SavedValue = RPU_ReadByteFromEEProm(RPU_CREDITS_EEPROM_BYTE);
      RPU_SetDisplay(0, SavedValue, true);
    }
    if (curSwitch==resetSwitch || resetDoubleClick) {
      if (RPU_GetUpDownSwitchState()) {
        SavedValue += 1;
        if (SavedValue>99) SavedValue = 0;
      } else {
        if (SavedValue>0) SavedValue -= 1;
        else SavedValue = 99;
      }
      RPU_SetDisplay(0, SavedValue, true);
      RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, SavedValue & 0x000000FF);
    }
  } else if (curState==MACHINE_STATE_TEST_TOTAL_PLAYS) {
    auditNumStartByte = RPU_TOTAL_PLAYS_EEPROM_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_TOTAL_REPLAYS) {
    auditNumStartByte = RPU_TOTAL_REPLAYS_EEPROM_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_HISCR_BEAT) {
    auditNumStartByte = RPU_TOTAL_HISCORE_BEATEN_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_CHUTE_2_COINS) {
    auditNumStartByte = RPU_CHUTE_2_COINS_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_CHUTE_1_COINS) {
    auditNumStartByte = RPU_CHUTE_1_COINS_START_BYTE;
  } else if (curState==MACHINE_STATE_TEST_CHUTE_3_COINS) {
    auditNumStartByte = RPU_CHUTE_3_COINS_START_BYTE;
#ifndef RPU_OS_DISABLE_CPC_FOR_SPACE      
  } else if (curState==MACHINE_STATE_ADJUST_CPC_CHUTE_1) {
    cpcSelectorStartByte = RPU_CPC_CHUTE_1_SELECTION_BYTE;
  } else if (curState==MACHINE_STATE_ADJUST_CPC_CHUTE_2) {
    cpcSelectorStartByte = RPU_CPC_CHUTE_2_SELECTION_BYTE;
  } else if (curState==MACHINE_STATE_ADJUST_CPC_CHUTE_3) {
    cpcSelectorStartByte = RPU_CPC_CHUTE_3_SELECTION_BYTE;
#endif    
  }

  if (savedScoreStartByte) {
    if (curStateChanged) {
      SavedValue = RPU_ReadULFromEEProm(savedScoreStartByte);
      RPU_SetDisplay(0, SavedValue, true);  
    }

    if (curSwitch==resetSwitch) {
      if (RPU_GetUpDownSwitchState()) {
        SavedValue += 1000;
      } else {
        if (SavedValue>1000) SavedValue -= 1000;
        else SavedValue = 0;
      }
      RPU_SetDisplay(0, SavedValue, true);  
      RPU_WriteULToEEProm(savedScoreStartByte, SavedValue);
    }

    if (resetBeingHeld && (CurrentTime>=NextSpeedyValueChange)) {
      if (RPU_GetUpDownSwitchState()) {
        SavedValue += 1000;
      } else {
        if (SavedValue>1000) SavedValue -= 1000;
        else SavedValue = 0;
      }
      RPU_SetDisplay(0, SavedValue, true);  
      if (NumSpeedyChanges<6) NextSpeedyValueChange = CurrentTime + 400;
      else if (NumSpeedyChanges<50) NextSpeedyValueChange = CurrentTime + 50;
      else NextSpeedyValueChange = CurrentTime + 10;
      NumSpeedyChanges += 1;
    }

    if (!resetBeingHeld && NumSpeedyChanges>0) {
      RPU_WriteULToEEProm(savedScoreStartByte, SavedValue);
      NumSpeedyChanges = 0;
    }
    
    if (resetDoubleClick) {
      SavedValue = 0;
      RPU_SetDisplay(0, SavedValue, true);  
      RPU_WriteULToEEProm(savedScoreStartByte, SavedValue);
    }
  }

  if (auditNumStartByte) {
    if (curStateChanged) {
      SavedValue = RPU_ReadULFromEEProm(auditNumStartByte);
      RPU_SetDisplay(0, SavedValue, true);
    }

    if (resetDoubleClick) {
      SavedValue = 0;
      RPU_SetDisplay(0, SavedValue, true);  
      RPU_WriteULToEEProm(auditNumStartByte, SavedValue);
    }
    
  }

#ifndef RPU_OS_DISABLE_CPC_FOR_SPACE  
  if (cpcSelectorStartByte) {
    if (curStateChanged) {
      SavedValue = RPU_ReadByteFromEEProm(cpcSelectorStartByte);
      if (SavedValue>NUM_CPC_PAIRS) SavedValue = 4;
      RPU_SetDisplay(0, CPCPairs[SavedValue][0], true);
      RPU_SetDisplay(1, CPCPairs[SavedValue][1], true);
    }

    if (curSwitch==resetSwitch) {
      byte lastValue = (byte)SavedValue;
      if (RPU_GetUpDownSwitchState()) {
        SavedValue += 1;
        if (SavedValue>=NUM_CPC_PAIRS) SavedValue = 0;
      } else {
        if (SavedValue>0) SavedValue -= 1;
      }
      RPU_SetDisplay(0, CPCPairs[SavedValue][0], true);
      RPU_SetDisplay(1, CPCPairs[SavedValue][1], true);
      if (lastValue!=SavedValue) {
        RPU_WriteByteToEEProm(cpcSelectorStartByte, (byte)SavedValue);
        if (cpcSelectorStartByte==RPU_CPC_CHUTE_1_SELECTION_BYTE) CPCSelection[0] = (byte)SavedValue;
        else if (cpcSelectorStartByte==RPU_CPC_CHUTE_2_SELECTION_BYTE) CPCSelection[1] = (byte)SavedValue;
        else if (cpcSelectorStartByte==RPU_CPC_CHUTE_3_SELECTION_BYTE) CPCSelection[2] = (byte)SavedValue;
      }
    }
  }
#endif  
  
  return returnState;
}

unsigned long GetLastSelfTestChangedTime() {
  return LastSelfTestChange;
}


void SetLastSelfTestChangedTime(unsigned long setSelfTestChange) {
  LastSelfTestChange = setSelfTestChange;
}
*/
