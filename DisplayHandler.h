
// Display Management Functions

#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF
#define DISPLAY_OVERRIDE_ANIMATION_NONE     0
#define DISPLAY_OVERRIDE_ANIMATION_BOUNCE   1
#define DISPLAY_OVERRIDE_ANIMATION_FLUTTER  2
#define DISPLAY_OVERRIDE_ANIMATION_FLYBY    3
#define DISPLAY_OVERRIDE_ANIMATION_CENTER   4
#define DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE   5

void ResetDisplayTrackingVariables();
void SetLastTimeScoreChanged(unsigned long scoreChangedTime);

void OverrideScoreDisplay(byte displayNum, unsigned long value, byte animationType);
void SetAnimationDisplayOrder(byte disp0, byte disp1, byte disp2, byte disp3);
void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0);
void ShowFlybyValue(byte numToShow, unsigned long timeBase);
void StartScoreAnimation(unsigned long scoreToAnimate, boolean playTick = true);

boolean UpdateDisplays(boolean finishAnimation);
