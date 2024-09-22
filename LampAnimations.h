#ifndef LAMP_ANIMATIONS_H

#include "RPU_Config.h"
#include "RPU.h"

byte ElwoodLampAssignments[] = {LAMP_E_1, LAMP_L, LAMP_W, LAMP_O_1, LAMP_O_2, LAMP_D};
byte JakeLampAssignments[] = {LAMP_J, LAMP_A, LAMP_K, LAMP_E_2};




// This file can define a series of animations, stored 
// with each lamp as a bit in the following array.
// Lamp 0 = the first bit of the first byte, so "{0x01," below.
// The animations can be played with either of the helper functions
// at the bottom of this file.
// 
// These demonstration animations should be replaced
// or removed for each specific implementation.



// Lamp animation arrays
#define NUM_LAMP_ANIMATIONS       5
#define LAMP_ANIMATION_STEPS      24
#define NUM_LAMP_ANIMATION_BYTES  6
byte LampAnimations[NUM_LAMP_ANIMATIONS][LAMP_ANIMATION_STEPS][NUM_LAMP_ANIMATION_BYTES] = {
  // Radar Animation (index = 0)
  {
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x0C, 0x00, 0x00, 0x80, 0x00}, // lamps on = 3
    {0x80, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x00, 0x44, 0x00, 0x00, 0x00, 0x08}, // lamps on = 3
    {0x00, 0x44, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x08, 0x44, 0x00, 0x00, 0x00, 0x80}, // lamps on = 5
    {0x00, 0x04, 0x04, 0x04, 0x00, 0x04}, // lamps on = 5
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x04}, // lamps on = 2
    {0x40, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x00, 0x04, 0x00, 0x00, 0x04, 0x40}, // lamps on = 3
    {0x04, 0x04, 0x00, 0x00, 0x44, 0x00}, // lamps on = 4
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x30}, // lamps on = 3
    {0x10, 0x04, 0x00, 0x00, 0x10, 0x00}, // lamps on = 3
    {0x00, 0x05, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x01, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x04, 0x00, 0x00, 0x20, 0x00}, // lamps on = 2
    {0x00, 0x04, 0x02, 0x00, 0x20, 0x00}, // lamps on = 3
    {0x20, 0x16, 0x10, 0x10, 0x00, 0x00}, // lamps on = 6
    {0x02, 0x24, 0x00, 0x00, 0x02, 0x02}, // lamps on = 5
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}  // lamps on = 1
  // Bits Missing
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  // Center Out Animation (index = 1)
  {
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x40, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x40, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x24}, // lamps on = 2
    {0x11, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x10, 0x00, 0x00, 0x00, 0x30, 0x80}, // lamps on = 4
    {0x00, 0x02, 0x00, 0x00, 0x80, 0x10}, // lamps on = 3
    {0x00, 0x20, 0x00, 0x04, 0x04, 0x00}, // lamps on = 3
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x08, 0x00, 0x00, 0x00, 0x40, 0x00}, // lamps on = 2
    {0x80, 0x00, 0x04, 0x00, 0x00, 0x40}, // lamps on = 3
    {0x04, 0x08, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x08}, // lamps on = 2
    {0x00, 0x11, 0x10, 0x10, 0x00, 0x00}, // lamps on = 4
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, // lamps on = 2
    {0x22, 0x00, 0x02, 0x00, 0x02, 0x02}  // lamps on = 6
  // Bits Missing
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  // Bottom to Top Animation (index = 2)
  {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x08}, // lamps on = 2
    {0x00, 0x00, 0x04, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x8C, 0x08, 0x00, 0x00, 0x00, 0x40}, // lamps on = 5
    {0x00, 0x00, 0x00, 0x04, 0x40, 0x00}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x80}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x80, 0x04}, // lamps on = 2
    {0x00, 0x40, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x20}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x10, 0x10}, // lamps on = 2
    {0x11, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x20, 0x00}, // lamps on = 1
    {0x00, 0x22, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x01, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x10, 0x10, 0x10, 0x00, 0x00}, // lamps on = 3
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02}, // lamps on = 1
    {0x22, 0x00, 0x02, 0x00, 0x02, 0x00}  // lamps on = 4
  // Bits Missing
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  // VUK Center Animation (index = 3)
  {
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x44, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x40}, // lamps on = 1
    {0x04, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x04, 0x00, 0x00, 0x04, 0x04, 0x04}, // lamps on = 3
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x80}, // lamps on = 2
    {0x48, 0x40, 0x04, 0x00, 0x04, 0x20}, // lamps on = 5
    {0x00, 0x04, 0x00, 0x00, 0x00, 0x30}, // lamps on = 3
    {0x10, 0x00, 0x00, 0x00, 0x94, 0x00}, // lamps on = 3
    {0x01, 0x00, 0x00, 0x00, 0x80, 0x00}, // lamps on = 2
    {0x80, 0x00, 0x00, 0x00, 0x24, 0x08}, // lamps on = 4
    {0x00, 0x0A, 0x00, 0x00, 0x20, 0x00}, // lamps on = 3
    {0x00, 0x20, 0x00, 0x00, 0x04, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x01, 0x00, 0x00, 0x04, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x10, 0x10, 0x10, 0x04, 0x00}, // lamps on = 3
    {0x22, 0x00, 0x02, 0x00, 0x02, 0x02}  // lamps on = 5
  // Bits Missing
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  // Pop Bumper Center Animation (index = 4)
  {
    {0x00, 0x10, 0x10, 0x10, 0x00, 0x00}, // lamps on = 3
    {0x00, 0x10, 0x10, 0x10, 0x00, 0x00}, // lamps on = 3
    {0x00, 0x00, 0x00, 0x10, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x22, 0x00, 0x00, 0x00, 0x02, 0x02}, // lamps on = 4
    {0x02, 0x00, 0x02, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // lamps on = 0
    {0x00, 0x20, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x02, 0x00, 0x00, 0x20, 0x00}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x20, 0x00}, // lamps on = 1
    {0x01, 0x01, 0x00, 0x00, 0x00, 0x00}, // lamps on = 2
    {0x10, 0x00, 0x00, 0x00, 0x10, 0x00}, // lamps on = 2
    {0x00, 0x04, 0x00, 0x00, 0x10, 0x30}, // lamps on = 4
    {0x00, 0x40, 0x00, 0x00, 0x00, 0x00}, // lamps on = 1
    {0x00, 0x00, 0x00, 0x00, 0x80, 0x04}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x04, 0x80}, // lamps on = 2
    {0x88, 0x08, 0x00, 0x04, 0x40, 0x00}, // lamps on = 5
    {0x04, 0x00, 0x04, 0x00, 0x00, 0x40}, // lamps on = 3
    {0x40, 0x00, 0x00, 0x00, 0x00, 0x08}, // lamps on = 2
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // lamps on = 1 *turned off lamp 35 so we don't promise shoot again
  // Bits Missing
  // 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  }  
};



void ShowLampAnimation(byte animationNum, unsigned long divisor, unsigned long baseTime, byte subOffset, boolean dim, boolean reverse = false, byte keepLampOn = 99) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte currentStep = (baseTime / divisor) % LAMP_ANIMATION_STEPS;
  if (reverse) currentStep = (LAMP_ANIMATION_STEPS - 1) - currentStep;

  byte curBitmask;
  byte *currentLampOffsetByte = LampAnimations[animationNum][(currentStep + subOffset) % LAMP_ANIMATION_STEPS];
  byte *currentLampByte = LampAnimations[animationNum][currentStep];

  byte lampNum = 0;
  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    curBitmask = 0x01;
    
    for (byte bitNum = 0; bitNum < 8; bitNum++) {

      byte lampOn = false;
      lampOn = (*currentLampByte) & curBitmask;

      // if there's a subOffset, turn off lights at that offset
      if (subOffset) {
        byte lampOff = true;
        lampOff = (*currentLampOffsetByte) & curBitmask;
        if (lampOff && lampNum != keepLampOn && !lampOn) RPU_SetLampState(lampNum, 0);
      }

      if (lampOn) RPU_SetLampState(lampNum, 1, dim);

      curBitmask *= 2;
      lampNum += 1;
    }
    currentLampByte += 1;
    currentLampOffsetByte += 1;
  }
}


void ShowLampAnimationSingleStep(byte animationNum, byte currentStep, byte *lampsToAvoid = NULL) {
  if (animationNum>=NUM_LAMP_ANIMATIONS) return;
  
  byte lampNum = 0;
  byte *currentLampByte = LampAnimations[animationNum][currentStep];
  byte *currentAvoidByte = lampsToAvoid;
  byte curBitmask;
  
  for (int byteNum = 0; byteNum < NUM_LAMP_ANIMATION_BYTES; byteNum++) {
    curBitmask = 0x01;
    for (byte bitNum = 0; bitNum < 8; bitNum++) {

      boolean avoidLamp = false;
      if (currentAvoidByte!=NULL) {
        if ((*currentAvoidByte) & curBitmask) avoidLamp = true;
      }

      if (!avoidLamp /*&& (*currentLampByte)&curBitmask*/) RPU_SetLampState(lampNum, (*currentLampByte)&curBitmask);

      lampNum += 1;
      curBitmask *= 2;
    }
    currentLampByte += 1;
    if (currentAvoidByte!=NULL) currentAvoidByte += 1;
  }
}



boolean LampAnimationMinimodeStart(unsigned long elapsedTime, boolean showJake, boolean showElwood, boolean showSpinner) {
  if (elapsedTime>2921) return false;

  if (elapsedTime<1796) {
    for (byte count=0; count<6; count++) RPU_SetLampState(ElwoodLampAssignments[count], showElwood, 0, 75);  
    for (byte count=0; count<4; count++) RPU_SetLampState(JakeLampAssignments[count], showJake, 0, 75);  
    RPU_SetLampState(LAMP_LEFT_SPINNER, showSpinner, 0, 75);  
    RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 75);
  }

  if (elapsedTime<275) {
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 0);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 0);
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
    RPU_SetLampState(LAMP_LEFT_INLANE, 0);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 0);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 0);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);
  } else if (elapsedTime<494 || (elapsedTime>866 && elapsedTime<1032)) {
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 1);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 1);
    RPU_SetLampState(LAMP_LEFT_INLANE, 1);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 1);  
    RPU_SetLampState(LAMP_HIDE_AWAY, 1);
    RPU_SetLampState(LAMP_BONUS_1, 1);
    RPU_SetLampState(LAMP_BONUS_2, 1);
    RPU_SetLampState(LAMP_BONUS_3, 1);
    RPU_SetLampState(LAMP_BONUS_4, 1);
    RPU_SetLampState(LAMP_BONUS_5, 1);
    RPU_SetLampState(LAMP_BONUS_6, 1);
    RPU_SetLampState(LAMP_SPECIAL, 1);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 0);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 0);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 0);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 0);
  } else if (elapsedTime<866) {
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
    RPU_SetLampState(LAMP_LEFT_INLANE, 0);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);  
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 1);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 1);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 1);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 1);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 1);
  } else if (elapsedTime<1230) {
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
    RPU_SetLampState(LAMP_LEFT_INLANE, 0);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);  
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_BONUS_1, 1);
    RPU_SetLampState(LAMP_BONUS_2, 1);
    RPU_SetLampState(LAMP_BONUS_3, 1);
    RPU_SetLampState(LAMP_BONUS_4, 1);
    RPU_SetLampState(LAMP_BONUS_5, 1);
    RPU_SetLampState(LAMP_BONUS_6, 1);
    RPU_SetLampState(LAMP_SPECIAL, 1);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 0);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 0);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 0);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 0);
  } else if (elapsedTime<1412) {
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 1);
    RPU_SetLampState(LAMP_LEFT_INLANE, 1);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 1);  
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 0);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 0);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 1);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 1);
  } else if (elapsedTime<1683) {
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 1);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
    RPU_SetLampState(LAMP_LEFT_INLANE, 0);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);  
    RPU_SetLampState(LAMP_HIDE_AWAY, 1);
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 1);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 1);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 1);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 1);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 1);
  } else if (elapsedTime<1796 || elapsedTime>=2500) {
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
    RPU_SetLampState(LAMP_LEFT_INLANE, 0);
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);  
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, 0);
    RPU_SetLampState(LAMP_POP_BLUE_UP, 0);
    RPU_SetLampState(LAMP_RIGHT_INLANE, 0);
    RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 0);
  } else if (elapsedTime<2500) {
    byte animationStep = ((elapsedTime-1796)/30)%LAMP_ANIMATION_STEPS;
    ShowLampAnimationSingleStep(1, animationStep);
  } 
  return true;
}


#define LAMP_ANIMATIONS_H
#endif
