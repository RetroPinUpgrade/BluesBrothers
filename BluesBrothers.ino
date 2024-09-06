/**************************************************************************
    This pinball Blues Brothers code is distributed in the hope that it
    will be useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.
*/

#include "RPU_Config.h"
#include "RPU.h"
#include "DropTargets.h"
#include "BluesBrothers.h"
#include "SelfTestAndAudit.h"
#include "AudioHandler.h"
#include "DisplayHandler.h"
#include "LampAnimations.h"
#include <EEPROM.h>

#define GAME_MAJOR_VERSION  2024
#define GAME_MINOR_VERSION  5
#define DEBUG_MESSAGES  1

/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - self-test modes
//  positive - game play
char MachineState = 0;
boolean MachineStateChanged = true;
#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110
#define MACHINE_STATE_DIAGNOSTICS     120

#define MACHINE_STATE_ADJUST_FREEPLAY             (MACHINE_STATE_TEST_DONE-1)
#define MACHINE_STATE_ADJUST_BALL_SAVE            (MACHINE_STATE_TEST_DONE-2)
#define MACHINE_STATE_ADJUST_SOUND_SELECTOR       (MACHINE_STATE_TEST_DONE-3)
#define MACHINE_STATE_ADJUST_MUSIC_VOLUME         (MACHINE_STATE_TEST_DONE-4)
#define MACHINE_STATE_ADJUST_SFX_VOLUME           (MACHINE_STATE_TEST_DONE-5)
#define MACHINE_STATE_ADJUST_CALLOUTS_VOLUME      (MACHINE_STATE_TEST_DONE-6)
#define MACHINE_STATE_ADJUST_TOURNAMENT_SCORING   (MACHINE_STATE_TEST_DONE-7)
#define MACHINE_STATE_ADJUST_TILT_WARNING         (MACHINE_STATE_TEST_DONE-8)
#define MACHINE_STATE_ADJUST_AWARD_OVERRIDE       (MACHINE_STATE_TEST_DONE-9)
#define MACHINE_STATE_ADJUST_BALLS_OVERRIDE       (MACHINE_STATE_TEST_DONE-10)
#define MACHINE_STATE_ADJUST_SCROLLING_SCORES     (MACHINE_STATE_TEST_DONE-11)
#define MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD     (MACHINE_STATE_TEST_DONE-12)
#define MACHINE_STATE_ADJUST_SPECIAL_AWARD        (MACHINE_STATE_TEST_DONE-13)
#define MACHINE_STATE_ADJUST_CREDIT_RESET_HOLD_TIME          (MACHINE_STATE_TEST_DONE-14)
#define MACHINE_STATE_ADJUST_DONE                 (MACHINE_STATE_TEST_DONE-15)


// Indices of EEPROM save locations
#define EEPROM_BALL_SAVE_BYTE           100
#define EEPROM_FREE_PLAY_BYTE           101
#define EEPROM_SOUND_SELECTOR_BYTE      102
#define EEPROM_SKILL_SHOT_BYTE          103
#define EEPROM_TILT_WARNING_BYTE        104
#define EEPROM_AWARD_OVERRIDE_BYTE      105
#define EEPROM_BALLS_OVERRIDE_BYTE      106
#define EEPROM_TOURNAMENT_SCORING_BYTE  107
#define EEPROM_SFX_VOLUME_BYTE          108
#define EEPROM_MUSIC_VOLUME_BYTE        109
#define EEPROM_SCROLLING_SCORES_BYTE    110
#define EEPROM_CALLOUTS_VOLUME_BYTE     111
#define EEPROM_CRB_HOLD_TIME            118
#define EEPROM_EXTRA_BALL_SCORE_UL      140
#define EEPROM_SPECIAL_SCORE_UL         144


#define GAME_MODE_SKILL_SHOT                        1
#define GAME_MODE_UNSTRUCTURED_PLAY                 2
#define GAME_MODE_MINIMODE_START                    3
#define GAME_MODE_MINIMODE                          4
#define GAME_MODE_MINIMODE_END                      5
#define GAME_MODE_LOCK_BALL_1                       6
#define GAME_MODE_LOCK_BALL_2                       7
#define GAME_MODE_MULTIBALL_2                       10
#define GAME_MODE_MULTIBALL_3_START                 20
#define GAME_MODE_MULTIBALL_3                       21
#define GAME_MODE_MINI_WIZARD_START                 30
#define GAME_MODE_MINI_WIZARD                       31
#define GAME_MODE_MINI_WIZARD_END                   32
#define GAME_MODE_FULL_WIZARD_START                 33
#define GAME_MODE_FULL_WIZARD                       34
#define GAME_MODE_FULL_WIZARD_END                   35



#define SOUND_EFFECT_NONE                     0
#define SOUND_EFFECT_BONUS_COUNT              1
#define SOUND_EFFECT_DROP_TARGET_COMPLETE     2
#define SOUND_EFFECT_TILT                     3
#define SOUND_EFFECT_TILT_WARNING             4
#define SOUND_EFFECT_DOOR_SLAM                5
#define SOUND_EFFECT_10_SECOND_WARNING        6
#define SOUND_EFFECT_SPINNER_UNLIT_1          7
#define SOUND_EFFECT_SPINNER_UNLIT_2          8
#define SOUND_EFFECT_SHORT_SIREN              9
#define SOUND_EFFECT_MEDIUM_SIREN             10
#define SOUND_EFFECT_LONG_SIREN               11
#define SOUND_EFFECT_CELL_DOOR                12
#define SOUND_EFFECT_LOOP_1                   13
#define SOUND_EFFECT_LOOP_2                   14
#define SOUND_EFFECT_TIRES_SQUEALING          15
#define SOUND_EFFECT_SLINGSHOT                16
#define SOUND_EFFECT_MISSED_MODE_SHOT         17
#define SOUND_EFFECT_CORRECT_MODE_SHOT        18
#define SOUND_EFFECT_JAIL_BREAK               19
#define SOUND_EFFECT_BALL_OVER                20
#define SOUND_EFFECT_GAME_OVER                21
#define SOUND_EFFECT_TOP_GATE_OPENING         22
#define SOUND_EFFECT_TOP_GATE_CLOSING         23
#define SOUND_EFFECT_DROP_TARGET_BONUS_SOUND  24
#define SOUND_EFFECT_BAND_HIT_1               25
#define SOUND_EFFECT_BAND_HIT_2               26
#define SOUND_EFFECT_BAND_HIT_3               27
#define SOUND_EFFECT_MATCH_SPIN               30
#define SOUND_EFFECT_RICOCHET_0               40
#define SOUND_EFFECT_RICOCHET_1               41
#define SOUND_EFFECT_RICOCHET_2               42
#define SOUND_EFFECT_RICOCHET_3               43
#define SOUND_EFFECT_RICOCHET_4               44
#define SOUND_EFFECT_RICOCHET_5               45
#define SOUND_EFFECT_RICOCHET_6               46
#define SOUND_EFFECT_RICOCHET_7               47
#define SOUND_EFFECT_ELECTRIC_PIANO_1         50
#define SOUND_EFFECT_ELECTRIC_PIANO_2         51
#define SOUND_EFFECT_ELECTRIC_PIANO_3         52
#define SOUND_EFFECT_ELECTRIC_PIANO_4         53
#define SOUND_EFFECT_ELECTRIC_PIANO_5         54
#define SOUND_EFFECT_SCORE_TICK               67
#define SOUND_EFFECT_STARTUP_1                90
#define SOUND_EFFECT_STARTUP_2                91


#define SOUND_EFFECT_COIN_DROP_1        100
#define SOUND_EFFECT_COIN_DROP_2        101
#define SOUND_EFFECT_COIN_DROP_3        102
#define SOUND_EFFECT_MACHINE_START      120

#if (RPU_MPU_ARCHITECTURE<10) && !defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
// This array maps the self-test modes to audio callouts
unsigned short SelfTestStateToCalloutMap[34] = {  136, 137, 135, 134, 133, 140, 141, 142, 139, 143, 144, 145, 146, 147, 148, 149, 138, 150, 151, 152,
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166
                                               };
#elif (RPU_MPU_ARCHITECTURE<10) && defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
unsigned short SelfTestStateToCalloutMap[31] = {  136, 137, 135, 134, 133, 140, 141, 142, 139, 143, 144, 145, 146, 147, 148, 149, 138,
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166
                                               };
#elif (RPU_MPU_ARCHITECTURE>=10) && !defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
// This array maps the self-test modes to audio callouts
unsigned short SelfTestStateToCalloutMap[34] = {  134, 135, 133, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152,
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166
                                               };
#elif (RPU_MPU_ARCHITECTURE>=10) && defined(RPU_OS_DISABLE_CPC_FOR_SPACE)
unsigned short SelfTestStateToCalloutMap[34] = {  134, 135, 133, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149,
                                                  153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166
                                               };
#endif

#define SOUND_EFFECT_SELF_TEST_MODE_START             132
#define SOUND_EFFECT_SELF_TEST_CPC_START              180
#define SOUND_EFFECT_SELF_TEST_AUDIO_OPTIONS_START    190

#define SOUND_EFFECT_BACKGROUND_SONG_1                400
#define SOUND_EFFECT_BACKGROUND_SONG_2                401
#define SOUND_EFFECT_BACKGROUND_SONG_3                402
#define SOUND_EFFECT_BACKGROUND_SONG_4                403
#define SOUND_EFFECT_BACKGROUND_SONG_5                404
#define SOUND_EFFECT_MINIMODE_SONG_1                  410
#define SOUND_EFFECT_MINIMODE_SONG_2                  411
#define SOUND_EFFECT_MINIMODE_SONG_3                  412
#define SOUND_EFFECT_MULTIBALL_SONG_1                 420
#define SOUND_EFFECT_MULTIBALL_SONG_2                 421
#define SOUND_EFFECT_MULTIBALL_SONG_3                 422
#define SOUND_EFFECT_RALLY_MUSIC_1                    450


// Game play status callouts
#define SOUND_EFFECT_VP_PLAYER_1_UP                   301
#define SOUND_EFFECT_VP_PLAYER_2_UP                   302
#define SOUND_EFFECT_VP_PLAYER_3_UP                   303
#define SOUND_EFFECT_VP_PLAYER_4_UP                   304
#define SOUND_EFFECT_VP_EXTRA_BALL                    305

#define SOUND_EFFECT_VP_ADD_PLAYER_1        306
#define SOUND_EFFECT_VP_ADD_PLAYER_2        (SOUND_EFFECT_VP_ADD_PLAYER_1+1)
#define SOUND_EFFECT_VP_ADD_PLAYER_3        (SOUND_EFFECT_VP_ADD_PLAYER_1+2)
#define SOUND_EFFECT_VP_ADD_PLAYER_4        (SOUND_EFFECT_VP_ADD_PLAYER_1+3)
#define SOUND_EFFECT_VP_SHOOT_AGAIN         310

#define SOUND_EFFECT_VP_JAKE_MINIMODE_QUALIFIED         311
#define SOUND_EFFECT_VP_ELWOOD_MINIMODE_QUALIFIED       312
#define SOUND_EFFECT_VP_BAND_MINIMODE_QUALIFIED         313
#define SOUND_EFFECT_VP_BONUS_2X                        314
#define SOUND_EFFECT_VP_BONUS_3X                        315
#define SOUND_EFFECT_VP_BONUS_5X                        316
#define SOUND_EFFECT_VP_BONUS_10X                       317
#define SOUND_EFFECT_VP_BALL_MISSING                    318
#define SOUND_EFFECT_VP_BALL_1_LOCKED                   322
#define SOUND_EFFECT_VP_LOCK_1_STOLEN                   323
#define SOUND_EFFECT_VP_BALL_2_LOCKED                   324
#define SOUND_EFFECT_VP_LOCK_2_STOLEN                   325
#define SOUND_EFFECT_VP_BALL_SAVE                       326


#define SOUND_EFFECT_DIAG_START                   1900
#define SOUND_EFFECT_DIAG_CREDIT_RESET_BUTTON     1900
#define SOUND_EFFECT_DIAG_SELECTOR_SWITCH_ON      1901
#define SOUND_EFFECT_DIAG_SELECTOR_SWITCH_OFF     1902
#define SOUND_EFFECT_DIAG_STARTING_ORIGINAL_CODE  1903
#define SOUND_EFFECT_DIAG_STARTING_NEW_CODE       1904
#define SOUND_EFFECT_DIAG_ORIGINAL_CPU_DETECTED   1905
#define SOUND_EFFECT_DIAG_ORIGINAL_CPU_RUNNING    1906
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_U10         1907
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_U11         1908
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_1           1909
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_2           1910
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_3           1911
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_4           1912
#define SOUND_EFFECT_DIAG_PROBLEM_PIA_5           1913
#define SOUND_EFFECT_DIAG_STARTING_DIAGNOSTICS    1914


#define MAX_DISPLAY_BONUS     21
#define TILT_WARNING_DEBOUNCE_TIME      1000

#define VUK_SOLENOID_STRENGTH           10
#define SHOOTER_KICK_FULL_STRENGTH      25
#define SHOOTER_KICK_LIGHT_STRENGTH     1
#define BALL_SERVE_SOLENOID_STRENGTH    5

/*********************************************************************

    Machine state and options

*********************************************************************/
byte Credits = 0;
byte SoundSelector = 3;
byte BallSaveNumSeconds = 0;
byte MaximumCredits = 40;
byte BallsPerGame = 3;
byte ScoreAwardReplay = 0;
byte MusicVolume = 6;
byte SoundEffectsVolume = 8;
byte CalloutsVolume = 10;
byte ChuteCoinsInProgress[3];
byte TotalBallsLoaded = 3;
byte TimeRequiredToResetGame = 1;
byte NumberOfBallsInPlay = 0;
byte NumberOfBallsLocked = 0;
byte LampType = 0;
boolean FreePlayMode = false;
boolean HighScoreReplay = true;
boolean MatchFeature = true;
boolean TournamentScoring = false;
boolean ScrollingScores = true;
unsigned long ExtraBallValue = 0;
unsigned long SpecialValue = 0;
unsigned long CurrentTime = 0;
unsigned long HighScore = 0;
unsigned long AwardScores[3];
unsigned long CreditResetPressStarted = 0;
unsigned long BallRampKicked = 0;
unsigned long TrapSwitchClosedTime;


AudioHandler Audio;



/*********************************************************************

    Game State

*********************************************************************/
byte CurrentPlayer = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;
byte Bonus[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte BonusX[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte GameMode = GAME_MODE_SKILL_SHOT;
byte LastGameMode = 0;
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;
byte CurrentAchievements[RPU_NUMBER_OF_PLAYERS_ALLOWED];

boolean SamePlayerShootsAgain = false;
boolean BallSaveUsed = false;
boolean ExtraBallCollected = false;
boolean SpecialCollected = false;
boolean TimersPaused = true;
boolean AllowResetAfterBallOne = true;

unsigned long CurrentScores[RPU_NUMBER_OF_PLAYERS_ALLOWED];
unsigned long BallFirstSwitchHitTime = 0;
unsigned long BallTimeInTrough = 0;
unsigned long GameModeStartTime = 0;
unsigned long GameModeEndTime = 0;
unsigned long LastTiltWarningTime;
unsigned long PlayfieldMultiplier;
unsigned long LastTimeThroughLoop;
unsigned long LastTimePopHit = 0;
unsigned long LastSwitchHitTime;
unsigned long BallSaveEndTime;

#define BALL_SAVE_GRACE_PERIOD  3000

/*********************************************************************

    Game Specific State Variables

*********************************************************************/
int SpinnerProgress;
int SpinnerHitsToQualifyBand = 500;

byte LastSpinnerSeen;
byte JakeStatus[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodStatus[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte JakeCompletions[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodCompletions[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte MiniModesRunning[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte MiniModesQualified[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodClearsToQualify = 2;
byte JakeClearsToQualify = 1;
byte BallSaveOnMinimodes = 5;
byte BallSaveOnMultiball = 5;
byte MachineLocks = 0;
byte PlayerLocks[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodTargetLit;
byte JakeTargetLit;
byte JakeShotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodShotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte BandShotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte LockHandling;
byte RightFlipsForAddABall;
byte RightFlipsSeen;
byte SkillshotLetter;
byte NumberOfSkillshotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte NumberOfShortPlunges;

#define BALL_1_LOCK_AVAILABLE     0x10
#define BALL_2_LOCK_AVAILABLE     0x20
#define BALL_LOCKS_AVAILABLE_MASK 0xF0
#define BALL_1_LOCKED             0x01
#define BALL_2_LOCKED             0x02
#define BALL_LOCKS_MASK           0x0F

#define JAKE_STATUS_J             0x01
#define JAKE_STATUS_A             0x02
#define JAKE_STATUS_K             0x04
#define JAKE_STATUS_E             0x08
#define JAKE_STATUS_COMPLETE      0x0F

#define ELWOOD_STATUS_E           0x01
#define ELWOOD_STATUS_L           0x02
#define ELWOOD_STATUS_W           0x04
#define ELWOOD_STATUS_O1          0x08
#define ELWOOD_STATUS_O2          0x10
#define ELWOOD_STATUS_D           0x20
#define ELWOOD_STATUS_COMPLETE    0x3F

#define ELWOOD_MINIMODE_FLAG      0x01
#define JAKE_MINIMODE_FLAG        0x02
#define BAND_MINIMODE_FLAG        0x04

#define ELWOOD_MINIMODE_COMPLETION_BONUS    100000

#define LOCK_HANDLING_LOCK_STEALING   0
#define LOCK_HANDLING_PURGE_LOCKS     1
#define LOCK_HANDLING_VIRTUAL_LOCKS   2

boolean ResetJakeProgressEachBall = true;
boolean ResetElwoodProgressEachBall = true;
boolean OneBankFirstClear = true;
boolean Multiball3StartShotHit;
boolean BallLaunched;
boolean AddABallUsed;
boolean TopGateCloseSoundPlayed;

unsigned long MiniModeTimeLeft = 0;
unsigned long MiniModeQualifiedExpiration = 0;
unsigned long LastJakeHit[4];
unsigned long PlayfieldMultiplierTimeLeft;
unsigned long BonusChanged;
unsigned long BonusXAnimationStart;
unsigned long LastSpinnerHitTime;
unsigned long LastShooterKickTime;
unsigned long LeftGateAvailableTime = 0;
unsigned long LeftGateCloseTime = 0;
unsigned long TopGateAvailableTime = 0;
unsigned long TopGateCloseTime = 0;
unsigned long LiftGateReleaseRequestTime;
unsigned long LiftGateLastReleaseTime;
unsigned long LastCaptiveBallHit;
unsigned long Multiball3StartAward;
unsigned long Multiball3Jackpot;
unsigned long LastTimeModeShotMissed = 0;
unsigned long LeftSpinnerLitTime;


#define LEFT_GATE_OPEN_TIME_MS  30000
#define LEFT_GATE_DELAY_TIME_MS 10000
#define TOP_GATE_OPEN_TIME_MS   30000
#define TOP_GATE_DELAY_TIME_MS  10000

DropTargetBank LeftDropTargets(3, 1, DROP_TARGET_TYPE_BLY_1, 50);
DropTargetBank RightDropTargets(3, 1, DROP_TARGET_TYPE_BLY_1, 50);





/******************************************************

   Adjustments Serialization

*/


void ReadStoredParameters() {
  for (byte count = 0; count < 3; count++) {
    ChuteCoinsInProgress[count] = 0;
  }

  HighScore = RPU_ReadULFromEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, 10000);
  Credits = RPU_ReadByteFromEEProm(RPU_CREDITS_EEPROM_BYTE);
  if (Credits > MaximumCredits) Credits = MaximumCredits;

  ReadSetting(EEPROM_FREE_PLAY_BYTE, 0);
  FreePlayMode = (EEPROM.read(EEPROM_FREE_PLAY_BYTE)) ? true : false;

  BallSaveNumSeconds = ReadSetting(EEPROM_BALL_SAVE_BYTE, 15);
  if (BallSaveNumSeconds > 20) BallSaveNumSeconds = 20;

  SoundSelector = ReadSetting(EEPROM_SOUND_SELECTOR_BYTE, 3);
  if (SoundSelector > 8) SoundSelector = 3;

  MusicVolume = ReadSetting(EEPROM_MUSIC_VOLUME_BYTE, 10);
  if (MusicVolume > 10) MusicVolume = 10;

  SoundEffectsVolume = ReadSetting(EEPROM_SFX_VOLUME_BYTE, 10);
  if (SoundEffectsVolume > 10) SoundEffectsVolume = 10;

  CalloutsVolume = ReadSetting(EEPROM_CALLOUTS_VOLUME_BYTE, 10);
  if (CalloutsVolume > 10) CalloutsVolume = 10;

  Audio.SetMusicVolume(MusicVolume);
  Audio.SetSoundFXVolume(SoundEffectsVolume);
  Audio.SetNotificationsVolume(CalloutsVolume);

  TournamentScoring = (ReadSetting(EEPROM_TOURNAMENT_SCORING_BYTE, 0)) ? true : false;

  MaxTiltWarnings = ReadSetting(EEPROM_TILT_WARNING_BYTE, 2);
  if (MaxTiltWarnings > 2) MaxTiltWarnings = 2;

  byte awardOverride = ReadSetting(EEPROM_AWARD_OVERRIDE_BYTE, 99);
  if (awardOverride != 99) {
    ScoreAwardReplay = awardOverride;
  }

  byte ballsOverride = ReadSetting(EEPROM_BALLS_OVERRIDE_BYTE, 99);
  if (ballsOverride == 3 || ballsOverride == 5) {
    BallsPerGame = ballsOverride;
  } else {
    if (ballsOverride != 99) EEPROM.write(EEPROM_BALLS_OVERRIDE_BYTE, 99);
  }

  ScrollingScores = (ReadSetting(EEPROM_SCROLLING_SCORES_BYTE, 1)) ? true : false;

  ExtraBallValue = RPU_ReadULFromEEProm(EEPROM_EXTRA_BALL_SCORE_UL);
  if (ExtraBallValue % 1000 || ExtraBallValue > 100000) ExtraBallValue = 20000;

  SpecialValue = RPU_ReadULFromEEProm(EEPROM_SPECIAL_SCORE_UL);
  if (SpecialValue % 1000 || SpecialValue > 100000) SpecialValue = 40000;

  TimeRequiredToResetGame = ReadSetting(EEPROM_CRB_HOLD_TIME, 1);
  if (TimeRequiredToResetGame > 3 && TimeRequiredToResetGame != 99) TimeRequiredToResetGame = 1;

  AwardScores[0] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE);
  AwardScores[1] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE);
  AwardScores[2] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE);


  // These parameters should all be adjustable
  LockHandling = LOCK_HANDLING_LOCK_STEALING;
  ElwoodClearsToQualify = 2;
  JakeClearsToQualify = 1;
  BallSaveOnMinimodes = 5;
  BallSaveOnMultiball = 5;
  RightFlipsForAddABall = 3;
  SpinnerHitsToQualifyBand = 500;
  ResetJakeProgressEachBall = true;
  ResetElwoodProgressEachBall = true;
  OneBankFirstClear = true;
  
}





void QueueDIAGNotification(unsigned short notificationNum) {
  // This is optional, but the machine can play an audio message at boot
  // time to indicate any errors and whether it's going to boot to original
  // or new code.
  //Audio.QueuePrioritizedNotification(notificationNum, 0, 10, CurrentTime);
  (void)notificationNum;
}

void setup() {

  if (DEBUG_MESSAGES) {
    // If debug is on, set up the Serial port for communication
    Serial.begin(115200);
    Serial.write("Starting\n");
  }

  // Set up the Audio handler in order to play boot messages
  CurrentTime = millis();
  Audio.InitDevices(AUDIO_PLAY_TYPE_WAV_TRIGGER | AUDIO_PLAY_TYPE_ORIGINAL_SOUNDS);
  Audio.StopAllAudio();
  Audio.SetMusicDuckingGain(25);
  Audio.SetSoundFXDuckingGain(20);

  // Tell the OS about game-specific switches
  // (this is for software-controlled pop bumpers and slings)
#if (RPU_MPU_ARCHITECTURE<10)
  // Machines with a -17, -35, 100, and 200 architecture
  // almost always have software based switch-triggered solenoids.
  // For those, you can define an array of solenoids and the switches
  // that will trigger them:
  RPU_SetupGameSwitches(NUM_SWITCHES_WITH_TRIGGERS, NUM_PRIORITY_SWITCHES_WITH_TRIGGERS, SolenoidAssociatedSwitches);
#endif

  // Set up the chips and interrupts
  unsigned long initResult = 0;
  if (DEBUG_MESSAGES) Serial.write("Initializing MPU\n");

  // If the hardware has the ability to switch on the Credit/Reset button (requires Rev 4 or greater)
  // then that can be used to choose Original or New code. Otherwise, the hardware switch
  // will choose Original if open, and New if closed
  initResult = RPU_InitializeMPU(   RPU_CMD_BOOT_ORIGINAL_IF_CREDIT_RESET | RPU_CMD_BOOT_ORIGINAL_IF_SWITCH_CLOSED |
                                    RPU_CMD_INIT_AND_RETURN_EVEN_IF_ORIGINAL_CHOSEN | RPU_CMD_PERFORM_MPU_TEST, SW_CREDIT_RESET);

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Return from init = 0x%04lX\n", initResult);
    Serial.write(buf);
    if (initResult & RPU_RET_6800_DETECTED) Serial.write("Detected 6800 clock\n");
    else if (initResult & RPU_RET_6802_OR_8_DETECTED) Serial.write("Detected 6802/8 clock\n");
    Serial.write("Back from init\n");
  }

  if (initResult & RPU_RET_SELECTOR_SWITCH_ON) QueueDIAGNotification(SOUND_EFFECT_DIAG_SELECTOR_SWITCH_ON);
  else QueueDIAGNotification(SOUND_EFFECT_DIAG_SELECTOR_SWITCH_OFF);

  if (initResult & RPU_RET_CREDIT_RESET_BUTTON_HIT) QueueDIAGNotification(SOUND_EFFECT_DIAG_CREDIT_RESET_BUTTON);

  if (initResult & RPU_RET_DIAGNOSTIC_REQUESTED) {
    QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_DIAGNOSTICS);
    // Run diagnostics here:
  }

  if (initResult & RPU_RET_ORIGINAL_CODE_REQUESTED) {
    if (DEBUG_MESSAGES) Serial.write("Asked to run original code\n");
    delay(100);
    QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_ORIGINAL_CODE);
    delay(100);
    while (Audio.Update(millis()));
    // Arduino should hang if original code is running
    while (1);
  }
  QueueDIAGNotification(SOUND_EFFECT_DIAG_STARTING_NEW_CODE);

  RPU_DisableSolenoidStack();
  RPU_SetDisableFlippers(true);

  // Read parameters from EEProm
  ReadStoredParameters();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  CurrentScores[0] = GAME_MAJOR_VERSION;
  CurrentScores[1] = GAME_MINOR_VERSION;
  CurrentScores[2] = RPU_OS_MAJOR_VERSION;
  CurrentScores[3] = RPU_OS_MINOR_VERSION;

  CurrentAchievements[0] = 0;
  CurrentAchievements[1] = 0;
  CurrentAchievements[2] = 0;
  CurrentAchievements[3] = 0;

  // Initialize any drop target variables here
  LeftDropTargets.DefineSwitch(0, SW_DROP_L_1);
  LeftDropTargets.DefineSwitch(1, SW_DROP_L_2);
  LeftDropTargets.DefineSwitch(2, SW_DROP_L_3);
  LeftDropTargets.DefineResetSolenoid(0, SOL_DROP_BANK_L_RESET);
  RightDropTargets.DefineSwitch(0, SW_DROP_R_1);
  RightDropTargets.DefineSwitch(1, SW_DROP_R_2);
  RightDropTargets.DefineSwitch(2, SW_DROP_R_3);
  RightDropTargets.DefineResetSolenoid(0, SOL_DROP_BANK_R_RESET);

  RPU_SetSwitchInversion(SW_DROP_L_1);
  RPU_SetSwitchInversion(SW_DROP_L_2);
  RPU_SetSwitchInversion(SW_DROP_L_3);
  RPU_SetSwitchInversion(SW_DROP_R_1);
  RPU_SetSwitchInversion(SW_DROP_R_2);
  RPU_SetSwitchInversion(SW_DROP_R_3);

  Audio.QueueSound(SOUND_EFFECT_STARTUP_1 + CurrentTime%2, AUDIO_PLAY_TYPE_WAV_TRIGGER, CurrentTime + 1200);
  MachineLocks = 0;
  RPU_PushToTimedSolenoidStack(SOL_LIFT_GATE, 5, CurrentTime + 3000, true);
}

byte ReadSetting(byte setting, byte defaultValue) {
  byte value = EEPROM.read(setting);
  if (value == 0xFF) {
    EEPROM.write(setting, defaultValue);
    return defaultValue;
  }
  return value;
}

// This function is useful for checking the status of drop target switches
byte CheckSequentialSwitches(byte startingSwitch, byte numSwitches) {
  byte returnSwitches = 0;
  for (byte count = 0; count < numSwitches; count++) {
    returnSwitches |= (RPU_ReadSingleSwitchState(startingSwitch + count) << count);
  }
  return returnSwitches;
}


////////////////////////////////////////////////////////////////////////////
//
//  Lamp Management functions
//
////////////////////////////////////////////////////////////////////////////
void SetGeneralIlluminationOn(boolean setGIOn = true) {
  // some machines have solenoids attached to relays
  // to control the GI
  //  RPU_SetContinuousSolenoidBit(!generalIlluminationOn, SOL_GI_RELAY);

  // Since this machine doesn't have GI control,
  // this line prevents compiler warnings.
  (void)setGIOn;
}

byte BonusLampAssignments[6] = {LAMP_BONUS_1, LAMP_BONUS_2, LAMP_BONUS_3, LAMP_BONUS_4, LAMP_BONUS_5, LAMP_BONUS_6};

void ShowBonusLamps() {
  boolean lampShown[6] = {false};
  if (GameMode == GAME_MODE_SKILL_SHOT) {
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
  } else if (GameMode==GAME_MODE_MULTIBALL_3_START) {
    byte lampPhase = (CurrentTime/150)%8;
    RPU_SetLampState(LAMP_BONUS_1, lampPhase==0);
    RPU_SetLampState(LAMP_BONUS_2, lampPhase==1);
    RPU_SetLampState(LAMP_BONUS_3, lampPhase==2);
    RPU_SetLampState(LAMP_BONUS_4, lampPhase==3);
    RPU_SetLampState(LAMP_BONUS_5, lampPhase==4);
    RPU_SetLampState(LAMP_BONUS_6, lampPhase==5);
    RPU_SetLampState(LAMP_SPECIAL, lampPhase==6);
  } else {
    RPU_SetLampState(LAMP_SPECIAL, 0);
    byte bonusToShow = Bonus[CurrentPlayer];

    if (bonusToShow > 6) {
      RPU_SetLampState(LAMP_BONUS_6, 1, 0, 150);
      lampShown[5] = true;
      bonusToShow -= 6;
      if (bonusToShow > 5) {
        RPU_SetLampState(LAMP_BONUS_5, 1, 0, 150);
        lampShown[4] = true;
        bonusToShow -= 5;
        if (bonusToShow > 4) {
          RPU_SetLampState(LAMP_BONUS_4, 1, 0, 150);
          lampShown[3] = true;
          bonusToShow -= 4;
          if (bonusToShow > 3) {
            RPU_SetLampState(LAMP_BONUS_3, 1, 0, 150);
            lampShown[2] = true;
            bonusToShow -= 3;
            if (bonusToShow > 2) {
              RPU_SetLampState(LAMP_BONUS_2, 1, 1, 150);
              lampShown[1] = true;
              bonusToShow -= 2;
            }
          }
        }
      }
    }

    if (bonusToShow) {
      for (byte count = 1; count < 7; count++) {
        if (bonusToShow >= count) {
          RPU_SetLampState(BonusLampAssignments[count - 1], 1);
          lampShown[count - 1] = true;
        }
      }
    }

    for (byte count = 0; count < 6; count++) {
      if (!lampShown[count]) RPU_SetLampState(BonusLampAssignments[count], 0);
    }
    
  }

}


void ShowPopBumperAndSlingLamps() {
  RPU_SetLampState(LAMP_POP_WHITE_DOWN, 1);
  if (GameMode==GAME_MODE_MINIMODE) {
    byte lampPhase = (CurrentTime/375)%3;
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, lampPhase==0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, lampPhase==1);
    RPU_SetLampState(LAMP_POP_BLUE_UP, lampPhase==2);

    lampPhase = (CurrentTime/187)%8;
    RPU_SetLampState(LAMP_LEFT_SLING, lampPhase<2 || lampPhase==4 || lampPhase==6);
    RPU_SetLampState(LAMP_RIGHT_SLING, lampPhase==2 || lampPhase==3 || lampPhase==5 || lampPhase==7);
  } else {
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, 1);
    if (LastTimePopHit) {
      if (CurrentTime > (LastTimePopHit + 1500)) {
        LastTimePopHit = 0;
      }
      RPU_SetLampState(LAMP_POP_SMALL_BLUE, 1, 0, 100);
    } else {
      RPU_SetLampState(LAMP_POP_SMALL_BLUE, 0);
    }
    RPU_SetLampState(LAMP_POP_BLUE_UP, 0);
    RPU_SetLampState(LAMP_LEFT_SLING, 1);
    RPU_SetLampState(LAMP_RIGHT_SLING, 1);
  }

}

void ShowStandupTargetLamps() {

  if (GameMode == GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer]&JAKE_MINIMODE_FLAG)) {
    for (byte count = 0; count < 4; count++) {
      RPU_SetLampState(JakeLampAssignments[count], count==JakeTargetLit, 0, 25);
    }
  } else {
    byte jakeFlag = 0x01;
    for (byte count = 0; count < 4; count++) {
      RPU_SetLampState(JakeLampAssignments[count], (JakeStatus[CurrentPlayer] & jakeFlag) ? true : false);
      jakeFlag *= 2;
    }
  }
}

void ShowLoopSpinnerAndLockLamps() {
  if (GameMode==GAME_MODE_MULTIBALL_3_START) {
    byte lampPhase = (CurrentTime/150)%8;
    RPU_SetLampState(LAMP_LEFT_SPINNER, lampPhase==7, 0, 25);
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);  
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 1, 0, 25);
    RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 1, 0, 25);
  } else {
    if (GameMode==GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer]&BAND_MINIMODE_FLAG)) {
      RPU_SetLampState(LAMP_LEFT_SPINNER, 1, 0, 25);
    } else {
      RPU_SetLampState(LAMP_LEFT_SPINNER, LeftSpinnerLitTime ? true : false, 0, 150);
    }
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
  
    if (TopGateAvailableTime && CurrentTime>TopGateAvailableTime && LeftDropTargets.GetStatus()&0x02) {
      RPU_SetLampState(LAMP_CAPTIVE_BALL, 1, 0, 600);
      RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 1, 0, 75);
    } else {
      RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
      RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 0);
    }
  }
}

void ShowDropTargetLamps() {
  if (GameMode == GAME_MODE_SKILL_SHOT) {
    byte elwoodFlag = 0x01;
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], count==SkillshotLetter, 0, 75);
      elwoodFlag *= 2;
    }
  } else if (GameMode == GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer]&ELWOOD_MINIMODE_FLAG)) {
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], count==ElwoodTargetLit, 0, 25);
    }
  } else {
    byte elwoodFlag = 0x01;
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], (ElwoodStatus[CurrentPlayer] & elwoodFlag) ? true : false);
      elwoodFlag *= 2;
    }
  }
}

void ShowLaneLamps() {
  if (LeftGateAvailableTime && CurrentTime>LeftGateAvailableTime) {
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 1, 0, 800);
  } else {
    RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
  }
  RPU_SetLampState(LAMP_LEFT_INLANE, 0);

  if (LeftGateCloseTime) {
    unsigned short flashRate = 0;
    if ((CurrentTime+3000) > LeftGateCloseTime) {
      flashRate = 100;
    }
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 1, 0, flashRate);
  } else {
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);
  }
  RPU_SetLampState(LAMP_RIGHT_INLANE, 0);
  RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, 0);
}


void ShowShootAgainLamp() {

  if ( (BallFirstSwitchHitTime == 0 && BallSaveNumSeconds) || (BallSaveEndTime && CurrentTime < BallSaveEndTime) ) {
    unsigned long msRemaining = 5000;
    if (BallSaveEndTime != 0) msRemaining = BallSaveEndTime - CurrentTime;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, (msRemaining < 5000) ? 100 : 500);
  } else {
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
  }
}




////////////////////////////////////////////////////////////////////////////
//
//  Machine State Helper functions
//
////////////////////////////////////////////////////////////////////////////
boolean AddPlayer(boolean resetNumPlayers = false) {

  if (Credits < 1 && !FreePlayMode) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers >= RPU_NUMBER_OF_PLAYERS_ALLOWED) return false;

  CurrentNumPlayers += 1;
  RPU_SetDisplay(CurrentNumPlayers - 1, 0, true, 2);
  //  RPU_SetDisplayBlank(CurrentNumPlayers - 1, 0x30);

  //  RPU_SetLampState(LAMP_HEAD_1_PLAYER, CurrentNumPlayers==1, 0, 500);
  //  RPU_SetLampState(LAMP_HEAD_2_PLAYERS, CurrentNumPlayers==2, 0, 500);
  //  RPU_SetLampState(LAMP_HEAD_3_PLAYERS, CurrentNumPlayers==3, 0, 500);
  //  RPU_SetLampState(LAMP_HEAD_4_PLAYERS, CurrentNumPlayers==4, 0, 500);

  if (!FreePlayMode) {
    Credits -= 1;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(false);
  }
  if (CurrentNumPlayers == 1) Audio.StopAllAudio();
  QueueNotification(SOUND_EFFECT_VP_ADD_PLAYER_1 + (CurrentNumPlayers - 1), 10);

  RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}


unsigned short ChuteAuditByte[] = {RPU_CHUTE_1_COINS_START_BYTE, RPU_CHUTE_2_COINS_START_BYTE, RPU_CHUTE_3_COINS_START_BYTE};
void AddCoinToAudit(byte chuteNum) {
  if (chuteNum > 2) return;
  unsigned short coinAuditStartByte = ChuteAuditByte[chuteNum];
  RPU_WriteULToEEProm(coinAuditStartByte, RPU_ReadULFromEEProm(coinAuditStartByte) + 1);
}


void AddCredit(boolean playSound = false, byte numToAdd = 1) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    if (playSound) {
      //PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
      RPU_PushToSolenoidStack(SOL_KNOCKER, 20, true);
    }
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(false);
  } else {
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    RPU_SetCoinLockout(true);
  }

}

byte SwitchToChuteNum(byte switchHit) {
  (void)switchHit;
  byte chuteNum = 0;
  return chuteNum;
}

boolean AddCoin(byte chuteNum) {
  boolean creditAdded = false;
  if (chuteNum > 2) return false;
  byte cpcSelection = GetCPCSelection(chuteNum);

  // Find the lowest chute num with the same ratio selection
  // and use that ChuteCoinsInProgress counter
  byte chuteNumToUse;
  for (chuteNumToUse = 0; chuteNumToUse <= chuteNum; chuteNumToUse++) {
    if (GetCPCSelection(chuteNumToUse) == cpcSelection) break;
  }

  PlaySoundEffect(SOUND_EFFECT_COIN_DROP_1 + (CurrentTime % 3));

  byte cpcCoins = GetCPCCoins(cpcSelection);
  byte cpcCredits = GetCPCCredits(cpcSelection);
  byte coinProgressBefore = ChuteCoinsInProgress[chuteNumToUse];
  ChuteCoinsInProgress[chuteNumToUse] += 1;

  if (ChuteCoinsInProgress[chuteNumToUse] == cpcCoins) {
    if (cpcCredits > cpcCoins) AddCredit(cpcCredits - (coinProgressBefore));
    else AddCredit(cpcCredits);
    ChuteCoinsInProgress[chuteNumToUse] = 0;
    creditAdded = true;
  } else {
    if (cpcCredits > cpcCoins) {
      AddCredit(1);
      creditAdded = true;
    } else {
    }
  }

  return creditAdded;
}


void AddSpecialCredit() {
  AddCredit(false, 1);
  RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime, true);
  RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 1);
}

void AwardSpecial() {
  if (SpecialCollected) return;
  SpecialCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += SpecialValue * PlayfieldMultiplier;
  } else {
    AddSpecialCredit();
  }
}

boolean AwardExtraBall() {
  if (ExtraBallCollected) return false;
  ExtraBallCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += ExtraBallValue * PlayfieldMultiplier;
  } else {
    SamePlayerShootsAgain = true;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
    QueueNotification(SOUND_EFFECT_VP_EXTRA_BALL, 8);
  }
  return true;
}


void IncreasePlayfieldMultiplier(unsigned long duration) {
  PlayfieldMultiplierTimeLeft += duration;

  PlayfieldMultiplier += 1;
  if (PlayfieldMultiplier > 5) {
    PlayfieldMultiplier = 5;
  }
}



#define ADJ_TYPE_LIST                 1
#define ADJ_TYPE_MIN_MAX              2
#define ADJ_TYPE_MIN_MAX_DEFAULT      3
#define ADJ_TYPE_SCORE                4
#define ADJ_TYPE_SCORE_WITH_DEFAULT   5
#define ADJ_TYPE_SCORE_NO_DEFAULT     6
byte AdjustmentType = 0;
byte NumAdjustmentValues = 0;
byte AdjustmentValues[8];
byte CurrentAdjustmentStorageByte = 0;
byte TempValue = 0;
byte *CurrentAdjustmentByte = NULL;
unsigned long *CurrentAdjustmentUL = NULL;
unsigned long SoundSettingTimeout = 0;
unsigned long AdjustmentScore;


int RunSelfTest(int curState, boolean curStateChanged) {
  int returnState = curState;
  CurrentNumPlayers = 0;

  if (curStateChanged) {
    // Send a stop-all command and reset the sample-rate offset, in case we have
    //  reset while the WAV Trigger was already playing.
    Audio.StopAllAudio();
    RPU_TurnOffAllLamps();
    Audio.StopAllAudio();
    int modeMapping = SelfTestStateToCalloutMap[-1 - curState];
    Audio.PlaySound((unsigned short)modeMapping, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
  } else {
    if (SoundSettingTimeout && CurrentTime > SoundSettingTimeout) {
      SoundSettingTimeout = 0;
      Audio.StopAllAudio();
    }
  }

  // Any state that's greater than MACHINE_STATE_TEST_DONE is handled by the Base Self-test code
  // Any that's less, is machine specific, so we handle it here.
  if (curState >= MACHINE_STATE_TEST_DONE) {
    byte cpcSelection = 0xFF;
    byte chuteNum = 0xFF;
    if (curState == MACHINE_STATE_ADJUST_CPC_CHUTE_1) chuteNum = 0;
    if (curState == MACHINE_STATE_ADJUST_CPC_CHUTE_2) chuteNum = 1;
    if (curState == MACHINE_STATE_ADJUST_CPC_CHUTE_3) chuteNum = 2;
    if (chuteNum != 0xFF) cpcSelection = GetCPCSelection(chuteNum);
    returnState = RunBaseSelfTest(returnState, curStateChanged, CurrentTime, SW_CREDIT_RESET, SW_TILT, SW_SELF_TEST_ON_MATRIX);
    if (chuteNum != 0xFF) {
      if (cpcSelection != GetCPCSelection(chuteNum)) {
        byte newCPC = GetCPCSelection(chuteNum);
        Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_SELF_TEST_CPC_START + newCPC, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
      }
    }
  } else {
    byte curSwitch = RPU_PullFirstFromSwitchStack();

    if ((curSwitch == SW_SELF_TEST_SWITCH || curSwitch == SW_SELF_TEST_ON_MATRIX) && (CurrentTime - GetLastSelfTestChangedTime()) > 50) {
      SetLastSelfTestChangedTime(CurrentTime);
      if (RPU_GetUpDownSwitchState()) returnState -= 1;
      else returnState += 1;
    }

    //    if (curSwitch == SW_SLAM) {
    //      returnState = MACHINE_STATE_ATTRACT;
    //    }

    if (curStateChanged) {
      for (int count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
        RPU_SetDisplay(count, 0);
        RPU_SetDisplayBlank(count, 0x00);
      }
      RPU_SetDisplayCredits(0, false);
#if (RPU_MPU_ARCHITECTURE<10)
      RPU_SetDisplayBallInPlay(MACHINE_STATE_TEST_SOUNDS - curState);
#else
      RPU_SetDisplayBallInPlay(MACHINE_STATE_TEST_BOOT - curState);
#endif
      CurrentAdjustmentByte = NULL;
      CurrentAdjustmentUL = NULL;
      CurrentAdjustmentStorageByte = 0;

      AdjustmentType = ADJ_TYPE_MIN_MAX;
      AdjustmentValues[0] = 0;
      AdjustmentValues[1] = 1;
      TempValue = 0;

      switch (curState) {
        case MACHINE_STATE_ADJUST_FREEPLAY:
          CurrentAdjustmentByte = (byte *)&FreePlayMode;
          CurrentAdjustmentStorageByte = EEPROM_FREE_PLAY_BYTE;
          break;
        case MACHINE_STATE_ADJUST_BALL_SAVE:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 5;
          AdjustmentValues[1] = 5;
          AdjustmentValues[2] = 10;
          AdjustmentValues[3] = 15;
          AdjustmentValues[4] = 20;
          CurrentAdjustmentByte = &BallSaveNumSeconds;
          CurrentAdjustmentStorageByte = EEPROM_BALL_SAVE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SOUND_SELECTOR:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[1] = 9;
          CurrentAdjustmentByte = &SoundSelector;
          CurrentAdjustmentStorageByte = EEPROM_SOUND_SELECTOR_BYTE;
          break;
        case MACHINE_STATE_ADJUST_MUSIC_VOLUME:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 10;
          CurrentAdjustmentByte = &MusicVolume;
          CurrentAdjustmentStorageByte = EEPROM_MUSIC_VOLUME_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SFX_VOLUME:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 10;
          CurrentAdjustmentByte = &SoundEffectsVolume;
          CurrentAdjustmentStorageByte = EEPROM_SFX_VOLUME_BYTE;
          break;
        case MACHINE_STATE_ADJUST_CALLOUTS_VOLUME:
          AdjustmentType = ADJ_TYPE_MIN_MAX;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 10;
          CurrentAdjustmentByte = &CalloutsVolume;
          CurrentAdjustmentStorageByte = EEPROM_CALLOUTS_VOLUME_BYTE;
          break;
        case MACHINE_STATE_ADJUST_TOURNAMENT_SCORING:
          CurrentAdjustmentByte = (byte *)&TournamentScoring;
          CurrentAdjustmentStorageByte = EEPROM_TOURNAMENT_SCORING_BYTE;
          break;
        case MACHINE_STATE_ADJUST_TILT_WARNING:
          AdjustmentValues[1] = 2;
          CurrentAdjustmentByte = &MaxTiltWarnings;
          CurrentAdjustmentStorageByte = EEPROM_TILT_WARNING_BYTE;
          break;
        case MACHINE_STATE_ADJUST_AWARD_OVERRIDE:
          AdjustmentType = ADJ_TYPE_MIN_MAX_DEFAULT;
          AdjustmentValues[1] = 7;
          CurrentAdjustmentByte = &ScoreAwardReplay;
          CurrentAdjustmentStorageByte = EEPROM_AWARD_OVERRIDE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_BALLS_OVERRIDE:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 3;
          AdjustmentValues[0] = 3;
          AdjustmentValues[1] = 5;
          AdjustmentValues[2] = 99;
          CurrentAdjustmentByte = &BallsPerGame;
          CurrentAdjustmentStorageByte = EEPROM_BALLS_OVERRIDE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SCROLLING_SCORES:
          CurrentAdjustmentByte = (byte *)&ScrollingScores;
          CurrentAdjustmentStorageByte = EEPROM_SCROLLING_SCORES_BYTE;
          break;
        case MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD:
          AdjustmentType = ADJ_TYPE_SCORE_WITH_DEFAULT;
          CurrentAdjustmentUL = &ExtraBallValue;
          CurrentAdjustmentStorageByte = EEPROM_EXTRA_BALL_SCORE_UL;
          break;
        case MACHINE_STATE_ADJUST_SPECIAL_AWARD:
          AdjustmentType = ADJ_TYPE_SCORE_WITH_DEFAULT;
          CurrentAdjustmentUL = &SpecialValue;
          CurrentAdjustmentStorageByte = EEPROM_SPECIAL_SCORE_UL;
          break;
        case MACHINE_STATE_ADJUST_CREDIT_RESET_HOLD_TIME:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 5;
          AdjustmentValues[0] = 0;
          AdjustmentValues[1] = 1;
          AdjustmentValues[2] = 2;
          AdjustmentValues[3] = 3;
          AdjustmentValues[4] = 99;
          CurrentAdjustmentByte = &TimeRequiredToResetGame;
          CurrentAdjustmentStorageByte = EEPROM_CRB_HOLD_TIME;
          break;
        case MACHINE_STATE_ADJUST_DONE:
          returnState = MACHINE_STATE_ATTRACT;
          break;
      }
    }

    // Change value, if the switch is hit
    if (curSwitch == SW_CREDIT_RESET) {

      if (CurrentAdjustmentByte && (AdjustmentType == ADJ_TYPE_MIN_MAX || AdjustmentType == ADJ_TYPE_MIN_MAX_DEFAULT)) {
        byte curVal = *CurrentAdjustmentByte;

        if (RPU_GetUpDownSwitchState()) {
          curVal += 1;
          if (curVal > AdjustmentValues[1]) {
            if (AdjustmentType == ADJ_TYPE_MIN_MAX) curVal = AdjustmentValues[0];
            else {
              if (curVal > 99) curVal = AdjustmentValues[0];
              else curVal = 99;
            }
          }
        } else {
          if (curVal == AdjustmentValues[0]) {
            if (AdjustmentType == ADJ_TYPE_MIN_MAX_DEFAULT) curVal = 99;
            else curVal = AdjustmentValues[1];
          } else {
            curVal -= 1;
          }
        }

        *CurrentAdjustmentByte = curVal;
        if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, curVal);

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

      } else if (CurrentAdjustmentByte && AdjustmentType == ADJ_TYPE_LIST) {
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
        if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, AdjustmentValues[newIndex]);
      } else if (CurrentAdjustmentUL && (AdjustmentType == ADJ_TYPE_SCORE_WITH_DEFAULT || AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT)) {
        unsigned long curVal = *CurrentAdjustmentUL;

        if (RPU_GetUpDownSwitchState()) curVal += 5000;
        else if (curVal >= 5000) curVal -= 5000;
        if (curVal > 100000) curVal = 0;
        if (AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT && curVal == 0) curVal = 5000;
        *CurrentAdjustmentUL = curVal;
        if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, curVal);
      }

    }

    // Show current value
    if (CurrentAdjustmentByte != NULL) {
      RPU_SetDisplay(0, (unsigned long)(*CurrentAdjustmentByte), true);
    } else if (CurrentAdjustmentUL != NULL) {
      RPU_SetDisplay(0, (*CurrentAdjustmentUL), true);
    }

  }

  if (returnState == MACHINE_STATE_ATTRACT) {
    // If any variables have been set to non-override (99), return
    // them to dip switch settings
    // Balls Per Game, Player Loses On Ties, Novelty Scoring, Award Score
    //    DecodeDIPSwitchParameters();
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    ReadStoredParameters();
  }

  return returnState;
}




////////////////////////////////////////////////////////////////////////////
//
//  Audio Output functions
//
////////////////////////////////////////////////////////////////////////////
void PlayBackgroundSong(unsigned int songNum) {

  if (MusicVolume == 0) return;

  Audio.PlayBackgroundSong(songNum);
}


unsigned long NextSoundEffectTime = 0;

void PlaySoundEffect(unsigned int soundEffectNum) {

  if (MachineState == MACHINE_STATE_INIT_GAMEPLAY) return;

  // Play digital samples on the WAV trigger (numbered same
  // as SOUND_EFFECT_ defines)
  Audio.PlaySound(soundEffectNum, AUDIO_PLAY_TYPE_WAV_TRIGGER);

  // SOUND_EFFECT_ defines can also be translated into
  // commands for the sound card
  switch (soundEffectNum) {
      /*
          case SOUND_EFFECT_LEFT_SHOOTER_LANE:
            Audio.PlaySoundCardWhenPossible(12, CurrentTime, 0, 500, 7);
            break;
          case SOUND_EFFECT_RETURN_TO_SHOOTER_LANE:
            Audio.PlaySoundCardWhenPossible(22, CurrentTime, 0, 500, 8);
            break;
          case SOUND_EFFECT_SAUCER:
            Audio.PlaySoundCardWhenPossible(14, CurrentTime, 0, 500, 7);
            break;
          case SOUND_EFFECT_DROP_TARGET_HURRY:
            Audio.PlaySoundCardWhenPossible(2, CurrentTime, 0, 45, 3);
            break;
          case SOUND_EFFECT_DROP_TARGET_COMPLETE:
            Audio.PlaySoundCardWhenPossible(9, CurrentTime, 0, 1400, 4);
            Audio.PlaySoundCardWhenPossible(19, CurrentTime, 1500, 10, 4);
            break;
          case SOUND_EFFECT_HOOFBEATS:
            Audio.PlaySoundCardWhenPossible(12, CurrentTime, 0, 100, 10);
            break;
          case SOUND_EFFECT_STOP_BACKGROUND:
            Audio.PlaySoundCardWhenPossible(19, CurrentTime, 0, 10, 10);
            break;
          case SOUND_EFFECT_DROP_TARGET_HIT:
            Audio.PlaySoundCardWhenPossible(7, CurrentTime, 0, 150, 5);
            break;
          case SOUND_EFFECT_SPINNER:
            Audio.PlaySoundCardWhenPossible(6, CurrentTime, 0, 25, 2);
            break;
      */
  }
}


void QueueNotification(unsigned int soundEffectNum, byte priority) {
  if (CalloutsVolume == 0) return;

  // With RPU_OS_HARDWARE_REV 4 and above, the WAV trigger has two-way communication,
  // so it's not necesary to tell it the length of a notification. For support for
  // earlier hardware, you'll need an array of VoicePromptLengths for each prompt
  // played (for queueing and ducking)
  //  Audio.QueuePrioritizedNotification(soundEffectNum, VoicePromptLengths[soundEffectNum-SOUND_EFFECT_VP_VOICE_NOTIFICATIONS_START], priority, CurrentTime);
  Audio.QueuePrioritizedNotification(soundEffectNum, 0, priority, CurrentTime);

}


void AlertPlayerUp() {
  QueueNotification(SOUND_EFFECT_VP_PLAYER_1_UP + CurrentPlayer, 1);
}





////////////////////////////////////////////////////////////////////////////
//
//  Diagnostics Mode
//
////////////////////////////////////////////////////////////////////////////

int RunDiagnosticsMode(int curState, boolean curStateChanged) {

  int returnState = curState;

  if (curStateChanged) {

    /*
        char buf[256];
        boolean errorSeen;

        Serial.write("Testing Volatile RAM at IC13 (0x0000 - 0x0080): writing & reading... ");
        Serial.write("3 ");
        delay(500);
        Serial.write("2 ");
        delay(500);
        Serial.write("1 \n");
        delay(500);
        errorSeen = false;
        for (byte valueCount=0; valueCount<0xFF; valueCount++) {
          for (unsigned short address=0x0000; address<0x0080; address++) {
            RPU_DataWrite(address, valueCount);
          }
          for (unsigned short address=0x0000; address<0x0080; address++) {
            byte readValue = RPU_DataRead(address);
            if (readValue!=valueCount) {
              sprintf(buf, "Write/Read failure at address=0x%04X (expected 0x%02X, read 0x%02X)\n", address, valueCount, readValue);
              Serial.write(buf);
              errorSeen = true;
            }
            if (errorSeen) break;
          }
          if (errorSeen) break;
        }
        if (errorSeen) {
          Serial.write("!!! Error in Volatile RAM\n");
        }

        Serial.write("Testing Volatile RAM at IC16 (0x0080 - 0x0100): writing & reading... ");
        Serial.write("3 ");
        delay(500);
        Serial.write("2 ");
        delay(500);
        Serial.write("1 \n");
        delay(500);
        errorSeen = false;
        for (byte valueCount=0; valueCount<0xFF; valueCount++) {
          for (unsigned short address=0x0080; address<0x0100; address++) {
            RPU_DataWrite(address, valueCount);
          }
          for (unsigned short address=0x0080; address<0x0100; address++) {
            byte readValue = RPU_DataRead(address);
            if (readValue!=valueCount) {
              sprintf(buf, "Write/Read failure at address=0x%04X (expected 0x%02X, read 0x%02X)\n", address, valueCount, readValue);
              Serial.write(buf);
              errorSeen = true;
            }
            if (errorSeen) break;
          }
          if (errorSeen) break;
        }
        if (errorSeen) {
          Serial.write("!!! Error in Volatile RAM\n");
        }

        // Check the CMOS RAM to see if it's operating correctly
        errorSeen = false;
        Serial.write("Testing CMOS RAM: writing & reading... ");
        Serial.write("3 ");
        delay(500);
        Serial.write("2 ");
        delay(500);
        Serial.write("1 \n");
        delay(500);
        for (byte valueCount=0; valueCount<0x10; valueCount++) {
          for (unsigned short address=0x0100; address<0x0200; address++) {
            RPU_DataWrite(address, valueCount);
          }
          for (unsigned short address=0x0100; address<0x0200; address++) {
            byte readValue = RPU_DataRead(address);
            if ((readValue&0x0F)!=valueCount) {
              sprintf(buf, "Write/Read failure at address=0x%04X (expected 0x%02X, read 0x%02X)\n", address, valueCount, (readValue&0x0F));
              Serial.write(buf);
              errorSeen = true;
            }
            if (errorSeen) break;
          }
          if (errorSeen) break;
        }

        if (errorSeen) {
          Serial.write("!!! Error in CMOS RAM\n");
        }


        // Check the ROMs
        Serial.write("CMOS RAM dump... ");
        Serial.write("3 ");
        delay(500);
        Serial.write("2 ");
        delay(500);
        Serial.write("1 \n");
        delay(500);
        for (unsigned short address=0x0100; address<0x0200; address++) {
          if ((address&0x000F)==0x0000) {
            sprintf(buf, "0x%04X:  ", address);
            Serial.write(buf);
          }
      //      RPU_DataWrite(address, address&0xFF);
          sprintf(buf, "0x%02X ", RPU_DataRead(address));
          Serial.write(buf);
          if ((address&0x000F)==0x000F) {
            Serial.write("\n");
          }
        }

    */

    //    RPU_EnableSolenoidStack();
    //    RPU_SetDisableFlippers(false);

  }

  return returnState;
}



////////////////////////////////////////////////////////////////////////////
//
//  Attract Mode
//
////////////////////////////////////////////////////////////////////////////

unsigned long AttractLastLadderTime = 0;
byte AttractLastLadderBonus = 0;
unsigned long AttractDisplayRampStart = 0;
byte AttractLastHeadMode = 255;
byte AttractLastPlayfieldMode = 255;
byte InAttractMode = false;

byte LastSolPhase = 0;

int RunAttractMode(int curState, boolean curStateChanged) {

  int returnState = curState;

  if (curStateChanged) {
    // Some sound cards have a special index
    // for a "sound" that will turn off
    // the current background drone or currently
    // playing sound
    RPU_DisableSolenoidStack();
    RPU_TurnOffAllLamps();
    RPU_SetDisableFlippers(true);
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract Mode\n\r");
    }
    AttractLastHeadMode = 0;
    AttractLastPlayfieldMode = 0;
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
  }

  RPU_SetLampState(LAMP_START_BUTTON, (FreePlayMode || Credits) ? true : false, 0, 200);

  // Alternate displays between high score and blank
  if (CurrentTime < 16000) {
    if (AttractLastHeadMode != 1) {
      ShowPlayerScores(0xFF, false, false);
      RPU_SetDisplayCredits(Credits, !FreePlayMode);
      RPU_SetDisplayBallInPlay(0, true);
    }
  } else if ((CurrentTime / 8000) % 2 == 0) {

    if (AttractLastHeadMode != 2) {
      SetLastTimeScoreChanged(CurrentTime);
    }
    AttractLastHeadMode = 2;
    ShowPlayerScores(0xFF, false, false, HighScore);
  } else {
    if (AttractLastHeadMode != 3) {
      if (CurrentTime < 32000) {
        for (int count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          CurrentScores[count] = 0;
        }
        CurrentNumPlayers = 0;
      }
      SetLastTimeScoreChanged(CurrentTime);
    }
    ShowPlayerScores(0xFF, false, false);

    AttractLastHeadMode = 3;
  }

  byte attractPlayfieldPhase = ((CurrentTime / 5000) % 6);

  if (attractPlayfieldPhase != AttractLastPlayfieldMode) {
    RPU_TurnOffAllLamps();
    AttractLastPlayfieldMode = attractPlayfieldPhase;
    if (attractPlayfieldPhase == 2) GameMode = GAME_MODE_SKILL_SHOT;
    else GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
    AttractLastLadderBonus = 1;
    AttractLastLadderTime = CurrentTime;
  }

  ShowLampAnimation(attractPlayfieldPhase % 3, 20, CurrentTime, 18, false, false);

  byte switchHit;
  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    if (switchHit == SW_CREDIT_RESET) {
      if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
    }
    if (switchHit == SW_COIN_1) {
      AddCoinToAudit(SwitchToChuteNum(switchHit));
      AddCoin(SwitchToChuteNum(switchHit));
    }
    if ((switchHit == SW_SELF_TEST_SWITCH || switchHit == SW_SELF_TEST_ON_MATRIX) && (CurrentTime - GetLastSelfTestChangedTime()) > 50) {
      returnState = MACHINE_STATE_TEST_LAMPS;
      SetLastSelfTestChangedTime(CurrentTime);
    }
  }

  return returnState;
}





////////////////////////////////////////////////////////////////////////////
//
//  Game Play functions
//
////////////////////////////////////////////////////////////////////////////
byte CountBits(unsigned short intToBeCounted) {
  byte numBits = 0;

  for (byte count = 0; count < 16; count++) {
    numBits += (intToBeCounted & 0x01);
    intToBeCounted = intToBeCounted >> 1;
  }

  return numBits;
}


void SetGameMode(byte newGameMode) {
  LastGameMode = GameMode;
  GameMode = newGameMode;
  GameModeStartTime = 0;
  GameModeEndTime = 0;
}

byte CountBallsInTrough() {

  byte numBalls = RPU_ReadSingleSwitchState(SW_TROUGH_1) + RPU_ReadSingleSwitchState(SW_TROUGH_2) + RPU_ReadSingleSwitchState(SW_TROUGH_3);

  return numBalls;
}



void AddToBonus(byte bonus) {
  Bonus[CurrentPlayer] += bonus;
  if (Bonus[CurrentPlayer] > MAX_DISPLAY_BONUS) {
    Bonus[CurrentPlayer] = MAX_DISPLAY_BONUS;
  } else {
    BonusChanged = CurrentTime;
  }
}



void IncreaseBonusX() {
  if (BonusX[CurrentPlayer] < 10) {
    if (BonusX[CurrentPlayer] == 1) {
      BonusX[CurrentPlayer] = 2;
      QueueNotification(SOUND_EFFECT_VP_BONUS_2X, 5);
    } else if (BonusX[CurrentPlayer] == 2) {
      BonusX[CurrentPlayer] = 3;
      QueueNotification(SOUND_EFFECT_VP_BONUS_3X, 5);
    } else if (BonusX[CurrentPlayer] == 3) {
      BonusX[CurrentPlayer] = 5;
      QueueNotification(SOUND_EFFECT_VP_BONUS_5X, 5);
    } else if (BonusX[CurrentPlayer] == 5) {
      BonusX[CurrentPlayer] = 10;
      QueueNotification(SOUND_EFFECT_VP_BONUS_10X, 5);
    }
    BonusXAnimationStart = CurrentTime;
  }

}

unsigned long GameStartNotificationTime = 0;
boolean WaitForBallToReachOuthole = false;
unsigned long UpperBallEjectTime = 0;

int InitGamePlay(boolean curStateChanged) {
  RPU_TurnOffAllLamps();
  SetGeneralIlluminationOn(true);

  if (curStateChanged) {
    GameStartNotificationTime = CurrentTime;
  }

  if (RPU_ReadSingleSwitchState(SW_VUK)) {
    RPU_PushToSolenoidStack(SOL_VUK, VUK_SOLENOID_STRENGTH, true);
  }

  if (MachineLocks) {
    MachineLocks = 0;
    RPU_PushToTimedSolenoidStack(SOL_LIFT_GATE, 5, CurrentTime + 750, true);
  }

  byte ballInShooterLane = RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) ? 1 : 0;
  if ( (CountBallsInTrough()+ballInShooterLane) < TotalBallsLoaded ) {

    if (CurrentTime > (GameStartNotificationTime + 5000)) {
      GameStartNotificationTime = CurrentTime;
      QueueNotification(SOUND_EFFECT_VP_BALL_MISSING, 10);
    }

    return MACHINE_STATE_INIT_GAMEPLAY;
  }

  if ( (CountBallsInTrough() + ballInShooterLane) < (TotalBallsLoaded - NumberOfBallsLocked)) {
    return MACHINE_STATE_INIT_GAMEPLAY;
  }

  // The start button has been hit only once to get
  // us into this mode, so we assume a 1-player game
  // at the moment
  RPU_EnableSolenoidStack();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  // Reset displays & game state variables
  for (int count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
    // Initialize game-specific variables
    Bonus[count] = 0;
    BonusX[count] = 1;
    CurrentAchievements[count] = 0;
    CurrentScores[count] = 0;
    JakeStatus[count] = 0;
    ElwoodStatus[count] = 0;
    JakeCompletions[count] = 0;
    ElwoodCompletions[count] = 0;
    PlayerLocks[count] = 0;
    JakeShotsMade[count] = 0;
    ElwoodShotsMade[count] = 0;
    BandShotsMade[count] = 0;
    NumberOfSkillshotsMade[count] = 0;
  }

  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
  CurrentPlayer = 0;
  NumberOfBallsInPlay = 0;
  NumberOfBallsLocked = 0;
  LastTimePopHit = 0;
  LastTiltWarningTime = 0;
  ShowPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}


byte PlayerUpLamps[4] = {LAMP_HEAD_PLAYER_1_UP, LAMP_HEAD_PLAYER_2_UP, LAMP_HEAD_PLAYER_3_UP, LAMP_HEAD_PLAYER_4_UP};

int InitNewBall(bool curStateChanged) {

  // If we're coming into this mode for the first time
  // then we have to do everything to set up the new ball
  if (curStateChanged) {
    RPU_TurnOffAllLamps();
    BallFirstSwitchHitTime = 0;

    RPU_SetDisableFlippers(false);
    RPU_EnableSolenoidStack();
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    if (CurrentNumPlayers > 1 && (CurrentBallInPlay != 1 || CurrentPlayer != 0) && !SamePlayerShootsAgain) AlertPlayerUp();
    SamePlayerShootsAgain = false;

    RPU_SetDisplayBallInPlay(CurrentBallInPlay);
    RPU_SetLampState(LAMP_HEAD_TILT, 0);
    for (byte count=0; count<4; count++) {
      RPU_SetLampState(PlayerUpLamps[count], count==CurrentPlayer, 0, 500);
    }

    if (BallSaveNumSeconds > 0) {
      RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    BallSaveUsed = false;
    BallTimeInTrough = 0;
    NumTiltWarnings = 0;

    // Initialize game-specific start-of-ball lights & variables
    GameModeStartTime = 0;
    GameModeEndTime = 0;
    GameMode = GAME_MODE_SKILL_SHOT;

    ExtraBallCollected = false;
    SpecialCollected = false;

    PlayfieldMultiplier = 1;
    PlayfieldMultiplierTimeLeft = 0;
    BonusXAnimationStart = 0;
    Bonus[CurrentPlayer] = 1;
    BonusX[CurrentPlayer] = 1;
    ResetDisplayTrackingVariables();

    BallSaveEndTime = 0;
    MiniModesRunning[CurrentPlayer] = 0;
    MiniModesQualified[CurrentPlayer] = 0;
    MiniModeQualifiedExpiration = 0;

    if (CurrentPlayer == 0) {
      // Only change skill shot on first ball of round.
      //      SkillShotTarget = CurrentTime % 3;
    }

    for (byte count = 0; count < 4; count++) {
      LastJakeHit[count] = 0;
    }
    if (ResetJakeProgressEachBall) {
      JakeStatus[CurrentPlayer] = 0;
      JakeCompletions[CurrentPlayer] = 0;
    }
    if (ResetElwoodProgressEachBall) {
      ElwoodCompletions[CurrentPlayer] = 0;
    }
    ElwoodStatus[CurrentPlayer] = 0;

    LastSpinnerSeen = 0xFF;
    LastSpinnerHitTime = 0;
    SpinnerProgress = 0;
    LeftGateAvailableTime = CurrentTime;    
    LeftGateCloseTime = 0;
    TopGateAvailableTime = CurrentTime;
    TopGateCloseTime = 0;
    TopGateCloseSoundPlayed = false;
    TrapSwitchClosedTime = 0;
    LastCaptiveBallHit = 0;
    LeftSpinnerLitTime = 0;

    // Reset Drop Targets
    LeftDropTargets.ResetDropTargets(CurrentTime + 100, true);
    RightDropTargets.ResetDropTargets(CurrentTime + 100, true);

    if (!RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime + 1000);
    LastShooterKickTime = 0;
    NumberOfBallsInPlay = 1;
    SkillshotLetter = 0;
    RPU_SetLampState(LAMP_LAUNCH_BUTTON, 1, 0, 150);
    BallLaunched = false;


    // Reallocate locks
    if ( (PlayerLocks[CurrentPlayer]&BALL_LOCKS_MASK) > MachineLocks) {
      // If the machine locks have been used, unset them
      byte qualifiedLocks = PlayerLocks[CurrentPlayer] & BALL_LOCKS_AVAILABLE_MASK;
      byte survivingLocks = PlayerLocks[CurrentPlayer] & MachineLocks;
      byte locksMoved = (PlayerLocks[CurrentPlayer] & ~MachineLocks) * 16;
      PlayerLocks[CurrentPlayer] = qualifiedLocks | locksMoved | survivingLocks;
    }

    PlayBackgroundSong(SOUND_EFFECT_RALLY_MUSIC_1);
  }

  if (CountBallsInTrough() == (TotalBallsLoaded - NumberOfBallsLocked)) {
    UpdateLiftGate();    
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    LiftGateReleaseRequestTime = 0;
    LiftGateLastReleaseTime = 0;
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

  LastTimeThroughLoop = CurrentTime;
}




unsigned long VUKClosedStart = 0;

void CheckVUKForStuckBall() {
  if (RPU_ReadSingleSwitchState(SW_VUK)) {
    if (VUKClosedStart == 0) {
      VUKClosedStart = CurrentTime;
    } else {
      if (CurrentTime > (VUKClosedStart + 1500)) {

        if (MiniModeQualifiedExpiration != 0 && GameMode == GAME_MODE_UNSTRUCTURED_PLAY) {
          SetGameMode(GAME_MODE_MINIMODE_START);
        } else if (GameMode != GAME_MODE_MINIMODE_START) {
          VUKClosedStart = 0;
          RPU_PushToSolenoidStack(SOL_VUK, VUK_SOLENOID_STRENGTH, true);
        }
      }
    }
  } else {
    VUKClosedStart = 0;
  }

}

void UpdateDropTargets() {
  LeftDropTargets.Update(CurrentTime);
  RightDropTargets.Update(CurrentTime);
}



void UpdateMachineLocks() {
  if (RPU_ReadSingleSwitchState(SW_TRAP)) {
    if (TrapSwitchClosedTime==0) TrapSwitchClosedTime = CurrentTime;
    if (CurrentTime > (TrapSwitchClosedTime+1500)) {
      // There's a ball in the trap
      if ( (MachineLocks&BALL_2_LOCKED)==0 ) {
        // We haven't seen this trapped ball before
        SetGameMode(GAME_MODE_LOCK_BALL_2);
      }
    }
  } else {
    TrapSwitchClosedTime = 0;
  }
}



void ReleaseLiftGate() {

  // We only release the lift gate every 10 seconds
  if (LiftGateReleaseRequestTime < CurrentTime) {
    if (CurrentTime > (LiftGateLastReleaseTime + 10000)) {
      LiftGateReleaseRequestTime = CurrentTime;
    } else {
      LiftGateReleaseRequestTime = LiftGateLastReleaseTime + 10000;      
    }
  }
}


void UpdateLiftGate() {
  if (LiftGateReleaseRequestTime!=0 && CurrentTime > LiftGateReleaseRequestTime) {
    LiftGateReleaseRequestTime = 0;
    LiftGateLastReleaseTime = CurrentTime;
    RPU_PushToSolenoidStack(SOL_LIFT_GATE, 10, true);
  }
}


void UpdateTopGate() {
  if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY || GameMode==GAME_MODE_MINIMODE) {
    if (PlayerLocks[CurrentPlayer] & BALL_LOCKS_AVAILABLE_MASK) { 
      if (TopGateAvailableTime && (CurrentTime > TopGateAvailableTime)) {
        PlaySoundEffect(SOUND_EFFECT_SHORT_SIREN);
        TopGateCloseTime = CurrentTime + TOP_GATE_OPEN_TIME_MS;
        TopGateCloseSoundPlayed = false;
        TopGateAvailableTime = 0;
        RPU_PushToSolenoidStack(SOL_TOP_GATE_OPEN, 20);
      }
    }
  }  
}


void AddABall(boolean releaseLockIfNecessary = false) {
  if (CountBallsInTrough()) {
    RPU_PushToSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, true);
    NumberOfBallsInPlay += 1;    
  } else if (releaseLockIfNecessary) {
    if (MachineLocks) {
      ReleaseLiftGate();
      MachineLocks = 0;
      for (byte count=0; count<RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
        byte curPlayerLocksQualified = PlayerLocks[count] & BALL_LOCKS_AVAILABLE_MASK;
        PlayerLocks[count] *= 16;
        PlayerLocks[count] |= curPlayerLocksQualified;
      }
    }
  }
}


byte GameModeStage;
boolean DisplaysNeedRefreshing = false;
unsigned long LastTimePromptPlayed = 0;
unsigned long LastLoopTick = 0;
unsigned short CurrentBattleLetterPosition = 0xFF;


// This function manages all timers, flags, and lights
int ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  boolean specialAnimationRunning = false;

  UpdateDropTargets();

  if ((CurrentTime - LastSwitchHitTime) > 3000) TimersPaused = true;
  else TimersPaused = false;

  if (LeftGateCloseTime) {
    if (CurrentTime > LeftGateCloseTime) {
      LeftGateCloseTime = 0;
      RPU_PushToSolenoidStack(SOL_LEFT_GATE_CLOSE, 20);
      LeftGateAvailableTime = CurrentTime + LEFT_GATE_DELAY_TIME_MS;
    }
  }


  if (TopGateCloseTime) {

    if ( ((CurrentTime+2700) > TopGateCloseTime) && !TopGateCloseSoundPlayed ) {
      TopGateCloseSoundPlayed = true;
      PlaySoundEffect(SOUND_EFFECT_TOP_GATE_CLOSING);      
    }
    
    if (CurrentTime > TopGateCloseTime) {
      TopGateCloseTime = 0; 
      RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, 20);
      TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
    }
  }

  if (CurrentTime > (LeftSpinnerLitTime+5000)) {
    LeftSpinnerLitTime = 0;
  }

  CheckVUKForStuckBall();
  UpdateMachineLocks();
  UpdateLiftGate();
  UpdateTopGate();

  switch ( GameMode ) {
    case GAME_MODE_SKILL_SHOT:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
        LastTimePromptPlayed = CurrentTime;
        GameModeStage = 0;
        SetGeneralIlluminationOn(true);
        NumberOfShortPlunges = 0;
      }

      // The switch handler will award the skill shot
      // (when applicable) and this mode will move
      // to unstructured play when any valid switch is
      // recorded

      if (CurrentTime > (LastTimePromptPlayed+20000)) {
        AlertPlayerUp();
        LastTimePromptPlayed = CurrentTime;
        ShowPlayerScores(0xFF, false, false);
        GameModeStage = 0;
      }
      if (CurrentTime > (LastTimePromptPlayed+15000)) {
        for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          OverrideScoreDisplay(count, ((unsigned long)CurrentPlayer+1) * 111111, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
        }
      }

      // If we've seen a tilt before plunge, then
      // we can show a countdown timer here
      if (LastTiltWarningTime) {
        if ( CurrentTime > (LastTiltWarningTime+30000) ) {
          LastTiltWarningTime = 0;
        } else {
          byte secondsSinceWarning = (CurrentTime - LastTiltWarningTime)/1000;
          for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
            if (count == CurrentPlayer) OverrideScoreDisplay(count, 30 - secondsSinceWarning, DISPLAY_OVERRIDE_ANIMATION_CENTER);
          }
          DisplaysNeedRefreshing = true;
        }
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
      }

      if (NumberOfShortPlunges>3 || BallFirstSwitchHitTime != 0) {
        ShowPlayerScores(0xFF, false, false);
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      break;

    case GAME_MODE_UNSTRUCTURED_PLAY:
      // If this is the first time in this mode
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        DisplaysNeedRefreshing = true;
        if (DEBUG_MESSAGES) {
          Serial.write("Entering unstructured play\n");
        }
        SetGeneralIlluminationOn(true);
        unsigned short songNum = SOUND_EFFECT_BACKGROUND_SONG_1 + (CurrentBallInPlay - 1);
        if (Audio.GetBackgroundSong()!=songNum) {
          PlayBackgroundSong(songNum);
        }          
        GameModeStage = 0;
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 1000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, CurrentTime + 100, true);
          LastShooterKickTime = CurrentTime;
        }
      }

      // Display Overrides in Unstructured Play
      if (MiniModeQualifiedExpiration) {
        specialAnimationRunning = true;
        byte animationStep = ((CurrentTime / 40) % LAMP_ANIMATION_STEPS);
        ShowLampAnimationSingleStep(3, LAMP_ANIMATION_STEPS - (animationStep + 1));

        if (CurrentTime > MiniModeQualifiedExpiration) {
          MiniModeQualifiedExpiration = 0;
          GameModeStage = 0;
          DisplaysNeedRefreshing = true;
        } else {
          if ( (CurrentTime + 10000) > MiniModeQualifiedExpiration ) {
            for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
              if (count != CurrentPlayer) OverrideScoreDisplay(count, (MiniModeQualifiedExpiration - CurrentTime) / 1000, DISPLAY_OVERRIDE_ANIMATION_CENTER);
            }
            if (GameModeStage==0) {
              GameModeStage = 1;
              PlaySoundEffect(SOUND_EFFECT_10_SECOND_WARNING);
            }
          }
        }
      } else if (LastSpinnerHitTime) {

        if (CurrentTime > (LastSpinnerHitTime + 5000)) {
          LastSpinnerHitTime = 0;
        } else {
          if (SpinnerProgress && SpinnerProgress < SpinnerHitsToQualifyBand) {
            int spinnerHitsRemaining = SpinnerHitsToQualifyBand - SpinnerProgress;
            OverrideScoreDisplay(CurrentPlayer, spinnerHitsRemaining, DISPLAY_OVERRIDE_ANIMATION_CENTER);
          }
        }
        DisplaysNeedRefreshing = true;
      } else if (PlayfieldMultiplier > 1) {
        // Playfield X value is only reset during unstructured play
        if (PlayfieldMultiplierTimeLeft && (CurrentTime > LastLoopTick)) {
          unsigned long numTicks = CurrentTime - LastLoopTick;
          if (numTicks > PlayfieldMultiplierTimeLeft) {
            PlayfieldMultiplierTimeLeft = 0;
            PlayfieldMultiplier = 1;
          } else {
            PlayfieldMultiplierTimeLeft -= numTicks;
          }
        } else {
          for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
            if (count != CurrentPlayer) OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_ANIMATION_BOUNCE);
          }
          DisplaysNeedRefreshing = true;
        }
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
      }

      break;

    case GAME_MODE_MINIMODE_START:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + 2921;
        PlayBackgroundSong(SOUND_EFFECT_MINIMODE_SONG_1);
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime + 500);
        NumberOfBallsInPlay += 1;
        GameModeStage = 0;
        AddABallUsed = false;
        RightFlipsSeen = 0;

        if (MiniModesQualified[CurrentPlayer] & ELWOOD_MINIMODE_FLAG) {
          LeftDropTargets.ResetDropTargets(CurrentTime + 250, true);
          RightDropTargets.ResetDropTargets(CurrentTime + 750, true);
        }
      }

      specialAnimationRunning = LampAnimationMinimodeStart( CurrentTime - GameModeStartTime, 
                                                            (MiniModesQualified[CurrentPlayer] & JAKE_MINIMODE_FLAG) ? true : false, 
                                                            (MiniModesQualified[CurrentPlayer] & ELWOOD_MINIMODE_FLAG) ? true : false, 
                                                            (MiniModesQualified[CurrentPlayer] & BAND_MINIMODE_FLAG) ? true : false );

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) && CurrentTime>(GameModeStartTime+1800)) {
        if (GameModeStage == 0) {
          RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, true);
          LastShooterKickTime = CurrentTime;
        }
        GameModeStage = 1;
      }

      if (GameModeStage == 1 && (CurrentTime > GameModeEndTime)) {
        RPU_PushToSolenoidStack(SOL_VUK, VUK_SOLENOID_STRENGTH, true);
        PlayfieldMultiplier = 1 + CountBits(MiniModesQualified[CurrentPlayer]);
        MiniModesRunning[CurrentPlayer] = MiniModesQualified[CurrentPlayer];       
        MiniModesQualified[CurrentPlayer] = 0;
        MiniModeQualifiedExpiration = 0;
        ElwoodTargetLit = 0;
        JakeTargetLit = 0;
        SetGameMode(GAME_MODE_MINIMODE);
        if (BallSaveOnMinimodes) {
          BallSaveEndTime = CurrentTime + ((unsigned long)BallSaveOnMinimodes * 1000);
        }
      }
      break;

    case GAME_MODE_MINIMODE:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
      }

      JakeTargetLit = (CurrentTime/250)%4;

      for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
        if (count != CurrentPlayer) OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 1000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, CurrentTime + 100, true);
          LastShooterKickTime = CurrentTime;
        }
      }

      if (NumberOfBallsInPlay == 1) {
        MiniModesRunning[CurrentPlayer] = 0;
        SetGameMode(GAME_MODE_MINIMODE_END);
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
      }
      break;

    case GAME_MODE_MINIMODE_END:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + 1000;
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_SONG_1);
        PlayfieldMultiplier = 1;
      }

      if (CurrentTime > GameModeEndTime) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      break;

    case GAME_MODE_LOCK_BALL_1:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
        MachineLocks = BALL_1_LOCKED;
        PlayerLocks[CurrentPlayer] = BALL_1_LOCKED | (PlayerLocks[CurrentPlayer]&BALL_LOCKS_AVAILABLE_MASK);
        QueueNotification(SOUND_EFFECT_VP_BALL_1_LOCKED, 10);
        NumberOfBallsLocked = 1;
        GameModeStage = 0;
        GameModeEndTime = CurrentTime + 2000;
        if (LockHandling==LOCK_HANDLING_VIRTUAL_LOCKS) {
          ReleaseLiftGate();
          MachineLocks = 0;
          GameModeStage = 1;
        } else {
          RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime + 500);
        }
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (GameModeStage == 0) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, CurrentTime + 100, true);
          LastShooterKickTime = CurrentTime;
        }
        GameModeStage = 1;
      }

      if (GameModeStage == 1 && (CurrentTime > GameModeEndTime)) {
        SetGameMode(LastGameMode);
      }
      break;

    case GAME_MODE_LOCK_BALL_2:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
        if (DEBUG_MESSAGES) {
          char buf[128];
          sprintf(buf, "Ball 2 lock start, ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
          Serial.write(buf);
        }
        QueueNotification(SOUND_EFFECT_VP_BALL_2_LOCKED, 10);
        MachineLocks |= BALL_2_LOCKED;
        PlayerLocks[CurrentPlayer] &= ~BALL_LOCKS_AVAILABLE_MASK;
        PlayerLocks[CurrentPlayer] |= BALL_2_LOCKED;
        NumberOfBallsInPlay = 2;
        GameModeStage = 0;
        GameModeEndTime = CurrentTime + 2000;
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime + 500);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (GameModeStage == 0) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, CurrentTime + 100, true);
          LastShooterKickTime = CurrentTime;
        }
        GameModeStage = 1;
      }

      if (GameModeStage==1 && CurrentTime>(LastShooterKickTime+1000) && CountBallsInTrough()) {
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime + 500);
        GameModeStage = 0;
      }

      if (GameModeStage == 1 && (CurrentTime > GameModeEndTime)) {
        NumberOfBallsLocked = 2;
        NumberOfBallsInPlay = 1;
        if (DEBUG_MESSAGES) {
          char buf[128];
          sprintf(buf, "Ball 2 lock end, ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
          Serial.write(buf);
        }
        SetGameMode(GAME_MODE_MULTIBALL_3_START);
      }
      break;

    case GAME_MODE_MULTIBALL_3_START:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeStage = 0;
        GameModeEndTime = CurrentTime + 25000;
        Multiball3StartShotHit = false;
        Multiball3StartAward = 10000;
      }
      BallSaveEndTime = GameModeEndTime;

      if (CurrentTime<GameModeEndTime) Multiball3StartAward = ((((GameModeEndTime - CurrentTime)/1000) + 1) * 1000) + 10000;
      else Multiball3StartAward = 10000;
      
      if (Multiball3StartShotHit) {
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
        SetGameMode(GAME_MODE_MULTIBALL_3);
      } else {
        for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          if (count != CurrentPlayer) OverrideScoreDisplay(count, Multiball3StartAward, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
        }
      }

      if (CurrentTime > GameModeEndTime) {
        // The player didn't start multiball themselves, so we have to start it now
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
        SetGameMode(GAME_MODE_MULTIBALL_3);
      }
      break;
    
    case GAME_MODE_MULTIBALL_3:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        StartScoreAnimation(Multiball3StartAward);
        PlaySoundEffect(SOUND_EFFECT_JAIL_BREAK);
        ReleaseLiftGate();
        BallSaveEndTime = CurrentTime + (15000);
        NumberOfBallsInPlay = 3;
        NumberOfBallsLocked = 0;
        unsigned short songNum = SOUND_EFFECT_MULTIBALL_SONG_1;
        PlayBackgroundSong(songNum);
        BallSaveEndTime = CurrentTime + ((unsigned long)BallSaveOnMultiball) * 1000;
      }

      PlayfieldMultiplier = NumberOfBallsInPlay;

      for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
        if (count != CurrentPlayer) OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 1000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, CurrentTime + 100, true);
          LastShooterKickTime = CurrentTime;
        }
      }

      if (NumberOfBallsInPlay == 1) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
        DisplaysNeedRefreshing = false;
        ShowPlayerScores(0xFF, false, false);
      }
      break;
  }

  if ( !specialAnimationRunning && NumTiltWarnings <= MaxTiltWarnings ) {
    ShowBonusLamps();
    ShowPopBumperAndSlingLamps();
    ShowStandupTargetLamps();
    ShowLoopSpinnerAndLockLamps();
    ShowDropTargetLamps();
    ShowLaneLamps();
    ShowShootAgainLamp();
  }



  if (UpdateDisplays(false)) {
    PlaySoundEffect(SOUND_EFFECT_SCORE_TICK);
  }

  // Check to see if ball is in the outhole
  if (CountBallsInTrough() > (TotalBallsLoaded - (NumberOfBallsInPlay + NumberOfBallsLocked))) {

    if (BallTimeInTrough == 0) {
      // If this is the first time we're seeing too many balls in the trough, we'll wait to make sure
      // everything is settled
      BallTimeInTrough = CurrentTime;
    } else {

      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing or passing through
      if ((CurrentTime - BallTimeInTrough) > 750) {

        if (BallFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings) {
          // Nothing hit yet, so return the ball to the player
          RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime);
          BallTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (BallSaveEndTime && CurrentTime < (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
            RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BALL_SERVE_SOLENOID_STRENGTH, CurrentTime + 100);
            QueueNotification(SOUND_EFFECT_VP_SHOOT_AGAIN, 10);

            RPU_SetLampState(LAMP_SHOOT_AGAIN, 0);
            BallTimeInTrough = CurrentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

            // Only 1 ball save if one ball in play
            if (NumberOfBallsInPlay == 1) {
              BallSaveEndTime = CurrentTime + 1000;
            } else {
              if (CurrentTime > BallSaveEndTime) BallSaveEndTime += 1000;
            }

          } else {

            NumberOfBallsInPlay -= 1;
            if (NumberOfBallsInPlay == 0) {

              // if we don't hold locks, release them
              if (LockHandling==LOCK_HANDLING_PURGE_LOCKS) {
                ReleaseLiftGate();
                MachineLocks = 0;
                NumberOfBallsLocked = 0;
              }
              
              ShowPlayerScores(0xFF, false, false);
              Audio.StopAllAudio();
              returnState = MACHINE_STATE_COUNTDOWN_BONUS;
            }
          }
        }
      }
    }
  } else {
    BallTimeInTrough = 0;
  }

  LastLoopTick = CurrentTime;
  LastTimeThroughLoop = CurrentTime;
  return returnState;
}



unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long BonusCountDownEndTime = 0;
byte DecrementingBonusCounter;
byte IncrementingBonusXCounter;
byte TotalBonus = 0;
byte TotalBonusX = 0;
boolean CountdownBonusHurryUp = false;

int CountDownDelayTimes[] = {175, 130, 105, 90, 80, 70, 60, 40, 30, 20};

int CountdownBonus(boolean curStateChanged) {

  // If this is the first time through the countdown loop
  if (curStateChanged) {

    if (LeftGateCloseTime) {
      LeftGateCloseTime = 0;
      LeftGateAvailableTime = 0;
      RPU_PushToSolenoidStack(SOL_LEFT_GATE_CLOSE, 10);
    }

    if (TopGateCloseTime) {
      TopGateCloseTime = 0;
      TopGateAvailableTime = 0;
      RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, 10);
    }

    CountdownStartTime = CurrentTime;
    LastCountdownReportTime = CurrentTime;
    ShowBonusLamps();
    IncrementingBonusXCounter = 1;
    DecrementingBonusCounter = Bonus[CurrentPlayer];
    TotalBonus = Bonus[CurrentPlayer];
    TotalBonusX = BonusX[CurrentPlayer];
    CountdownBonusHurryUp = false;

    BonusCountDownEndTime = 0xFFFFFFFF;
    // Some sound cards have a special index
    // for a "sound" that will turn off
    // the current background drone or currently
    // playing sound
    //    PlaySoundEffect(SOUND_EFFECT_STOP_BACKGROUND);
  }

  unsigned long countdownDelayTime = (unsigned long)(CountDownDelayTimes[IncrementingBonusXCounter - 1]);
  if (CountdownBonusHurryUp && countdownDelayTime > ((unsigned long)CountDownDelayTimes[9])) countdownDelayTime = CountDownDelayTimes[9];

  if ((CurrentTime - LastCountdownReportTime) > countdownDelayTime) {

    if (DecrementingBonusCounter) {

      // Only give sound & score if this isn't a tilt
      if (NumTiltWarnings <= MaxTiltWarnings) {
        PlaySoundEffect(SOUND_EFFECT_BONUS_COUNT);
        CurrentScores[CurrentPlayer] += 1000;
      }

      DecrementingBonusCounter -= 1;
      Bonus[CurrentPlayer] = DecrementingBonusCounter;
      ShowBonusLamps();

    } else if (BonusCountDownEndTime == 0xFFFFFFFF) {
      IncrementingBonusXCounter += 1;
      if (BonusX[CurrentPlayer] > 1) {
        DecrementingBonusCounter = TotalBonus;
        Bonus[CurrentPlayer] = TotalBonus;
        ShowBonusLamps();
        BonusX[CurrentPlayer] -= 1;
        if (BonusX[CurrentPlayer] == 9) BonusX[CurrentPlayer] = 8;
      } else {
        BonusX[CurrentPlayer] = TotalBonusX;
        Bonus[CurrentPlayer] = TotalBonus;
        BonusCountDownEndTime = CurrentTime + 1000;
      }
    }
    LastCountdownReportTime = CurrentTime;
  }

  if (CurrentTime > BonusCountDownEndTime) {

    // Reset any lights & variables of goals that weren't completed
    BonusCountDownEndTime = 0xFFFFFFFF;
    return MACHINE_STATE_BALL_OVER;
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}



void CheckHighScores() {
  unsigned long highestScore = 0;
  int highScorePlayerNum = 0;
  for (int count = 0; count < CurrentNumPlayers; count++) {
    if (CurrentScores[count] > highestScore) highestScore = CurrentScores[count];
    highScorePlayerNum = count;
  }

  if (highestScore > HighScore) {
    HighScore = highestScore;
    if (HighScoreReplay) {
      AddCredit(false, 3);
      RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 3);
    }
    RPU_WriteULToEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, highestScore);
    RPU_WriteULToEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE) + 1);

    for (int count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
      if (count == highScorePlayerNum) {
        RPU_SetDisplay(count, CurrentScores[count], true, 2);
      } else {
        RPU_SetDisplayBlank(count, 0x00);
      }
    }

    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime + 300, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 20, CurrentTime + 600, true);
  }
}


unsigned long MatchSequenceStartTime = 0;
unsigned long MatchDelay = 150;
byte MatchDigit = 0;
byte NumMatchSpins = 0;
byte ScoreMatches = 0;

int ShowMatchSequence(boolean curStateChanged) {
  if (!MatchFeature) return MACHINE_STATE_ATTRACT;

  if (curStateChanged) {
    MatchSequenceStartTime = CurrentTime;
    MatchDelay = 1500;
    MatchDigit = CurrentTime % 10;
    NumMatchSpins = 0;
    RPU_SetLampState(LAMP_HEAD_MATCH, 1, 0);
    RPU_SetDisableFlippers();
    ScoreMatches = 0;
  }

  if (NumMatchSpins < 40) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      MatchDigit += 1;
      if (MatchDigit > 9) MatchDigit = 0;
      //PlaySoundEffect(10+(MatchDigit%2));
      PlaySoundEffect(SOUND_EFFECT_MATCH_SPIN);
      RPU_SetDisplayBallInPlay((int)MatchDigit * 10);
      MatchDelay += 50 + 4 * NumMatchSpins;
      NumMatchSpins += 1;
      RPU_SetLampState(LAMP_HEAD_MATCH, NumMatchSpins % 2, 0);

      if (NumMatchSpins == 40) {
        RPU_SetLampState(LAMP_HEAD_MATCH, 0);
        MatchDelay = CurrentTime - MatchSequenceStartTime;
      }
    }
  }

  if (NumMatchSpins >= 40 && NumMatchSpins <= 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      if ( (CurrentNumPlayers > (NumMatchSpins - 40)) && ((CurrentScores[NumMatchSpins - 40] / 10) % 10) == MatchDigit) {
        ScoreMatches |= (1 << (NumMatchSpins - 40));
        AddSpecialCredit();
        MatchDelay += 1000;
        NumMatchSpins += 1;
        RPU_SetLampState(LAMP_HEAD_MATCH, 1);
      } else {
        NumMatchSpins += 1;
      }
      if (NumMatchSpins == 44) {
        MatchDelay += 5000;
      }
    }
  }

  if (NumMatchSpins > 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      return MACHINE_STATE_ATTRACT;
    }
  }

  for (int count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
    if ((ScoreMatches >> count) & 0x01) {
      // If this score matches, we're going to flash the last two digits
      byte upperMask = 0x0F;
      byte lowerMask = 0x30;
      if (RPU_OS_NUM_DIGITS == 7) {
        upperMask = 0x1F;
        lowerMask = 0x60;
      }
      if ( (CurrentTime / 200) % 2 ) {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) & upperMask);
      } else {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) | lowerMask);
      }
    }
  }

  return MACHINE_STATE_MATCH_MODE;
}




////////////////////////////////////////////////////////////////////////////
//
//  Switch Handling functions
//
////////////////////////////////////////////////////////////////////////////
/*
  // Example lock function

  void HandleLockSwitch(byte lockIndex) {

  if (GameMode==GAME_MODE_UNSTRUCTURED_PLAY) {
    // If this player has a lock available
    if (PlayerLockStatus[CurrentPlayer] & (LOCK_1_AVAILABLE<<lockIndex)) {
      // Lock the ball
      LockBall(lockIndex);
      SetGameMode(GAME_MODE_OFFER_LOCK);
    } else {
      if ((MachineLocks & (LOCK_1_ENGAGED<<lockIndex))==0) {
        // Kick unlocked ball
        RPU_PushToSolenoidStack(SOL_UPPER_BALL_EJECT, 12, true);
      }
    }
  }
  }

*/

int HandleSystemSwitches(int curState, byte switchHit) {
  int returnState = curState;
  switch (switchHit) {
    case SW_SELF_TEST_SWITCH:
    case SW_SELF_TEST_ON_MATRIX:
#if (RPU_MPU_ARCHITECTURE<10)
      returnState = MACHINE_STATE_TEST_LAMPS;
#else
      returnState = MACHINE_STATE_TEST_BOOT;
#endif
      SetLastSelfTestChangedTime(CurrentTime);
      break;
    case SW_COIN_1:
      AddCoinToAudit(SwitchToChuteNum(switchHit));
      AddCoin(SwitchToChuteNum(switchHit));
      break;
    case SW_CREDIT_RESET:
      if (MachineState == MACHINE_STATE_MATCH_MODE) {
        // If the first ball is over, pressing start again resets the game
        if (Credits >= 1 || FreePlayMode) {
          if (!FreePlayMode) {
            Credits -= 1;
            RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
            RPU_SetDisplayCredits(Credits, !FreePlayMode);
          }
          returnState = MACHINE_STATE_INIT_GAMEPLAY;
        }
      } else {
        CreditResetPressStarted = CurrentTime;
      }
      break;
    case SW_OUTHOLE:
      // Some machines have a kicker to move the ball
      // from the outhole to the re-shooter ramp
      break;
    case SW_PLUMB_TILT:
      if (BallFirstSwitchHitTime) {
        if ( CurrentTime > (LastTiltWarningTime + TILT_WARNING_DEBOUNCE_TIME) ) {
          LastTiltWarningTime = CurrentTime;
          NumTiltWarnings += 1;
          if (NumTiltWarnings > MaxTiltWarnings) {
            RPU_DisableSolenoidStack();
            RPU_SetDisableFlippers(true);
            RPU_TurnOffAllLamps();
            Audio.StopAllAudio();
            RPU_SetLampState(LAMP_HEAD_TILT, 1);
            PlaySoundEffect(SOUND_EFFECT_TILT);
          } else {
            PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
          }
        }
      } else {
        // Tilt before ball is plunged -- show a timer in ManageGameMode if desired
        if ( CurrentTime > (LastTiltWarningTime + TILT_WARNING_DEBOUNCE_TIME) ) {
          PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
        }
        LastTiltWarningTime = CurrentTime;
      }
      break;
  }

  return returnState;
}


void QualifyMiniMode(byte miniModeFlag) {
  // Can't qualify a minimode while you're in a minimode
  if (GameMode==GAME_MODE_MINIMODE) return;
  
  if ( (MiniModesQualified[CurrentPlayer]&miniModeFlag) == 0) {
    MiniModesQualified[CurrentPlayer] |= miniModeFlag;
    if (miniModeFlag == ELWOOD_MINIMODE_FLAG) QueueNotification(SOUND_EFFECT_VP_ELWOOD_MINIMODE_QUALIFIED, 8);
    else if (miniModeFlag == JAKE_MINIMODE_FLAG) QueueNotification(SOUND_EFFECT_VP_JAKE_MINIMODE_QUALIFIED, 8);
    else if (miniModeFlag == BAND_MINIMODE_FLAG) QueueNotification(SOUND_EFFECT_VP_BAND_MINIMODE_QUALIFIED, 8);

    if (MiniModeQualifiedExpiration == 0) {
      MiniModeQualifiedExpiration = CurrentTime + 30000;
    } else {
      MiniModeQualifiedExpiration += 15000;
    }
  }
}



void HandleLeftDropTarget(byte switchHit) {

  byte result;
  unsigned long numTargetsDown = 0;
  result = LeftDropTargets.HandleDropTargetHit(switchHit);
  numTargetsDown = (unsigned long)CountBits(result);
  boolean soundPlayed = false;
  boolean scoreAdded = false;

  if (GameMode==GAME_MODE_SKILL_SHOT && SkillshotLetter < 3) {
    if (  (SkillshotLetter==0 && switchHit==SW_DROP_L_1) ||
          (SkillshotLetter==1 && switchHit==SW_DROP_L_2) || 
          (SkillshotLetter==2 && switchHit==SW_DROP_L_3) ) {
      NumberOfSkillshotsMade[CurrentPlayer] += 1;
      StartScoreAnimation(50000 * ((unsigned long)NumberOfSkillshotsMade[CurrentPlayer]));
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
    } else {
      PlaySoundEffect(SOUND_EFFECT_MISSED_MODE_SHOT);
    }
  } else if (GameMode==GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer]&ELWOOD_MINIMODE_FLAG)) {
    if (  (ElwoodTargetLit==0 && switchHit==SW_DROP_L_1) ||
          (ElwoodTargetLit==1 && switchHit==SW_DROP_L_2) || 
          (ElwoodTargetLit==2 && switchHit==SW_DROP_L_3) ) {
      ElwoodTargetLit += 1;
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
      ElwoodShotsMade[CurrentPlayer] += 1;
      LeftDropTargets.ResetDropTargets(CurrentTime + 500, true);
      RightDropTargets.ResetDropTargets(CurrentTime + 1000, true);
    }
    return;
  }

  boolean cleared = LeftDropTargets.CheckIfBankCleared();
  boolean readyToClearBoth = false;
  if (cleared) {

    if (ElwoodCompletions[CurrentPlayer] == 0 && OneBankFirstClear) {
      readyToClearBoth = true;
    } else {
      if (RightDropTargets.GetStatus() == 0x07) readyToClearBoth = true;
    }

    if (readyToClearBoth) {
      LeftDropTargets.ResetDropTargets(CurrentTime + 500, true);
      RightDropTargets.ResetDropTargets(CurrentTime + 1000, true);
      ElwoodCompletions[CurrentPlayer] += 1;
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 5000 * ElwoodCompletions[CurrentPlayer];
      PlaySoundEffect(SOUND_EFFECT_DROP_TARGET_COMPLETE);
      soundPlayed = true;
      scoreAdded = true;
      IncreaseBonusX();
      if ((ElwoodCompletions[CurrentPlayer] % ElwoodClearsToQualify) == 0) {
        QualifyMiniMode(ELWOOD_MINIMODE_FLAG);
      }
      ElwoodStatus[CurrentPlayer] = 0;
    } else {
      ElwoodStatus[CurrentPlayer] |= 0x07;
    }

  } else {
    ElwoodStatus[CurrentPlayer] = LeftDropTargets.GetStatus();
    ElwoodStatus[CurrentPlayer] |= (RightDropTargets.GetStatus()) * 0x08;
  }

  if (!soundPlayed) {
    PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
  }
  if (!scoreAdded) {
    CurrentScores[CurrentPlayer] += PlayfieldMultiplier * numTargetsDown * 100;
  }

}


void HandleRightDropTarget(byte switchHit) {

  byte result;
  unsigned long numTargetsDown = 0;
  result = RightDropTargets.HandleDropTargetHit(switchHit);
  numTargetsDown = (unsigned long)CountBits(result);
  boolean soundPlayed = false;
  boolean scoreAdded = false;

  if (GameMode==GAME_MODE_SKILL_SHOT && SkillshotLetter > 2) {
    if (  (SkillshotLetter==3 && switchHit==SW_DROP_R_1) ||
          (SkillshotLetter==4 && switchHit==SW_DROP_R_2) || 
          (SkillshotLetter==5 && switchHit==SW_DROP_R_3) ) {
      NumberOfSkillshotsMade[CurrentPlayer] += 1;
      StartScoreAnimation(50000 * ((unsigned long)NumberOfSkillshotsMade[CurrentPlayer]));
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
    } else {
      PlaySoundEffect(SOUND_EFFECT_MISSED_MODE_SHOT);
    }
  } else if (GameMode==GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer]&ELWOOD_MINIMODE_FLAG)) {
    if (  (ElwoodTargetLit==3 && switchHit==SW_DROP_R_1) ||
          (ElwoodTargetLit==4 && switchHit==SW_DROP_R_2) || 
          (ElwoodTargetLit==5 && switchHit==SW_DROP_R_3) ) {
      ElwoodTargetLit += 1;
      if (ElwoodTargetLit>5) {
        ElwoodTargetLit = 0;
        StartScoreAnimation(ELWOOD_MINIMODE_COMPLETION_BONUS);
        PlaySoundEffect(SOUND_EFFECT_DROP_TARGET_BONUS_SOUND);
      } else {      
        PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
        CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
      }
      ElwoodShotsMade[CurrentPlayer] += 1;
      LeftDropTargets.ResetDropTargets(CurrentTime + 500, true);
      RightDropTargets.ResetDropTargets(CurrentTime + 1000, true);
    }
    return;
  }
  
  boolean cleared = RightDropTargets.CheckIfBankCleared();
  boolean readyToClearBoth = false;
  if (cleared) {

    if (ElwoodCompletions[CurrentPlayer] == 0 && OneBankFirstClear) {
      readyToClearBoth = true;
    } else {
      if (LeftDropTargets.GetStatus() == 0x07) readyToClearBoth = true;
    }

    if (readyToClearBoth) {
      RightDropTargets.ResetDropTargets(CurrentTime + 500, true);
      LeftDropTargets.ResetDropTargets(CurrentTime + 1000, true);
      ElwoodCompletions[CurrentPlayer] += 1;
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 5000 * ElwoodCompletions[CurrentPlayer];
      PlaySoundEffect(SOUND_EFFECT_DROP_TARGET_COMPLETE);
      scoreAdded = true;
      soundPlayed = true;
      IncreaseBonusX();
      if ((ElwoodCompletions[CurrentPlayer] % ElwoodClearsToQualify) == 0) {
        QualifyMiniMode(ELWOOD_MINIMODE_FLAG);
      }
      ElwoodStatus[CurrentPlayer] = 0;
    } else {
      ElwoodStatus[CurrentPlayer] |= 0x38;
    }

  } else {
    ElwoodStatus[CurrentPlayer] = LeftDropTargets.GetStatus();
    ElwoodStatus[CurrentPlayer] |= (RightDropTargets.GetStatus()) * 0x08;
  }

  if (!soundPlayed) {
    PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
  }
  if (!scoreAdded) {
    CurrentScores[CurrentPlayer] += PlayfieldMultiplier * numTargetsDown * 100;
  }  
}


void HandleJakeStandup(byte switchHit) {

  byte jakeNum = switchHit - SW_STANDUP_J;
  if ( CurrentTime < (LastJakeHit[jakeNum]+500) ) return;
  
  LastJakeHit[jakeNum] = CurrentTime;

  byte prevStatus = JakeStatus[CurrentPlayer];
  JakeStatus[CurrentPlayer] |= 1 << (jakeNum);
  if (prevStatus==JakeStatus[CurrentPlayer]) CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
  else CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "SU switch %d (0x%02X)\n", switchHit, JakeStatus[CurrentPlayer]);
    Serial.write(buf);
  }

  if (JakeStatus[CurrentPlayer] == JAKE_STATUS_COMPLETE) {
    // Qualify the Jake mini mode
    JakeStatus[CurrentPlayer] = 0x00;
    JakeCompletions[CurrentPlayer] += 1;

    if ((JakeCompletions[CurrentPlayer] % JakeClearsToQualify) == 0) {
      QualifyMiniMode(JAKE_MINIMODE_FLAG);
    }

  }

  if (GameMode==GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer] & JAKE_MINIMODE_FLAG) ) {
    if ( (switchHit-SW_STANDUP_J)==JakeTargetLit ) {
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
      JakeShotsMade[CurrentPlayer] += 1;
    } else {
      if (CurrentTime>(LastTimeModeShotMissed+1000)) {
        PlaySoundEffect(SOUND_EFFECT_MISSED_MODE_SHOT);
        LastTimeModeShotMissed = CurrentTime;
      }
    }
  } else {
    PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
  }

}


void HandleSpinnerProgress(boolean leftSpinnerSeen = true) {
  if (LastSpinnerSeen == 0xFF) {
    SpinnerProgress += 1;
  } else {
    if (leftSpinnerSeen && (LastSpinnerSeen == 1)) SpinnerProgress += 10;
    else if (!leftSpinnerSeen && (LastSpinnerSeen == 0)) SpinnerProgress += 10;
    else SpinnerProgress += 1;
  }

  if (SpinnerProgress >= SpinnerHitsToQualifyBand) {
    QualifyMiniMode(BAND_MINIMODE_FLAG);
    SpinnerProgress = 0;
  }

  if (leftSpinnerSeen) LastSpinnerSeen = 0;
  else LastSpinnerSeen = 1;
  LastSpinnerHitTime = CurrentTime;

  if (GameMode==GAME_MODE_MINIMODE && (MiniModesRunning[CurrentPlayer]&BAND_MINIMODE_FLAG) && leftSpinnerSeen) {
    CurrentScores[CurrentPlayer] += 2500 * PlayfieldMultiplier;
    BandShotsMade[CurrentPlayer] += 1;
  } else {
    if (JakeStatus[CurrentPlayer] && !leftSpinnerSeen) {
      unsigned long numBits = CountBits(JakeStatus[CurrentPlayer]);
      JakeStatus[CurrentPlayer] *= 2;
      if (JakeStatus[CurrentPlayer] & 0x10) {
        JakeStatus[CurrentPlayer] &= 0x0F;
        JakeStatus[CurrentPlayer] |= 0x01;
      }
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier * numBits;
    } else if (LeftSpinnerLitTime && leftSpinnerSeen) {
      CurrentScores[CurrentPlayer] += 2000 * PlayfieldMultiplier;      
    } else {
      CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
    }
  }
  
}

void HandleCaptiveBallHit() {

  boolean openGate = false;

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Captive ball start ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
    Serial.write(buf);
  }

  if ( (MachineLocks&BALL_LOCKS_MASK)==0x00 ) {
    // No locks yet, so qualify one
    PlayerLocks[CurrentPlayer] |= BALL_1_LOCK_AVAILABLE;
    openGate = true;
  } else if ( (MachineLocks&BALL_LOCKS_MASK)==0x01 ) {
    // There's one machine lock, so it either needs to be stolen 
    // or we can qualify lock 2
    if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK)==0x00 ) {
      // We can steal lock 1
      QueueNotification(SOUND_EFFECT_VP_LOCK_1_STOLEN, 10);
      PlayerLocks[CurrentPlayer] = BALL_1_LOCKED;
    } else {
      // This player already has this lock -- qualify lock 2
      PlayerLocks[CurrentPlayer] |= BALL_2_LOCK_AVAILABLE;
      openGate = true;
      // This purposely ignores the case where there are more
      // player locks than machine locks because it shouldn't
      // happen.
    }
  } else if ( (MachineLocks&BALL_LOCKS_MASK)==0x03 ) {
    if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK)==0x00 ) {
      // We can steal lock 1
      QueueNotification(SOUND_EFFECT_VP_LOCK_1_STOLEN, 10);
      PlayerLocks[CurrentPlayer] = BALL_1_LOCKED;
    } else if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK)==0x01 ) {
      // We can steal lock 2
      QueueNotification(SOUND_EFFECT_VP_LOCK_2_STOLEN, 10);
      PlayerLocks[CurrentPlayer] |= BALL_2_LOCKED;
    } else if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK)==0x03 ) {
      // Start the multiball
      Multiball3StartShotHit = true;
    }
  }

  if (openGate) {

    if (DEBUG_MESSAGES) {
      char buf[128]; 
      sprintf(buf, "Open gate: Avail=%lu, Current=%lu\n", TopGateAvailableTime, CurrentTime);
      Serial.write(buf);
    }
    
    if (TopGateAvailableTime && (CurrentTime > TopGateAvailableTime)) {
      PlaySoundEffect(SOUND_EFFECT_SHORT_SIREN);
      TopGateCloseSoundPlayed = false;
      TopGateCloseTime = CurrentTime + TOP_GATE_OPEN_TIME_MS;
      TopGateAvailableTime = 0;
      RPU_PushToSolenoidStack(SOL_TOP_GATE_OPEN, 20);
    }
  }

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Captive ball end ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
    Serial.write(buf);
  }
  

}


void ValidateAndRegisterPlayfieldSwitch() {
  LastSwitchHitTime = CurrentTime;
  if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
}


void HandleGamePlaySwitches(byte switchHit) {

  switch (switchHit) {

    case SW_RIGHT_FLIPPER:
      if (!BallLaunched) {
        SkillshotLetter += 1;
        if (SkillshotLetter>5) SkillshotLetter = 0;
      }
      break;

    case SW_LAUNCH:
      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        RPU_SetLampState(LAMP_LAUNCH_BUTTON, 0);
        BallLaunched = true;
        if (RPU_ReadSingleSwitchState(SW_LEFT_FLIPPER)) {
          NumberOfShortPlunges += 1;
          if (NumberOfShortPlunges<3) RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_LIGHT_STRENGTH, true);
          else RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_LIGHT_STRENGTH + 1, true);
          if (Audio.GetBackgroundSong()==SOUND_EFFECT_RALLY_MUSIC_1) {
            Audio.StopAllMusic();
            PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_1);
          }
        } else {
          RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, SHOOTER_KICK_FULL_STRENGTH, true);
          if (Audio.GetBackgroundSong()==SOUND_EFFECT_RALLY_MUSIC_1) {
            Audio.StopAllMusic();
            PlaySoundEffect(SOUND_EFFECT_TIRES_SQUEALING);
          }
        }
      }
      break;

    case SW_POP_BUMPER:
      LastTimePopHit = CurrentTime;
      CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
      PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_1);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_BOTTOM_LEFT:
      CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
      if (LeftGateAvailableTime && (CurrentTime > LeftGateAvailableTime)) {
        PlaySoundEffect(SOUND_EFFECT_CELL_DOOR);
        LeftGateCloseTime = CurrentTime + LEFT_GATE_OPEN_TIME_MS;
        LeftGateAvailableTime = 0;
        RPU_PushToSolenoidStack(SOL_LEFT_GATE_OPEN, 20);
      }
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_CAPTIVE_BALL:
      if (CurrentTime>(LastCaptiveBallHit+500)) {
        LastCaptiveBallHit = CurrentTime;
        CurrentScores[CurrentPlayer] += 10000 * PlayfieldMultiplier;
        HandleCaptiveBallHit();
      }
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_LEFT_INLANE:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 100;
      PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_3);
      AddToBonus(1);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_INLANE:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 100;
      PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_3);
      AddToBonus(1);
      LeftSpinnerLitTime = CurrentTime;
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_LEFT_OUTLANE:
      if (BallSaveEndTime) BallSaveEndTime += 4000;
      PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_4);
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 1000;
      AddToBonus(1);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_OUTLANE:
      if (BallSaveEndTime) BallSaveEndTime += 4000;
      PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_4);
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 1000;
      AddToBonus(1);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_FLIP_LANE:
      if (GameMode==GAME_MODE_MULTIBALL_3) {
        Multiball3StartShotHit = true;
      } else if (GameMode==GAME_MODE_MINIMODE) {
        byte soundToPlay = RightFlipsSeen;
        if (soundToPlay>2) soundToPlay = 2;
        PlaySoundEffect(SOUND_EFFECT_BAND_HIT_1 + soundToPlay);
        RightFlipsSeen += 1;
        if (!AddABallUsed) {
          if (RightFlipsSeen==RightFlipsForAddABall) {
            AddABallUsed = true;
            AddABall(true);
          }
        }
      } else {
        PlaySoundEffect(SOUND_EFFECT_LOOP_1 + CurrentTime%2);
        CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 1000;
      }
      AddToBonus(1);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;    

    case SW_LEFT_SLING:
    case SW_RIGHT_SLING:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 10;
      PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_5);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_DROP_L_1:
    case SW_DROP_L_2:
    case SW_DROP_L_3:
      HandleLeftDropTarget(switchHit);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_DROP_R_1:
    case SW_DROP_R_2:
    case SW_DROP_R_3:
      HandleRightDropTarget(switchHit);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_LEFT_SPINNER:
      HandleSpinnerProgress(true);
      if (LeftSpinnerLitTime) PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      else PlaySoundEffect(SOUND_EFFECT_SPINNER_UNLIT_1);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_SPINNER:
      HandleSpinnerProgress(false);
      PlaySoundEffect(SOUND_EFFECT_SPINNER_UNLIT_2);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_STANDUP_J:
    case SW_STANDUP_A:
    case SW_STANDUP_K:
    case SW_STANDUP_E:
      HandleJakeStandup(switchHit);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_VUK:
      // VUK is now handled by debouncer
      // CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 5000;
      // PlaySoundEffect(SOUND_EFFECT_SAUCER);
      // RPU_PushToTimedSolenoidStack(SOL_VUK, VUK_SOLENOID_STRENGTH, CurrentTime+750, true);
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_TRAP:
      if (MachineLocks==0) {
        if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_AVAILABLE_MASK)==0 ) {
          // A ball fell in the trap when the player doesn't have a lock
          // available -- need to eject the trap
          ReleaseLiftGate();
        } else {
          // This is the first ball in the trap
          if (LockHandling==LOCK_HANDLING_VIRTUAL_LOCKS) {
            // For virtual locks, they always appear here, so we decide
            // based on the player locks
            if (PlayerLocks[CurrentPlayer] & BALL_1_LOCKED) {
              SetGameMode(GAME_MODE_LOCK_BALL_2);
            } else {
              SetGameMode(GAME_MODE_LOCK_BALL_1);
            }
          } else {
            SetGameMode(GAME_MODE_LOCK_BALL_1);
          }
        }
      } else {
        // We will see this lock when the trap switch is closed
        // for an extended period
      }

      // Close the top gate (hide away)
      TopGateCloseTime = 0;
      RPU_PushToTimedSolenoidStack(SOL_TOP_GATE_CLOSE, 10, CurrentTime + 500, true);
      PlaySoundEffect(SOUND_EFFECT_TOP_GATE_CLOSING);
      if (TopGateAvailableTime==0) TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;      
      if (BallLaunched) ValidateAndRegisterPlayfieldSwitch();
      break;

  }

}


int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long scoreAtTop = CurrentScores[CurrentPlayer];

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = InitGamePlay(curStateChanged);
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = InitNewBall(curStateChanged);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    UpdateDisplays(true);
    returnState = CountdownBonus(curStateChanged);
    ShowPlayerScores(0xFF, false, false);
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    RPU_SetDisplayCredits(Credits, !FreePlayMode);

    if (SamePlayerShootsAgain) {
      QueueNotification(SOUND_EFFECT_VP_SHOOT_AGAIN, 10);
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {

      CurrentPlayer += 1;
      if (CurrentPlayer >= CurrentNumPlayers) {
        CurrentPlayer = 0;
        CurrentBallInPlay += 1;
        RPU_SetLampState(LAMP_START_BUTTON, 0);
      }

      scoreAtTop = CurrentScores[CurrentPlayer];

      if (CurrentBallInPlay > BallsPerGame) {
        CheckHighScores();
        PlaySoundEffect(SOUND_EFFECT_GAME_OVER);
        for (int count = 0; count < CurrentNumPlayers; count++) {
          RPU_SetDisplay(count, CurrentScores[count], true, 2);
        }

        for (byte count=0; count<4; count++) {
          RPU_SetLampState(PlayerUpLamps[count], 0);
        }

        returnState = MACHINE_STATE_MATCH_MODE;
      }
      else returnState = MACHINE_STATE_INIT_NEW_BALL;
    }
  } else if (curState == MACHINE_STATE_MATCH_MODE) {
    returnState = ShowMatchSequence(curStateChanged);
  }

  byte switchHit;
  unsigned long lastBallFirstSwitchHitTime = BallFirstSwitchHitTime;

  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    returnState = HandleSystemSwitches(curState, switchHit);
    if (NumTiltWarnings <= MaxTiltWarnings) HandleGamePlaySwitches(switchHit);
  }

  if (CreditResetPressStarted) {
    if (CurrentBallInPlay < 2) {
      // If we haven't finished the first ball, we can add players
      AddPlayer();
      if (DEBUG_MESSAGES) {
        Serial.write("Start game button pressed\n\r");
      }
      CreditResetPressStarted = 0;
    } else {
      if (RPU_ReadSingleSwitchState(SW_CREDIT_RESET)) {
        if (TimeRequiredToResetGame != 99 && (CurrentTime - CreditResetPressStarted) >= ((unsigned long)TimeRequiredToResetGame * 1000)) {
          // If the first ball is over, pressing start again resets the game
          if (Credits >= 1 || FreePlayMode) {
            if (!FreePlayMode) {
              Credits -= 1;
              RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
              RPU_SetDisplayCredits(Credits, !FreePlayMode);
            }
            returnState = MACHINE_STATE_INIT_GAMEPLAY;
            CreditResetPressStarted = 0;
          }
        }
      } else {
        CreditResetPressStarted = 0;
      }
    }

  }

  if (lastBallFirstSwitchHitTime == 0 && BallFirstSwitchHitTime != 0) {
    BallSaveEndTime = BallFirstSwitchHitTime + ((unsigned long)BallSaveNumSeconds) * 1000;
  }
  if (CurrentTime > (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
    BallSaveEndTime = 0;
  }

  if (!ScrollingScores && CurrentScores[CurrentPlayer] > RPU_OS_MAX_DISPLAY_SCORE) {
    CurrentScores[CurrentPlayer] -= RPU_OS_MAX_DISPLAY_SCORE;
    if (!TournamentScoring) AddSpecialCredit();
  }

  if (scoreAtTop != CurrentScores[CurrentPlayer]) {
    SetLastTimeScoreChanged(CurrentTime);
    if (!TournamentScoring) {
      for (int awardCount = 0; awardCount < 3; awardCount++) {
        if (AwardScores[awardCount] != 0 && scoreAtTop < AwardScores[awardCount] && CurrentScores[CurrentPlayer] >= AwardScores[awardCount]) {
          // Player has just passed an award score, so we need to award it
          if (((ScoreAwardReplay >> awardCount) & 0x01)) {
            AddSpecialCredit();
          } else if (!ExtraBallCollected) {
            AwardExtraBall();
          }
        }
      }
    }

  }

  return returnState;
}


#if (RPU_MPU_ARCHITECTURE>=10)
unsigned long LastLEDUpdateTime = 0;
byte LEDPhase = 0;
#endif
//unsigned long NumLoops = 0;
//unsigned long LastLoopReportTime = 0;

void loop() {

  /*
    if (DEBUG_MESSAGES) {
      NumLoops += 1;
      if (CurrentTime>(LastLoopReportTime+1000)) {
        LastLoopReportTime = CurrentTime;
        char buf[128];
        sprintf(buf, "Loop running at %lu Hz\n", NumLoops);
        Serial.write(buf);
        NumLoops = 0;
      }
    }
  */

  CurrentTime = millis();
  int newMachineState = MachineState;

  if (MachineState < 0) {
    newMachineState = RunSelfTest(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractMode(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_DIAGNOSTICS) {
    newMachineState = RunDiagnosticsMode(MachineState, MachineStateChanged);
  } else {
    newMachineState = RunGamePlayMode(MachineState, MachineStateChanged);
  }

  if (newMachineState != MachineState) {
    MachineState = newMachineState;
    MachineStateChanged = true;
  } else {
    MachineStateChanged = false;
  }

  RPU_Update(CurrentTime);
  Audio.Update(CurrentTime);

#if (RPU_MPU_ARCHITECTURE>=10)
  if (LastLEDUpdateTime == 0 || (CurrentTime - LastLEDUpdateTime) > 250) {
    LastLEDUpdateTime = CurrentTime;
    RPU_SetBoardLEDs((LEDPhase % 8) == 1 || (LEDPhase % 8) == 3, (LEDPhase % 8) == 5 || (LEDPhase % 8) == 7);
    LEDPhase += 1;
  }
#endif

}
