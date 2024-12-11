
// Display Management Functions

#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF
#define DISPLAY_OVERRIDE_ANIMATION_NONE     0
#define DISPLAY_OVERRIDE_ANIMATION_BOUNCE   1
#define DISPLAY_OVERRIDE_ANIMATION_FLUTTER  2
#define DISPLAY_OVERRIDE_ANIMATION_FLYBY    3
#define DISPLAY_OVERRIDE_ANIMATION_CENTER   4
#define DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE   5



void Display_ResetDisplayTrackingVariables();
byte Display_MagnitudeOfScore(unsigned long score);
void Display_SetAnimationDisplayOrder(byte disp0, byte disp1, byte disp2, byte disp3);

void Display_OverrideScoreDisplay(byte displayNum, unsigned long value, byte animationType, byte overrideMask = 0xFF);
void Display_ClearOverride(byte displayNum = 0xFF);
void Display_StartScoreAnimation(unsigned long scoreAdditionValue, boolean playTick);
//void Display_ShowflybyValue(byte numToShow, unsigned long timeBase);

void Display_SetLastTimeScoreChanged(unsigned long scoreChangedTime);
unsigned long Display_GetLastTimeScoreChanged();
byte Display_UpdateDisplays(byte displayNum = 0xFF, boolean finishAnimation = false, boolean flashCurrent = false, boolean dashCurrent = false, unsigned long allScoresShowValue = 0xFFFFFFFF);
