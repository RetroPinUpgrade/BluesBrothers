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
#define OPERATOR_MENUS_CPP
#include "OperatorMenus.h"

#ifndef RPU_NUMBER_OF_PLAYER_DISPLAYS
#define RPU_NUMBER_OF_PLAYER_DISPLAYS 4
#endif


#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
  #if (RPU_NUMBER_OF_PLAYER_DISPLAYS==4)
    #ifdef RPU_OS_USE_6_DIGIT_CREDIT_DISPLAY_WITH_7_DIGIT_DISPLAYS
      #define TOTAL_DISPLAY_DIGITS 34
    #else
      #define TOTAL_DISPLAY_DIGITS 35
    #endif
  #else
    #ifdef RPU_OS_USE_6_DIGIT_CREDIT_DISPLAY_WITH_7_DIGIT_DISPLAYS
      #define TOTAL_DISPLAY_DIGITS 20
    #else
      #define TOTAL_DISPLAY_DIGITS 21
    #endif
  #endif
#else
  #if (RPU_NUMBER_OF_PLAYER_DISPLAYS==4)
    #define TOTAL_DISPLAY_DIGITS 30
  #else
    #define TOTAL_DISPLAY_DIGITS 18
  #endif
#endif


byte DefaultLampLookup(byte displayID) {
  if (displayID==0) return OPERATOR_MENU_VALUE_UNUSED;
  if (displayID<65) return (displayID - 1);
  return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
}

unsigned short DefaultSolenoidIDLookup(byte displayID) {
  if (displayID==0) return OPERATOR_MENU_VALUE_UNUSED;
  if (displayID<16) return (displayID - 1);
  return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
}

byte DefaultSolenoidStrengthLookup(byte displayID) {
  if (displayID==0) return OPERATOR_MENU_VALUE_UNUSED;
  if (displayID<16) return 5;
  return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
}


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
  CurrentAdjustmentByte = NULL;

  LastTestTime = 0;
  SavedValue = 0;
  ResetHold = 0;
  NextSpeedyValueChange = 0;
  LastResetPress = 0;
  LastTestValue = 0;
  NextTestValue = 0;
  CycleTest = true;
    
  CurrentAdjustmentUL = NULL;
  SoundSettingTimeout = 0;
  AdjustmentScore = 0;
  SolenoidStackStateOnEntry = false;
  LampLookupFunction = DefaultLampLookup;
  SolenoidIDLookupFunction = DefaultSolenoidIDLookup;
  SolenoidStrengthLookupFunction = DefaultSolenoidStrengthLookup;
}


boolean OperatorMenus::OperatorMenusActive() {
  if (TopLevel==OPERATOR_MENU_NOT_ACTIVE) return false;
  return true;
}

void OperatorMenus::SetLampsLookupCallback(byte (*lampLookup)(byte)) {
  LampLookupFunction = lampLookup;
}

void OperatorMenus::SetSolenoidIDLookupCallback(unsigned short (*solenoidIDLookup)(byte)) {
  SolenoidIDLookupFunction = solenoidIDLookup;
}

void OperatorMenus::SetSolenoidStrengthLookupCallback(byte (*solenoidStrengthLookup)(byte)) {
  SolenoidStrengthLookupFunction = solenoidStrengthLookup;
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


void OperatorMenus::SetAuditControls( unsigned long *currentAuditUL, byte currentAuditStorageByte, byte adjustmentType ) {
  CurrentAdjustmentUL = currentAuditUL;
  CurrentAdjustmentByte = NULL;
  CurrentAdjustmentStorageByte = currentAuditStorageByte;
  AdjustmentType = adjustmentType;

  ShowAuditValue();
}


void OperatorMenus::SetParameterControls(   byte adjustmentType, byte numAdjustmentValues, byte *adjustmentValues,
                                            short parameterCallout, byte currentAdjustmentStorageByte, 
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


boolean OperatorMenus::BallEjectInProgress(boolean startBallEject) {
  if (startBallEject) EjectBalls = true;
  return EjectBalls;
}


void OperatorMenus::EnterOperatorMenu() {
  // make note of the game over status (to restore later)
  TopLevel = OPERATOR_MENU_RETURN_TO_GAME;  
  TopLevelChanged = true;
  SubLevelChanged = false;
  ParameterChanged = false;
  CurrentAdjustmentByte = NULL;
  CurrentAdjustmentUL = NULL;
  LastResetPress = 0;
  EjectBalls = false;  

  RPU_SetDisplayCredits(0, false);
  RPU_SetDisplayBallInPlay(0, false);
  for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
    RPU_SetDisplayBlank(count, 0);
  }
  
  // Remember credits & BIP display
  SolenoidStackStateOnEntry = RPU_IsSolenoidStackEnabled();
  FlipperStateOnEntry = RPU_GetDisableFlippers();
}

void OperatorMenus::ExitOperatorMenu() {
  TopLevel = OPERATOR_MENU_NOT_ACTIVE;
  SubLevel = OPERATOR_MENU_NOT_ACTIVE;
  // Restore credits & BIP display
  
  if (SolenoidStackStateOnEntry) RPU_EnableSolenoidStack();
  else RPU_DisableSolenoidStack();
  RPU_SetDisableFlippers(FlipperStateOnEntry);
}


void OperatorMenus::ShowAuditValue() {
  for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
    if (count==0 && CurrentAdjustmentStorageByte) {
      RPU_SetDisplay(count, RPU_ReadULFromEEProm(CurrentAdjustmentStorageByte), true);
    } else {
      RPU_SetDisplayBlank(count, 0);
    }
  }
}

void OperatorMenus::ShowParameterValue() {

  if (AdjustmentType==OPERATOR_MENU_ADJ_TYPE_CPC) {
    if (CurrentAdjustmentByte) {
      byte coins = CPCPairs[*CurrentAdjustmentByte][0];
      byte credits = CPCPairs[*CurrentAdjustmentByte][1];
      RPU_SetDisplay(0, coins, true, 1);
      RPU_SetDisplay(1, credits, true, 1);
    }
  } else if (AdjustmentType==OPERATOR_MENU_AUD_CLEARABLE || AdjustmentType==OPERATOR_MENU_AUD_DISPLAY_ONLY) {
    if (CurrentAdjustmentUL) {
      RPU_SetDisplay(0, *CurrentAdjustmentUL, true, 1);
    }
  } else {
    if (CurrentAdjustmentByte) {
      RPU_SetDisplay(0, *CurrentAdjustmentByte, true, 1);
      RPU_SetDisplayBlank(1, 0);
    } else if (CurrentAdjustmentUL) {
      unsigned long value0, value1;
#ifdef RPU_OS_USE_7_DIGIT_DISPLAYS
      value0 = (*CurrentAdjustmentUL) / 10000000;
      value1 = (*CurrentAdjustmentUL) % 10000000;
#else      
      value0 = (*CurrentAdjustmentUL) / 1000000;
      value1 = (*CurrentAdjustmentUL) % 1000000;
#endif      
      if (value0) {
        RPU_SetDisplay(0, value0, true, 1);
        RPU_SetDisplayBlank(1, 0xFF);
        RPU_SetDisplay(1, value1, false);
      } else {
        RPU_SetDisplayBlank(0, 0);
        RPU_SetDisplay(1, value1, true, 2);
      }
    }
  }
}


int OperatorMenus::UpdateMenu(unsigned long currentTime) {

  byte curSwitch;
  
  if (RPU_ReadSingleSwitchState(EnterButton)) {
    if (ResetHold==0) {
      ResetHold = currentTime;
    } else if (ResetHold!=1 && currentTime > (ResetHold+1300)){
      // Handle held enter button
      HandleEnterButton(false, true);
      ResetHold = 1;
    }

    if (NextSpeedyValueChange==0) {
      NextSpeedyValueChange = currentTime;
      NumSpeedyChanges = 0;
    } else if (NextSpeedyValueChange!=1 && currentTime > (NextSpeedyValueChange+500)) {
      HandleEnterButton(false, false, true);
      NextSpeedyValueChange = currentTime;
      NumSpeedyChanges += 1;
    }
  } else {
    ResetHold = 0;
    NextSpeedyValueChange = 0;
    NumSpeedyChanges = 0;
  }

  LastSwitchSeen = SWITCH_STACK_EMPTY;

  while ( (curSwitch = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    LastSwitchSeen = curSwitch;
    if (curSwitch==MenuButton) {
      EjectBalls = false;
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
      if (TopLevel==OPERATOR_MENU_AUDITS_MENU) NumSubLevels = 6;
    } else if (curSwitch==EnterButton) {
      EjectBalls = false;

      boolean resetDoubleClick = false;
  
      if (curSwitch==EnterButton) {
        ResetHold = currentTime;
        if (currentTime<(LastResetPress+400)) {
          resetDoubleClick = true;
        }
        LastResetPress = currentTime;
      }
      
      if (TopLevel==OPERATOR_MENU_RETURN_TO_GAME) {
        ExitOperatorMenu();
        return 0;
      } else {
        if (SubLevel==OPERATOR_MENU_NOT_ACTIVE) {
          SubLevel = 0;
          SubLevelChanged = true;
        } else {
          // Handle enter button for test/audit/adjustment
          HandleEnterButton(resetDoubleClick);
        }
      }
    } else if (curSwitch==ForwardButton) {
      EjectBalls = false;
      ParameterChanged = false;
      if (TopLevel!=OPERATOR_MENU_RETURN_TO_GAME) {

        if (NextSpeedyValueChange>1 && 
            ( AdjustmentType==OPERATOR_MENU_ADJ_TYPE_SCORE || AdjustmentType==OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT ||
              AdjustmentType==OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT ) ) {
          if (CurrentAdjustmentUL && *CurrentAdjustmentUL>50000) *CurrentAdjustmentUL -= 50000;
          NextSpeedyValueChange = 1;
          ShowParameterValue();
        } else {                
          if (SubLevel==OPERATOR_MENU_NOT_ACTIVE) {
            SubLevel = 0;
            CurrentAdjustmentByte = NULL;
            CurrentAdjustmentUL = NULL;
          } else {
            SubLevel += 1;
            if (SubLevel>=NumSubLevels) SubLevel = 0;
          }
          SubLevelChanged = true;
          if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) StartTestMode(currentTime);
          //ShowParameterValue();
          RPU_SetDisplayCredits(TopLevel);
          RPU_SetDisplayBallInPlay(SubLevel + 1);
        }
      }
    } else if (curSwitch==BackButton) {
      EjectBalls = false;
      ParameterChanged = false;
      if (TopLevel!=OPERATOR_MENU_RETURN_TO_GAME) {

        if (NextSpeedyValueChange>1 && 
            ( AdjustmentType==OPERATOR_MENU_ADJ_TYPE_SCORE || AdjustmentType==OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT ||
              AdjustmentType==OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT ) ) {
          if (CurrentAdjustmentUL) *CurrentAdjustmentUL = 0;
          NextSpeedyValueChange = 1;
          ShowParameterValue();
        } else {                
          if (SubLevel==OPERATOR_MENU_NOT_ACTIVE) {
            SubLevel = 0;
            CurrentAdjustmentByte = NULL;
            CurrentAdjustmentUL = NULL;
          } else {
            if (SubLevel==0) {
              if (NumSubLevels) SubLevel = NumSubLevels - 1;
              else SubLevel = 0;
            } else {
              SubLevel -= 1;
            }
          }
          SubLevelChanged = true;
          if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) StartTestMode(currentTime);
          //ShowParameterValue();
          RPU_SetDisplayCredits(TopLevel);
          RPU_SetDisplayBallInPlay(SubLevel + 1);
        }
      }
    }
    
  }

  // See if any modes need to be updated
  if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) {
    UpdateSelfTest(currentTime);
  } 

  if (AdjustmentType==OPERATOR_MENU_ADJ_HOLD_TO_INSTALL) {
    if (ResetHold>1) {
      unsigned long msLeft = 1300 - (currentTime-ResetHold);
      RPU_SetDisplay(0, msLeft, true, 0);
    } else {
      RPU_SetDisplayBlank(0, 0);
    }
  }

  return 1;
}


void OperatorMenus::HandleEnterButton(boolean doubleClick, boolean resetHeld, boolean speedyChange) {

  if (AdjustmentType==OPERATOR_MENU_ADJ_HOLD_TO_INSTALL) {
    if (resetHeld) {
      if (CurrentAdjustmentByte && NumAdjustmentValues==2) {
        *CurrentAdjustmentByte = AdjustmentValues[1];
        ParameterID = *CurrentAdjustmentByte;
      }
      ParameterChanged = true;
    }
  }

  if (TopLevel==OPERATOR_MENU_SELF_TEST_MENU) {
    if (SubLevel==OPERATOR_MENU_TEST_LAMPS) {
      if (LastTestValue==99) {
        LastTestValue = 1;
      } else {
        LastTestValue += 1;
        if (LampLookupFunction(LastTestValue)==OPERATOR_MENU_VALUE_OUT_OF_RANGE) {
          LastTestValue = 99;
        }
      }

      if (LastTestValue==99) {
        RPU_SetDisplay(0, 99, true);
        for (byte count=1; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
          RPU_SetDisplayBlank(count, 0x00);
        }
        RPU_TurnOffAllLamps();
        for (int count=0; count<RPU_MAX_LAMPS; count++) {
          RPU_SetLampState(count, 1, 0, 500);
        }
      } else {
        RPU_SetDisplay(0, LastTestValue, true);
        RPU_TurnOffAllLamps();
        RPU_SetLampState(LampLookupFunction(LastTestValue), 1);
      }
      
    } else if (SubLevel==OPERATOR_MENU_TEST_DISPLAYS) {
      if (CycleTest) {
        LastTestValue = 0;
        SavedValue = 0;
        LastTestTime = 0;
        CycleTest = false;
      } else {
        for (byte count=0; count<5; count++) RPU_SetDisplayBlank(count, 0xFF);
        CycleTest = true;
      }
      
    } else if (SubLevel==OPERATOR_MENU_TEST_SOLENOIDS) {
      if (CycleTest) {
        CycleTest = false;
        TestDelay = 3;
      } else {
        CycleTest = true; 
        TestDelay = 1;
      }
    }
  } else if (TopLevel==OPERATOR_MENU_AUDITS_MENU) {
    if (AdjustmentType==OPERATOR_MENU_AUD_CLEARABLE) {
      if (resetHeld) {
        if (CurrentAdjustmentUL) {
          *CurrentAdjustmentUL = 0;
          if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, 0);
          ShowParameterValue();
        }
      }
    }
  } else if (TopLevel==OPERATOR_MENU_GAME_RULES_LEVEL) {
  } else if (TopLevel==OPERATOR_MENU_BASIC_ADJ_MENU || TopLevel==OPERATOR_MENU_GAME_ADJ_MENU) {
    
    if (CurrentAdjustmentByte && (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX || AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX_DEFAULT || AdjustmentType == OPERATOR_MENU_ADJ_TYPE_CPC)) {
      byte curVal = *CurrentAdjustmentByte;

      if (RPU_GetUpDownSwitchState()) {
        curVal += 1;
        if (curVal > AdjustmentValues[1]) {
          if (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_MIN_MAX || AdjustmentType == OPERATOR_MENU_ADJ_TYPE_CPC) curVal = AdjustmentValues[0];
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
      ParameterID = *CurrentAdjustmentByte;
      ParameterChanged = true;
      if (CurrentAdjustmentStorageByte) RPU_WriteByteToEEProm(CurrentAdjustmentStorageByte, curVal);

    } else if (CurrentAdjustmentByte && AdjustmentType == OPERATOR_MENU_ADJ_TYPE_LIST) {
      byte valCount = 0;
      byte curVal = *CurrentAdjustmentByte;
      byte newIndex = 0;
      boolean upDownState = RPU_GetUpDownSwitchState();
      for (valCount = 0; valCount < (NumAdjustmentValues); valCount++) {
        if (curVal == AdjustmentValues[valCount]) {
          if (upDownState) {
            if (valCount < (NumAdjustmentValues - 1)) newIndex = valCount + 1;
            else newIndex = 0;
          } else {
            if (valCount > 0) newIndex = valCount - 1;
          }
          break;
        }
      }
      *CurrentAdjustmentByte = AdjustmentValues[newIndex];
      ParameterID = newIndex;
      ParameterChanged = true;
      if (CurrentAdjustmentStorageByte) RPU_WriteByteToEEProm(CurrentAdjustmentStorageByte, AdjustmentValues[newIndex]);
    } else if (CurrentAdjustmentUL && 
                ( AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT || AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT || 
                  AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE ) ) {
      unsigned long curVal = *CurrentAdjustmentUL;
      unsigned long changeVal = 10000;
      if (NumSpeedyChanges>6) changeVal = 25000;

      if (speedyChange) {
        if (RPU_GetUpDownSwitchState()) curVal += changeVal;
        else if (curVal >= changeVal) curVal -= changeVal;
        if (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT && curVal == 0) curVal = 1000;
        *CurrentAdjustmentUL = curVal;
        ParameterChanged = true;
        if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, curVal);
      } else if (resetHeld==false) {
        if (RPU_GetUpDownSwitchState()) curVal += 1000;
        else if (curVal >= 1000) curVal -= 1000;
        if (AdjustmentType == OPERATOR_MENU_ADJ_TYPE_SCORE_NO_DEFAULT && curVal == 0) curVal = 1000;
        *CurrentAdjustmentUL = curVal;
        ParameterChanged = true;
        if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, curVal);
      }
    }

    ShowParameterValue();
  }

  (void)doubleClick;
}



void OperatorMenus::ReadCurrentSwitches() {
  byte numSwitchesToShow = RPU_NUMBER_OF_PLAYER_DISPLAYS * 2;
  byte count, switchCount;

  for (count=0; count<numSwitchesToShow; count++) {
    AdjustmentValues[count] = SWITCH_STACK_EMPTY;
  }
  
  for (count=0; count<numSwitchesToShow; count++) {
    for (switchCount=LastTestValue; switchCount<64; switchCount++) {
      if (RPU_ReadSingleSwitchState(switchCount)) {
        // We found a switch, so we can record it and move on
        AdjustmentValues[count] = switchCount;
        LastTestValue = switchCount + 1;
        break;
      }      
    }
    if (switchCount==64) {
      // can't find anymore switches, so break
      LastTestValue = 0;
      return;
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
      curSwitch += 1;
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
      curSwitch += 1;
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

    if (LastSwitchSeen!=SWITCH_STACK_EMPTY) {
      RPU_SetDisplayBallInPlay(LastSwitchSeen+1, true);
    } 
    
    if (LastTestTime==0 || currentTime>(LastTestTime+1500) || LastSwitchSeen!=SWITCH_STACK_EMPTY) {
      ReadCurrentSwitches();
      unsigned long displayValue = 0;
      byte switchToShow = 0;
      byte displayMask = 0;
      for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
        // Switches have a "1" starting index now
        displayValue = (unsigned long)(AdjustmentValues[switchToShow]+1) * 1000 + (unsigned long)(AdjustmentValues[switchToShow+1]+1);
        displayMask = GetDisplayMaskForSwitches(AdjustmentValues[switchToShow], AdjustmentValues[switchToShow+1]);
        switchToShow += 2;
        RPU_SetDisplay(count, displayValue, false);
        RPU_SetDisplayBlank(count, displayMask);
      }
      LastTestTime = currentTime;
    }
  } else if (SubLevel==OPERATOR_MENU_TEST_DISPLAYS) {

    if (!CycleTest) {
      if (LastTestTime==0 || currentTime>(LastTestTime+100)) {
        LastTestTime = currentTime;
        LastTestValue += 1;
        RPU_CycleAllDisplays(currentTime, LastTestValue, SavedValue);

        if (LastTestValue>=TOTAL_DISPLAY_DIGITS) {
          LastTestValue = 0;
          SavedValue += 1;
          if (SavedValue>9) SavedValue = 0;
        }
      }
    } else {
      RPU_CycleAllDisplays(currentTime, 0);
    }
        
  } else if (SubLevel==OPERATOR_MENU_TEST_SOLENOIDS) {
    if (currentTime>(LastTestTime+((unsigned long)TestDelay * 1000))) {

      if (CycleTest) {
        LastTestValue = NextTestValue;
      }
      LastTestTime = currentTime;

      // make sure next value is used
      for (byte count=0; count<32; count++) {
        if (SolenoidIDLookupFunction(LastTestValue)==OPERATOR_MENU_VALUE_UNUSED) LastTestValue += 1;
        else if (SolenoidIDLookupFunction(LastTestValue)==OPERATOR_MENU_VALUE_OUT_OF_RANGE) LastTestValue = 1;
        else break;
      }

      RPU_SetDisplay(0, LastTestValue, true);      

      unsigned short solenoidIndex = SolenoidIDLookupFunction(LastTestValue);
      byte solenoidStrength = SolenoidStrengthLookupFunction(LastTestValue);
      if (solenoidIndex!=OPERATOR_MENU_VALUE_UNUSED && solenoidIndex!=OPERATOR_MENU_VALUE_OUT_OF_RANGE) {
        if (solenoidStrength==OPERATOR_MENU_VALUE_UNUSED || solenoidStrength==OPERATOR_MENU_VALUE_OUT_OF_RANGE) {
          solenoidStrength = 4; // low default value
        }
        if (solenoidIndex & 0xFF00) {
          // This is a continuous solenoid to be tested
          RPU_FireContinuousSolenoid(solenoidIndex / 256, 5);
        } else {
          RPU_PushToSolenoidStack(solenoidIndex & 0x00FF, solenoidStrength);
        }
      }
//      if (LastTestValue<15) {
//        RPU_PushToSolenoidStack(LastTestValue, 10);
//      } else {
//        RPU_FireContinuousSolenoid(0x10<<(LastTestValue-15), 5);
//      }

      NextTestValue = LastTestValue + 1;

      if (CycleTest) {
        SavedValue = 0;
      } else {
        SavedValue += 1;
        if (SavedValue>10) TestDelay = 5;
      }
    }
  }
}


void OperatorMenus::StartTestMode(unsigned long currentTime) {

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
    LastTestValue = 99;
  } else if (SubLevel==OPERATOR_MENU_TEST_DISPLAYS) {
    RPU_TurnOffAllLamps();
    for (int count=0; count<4; count++) {
      RPU_SetDisplayBlank(count, RPU_OS_ALL_DIGITS_MASK);        
    }
    CycleTest = true;
    LastTestValue = 0;
    RPU_CycleAllDisplays(currentTime, LastTestValue);
  } else if (SubLevel==OPERATOR_MENU_TEST_SOLENOIDS) {
    for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
      RPU_SetDisplayBlank(count, 0);
    }
    RPU_TurnOffAllLamps();
    LastTestTime = currentTime;    
    RPU_EnableSolenoidStack();
    CycleTest = true;
    NextTestValue = 1;
    LastTestValue = 1;
    TestDelay = 1;
  } else if (SubLevel==OPERATOR_MENU_TEST_SWITCHES) {
    RPU_TurnOffAllLamps();
    LastTestTime = 0;
    LastTestValue = 0;
    RPU_SetDisplayBallInPlay(0, false);
  } else if (SubLevel==OPERATOR_MENU_TEST_SOUNDS) {
    for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
      RPU_SetDisplayBlank(count, 0x00);
    }
  } else if (SubLevel==OPERATOR_MENU_TEST_EJECT_BALLS) {
    for (byte count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) {
      RPU_SetDisplayBlank(count, 0x00);
    }
    EjectBalls = true;
  }

}


OperatorMenus::~OperatorMenus() {
}
