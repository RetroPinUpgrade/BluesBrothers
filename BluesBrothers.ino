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
#include "OperatorMenus.h"
#include "AudioHandler.h"
#include "DisplayHandler.h"
#include "LampAnimations.h"
#include <EEPROM.h>

#define GAME_MAJOR_VERSION  2024
#define GAME_MINOR_VERSION  20
#define DEBUG_MESSAGES  1

#if (DEBUG_MESSAGES==1)
//#define DEBUG_SHOW_LOOPS_PER_SECOND
#endif

/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - no longer used
//  positive - game play
boolean InOperatorMenu = false;
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

#define MACHINE_STATE_ADJUST_FREEPLAY                         (MACHINE_STATE_TEST_DONE-1)
#define MACHINE_STATE_ADJUST_BALL_SAVE                        (MACHINE_STATE_TEST_DONE-2)
#define MACHINE_STATE_ADJUST_SOUND_SELECTOR                   (MACHINE_STATE_TEST_DONE-3)
#define MACHINE_STATE_ADJUST_MUSIC_VOLUME                     (MACHINE_STATE_TEST_DONE-4)
#define MACHINE_STATE_ADJUST_SFX_VOLUME                       (MACHINE_STATE_TEST_DONE-5)
#define MACHINE_STATE_ADJUST_CALLOUTS_VOLUME                  (MACHINE_STATE_TEST_DONE-6)
#define MACHINE_STATE_ADJUST_TOURNAMENT_SCORING               (MACHINE_STATE_TEST_DONE-7)
#define MACHINE_STATE_ADJUST_TILT_WARNING                     (MACHINE_STATE_TEST_DONE-8)
#define MACHINE_STATE_ADJUST_AWARD_OVERRIDE                   (MACHINE_STATE_TEST_DONE-9)
#define MACHINE_STATE_ADJUST_BALLS_OVERRIDE                   (MACHINE_STATE_TEST_DONE-10)
#define MACHINE_STATE_ADJUST_SCROLLING_SCORES                 (MACHINE_STATE_TEST_DONE-11)
#define MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD                 (MACHINE_STATE_TEST_DONE-12)
#define MACHINE_STATE_ADJUST_SPECIAL_AWARD                    (MACHINE_STATE_TEST_DONE-13)
#define MACHINE_STATE_ADJUST_CREDIT_RESET_HOLD_TIME           (MACHINE_STATE_TEST_DONE-14)
#define MACHINE_STATE_ADJUST_DONE                             (MACHINE_STATE_TEST_DONE-15)


// Indices of EEPROM save locations
#define EEPROM_RPOS_INIT_PROOF_UL                 90
#define RPOS_INIT_PROOF                           0x52503031
#define EEPROM_BALL_SAVE_BYTE                     100
#define EEPROM_FREE_PLAY_BYTE                     101
#define EEPROM_TILT_WARNING_BYTE                  104
#define EEPROM_AWARD_OVERRIDE_BYTE                105
#define EEPROM_BALLS_OVERRIDE_BYTE                106
#define EEPROM_TOURNAMENT_SCORING_BYTE            107
#define EEPROM_SFX_VOLUME_BYTE                    108
#define EEPROM_MUSIC_VOLUME_BYTE                  109
#define EEPROM_SCROLLING_SCORES_BYTE              110
#define EEPROM_CALLOUTS_VOLUME_BYTE               111
#define EEPROM_LOCK_BEHAVIOR_BYTE                 112
#define EEPROM_ELWOOD_CLEARS_TO_QUALIFY_BYTE      113
#define EEPROM_ONE_DROP_BANK_FIRST_CLEAR_BYTE     114
#define EEPROM_JAKE_CLEARS_TO_QUALIFY_BYTE        115
#define EEPROM_ALT_SPINNER_ACCELERATOR_BYTE       116
#define EEPROM_FLIPS_FOR_ADDABALL_BYTE            117
#define EEPROM_CRB_HOLD_TIME                      118
#define EEPROM_RESET_JAKE_PROGRESS_EACH_BALL      119
#define EEPROM_RESET_ELWOOD_PROGRESS_EACH_BALL    120
#define EEPROM_RESET_BAND_PROGRESS_EACH_BALL      121
#define EEPROM_RESET_QUALIFIED_MODES_EACH_BALL    122
#define EEPROM_OUTLANE_BALL_SAVES                 123
#define EEPROM_BONUS_COLLECT_HURRY_UP_SECS        124
#define EEPROM_SECONDS_UNTIL_STATUS               125
#define EEPROM_BALL_SAVE_ON_MINIMODES             126
#define EEPROM_BALL_SAVE_ON_MULTIBALL             127
#define EEPROM_BALL_SAVE_ON_ADD_A_BALL            128
#define EEPROM_BALL_SAVE_ON_MINI_WIZARD           129
#define EEPROM_BALL_SAVE_ON_WIZARD                130
#define EEPROM_TROUGH_EJECT_STRENGTH              131
#define EEPROM_PLUNGER_FULL_STRENGTH              132
#define EEPROM_PLUNGER_SOFT_STRENGTH              133
#define EEPROM_SAUCER_EJECT_STRENGTH              134
#define EEPROM_SLINGSHOT_STRENGTH                 135 
#define EEPROM_POP_BUMPER_STRENGTH                136
#define EEPROM_GAME_RULES_SELECTION               137
#define EEPROM_MATCH_FEATURE_BYTE                 138
#define EEPROM_EXTRA_BALL_SCORE_UL                160
#define EEPROM_SPECIAL_SCORE_UL                   164


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
#define GAME_MODE_MINI_WIZARD_READY                 30
#define GAME_MODE_MINI_WIZARD_START                 31
#define GAME_MODE_MINI_WIZARD                       32
#define GAME_MODE_MINI_WIZARD_END                   33
#define GAME_MODE_FULL_WIZARD_READY                 40
#define GAME_MODE_FULL_WIZARD_START                 41
#define GAME_MODE_FULL_WIZARD                       42
#define GAME_MODE_FULL_WIZARD_END                   43



#define SOUND_EFFECT_NONE                     0
#define SOUND_EFFECT_BONUS_COUNT              1
#define SOUND_EFFECT_DROP_TARGET_COMPLETE     2
#define SOUND_EFFECT_TILT                     3
#define SOUND_EFFECT_TILT_WARNING             4
#define SOUND_EFFECT_DOOR_SLAM                5
#define SOUND_EFFECT_10_SECOND_WARNING        6
#define SOUND_EFFECT_SPINNER_UNLIT_1          7
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
#define SOUND_EFFECT_MATCH_SPIN               28
#define SOUND_EFFECT_SPINNER_UNLIT_2          31
#define SOUND_EFFECT_BIG_HIT_1                32
#define SOUND_EFFECT_BIG_HIT_2                33
#define SOUND_EFFECT_BIG_HIT_3                34
#define SOUND_EFFECT_BIG_HIT_4                35
#define SOUND_EFFECT_RICOCHET_0               40
#define SOUND_EFFECT_RICOCHET_1               41
#define SOUND_EFFECT_RICOCHET_2               42
#define SOUND_EFFECT_RICOCHET_3               43
#define SOUND_EFFECT_RICOCHET_4               44
#define SOUND_EFFECT_RICOCHET_5               45
#define SOUND_EFFECT_RICOCHET_6               46
#define SOUND_EFFECT_RICOCHET_7               47
#define SOUND_EFFECT_BONUS_PAYOUT_START       48
#define SOUND_EFFECT_BONUS_PAYOUT             49
#define SOUND_EFFECT_ELECTRIC_PIANO_1         50
#define SOUND_EFFECT_ELECTRIC_PIANO_2         51
#define SOUND_EFFECT_ELECTRIC_PIANO_3         52
#define SOUND_EFFECT_ELECTRIC_PIANO_4         53
#define SOUND_EFFECT_ELECTRIC_PIANO_5         54
#define SOUND_EFFECT_RIDE_CYMBAL_1            55
#define SOUND_EFFECT_RIDE_CYMBAL_2            56
#define SOUND_EFFECT_FLOOR_TOM_1              57
#define SOUND_EFFECT_SNARE_1                  58
#define SOUND_EFFECT_SNARE_AND_CYMBAL         59
#define SOUND_EFFECT_SHORT_FILL_1             60
#define SOUND_EFFECT_COWBELL                  61
#define SOUND_EFFECT_CRASH_PING               62
#define SOUND_EFFECT_MID_TOM_1                63
#define SOUND_EFFECT_MID_TOM_2                64
#define SOUND_EFFECT_STICK_CLICK              65
#define SOUND_EFFECT_SHORT_FILL_2             66
#define SOUND_EFFECT_SCORE_TICK               67
#define SOUND_EFFECT_MEDIUM_FILL_1            68
#define SOUND_EFFECT_MEDIUM_FILL_2            69
#define SOUND_EFFECT_MINI_WIZ_SCORE_1         70
#define SOUND_EFFECT_MINI_WIZ_SCORE_2         71
#define SOUND_EFFECT_MINI_WIZ_SCORE_3         72
#define SOUND_EFFECT_RIM_SHOT                 73
#define SOUND_EFFECT_KICK_DRUM                74
#define SOUND_EFFECT_TOM_SHOT_1               75
#define SOUND_EFFECT_RIM_SHOT_2               76
#define SOUND_EFFECT_RIM_SHOT_3               77
#define SOUND_EFFECT_KICK_SNARE               78
#define SOUND_EFFECT_KICK_SNARE_MUTED_CRASH   79
#define SOUND_EFFECT_BONUS_1                  81
#define SOUND_EFFECT_BONUS_2                  82
#define SOUND_EFFECT_BONUS_3                  83
#define SOUND_EFFECT_BONUS_4                  84
#define SOUND_EFFECT_BONUS_5                  85
#define SOUND_EFFECT_BONUS_6                  86
#define SOUND_EFFECT_BONUS_7                  87
#define SOUND_EFFECT_BONUS_8                  88
#define SOUND_EFFECT_BONUS_9                  89
#define SOUND_EFFECT_BONUS_10                 90
#define SOUND_EFFECT_BONUS_11                 91
#define SOUND_EFFECT_BONUS_12                 92
#define SOUND_EFFECT_BONUS_13                 93
#define SOUND_EFFECT_BONUS_14                 94
#define SOUND_EFFECT_BONUS_15                 95
#define SOUND_EFFECT_BONUS_16                 96
#define SOUND_EFFECT_BONUS_17                 97
#define SOUND_EFFECT_STARTUP_1                100
#define SOUND_EFFECT_STARTUP_2                101


#define SOUND_EFFECT_COIN_DROP_1        100
#define SOUND_EFFECT_COIN_DROP_2        101
#define SOUND_EFFECT_COIN_DROP_3        102
#define SOUND_EFFECT_MACHINE_START      120

#define SOUND_EFFECT_BACKGROUND_SONG_1                400
#define SOUND_EFFECT_BACKGROUND_SONG_2                401
#define SOUND_EFFECT_BACKGROUND_SONG_3                402
#define SOUND_EFFECT_BACKGROUND_SONG_4                403
#define SOUND_EFFECT_BACKGROUND_SONG_5                404
#define SOUND_EFFECT_BACKGROUND_SONG_6                405
#define SOUND_EFFECT_BACKGROUND_SONG_7                406
#define SOUND_EFFECT_MINIMODE_SONG_1                  410
#define SOUND_EFFECT_MINIMODE_SONG_2                  411
#define SOUND_EFFECT_MINIMODE_SONG_3                  412
#define SOUND_EFFECT_MULTIBALL_SONG_1                 420
#define SOUND_EFFECT_MULTIBALL_SONG_2                 421
#define SOUND_EFFECT_MULTIBALL_SONG_3                 422
#define SOUND_EFFECT_MINI_WIZ_START_SONG              424
#define SOUND_EFFECT_MINI_WIZ_MUSIC_1                 425
#define SOUND_EFFECT_MINI_WIZ_MUSIC_2                 426
#define SOUND_EFFECT_MINI_WIZ_MUSIC_3                 427
#define SOUND_EFFECT_FULL_WIZ_MUSIC_1                 430
#define SOUND_EFFECT_FULL_WIZ_MUSIC_2                 431
#define SOUND_EFFECT_FULL_WIZ_MUSIC_3                 432
#define SOUND_EFFECT_RALLY_MUSIC_1                    450
#define SOUND_EFFECT_RALLY_MUSIC_2                    451
#define SOUND_EFFECT_RALLY_MUSIC_3                    452
#define SOUND_EFFECT_RALLY_MUSIC_4                    453
#define SOUND_EFFECT_RALLY_MUSIC_5                    454
#define SOUND_EFFECT_INTRO_LOOP                       460


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
#define SOUND_EFFECT_VP_JACKPOT_1                       327
#define SOUND_EFFECT_VP_JACKPOT_2                       328
#define SOUND_EFFECT_VP_JACKPOT_3                       329
#define SOUND_EFFECT_VP_SUPER_JACKPOT                   330
#define SOUND_EFFECT_VP_MEGA_JACKPOT                    331
#define SOUND_EFFECT_VP_HIDEAWAY_FOR_SUPER_JACKPOT      332
#define SOUND_EFFECT_VP_LEFT_FLIPPER_FOR_STATUS         333
#define SOUND_EFFECT_VP_RIGHT_FLIPPER_FOR_STATUS        334
#define SOUND_EFFECT_VP_FINISH_ELWOOD                   335
#define SOUND_EFFECT_VP_FINISH_JAKE                     336
#define SOUND_EFFECT_VP_ONCE_MORE                       337
#define SOUND_EFFECT_VP_TWICE_MORE                      338
#define SOUND_EFFECT_VP_THREE_MORE_TIMES                339
#define SOUND_EFFECT_VP_FOUR_MORE_TIMES                 340
#define SOUND_EFFECT_VP_FIVE_MORE_TIMES                 341
#define SOUND_EFFECT_VP_SPINS_NEEDED                    342
#define SOUND_EFFECT_VP_TO_QUALIFY_MODE                 343
#define SOUND_EFFECT_VP_ELWOOD_SHOTS_COLLECTED          344
#define SOUND_EFFECT_VP_JAKE_SHOTS_COLLECTED            345
#define SOUND_EFFECT_VP_BAND_SHOTS_COLLECTED            346
#define SOUND_EFFECT_VP_RAWHIDE_JACKPOTS_COLLECTED      347
#define SOUND_EFFECT_VP_CONCERT_JACKPOTS_COLLECTED      348
#define SOUND_EFFECT_VP_JOLIET_JACKPOTS_COLLECTED       349
#define SOUND_EFFECT_VP_ONE_TILT_WARNING_LEFT           350
#define SOUND_EFFECT_VP_TWO_TILT_WARNINGS_LEFT          351
#define SOUND_EFFECT_VP_THREE_TILT_WARNINGS_LEFT        352
#define SOUND_EFFECT_VP_YOUR_SCORE                      353
#define SOUND_EFFECT_VP_PLAYER_ONE_SCORE                354
#define SOUND_EFFECT_VP_PLAYER_TWO_SCORE                355
#define SOUND_EFFECT_VP_PLAYER_THREE_SCORE              356
#define SOUND_EFFECT_VP_PLAYER_FOUR_SCORE               357
#define SOUND_EFFECT_VP_NO_LOCKS_NONE_QUALIFIED         358
#define SOUND_EFFECT_VP_NO_LOCKS_ONE_QUALIFIED          359
#define SOUND_EFFECT_VP_NO_LOCKS_TWO_QUALIFIED          360
#define SOUND_EFFECT_VP_ONE_LOCK_NONE_QUALIFIED         361
#define SOUND_EFFECT_VP_ONE_LOCK_ONE_QUALIFIED          362
#define SOUND_EFFECT_VP_TWO_LOCKS                       363
#define SOUND_EFFECT_VP_ELWOOD_MODE_QUALIFIED           364
#define SOUND_EFFECT_VP_JAKE_MODE_QUALIFIED             365
#define SOUND_EFFECT_VP_BAND_MODE_QUALIFIED             366
#define SOUND_EFFECT_VP_ELWOOD_MODE_RUNNING             367
#define SOUND_EFFECT_VP_JAKE_MODE_RUNNING               368
#define SOUND_EFFECT_VP_BAND_MODE_RUNNING               369
#define SOUND_EFFECT_VP_CONCERT_MULTIBALL_RUNNING       370
#define SOUND_EFFECT_VP_RAWHIDE_MULTIBALL_RUNNING       371
#define SOUND_EFFECT_VP_JOLIET_MULTIBALL_RUNNING        372
#define SOUND_EFFECT_VP_TOTAL_BONUS                     373
#define SOUND_EFFECT_VP_NUMBER_OF_RIGHT_FLIPS           374
#define SOUND_EFFECT_VP_BONUS_1X                        375
#define SOUND_EFFECT_VP_MINI_WIZARD_INTRO               376
#define SOUND_EFFECT_VP_MINI_WIZARD_START               377
#define SOUND_EFFECT_VP_CONCERT_1_FINISHED              378
#define SOUND_EFFECT_VP_CONCERT_2_FINISHED              379
#define SOUND_EFFECT_VP_CONCERT_3_FINISHED              380
#define SOUND_EFFECT_VP_MINI_WIZARD_HALF_JACKPOT        381
#define SOUND_EFFECT_VP_ONE_SECOND_ADDED_TO_LEFT        382
#define SOUND_EFFECT_VP_ONE_SECOND_ADDED_TO_RIGHT       383
#define SOUND_EFFECT_VP_BONUS_COLLECT                   384
#define SOUND_EFFECT_VP_HIT_FLIP_FOR_BONUS              385
#define SOUND_EFFECT_VP_LAUNCH_BUTTON_TO_SAVE           386
#define SOUND_EFFECT_VP_FULL_WIZARD_INTRO               387


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

//#define SAUCER_SOLENOID_STRENGTH            10
//#define SHOOTER_KICK_FULL_STRENGTH          4 
//#define SHOOTER_KICK_LIGHT_STRENGTH         2
//#define BALL_SERVE_SOLENOID_STRENGTH        50
byte SaucerSolenoidStrength = 10;
byte ShooterKickFullStrength = 4;
byte ShooterKickLightStrength = 2;
byte BallServeSolenoidStrength = 50;
#define LEFT_GATE_CLOSE_SOLENOID_STRENGTH   5
#define LEFT_GATE_OPEN_SOLENOID_STRENGTH    5
#define TOP_GATE_OPEN_SOLENOID_STRENGTH     5
#define TOP_GATE_CLOSE_SOLENOID_STRENGTH    5
#define LIFT_GATE_SOLENOID_STRENGTH         7
#define KNOCKER_SOLENOID_STRENGTH           4
#define DROP_TARGET_RESET_STRENGTH          6


#define BALL_SAVE_GRACE_PERIOD            3000
#define BALL_SEARCH_INACTIVITY_DELAY      15000
#define BALL_SEARCH_NUMBER_OF_SOLENOIDS   12
byte BallSearchLastSolenoidFired = 0xFF;
byte BallSearchSolenoid[BALL_SEARCH_NUMBER_OF_SOLENOIDS] = {
  SOL_TOP_GATE_OPEN, SOL_TOP_GATE_CLOSE, SOL_POP_BUMPER, SOL_TOP_SLING,  
  SOL_LEFT_SLING, SOL_RIGHT_SLING, SOL_LEFT_GATE_OPEN, SOL_LEFT_GATE_CLOSE,
  SOL_DROP_BANK_R_RESET, SOL_DROP_BANK_L_RESET, SOL_SAUCER, SOL_LIFT_GATE
  };
byte BallSearchSolenoidStrength[BALL_SEARCH_NUMBER_OF_SOLENOIDS] = {
  TOP_GATE_OPEN_SOLENOID_STRENGTH, TOP_GATE_CLOSE_SOLENOID_STRENGTH, 4, 4,
  4, 4, LEFT_GATE_OPEN_SOLENOID_STRENGTH, LEFT_GATE_CLOSE_SOLENOID_STRENGTH,
  2, 2, 10, LIFT_GATE_SOLENOID_STRENGTH
};


/*********************************************************************

    Machine state and options

*********************************************************************/
byte Credits = 0;
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
boolean MatchEnabled = true;
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
unsigned long OperatorSwitchPressStarted = 0;
unsigned long BallRampKicked = 0;
unsigned long TrapSwitchClosedTime;

#define NUM_CPC_PAIRS 9
boolean CPCSelectionsHaveBeenRead = false;
byte CPCPairs[NUM_CPC_PAIRS][2] = {
  {1, 5},
  {1, 4},
  {1, 3},
  {1, 2},
  {1, 1},
  {2, 3},
  {2, 1},
  {3, 1},
  {4, 1}
};
byte CPCSelection[3];

AudioHandler  Audio;
OperatorMenus Menus;



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
byte NumberOfBallSavesRemaining;
byte CurrentStatusReportPage = 0;
byte LastStatusReportPage = 0;

#define STATUS_REPORT_NUM_PAGES 17

boolean SamePlayerShootsAgain = false;
boolean BallSaveUsed = false;
boolean SpecialAvailable = false;
boolean SpecialCollected = false;
boolean TimersPaused = true;
boolean AllowResetAfterBallOne = true;
boolean LeftFlipperBeingHeld;
boolean RightFlipperBeingHeld;

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
unsigned long LeftFlipperHoldTime;
unsigned long RightFlipperHoldTime;
unsigned long LastFlipperSeen;
unsigned long LastTimeSaucerSeen;
unsigned long WaitingForTrapSwitchToHold;


/*********************************************************************

    Game Specific State Variables

*********************************************************************/
int SpinnerProgress[RPU_NUMBER_OF_PLAYERS_ALLOWED];
int SpinnerHitsToQualifyBand = 500;

byte LastSpinnerSeen;
byte JakeStatus[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodStatus[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte JakeCompletions[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodCompletions[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte MiniModesRunning;
byte MiniModesQualified[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte RawhideJackpotsCollected[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ConcertJackpotsCollected[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte JolietJackpotsCollected[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte RightFlipLanes[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte BuildBonusLamps[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte BuildBonusLampsCompletions[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte LeftOutlaneSaveSeconds[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte RightOutlaneSaveSeconds[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodClearsToQualify = 2;
byte JakeClearsToQualify = 1;
byte BallSaveOnMinimodes = 5;
byte BallSaveOnMultiball = 5;
byte BallSaveOnAddABall = 30;
byte BallSaveOnMiniWizard = 60;
byte BallSaveOnWizard = 30;
byte BonusCollectHurryUp = 20;
byte RightFlipsForAddABall;
byte MachineLocks = 0;
byte PlayerLocks[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodTargetLit;
byte JakeTargetLit;
byte JakeShotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ElwoodShotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte BandShotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte ExtraBallAvailable[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte LockHandling;
byte RightFlipsSeen;
byte SkillshotLetter;
byte NumberOfSkillshotsMade[RPU_NUMBER_OF_PLAYERS_ALLOWED];
byte NumberOfShortPlunges;
byte MB3JackpotQualified;
byte MB3SuperJackpotQualified;
byte GameRulesSelection;
byte AlternatingSpinnerAccelerator;
byte StatusModeInactivityDelay = 10;

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

#define LOCK_HANDLING_LOCK_STEALING                 0
#define LOCK_HANDLING_LOCK_SHARING                  1
#define LOCK_HANDLING_PURGE_LOCKS                   2
#define LOCK_HANDLING_PURGE_LOCKS_WITH_RELOCKS      3
#define LOCK_HANDLING_VIRTUAL_LOCKS                 4

#define GAME_RULES_EASY         1
#define GAME_RULES_MEDIUM       2
#define GAME_RULES_HARD         3
#define GAME_RULES_PROGRESSIVE  4
#define GAME_RULES_CUSTOM       5

boolean WaitForPurgedLock = false;
boolean ResetJakeProgressEachBall = true;
boolean ResetElwoodProgressEachBall = true;
boolean ResetBandProgressEachBall = true;
boolean ResetQualifiedModesEachBall = false;
boolean OneBankFirstClear = true;
boolean OutlaneBallSaves = true;
boolean Multiball3StartShotHit;
boolean BallLaunched;
boolean AddABallUsed;
boolean TopGateCloseSoundPlayed;

unsigned long MiniModeTimeLeft = 0;
unsigned long MiniModeMultiplierExpiration = 0;
unsigned long LastJakeHit[4];
unsigned long PlayfieldMultiplierTimeLeft;
unsigned long BonusChanged;
unsigned long BonusXAnimationStart;
unsigned long LastSpinnerHitTime;
unsigned long LastShooterKickTime;
unsigned long LastTimeBallServed;
unsigned long LeftGateAvailableTime = 0;
unsigned long LeftGateCloseTime = 0;
unsigned long TopGateAvailableTime = 0;
unsigned long TopGateCloseTime = 0;
unsigned long LiftGateReleaseRequestTime;
unsigned long LiftGateLastReleaseTime;
unsigned long LastCaptiveBallHit;
unsigned long Multiball3StartAward;
unsigned long JackpotValue;
unsigned long LastTimeModeShotMissed = 0;
unsigned long LeftSpinnerLitTime;
unsigned long LastFlipLaneTime;
unsigned long LastDropTargetHitTime;
unsigned long CollectFlipBonusTimeout;
unsigned long NextBonusPayoutTime;
unsigned long LeftOutlaneSaveEndTime;
unsigned long RightOutlaneSaveEndTime;
unsigned long WizardShotsCompleted;
unsigned long WizardShotsRotateTime;
unsigned long BuildBonusLampsCompletionTime;

#define BUILD_BONUS_LAMP_LEFT_INLANE    0x01
#define BUILD_BONUS_LAMP_SAUCER         0x02
#define BUILD_BONUS_LAMP_CAPTIVE_BALL   0x04
#define BUILD_BONUS_LAMP_LOOP_LANE      0x08
#define BUILD_BONUS_LAMP_RIGHT_INLANE   0x10
#define BUILD_BONUS_LAMPS_COMPLETE      0x1F

#define WIZARD_SHOT_E1            0x0001
#define WIZARD_SHOT_L             0x0002
#define WIZARD_SHOT_W             0x0004
#define WIZARD_SHOT_SPINNER       0x0008
#define WIZARD_SHOT_O1            0x0010
#define WIZARD_SHOT_O2            0x0020
#define WIZARD_SHOT_D             0x0040
#define WIZARD_SHOT_J             0x0080
#define WIZARD_SHOT_A             0x0100
#define WIZARD_SHOT_K             0x0200
#define WIZARD_SHOT_E2            0x0400
#define WIZARD_SHOT_RIGHT_FLIP    0x0800
#define WIZARD_SHOT_SAVE_GATE     0x1000
#define WIZARD_SHOT_SAUCER        0x2000
#define WIZARD_SHOTS_COMPLETE     0x3FFF

#define LEFT_GATE_OPEN_TIME_MS  30000
#define LEFT_GATE_DELAY_TIME_MS 10000
#define TOP_GATE_OPEN_TIME_MS   30000
#define TOP_GATE_DELAY_TIME_MS  10000

#define MULTIBALL_3_JACKPOT_VALUE             50000
#define MULTIBALL_3_SUPER_JACKPOT_VALUE       75000
#define MULTIBALL_3_MEGA_JACKPOT_VALUE        100000

DropTargetBank LeftDropTargets(3, 1, DROP_TARGET_TYPE_BLY_1, DROP_TARGET_RESET_STRENGTH);
DropTargetBank RightDropTargets(3, 1, DROP_TARGET_TYPE_BLY_1, DROP_TARGET_RESET_STRENGTH);

byte PlayerUpLamps[4] = {LAMP_HEAD_PLAYER_1_UP, LAMP_HEAD_PLAYER_2_UP, LAMP_HEAD_PLAYER_3_UP, LAMP_HEAD_PLAYER_4_UP};



/******************************************************

   Adjustments Serialization

*/


void SetAllParameterDefaults() {
  HighScore = 10000;
  Credits = 4;
  FreePlayMode = false;
  BallSaveNumSeconds = 15;
  MusicVolume = 10;
  SoundEffectsVolume = 10;
  CalloutsVolume = 10;
  AwardScores[0] = 1000000;
  AwardScores[1] = 3000000;
  AwardScores[2] = 5000000;
  TournamentScoring = false;
  MaxTiltWarnings = 2;
  ScoreAwardReplay = 0x07;
  BallsPerGame = 3;
  ScrollingScores = true;
  MatchFeature = true;
  ExtraBallValue = 20000;
  SpecialValue = 40000;
  TimeRequiredToResetGame = 2;
  LockHandling = LOCK_HANDLING_LOCK_SHARING;
  StatusModeInactivityDelay = 10;
  SaucerSolenoidStrength = 10;
  ShooterKickFullStrength = 4;
  ShooterKickLightStrength = 2;
  BallServeSolenoidStrength = 50;
  CPCSelection[0] = 4;
  CPCSelection[1] = 4;
  CPCSelection[2] = 4;

  // EASY / MEDIUM / HARD rules
  GameRulesSelection = GAME_RULES_MEDIUM;
  ElwoodClearsToQualify = 2;
  JakeClearsToQualify = 1;
  AlternatingSpinnerAccelerator = 10;
  BallSaveOnMinimodes = 10;
  BallSaveOnMultiball = 10;
  BallSaveOnAddABall = 30;
  BallSaveOnMiniWizard = 30;
  BallSaveOnWizard = 30;
  RightFlipsForAddABall = 3;
  ResetJakeProgressEachBall = true;
  ResetElwoodProgressEachBall = true;
  ResetBandProgressEachBall = true;
  OneBankFirstClear = true;
  ResetQualifiedModesEachBall = false;
  OutlaneBallSaves = true;
  BonusCollectHurryUp = 20;
}


boolean LoadRuleDefaults(byte ruleLevel) {
  // This function just puts the rules in RAM.
  // It's up to the caller to ensure that they're 
  // written to EEPROM when desired (with WriteParameters)
  if (ruleLevel==GAME_RULES_EASY) {
    ElwoodClearsToQualify = 1;
    JakeClearsToQualify = 1;
    AlternatingSpinnerAccelerator = 25;
    BallSaveNumSeconds = 20;
    BallSaveOnMinimodes = 20;
    BallSaveOnMultiball = 35;
    BallSaveOnAddABall = 40;
    BallSaveOnMiniWizard = 60;
    BallSaveOnWizard = 60;
    RightFlipsForAddABall = 1;
    ResetJakeProgressEachBall = false;
    ResetElwoodProgressEachBall = false;
    ResetBandProgressEachBall = false;
    OneBankFirstClear = true;
    ResetQualifiedModesEachBall = false;
    OutlaneBallSaves = true;
    BonusCollectHurryUp = 60;
  } else if (ruleLevel==GAME_RULES_MEDIUM) {
    ElwoodClearsToQualify = 2;
    JakeClearsToQualify = 1;
    AlternatingSpinnerAccelerator = 10;
    BallSaveNumSeconds = 10;
    BallSaveOnMinimodes = 15;
    BallSaveOnMultiball = 15;
    BallSaveOnAddABall = 30;
    BallSaveOnMiniWizard = 20;
    BallSaveOnWizard = 30;
    RightFlipsForAddABall = 3;
    ResetJakeProgressEachBall = false;
    ResetElwoodProgressEachBall = true;
    ResetBandProgressEachBall = true;
    OneBankFirstClear = true;
    ResetQualifiedModesEachBall = false;
    OutlaneBallSaves = true;
    BonusCollectHurryUp = 30;
  } else if (ruleLevel==GAME_RULES_HARD) {
    ElwoodClearsToQualify = 3;
    JakeClearsToQualify = 2;
    AlternatingSpinnerAccelerator = 5;
    BallSaveNumSeconds = 0;
    BallSaveOnMinimodes = 5;
    BallSaveOnMultiball = 5;
    BallSaveOnAddABall = 10;
    BallSaveOnMiniWizard = 0;
    BallSaveOnWizard = 10;
    RightFlipsForAddABall = 4;
    ResetJakeProgressEachBall = true;
    ResetElwoodProgressEachBall = true;
    ResetBandProgressEachBall = true;
    OneBankFirstClear = false;
    ResetQualifiedModesEachBall = true;
    OutlaneBallSaves = false;
    BonusCollectHurryUp = 15;
  } else {
    return false;
  }

  return true;
}


void WriteParameters(boolean onlyWriteRulesParameters = true) {
  if (!onlyWriteRulesParameters) {
    RPU_WriteULToEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, HighScore);
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_WriteByteToEEProm(RPU_CPC_CHUTE_1_SELECTION_BYTE, CPCSelection[0]);
    RPU_WriteByteToEEProm(RPU_CPC_CHUTE_2_SELECTION_BYTE, CPCSelection[1]);
    RPU_WriteByteToEEProm(RPU_CPC_CHUTE_3_SELECTION_BYTE, CPCSelection[2]);
        
    RPU_WriteByteToEEProm(EEPROM_FREE_PLAY_BYTE, FreePlayMode);
    RPU_WriteByteToEEProm(EEPROM_MUSIC_VOLUME_BYTE, MusicVolume);
    RPU_WriteByteToEEProm(EEPROM_SFX_VOLUME_BYTE, SoundEffectsVolume);
    RPU_WriteByteToEEProm(EEPROM_CALLOUTS_VOLUME_BYTE, CalloutsVolume);

    RPU_WriteULToEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE, AwardScores[0]);
    RPU_WriteULToEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE, AwardScores[1]);
    RPU_WriteULToEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE, AwardScores[2]);

    RPU_WriteByteToEEProm(EEPROM_TOURNAMENT_SCORING_BYTE, TournamentScoring);
    RPU_WriteByteToEEProm(EEPROM_AWARD_OVERRIDE_BYTE, ScoreAwardReplay);
    RPU_WriteByteToEEProm(EEPROM_BALLS_OVERRIDE_BYTE, BallsPerGame);
    RPU_WriteByteToEEProm(EEPROM_SCROLLING_SCORES_BYTE, ScrollingScores);
    RPU_WriteByteToEEProm(EEPROM_MATCH_FEATURE_BYTE, MatchFeature);
    
    RPU_WriteULToEEProm(EEPROM_EXTRA_BALL_SCORE_UL, ExtraBallValue);
    RPU_WriteULToEEProm(EEPROM_SPECIAL_SCORE_UL, SpecialValue);
    RPU_WriteByteToEEProm(EEPROM_CRB_HOLD_TIME, TimeRequiredToResetGame);

    RPU_WriteByteToEEProm(EEPROM_SECONDS_UNTIL_STATUS, StatusModeInactivityDelay);
    RPU_WriteByteToEEProm(EEPROM_SAUCER_EJECT_STRENGTH, SaucerSolenoidStrength);
    RPU_WriteByteToEEProm(EEPROM_PLUNGER_FULL_STRENGTH, ShooterKickFullStrength);
    RPU_WriteByteToEEProm(EEPROM_PLUNGER_SOFT_STRENGTH, ShooterKickLightStrength);
    RPU_WriteByteToEEProm(EEPROM_TROUGH_EJECT_STRENGTH, BallServeSolenoidStrength);
    RPU_WriteByteToEEProm(EEPROM_SLINGSHOT_STRENGTH, SolenoidAssociatedSwitches[0].solenoidHoldTime);
    RPU_WriteByteToEEProm(EEPROM_POP_BUMPER_STRENGTH, SolenoidAssociatedSwitches[3].solenoidHoldTime);

    // Set baseline for audits
    RPU_WriteByteToEEProm(RPU_CHUTE_1_COINS_START_BYTE, 0);
    RPU_WriteByteToEEProm(RPU_CHUTE_2_COINS_START_BYTE, 0);
    RPU_WriteByteToEEProm(RPU_CHUTE_3_COINS_START_BYTE, 0);
    RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, 0);
    RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, 0);
    RPU_WriteULToEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE, 0);
  }

  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_BYTE, BallSaveNumSeconds);
  RPU_WriteByteToEEProm(EEPROM_TILT_WARNING_BYTE, MaxTiltWarnings);  
  RPU_WriteByteToEEProm(EEPROM_ALT_SPINNER_ACCELERATOR_BYTE, AlternatingSpinnerAccelerator);
  RPU_WriteByteToEEProm(EEPROM_LOCK_BEHAVIOR_BYTE, LockHandling);
  RPU_WriteByteToEEProm(EEPROM_ELWOOD_CLEARS_TO_QUALIFY_BYTE, ElwoodClearsToQualify);
  RPU_WriteByteToEEProm(EEPROM_JAKE_CLEARS_TO_QUALIFY_BYTE, JakeClearsToQualify);
  RPU_WriteByteToEEProm(EEPROM_FLIPS_FOR_ADDABALL_BYTE, RightFlipsForAddABall);
  RPU_WriteByteToEEProm(EEPROM_RESET_JAKE_PROGRESS_EACH_BALL, ResetJakeProgressEachBall);
  RPU_WriteByteToEEProm(EEPROM_RESET_ELWOOD_PROGRESS_EACH_BALL, ResetElwoodProgressEachBall);
  RPU_WriteByteToEEProm(EEPROM_ONE_DROP_BANK_FIRST_CLEAR_BYTE, OneBankFirstClear);
  RPU_WriteByteToEEProm(EEPROM_RESET_QUALIFIED_MODES_EACH_BALL, ResetQualifiedModesEachBall);
  RPU_WriteByteToEEProm(EEPROM_OUTLANE_BALL_SAVES, OutlaneBallSaves);
  RPU_WriteByteToEEProm(EEPROM_RESET_BAND_PROGRESS_EACH_BALL, ResetBandProgressEachBall);
  RPU_WriteByteToEEProm(EEPROM_BONUS_COLLECT_HURRY_UP_SECS, BonusCollectHurryUp);
  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_ON_MINIMODES, BallSaveOnMinimodes);
  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_ON_MULTIBALL, BallSaveOnMultiball);
  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_ON_ADD_A_BALL, BallSaveOnAddABall);
  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_ON_MINI_WIZARD, BallSaveOnMiniWizard);
  RPU_WriteByteToEEProm(EEPROM_BALL_SAVE_ON_WIZARD, BallSaveOnWizard);
}

void ReadStoredParameters() {
  for (byte count = 0; count < 3; count++) {
    ChuteCoinsInProgress[count] = 0;
  }

  unsigned long RPUProofValue = RPU_ReadULFromEEProm(EEPROM_RPOS_INIT_PROOF_UL, 0);
  if (RPUProofValue!=RPOS_INIT_PROOF) {
    // Doesn't look like this memory has been initialized
    RPU_WriteULToEEProm(EEPROM_RPOS_INIT_PROOF_UL, RPOS_INIT_PROOF);
    SetAllParameterDefaults();
    WriteParameters(false);
  } else {

    // Read machine settings
    HighScore = RPU_ReadULFromEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, 10000);
    Credits = RPU_ReadByteFromEEProm(RPU_CREDITS_EEPROM_BYTE);
    if (Credits > MaximumCredits) Credits = MaximumCredits;
  
    FreePlayMode = ReadSetting(EEPROM_FREE_PLAY_BYTE, false, true);
    MusicVolume = ReadSetting(EEPROM_MUSIC_VOLUME_BYTE, 10, 10);
    SoundEffectsVolume = ReadSetting(EEPROM_SFX_VOLUME_BYTE, 10, 10);
    CalloutsVolume = ReadSetting(EEPROM_CALLOUTS_VOLUME_BYTE, 10, 10);
    Audio.SetMusicVolume(MusicVolume);
    Audio.SetSoundFXVolume(SoundEffectsVolume);
    Audio.SetNotificationsVolume(CalloutsVolume);

    AwardScores[0] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE);
    AwardScores[1] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE);
    AwardScores[2] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE);
  
    TournamentScoring = ReadSetting(EEPROM_TOURNAMENT_SCORING_BYTE, false, true);
    ScoreAwardReplay = ReadSetting(EEPROM_AWARD_OVERRIDE_BYTE, 0x03, 0x07);
    BallsPerGame = ReadSetting(EEPROM_BALLS_OVERRIDE_BYTE, 3, 10);
    ScrollingScores = ReadSetting(EEPROM_SCROLLING_SCORES_BYTE, true, true);
    MatchFeature = ReadSetting(EEPROM_MATCH_FEATURE_BYTE, true, true);

    CPCSelection[0] = ReadSetting(RPU_CPC_CHUTE_1_SELECTION_BYTE, 4, 8);
    CPCSelection[1] = ReadSetting(RPU_CPC_CHUTE_2_SELECTION_BYTE, 4, 8);
    CPCSelection[2] = ReadSetting(RPU_CPC_CHUTE_3_SELECTION_BYTE, 4, 8);
    CPCSelectionsHaveBeenRead = true;

    ExtraBallValue = RPU_ReadULFromEEProm(EEPROM_EXTRA_BALL_SCORE_UL);
    if (ExtraBallValue % 1000 || ExtraBallValue > 1000000) ExtraBallValue = 20000;
  
    SpecialValue = RPU_ReadULFromEEProm(EEPROM_SPECIAL_SCORE_UL);
    if (SpecialValue % 1000 || SpecialValue > 1000000) SpecialValue = 40000;
  
    TimeRequiredToResetGame = ReadSetting(EEPROM_CRB_HOLD_TIME, 1, 99);
    if (TimeRequiredToResetGame > 3 && TimeRequiredToResetGame != 99) TimeRequiredToResetGame = 1;
    
    StatusModeInactivityDelay = ReadSetting(EEPROM_SECONDS_UNTIL_STATUS, 10, 99);
    SaucerSolenoidStrength = ReadSetting(EEPROM_SAUCER_EJECT_STRENGTH, 10, 15);
    ShooterKickFullStrength = ReadSetting(EEPROM_PLUNGER_FULL_STRENGTH, 4, 8);
    ShooterKickLightStrength = ReadSetting(EEPROM_PLUNGER_SOFT_STRENGTH, 2, 5);
    BallServeSolenoidStrength = ReadSetting(EEPROM_TROUGH_EJECT_STRENGTH, 50, 60);
    SolenoidAssociatedSwitches[0].solenoidHoldTime = ReadSetting(EEPROM_SLINGSHOT_STRENGTH, 4, 8); 
    SolenoidAssociatedSwitches[3].solenoidHoldTime = ReadSetting(EEPROM_POP_BUMPER_STRENGTH, 4, 8);

    // Read game rules
    GameRulesSelection = ReadSetting(EEPROM_GAME_RULES_SELECTION, GAME_RULES_MEDIUM, GAME_RULES_CUSTOM);

    BallSaveNumSeconds = ReadSetting(EEPROM_BALL_SAVE_BYTE, 15, 20);  
    MaxTiltWarnings = ReadSetting(EEPROM_TILT_WARNING_BYTE, 2, 3);
    
    AlternatingSpinnerAccelerator = ReadSetting(EEPROM_ALT_SPINNER_ACCELERATOR_BYTE, 10, 25);
    LockHandling = ReadSetting(EEPROM_LOCK_BEHAVIOR_BYTE, LOCK_HANDLING_LOCK_SHARING, LOCK_HANDLING_VIRTUAL_LOCKS);
    ElwoodClearsToQualify = ReadSetting(EEPROM_ELWOOD_CLEARS_TO_QUALIFY_BYTE, 2, 4);    
    JakeClearsToQualify = ReadSetting(EEPROM_JAKE_CLEARS_TO_QUALIFY_BYTE, 1, 3);
    RightFlipsForAddABall = ReadSetting(EEPROM_FLIPS_FOR_ADDABALL_BYTE, 3, 5);
    ResetJakeProgressEachBall = ReadSetting(EEPROM_RESET_JAKE_PROGRESS_EACH_BALL, false, true);
    ResetElwoodProgressEachBall = ReadSetting(EEPROM_RESET_ELWOOD_PROGRESS_EACH_BALL, false, true);
    OneBankFirstClear = ReadSetting(EEPROM_ONE_DROP_BANK_FIRST_CLEAR_BYTE, true, true);
    ResetQualifiedModesEachBall = ReadSetting(EEPROM_RESET_QUALIFIED_MODES_EACH_BALL, false, true);
    OutlaneBallSaves = ReadSetting(EEPROM_OUTLANE_BALL_SAVES, true, true);
    ResetBandProgressEachBall = ReadSetting(EEPROM_RESET_BAND_PROGRESS_EACH_BALL, true, true);
    BonusCollectHurryUp = ReadSetting(EEPROM_BONUS_COLLECT_HURRY_UP_SECS, 30, 60);
    BallSaveOnMinimodes = ReadSetting(EEPROM_BALL_SAVE_ON_MINIMODES, 10, 20);
    BallSaveOnMultiball = ReadSetting(EEPROM_BALL_SAVE_ON_MULTIBALL, 10, 35);
    BallSaveOnAddABall = ReadSetting(EEPROM_BALL_SAVE_ON_ADD_A_BALL, 30, 40);
    BallSaveOnMiniWizard = ReadSetting(EEPROM_BALL_SAVE_ON_MINI_WIZARD, 30, 60);
    BallSaveOnWizard = ReadSetting(EEPROM_BALL_SAVE_ON_WIZARD, 30, 60);
  }
}



byte GetCPCSelection(byte chuteNumber) {
  if (chuteNumber>2) return 0xFF;

  if (CPCSelectionsHaveBeenRead==false) {
    CPCSelection[0] = RPU_ReadByteFromEEProm(RPU_CPC_CHUTE_1_SELECTION_BYTE);
    if (CPCSelection[0]>=NUM_CPC_PAIRS) {
      CPCSelection[0] = 4;
      RPU_WriteByteToEEProm(RPU_CPC_CHUTE_1_SELECTION_BYTE, 4);
    }
    CPCSelection[1] = RPU_ReadByteFromEEProm(RPU_CPC_CHUTE_2_SELECTION_BYTE);  
    if (CPCSelection[1]>=NUM_CPC_PAIRS) {
      CPCSelection[1] = 4;
      RPU_WriteByteToEEProm(RPU_CPC_CHUTE_2_SELECTION_BYTE, 4);
    }
    CPCSelection[2] = RPU_ReadByteFromEEProm(RPU_CPC_CHUTE_3_SELECTION_BYTE);  
    if (CPCSelection[2]>=NUM_CPC_PAIRS) {
      CPCSelection[2] = 4;
      RPU_WriteByteToEEProm(RPU_CPC_CHUTE_3_SELECTION_BYTE, 4);
    }
    CPCSelectionsHaveBeenRead = true;
  }
  
  return CPCSelection[chuteNumber];
}


byte GetCPCCoins(byte cpcSelection) {
  if (cpcSelection>=NUM_CPC_PAIRS) return 1;
  return CPCPairs[cpcSelection][0];
}


byte GetCPCCredits(byte cpcSelection) {
  if (cpcSelection>=NUM_CPC_PAIRS) return 1;
  return CPCPairs[cpcSelection][1];
}


void QueueDIAGNotification(unsigned short notificationNum) {
  // This is optional, but the machine can play an audio message at boot
  // time to indicate any errors and whether it's going to boot to original
  // or new code.
  //Audio.QueuePrioritizedNotification(notificationNum, 0, 10, CurrentTime);
  (void)notificationNum;
}


// I'm doing this as a function instead of an array because 
// memory is short and spending 44 bytes on a converstion array
// seems wasteful when the board has tons and tons of code space.
// There's a way to store this data in code space and then convert
// it when needed, but that's slow compared to this (ugly) method.
byte LampConvertDisplayNumberToIndex(byte displayNumber) {
  switch(displayNumber) {
    case  0: return OPERATOR_MENU_VALUE_UNUSED;
    case  1: return 35;
    case  2: return  2;
    case  3: return 46;
    case  4: return  7;
    case  5: return 11;
    case  6: return 38;
    case  7: return 34;
    case  8: return 39;
    case  9: return 18;
    case 10: return  3;
    case 11: return 26;
    case 12: return 47;
    case 13: return 42;
    case 14: return 14;
    case 15: return 10;
    case 16: return  6;
    case 17: return 43;
    case 18: return 45;
    case 19: return 44;
    case 20: return 36;
    case 21: return  4;
    case 22: return  0;
    case 23: return 37;
    case 24: return  9;
    case 25: return 13;
    case 26: return 41;
    case 27: return 33;
    case 28: return  1;
    case 29: return  5;
    case 30: return 17;
    case 31: return 28;
    case 32: return 12;
    case 33: return 20;
    case 34: return  8;
    case 35: return 57;
    case 36: return 49;
    case 37: return 52;
    case 38: return 53;
    case 39: return 56;
    case 40: return 48;
    case 41: return 54;
    case 42: return 50;
    case 43: return 51;
    case 44: return 55;
    default: return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
  }
  return 0;
}



unsigned short SolenoidConvertDisplayNumberToIndex(byte displayNumber) {
  switch (displayNumber) {
    case  0: return OPERATOR_MENU_VALUE_UNUSED;
    case  1: return SOL_TOP_GATE_OPEN;
    case  2: return SOL_TOP_GATE_CLOSE;
    case  3: return SOL_LIFT_GATE;
    case  4: return SOL_POP_BUMPER;
    case  5: return SOL_DROP_BANK_R_RESET;
    case  6: return SOL_DROP_BANK_L_RESET;
    case  7: return SOL_KNOCKER;
    case  8: return SOL_SAUCER;
    case  9: return SOL_LEFT_SLING;
    case 10: return SOL_RIGHT_SLING;
    case 11: return SOL_TOP_SLING;
    case 12: return SOL_SHOOTER_KICK;
    case 13: return SOL_SERVE_BALL;
    case 14: return SOL_LEFT_GATE_OPEN;
    case 15: return 0x4000; // Flipper Mute
    case 16: return SOL_LEFT_GATE_CLOSE;
    case 17: return OPERATOR_MENU_VALUE_UNUSED; // unused 0x1000
    case 18: return OPERATOR_MENU_VALUE_UNUSED; // unused 0x8000
    case 19: return 0x2000; // Topper
    default: return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
  }
}


byte SolenoidConvertDisplayNumberToTestStrength(byte displayNumber) {
  switch (displayNumber) {
    case  0: return OPERATOR_MENU_VALUE_UNUSED;
    case  1: return TOP_GATE_OPEN_SOLENOID_STRENGTH;
    case  2: return TOP_GATE_CLOSE_SOLENOID_STRENGTH;
    case  3: return LIFT_GATE_SOLENOID_STRENGTH;
    case  4: return 4;
    case  5: return DROP_TARGET_RESET_STRENGTH;
    case  6: return DROP_TARGET_RESET_STRENGTH;
    case  7: return KNOCKER_SOLENOID_STRENGTH;
    case  8: return SaucerSolenoidStrength;
    case  9: return 4;
    case 10: return 4;
    case 11: return 4;
    case 12: return ShooterKickFullStrength;
    case 13: return BallServeSolenoidStrength;
    case 14: return LEFT_GATE_OPEN_SOLENOID_STRENGTH;
    case 15: return 10; // Flipper Mute
    case 16: return LEFT_GATE_CLOSE_SOLENOID_STRENGTH;
    case 17: return OPERATOR_MENU_VALUE_UNUSED; // unused 0x1000
    case 18: return OPERATOR_MENU_VALUE_UNUSED; // unused 0x8000
    case 19: return 30; // Topper
    default: return OPERATOR_MENU_VALUE_OUT_OF_RANGE;
  }
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
  RPU_SetContinuousSolenoidBit(true, 0x20);

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

  Audio.QueueSound(SOUND_EFFECT_STARTUP_1 + CurrentTime % 2, AUDIO_PLAY_TYPE_WAV_TRIGGER, CurrentTime + 1200);
  MachineLocks = 0;
  LiftGateLastReleaseTime = 0;
  LiftGateReleaseRequestTime = 0;
  OperatorSwitchPressStarted = 0;
  InOperatorMenu = false;
  Menus.SetNavigationButtons(SW_RIGHT_FLIPPER, SW_LEFT_FLIPPER, SW_CREDIT_RESET, SW_SELF_TEST_ON_MATRIX);
  Menus.SetLampsLookupCallback(LampConvertDisplayNumberToIndex);
  Menus.SetSolenoidIDLookupCallback(SolenoidConvertDisplayNumberToIndex);
  Menus.SetSolenoidStrengthLookupCallback(SolenoidConvertDisplayNumberToTestStrength);
}

byte ReadSetting(byte setting, byte defaultValue, byte maxValue) {
  byte value = EEPROM.read(setting);
  if (value == 0xFF || value>maxValue) {
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
  // Since this machine doesn't have GI control,
  // this line prevents compiler warnings.
  (void)setGIOn;
}

void ShowPlayerLamps() {
  if (CurrentStatusReportPage!=0 && CurrentStatusReportPage<5) return;
  
  for (byte count = 0; count < 4; count++) {
    if (count==CurrentPlayer) RPU_SetLampState(PlayerUpLamps[count], 1, 0, 250);
    else if (count<CurrentNumPlayers) RPU_SetLampState(PlayerUpLamps[count], 1);
    else RPU_SetLampState(PlayerUpLamps[count], 0);
  }  
}

byte BonusLampAssignments[6] = {LAMP_BONUS_1, LAMP_BONUS_2, LAMP_BONUS_3, LAMP_BONUS_4, LAMP_BONUS_5, LAMP_BONUS_6};

void ShowBonusLamps() {
  boolean lampShown[6] = {false};
  if (CurrentStatusReportPage==1 && CurrentTime < (LastSwitchHitTime + ((unsigned long)StatusModeInactivityDelay*1000) + 3000)) {
    byte lampPhase = ((CurrentTime - (LastSwitchHitTime + ((unsigned long)StatusModeInactivityDelay*1000)))/75) % 13;
    if (lampPhase>6) lampPhase = 12-lampPhase;
    for (byte count = 0; count<6; count++) {
      RPU_SetLampState(BonusLampAssignments[count], count==lampPhase);
    }
    RPU_SetLampState(LAMP_SPECIAL, (lampPhase==6)?true:false);
  } else if (GameMode == GAME_MODE_SKILL_SHOT) {
    RPU_SetLampState(LAMP_BONUS_1, 0);
    RPU_SetLampState(LAMP_BONUS_2, 0);
    RPU_SetLampState(LAMP_BONUS_3, 0);
    RPU_SetLampState(LAMP_BONUS_4, 0);
    RPU_SetLampState(LAMP_BONUS_5, 0);
    RPU_SetLampState(LAMP_BONUS_6, 0);
    RPU_SetLampState(LAMP_SPECIAL, 0);
  } else if (GameMode == GAME_MODE_MULTIBALL_3_START) {
    byte lampPhase = (CurrentTime / 150) % 8;
    RPU_SetLampState(LAMP_BONUS_1, lampPhase == 0);
    RPU_SetLampState(LAMP_BONUS_2, lampPhase == 1);
    RPU_SetLampState(LAMP_BONUS_3, lampPhase == 2);
    RPU_SetLampState(LAMP_BONUS_4, lampPhase == 3);
    RPU_SetLampState(LAMP_BONUS_5, lampPhase == 4);
    RPU_SetLampState(LAMP_BONUS_6, lampPhase == 5);
    RPU_SetLampState(LAMP_SPECIAL, lampPhase == 6);
  } else {
    RPU_SetLampState(LAMP_SPECIAL, SpecialAvailable, 0, 100);
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
  if (GameMode == GAME_MODE_MINIMODE) {
    byte lampPhase = (CurrentTime / 375) % 3;
    RPU_SetLampState(LAMP_POP_WHITE_DOWN, lampPhase == 0);
    RPU_SetLampState(LAMP_POP_SMALL_BLUE, lampPhase == 1);
    RPU_SetLampState(LAMP_POP_BLUE_UP, lampPhase == 2);

    lampPhase = (CurrentTime / 187) % 8;
    RPU_SetLampState(LAMP_LEFT_SLING, lampPhase < 2 || lampPhase == 4 || lampPhase == 6);
    RPU_SetLampState(LAMP_RIGHT_SLING, lampPhase == 2 || lampPhase == 3 || lampPhase == 5 || lampPhase == 7);
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
  
  if ( GameMode == GAME_MODE_MULTIBALL_3 && MB3JackpotQualified == 0 ) {
    byte jakeFlag = 0x01;
    for (byte count = 0; count < 4; count++) {
      RPU_SetLampState(JakeLampAssignments[count], (JakeStatus[CurrentPlayer] & jakeFlag) ? false : true, 0, 75);
      jakeFlag *= 2;
    }
  } else if ( GameMode == GAME_MODE_MINI_WIZARD ) {
    unsigned long jakeFlag = WIZARD_SHOT_J;
    for (byte count=0; count<4; count++) {
      if (WizardShotsCompleted & jakeFlag) {
        RPU_SetLampState(JakeLampAssignments[count], 1);
      } else {
        byte availPhase = (CurrentTime / 25) % 12;
        if ((availPhase==0 && (count%2)) || ((availPhase==6) && (count%2)==0)) RPU_SetLampState(JakeLampAssignments[count], 1);
        else RPU_SetLampState(JakeLampAssignments[count], 0);
      }
      jakeFlag *= 2;
    }
  } else if ((MiniModesRunning&JAKE_MINIMODE_FLAG)) {
    for (byte count = 0; count < 4; count++) {
      RPU_SetLampState(JakeLampAssignments[count], count == JakeTargetLit, 0, 25);
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
  if (GameMode == GAME_MODE_MULTIBALL_3_START) {
    byte lampPhase = (CurrentTime / 150) % 8;
    RPU_SetLampState(LAMP_LEFT_SPINNER, lampPhase == 7, 0, 25);
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 1, 0, 25);
    RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 1, 0, 25);
    RPU_SetLampState(LAMP_LOOP_BONUS, 1, 0, 25);
    RPU_SetLampState(LAMP_SAUCER, 0);
  } else if (GameMode == GAME_MODE_MULTIBALL_3) {
    if ((MiniModesRunning&BAND_MINIMODE_FLAG)) {
      RPU_SetLampState(LAMP_LEFT_SPINNER, 1, 0, 25);
    } else {
      RPU_SetLampState(LAMP_LEFT_SPINNER, LeftSpinnerLitTime ? true : false, 0, 150);
    }
    boolean hideAwayOn = false;
    if (MB3SuperJackpotQualified>1 && CurrentTime<TopGateCloseTime) hideAwayOn = true;
    RPU_SetLampState(LAMP_HIDE_AWAY, hideAwayOn, 0, 125);

    if (MB3SuperJackpotQualified==1 && TopGateAvailableTime && CurrentTime > TopGateAvailableTime && (LeftDropTargets.GetStatus() & 0x02)) {
      RPU_SetLampState(LAMP_CAPTIVE_BALL, 1, 0, 600);
      RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 1, 0, 75);
    } else {
      RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
      RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 0);
    }

    RPU_SetLampState(LAMP_SAUCER, MB3JackpotQualified==1, 0, 75);
    RPU_SetLampState(LAMP_LOOP_BONUS, 0);
  } else if (GameMode==GAME_MODE_MINIMODE) {
    if (LeftSpinnerLitTime) RPU_SetLampState(LAMP_LEFT_SPINNER, 1, 0, 150);
    else RPU_SetLampState(LAMP_LEFT_SPINNER, MiniModesRunning&BAND_MINIMODE_FLAG, 0, 25);
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    RPU_SetLampState(LAMP_SAUCER, 0);
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 0);
    int flashSpeed = 100;
    if (RightFlipsSeen) flashSpeed = 50;
    if (RightFlipsSeen==(RightFlipsForAddABall-1)) flashSpeed = 25;
    else if (RightFlipsSeen>=RightFlipsForAddABall) flashSpeed = 250;
    RPU_SetLampState(LAMP_LOOP_BONUS, 1, 0, flashSpeed);
  } else if (GameMode==GAME_MODE_MINI_WIZARD) {
    boolean phase2;
    phase2 = (((CurrentTime / 25) % 12)==6) ? true : false; 
    if (WizardShotsCompleted & WIZARD_SHOT_SPINNER) RPU_SetLampState(LAMP_LEFT_SPINNER, 1);
    else RPU_SetLampState(LAMP_LEFT_SPINNER, phase2);
    RPU_SetLampState(LAMP_HIDE_AWAY, 0);
    if (WizardShotsCompleted & WIZARD_SHOT_SAUCER) RPU_SetLampState(LAMP_SAUCER, 1);
    else RPU_SetLampState(LAMP_SAUCER, phase2);
    RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
    RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 0);
    if (WizardShotsCompleted & WIZARD_SHOT_RIGHT_FLIP) RPU_SetLampState(LAMP_LOOP_BONUS, 1);
    else RPU_SetLampState(LAMP_LOOP_BONUS, phase2);
  } else {
    RPU_SetLampState(LAMP_LEFT_SPINNER, LeftSpinnerLitTime ? true : false, 0, 150);
    RPU_SetLampState(LAMP_HIDE_AWAY, (CurrentTime<TopGateCloseTime)?true:false, 0, 250);

    if (BuildBonusLampsCompletionTime) {
      RPU_SetLampState(LAMP_SAUCER, 1, 0, 50);
    } else if (BuildBonusLamps[CurrentPlayer] & BUILD_BONUS_LAMP_SAUCER) {
      if (MiniModesQualified[CurrentPlayer]) {
        byte lampPhase = (CurrentTime/1000)%2;
        if (lampPhase) RPU_SetLampState(LAMP_SAUCER, 1);
        else RPU_SetLampState(LAMP_SAUCER, 1, 0, 75);
      } else {
        RPU_SetLampState(LAMP_SAUCER, 1);
      }
    } else {
      RPU_SetLampState(LAMP_SAUCER, MiniModesQualified[CurrentPlayer] ? true : false, 0, 75);
    }
    if (CollectFlipBonusTimeout) {
      if ( (CurrentTime + 2000) > CollectFlipBonusTimeout ) {
        RPU_SetLampState(LAMP_LOOP_BONUS, 100);
      } else {
        if ( (CurrentTime/500)%2 ) RPU_SetLampState(LAMP_LOOP_BONUS, 1, 0, 100);
        else RPU_SetLampState(LAMP_LOOP_BONUS, 0);
      }
    } else {
      if (BuildBonusLampsCompletionTime) RPU_SetLampState(LAMP_LOOP_BONUS, 1, 50);
      else RPU_SetLampState(LAMP_LOOP_BONUS, BuildBonusLamps[CurrentPlayer] & BUILD_BONUS_LAMP_LOOP_LANE);
    }
    if (BuildBonusLampsCompletionTime) RPU_SetLampState(LAMP_CAPTIVE_BALL, 1, 50);
    else RPU_SetLampState(LAMP_CAPTIVE_BALL, BuildBonusLamps[CurrentPlayer] & BUILD_BONUS_LAMP_CAPTIVE_BALL);
    
    if (TopGateAvailableTime && CurrentTime > TopGateAvailableTime && (LeftDropTargets.GetStatus() & 0x02)) {
      RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 1, 0, 75);
    } else {
      RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 0);
    }
  }
}

void ShowDropTargetLamps() {
  if (GameMode ==  GAME_MODE_MULTIBALL_3 && MB3SuperJackpotQualified==0) {
    byte elwoodFlag = 0x01;
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], (ElwoodStatus[CurrentPlayer] & elwoodFlag) ? false : true, 0, 25);
      elwoodFlag *= 2;
    }
  } else if (GameMode == GAME_MODE_SKILL_SHOT) {
    byte elwoodFlag = 0x01;
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], count == SkillshotLetter, 0, 75);
      elwoodFlag *= 2;
    }
  } else if ((MiniModesRunning&ELWOOD_MINIMODE_FLAG)) {
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], count == ElwoodTargetLit, 0, 25);
    }
  } else if ( GameMode == GAME_MODE_MINI_WIZARD ) {
    unsigned long elwoodFlag = WIZARD_SHOT_E1;
    for (byte count=0; count<3; count++) {
      if (WizardShotsCompleted & elwoodFlag) {
        RPU_SetLampState(ElwoodLampAssignments[count], 1);
      } else {
        byte availPhase = (CurrentTime / 25) % 12;
        if ((availPhase==0 && (count%2)) || ((availPhase==6) && (count%2)==0)) RPU_SetLampState(ElwoodLampAssignments[count], 1);
        else RPU_SetLampState(ElwoodLampAssignments[count], 0);
      }
      elwoodFlag *= 2;
    }
    elwoodFlag = WIZARD_SHOT_O1; // move past flag for spinner
    for (byte count=0; count<3; count++) {
      if (WizardShotsCompleted & elwoodFlag) {
        RPU_SetLampState(ElwoodLampAssignments[count+3], 1);
      } else {
        byte availPhase = (CurrentTime / 25) % 6;
        if ((availPhase==0 && (count%2)) || ((availPhase==3) && (count%2)==0)) RPU_SetLampState(ElwoodLampAssignments[count+3], 1);
        else RPU_SetLampState(ElwoodLampAssignments[count+3], 0);
      }
      elwoodFlag *= 2;
    }
  } else {
    byte elwoodFlag = 0x01;
    byte lampFeatured = 0xFF;

    if ( CurrentTime > (LastDropTargetHitTime + 5000) ) {
      unsigned long deltaTime = (CurrentTime-(LastDropTargetHitTime + 5000));
      if (ElwoodCompletions[CurrentPlayer] == 0 && OneBankFirstClear) {
        if ( (deltaTime/1000)%2 ) {
          lampFeatured = (deltaTime/100)%5;
          if (lampFeatured==3) lampFeatured = 1;
          if (lampFeatured==4) lampFeatured = 0;
          lampFeatured += 3;
        } else {
          lampFeatured = (deltaTime/100)%5;
          if (lampFeatured==3) lampFeatured = 1;
          if (lampFeatured==4) lampFeatured = 0;
        }
      } else {
        lampFeatured = (deltaTime/100)%10;
        if (lampFeatured>5) lampFeatured = 10-lampFeatured;
      }
    }
    
    for (byte count = 0; count < 6; count++) {
      RPU_SetLampState(ElwoodLampAssignments[count], ((ElwoodStatus[CurrentPlayer] & elwoodFlag) ? true : false) || count==lampFeatured);
      elwoodFlag *= 2;
    }
  }
}

void ShowLaneLamps() {
  if (GameMode == GAME_MODE_MINI_WIZARD) {
    if (WizardShotsCompleted & WIZARD_SHOT_SAVE_GATE) {
      RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 1);
    } else {
      byte availPhase = (CurrentTime / 25) % 12;
      RPU_SetLampState(LAMP_OPEN_SAVE_GATE, (availPhase==0)?true:false);
    }
  } else {
    if (LeftGateAvailableTime && CurrentTime > LeftGateAvailableTime) {
      RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 1, 0, 800);
    } else {
      RPU_SetLampState(LAMP_OPEN_SAVE_GATE, 0);
    }
  }
  if (BuildBonusLampsCompletionTime) RPU_SetLampState(LAMP_LEFT_INLANE, 1, 50);
  else RPU_SetLampState(LAMP_LEFT_INLANE, (BuildBonusLamps[CurrentPlayer] & BUILD_BONUS_LAMP_LEFT_INLANE)?true:false);

  if (LeftGateCloseTime) {
    unsigned short flashRate = 0;
    if ((CurrentTime + 3000) > LeftGateCloseTime) {
      flashRate = 100;
    }
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 1, 0, flashRate);
  } else if (LeftOutlaneSaveEndTime) {
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 1, 0, 75);
  } else {
    RPU_SetLampState(LAMP_LEFT_OUTLANE, 0);
  }
  if (BuildBonusLampsCompletionTime) RPU_SetLampState(LAMP_RIGHT_INLANE, 1, 50);
  else RPU_SetLampState(LAMP_RIGHT_INLANE, (BuildBonusLamps[CurrentPlayer] & BUILD_BONUS_LAMP_RIGHT_INLANE)?true:false);
  RPU_SetLampState(LAMP_RIGHT_OUTLANE_EB, ExtraBallAvailable[CurrentPlayer]==1, 0, 100);
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


void ActivateTopper(boolean longDisplay = false) {
  if (longDisplay) RPU_FireContinuousSolenoid(0x20, 5);
  else RPU_FireContinuousSolenoid(0x20, 30);
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
  
  if (CurrentNumPlayers > 1) {
    // If this is second, third, or fourth player, then playe the announcment
    QueueNotification(SOUND_EFFECT_VP_ADD_PLAYER_1 + (CurrentNumPlayers - 1), 10);
  }

  for (byte count = 0; count < 4; count++) {
    if (count==CurrentPlayer) RPU_SetLampState(PlayerUpLamps[count], 1, 0, 250);
    else if (count<CurrentNumPlayers) RPU_SetLampState(PlayerUpLamps[count], 1);
    else RPU_SetLampState(PlayerUpLamps[count], 0);
  }

  if (!FreePlayMode) {
    Credits -= 1;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
//    RPU_SetCoinLockout(false);
  }

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
      RPU_PushToSolenoidStack(SOL_KNOCKER, KNOCKER_SOLENOID_STRENGTH, true);
    }
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
//    RPU_SetCoinLockout(false);
  } else {
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
//    RPU_SetCoinLockout(true);
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
  RPU_PushToTimedSolenoidStack(SOL_KNOCKER, KNOCKER_SOLENOID_STRENGTH, CurrentTime, true);
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

boolean AwardExtraBall(boolean basedOnScore = false) {
  if (ExtraBallAvailable[CurrentPlayer]==1 || (basedOnScore && ExtraBallAvailable[CurrentPlayer]!=2)) {
    ExtraBallAvailable[CurrentPlayer] = 2;
    if (TournamentScoring) {
      CurrentScores[CurrentPlayer] += ExtraBallValue * PlayfieldMultiplier;
    } else {
      SamePlayerShootsAgain = true;
      RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
      QueueNotification(SOUND_EFFECT_VP_EXTRA_BALL, 8);
    }
    return true;
  }
  return false;
}


void IncreasePlayfieldMultiplier(unsigned long duration) {
  PlayfieldMultiplierTimeLeft += duration;

  PlayfieldMultiplier += 1;
  if (PlayfieldMultiplier > 5) {
    PlayfieldMultiplier = 5;
  }
}


void SetBallSave(unsigned long duration, byte numberOfSaves = 0xFF, boolean addToBallSave = false) {

  if (duration == 0) {
    BallSaveEndTime = 0;
    NumberOfBallSavesRemaining = 0;
  } else if (addToBallSave) {
    if (BallSaveEndTime) BallSaveEndTime += duration;
  } else {
    BallSaveEndTime = CurrentTime + duration;
    NumberOfBallSavesRemaining = numberOfSaves;
  }
}



void ReleaseLiftGate(unsigned long liftDelay = 0) {

  // We only release the lift gate every 10 seconds
  if (LiftGateReleaseRequestTime < CurrentTime) {
    if ( (CurrentTime + liftDelay) > (LiftGateLastReleaseTime + 10000)) {
      // it has been more than 10 seconds since the last
      // time the gate was raised, so we can schedule it at any point
      LiftGateReleaseRequestTime = CurrentTime + liftDelay;
    } else {
      LiftGateReleaseRequestTime = LiftGateLastReleaseTime + 10000;
    }
  }
}


void UpdateLockStatus(boolean releaseMachineLocks = false) {

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Update locks before: ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
    Serial.write(buf);
  }

  if (releaseMachineLocks) {
    MachineLocks = 0;
    NumberOfBallsLocked = 0;
  }

  if (LockHandling!=LOCK_HANDLING_VIRTUAL_LOCKS) {
    // Take away locks that have been released if we're in 
    // Stealing or Purge with relocks
    if (LockHandling==LOCK_HANDLING_LOCK_STEALING || LockHandling==LOCK_HANDLING_PURGE_LOCKS_WITH_RELOCKS) {
      if ( (PlayerLocks[CurrentPlayer]&BALL_1_LOCKED) && (MachineLocks & BALL_1_LOCKED) == 0 ) {
        // if the player had a lock but it has been released,
        // then we should move that player's lock to available
        PlayerLocks[CurrentPlayer] &= ~BALL_1_LOCKED;
        PlayerLocks[CurrentPlayer] |= BALL_1_LOCK_AVAILABLE;
      }
      if ( (PlayerLocks[CurrentPlayer]&BALL_2_LOCKED) && (MachineLocks & BALL_2_LOCKED) == 0 ) {
        // if the player had a lock but it has been rejected,
        // then we should move that player's lock to available
        PlayerLocks[CurrentPlayer] &= ~BALL_2_LOCKED;
        PlayerLocks[CurrentPlayer] |= BALL_2_LOCK_AVAILABLE;
      }
    }
  
    // Steal/share locks
    if (LockHandling==LOCK_HANDLING_LOCK_STEALING || LockHandling==LOCK_HANDLING_LOCK_SHARING) {
      if ( (MachineLocks & BALL_1_LOCKED) && (PlayerLocks[CurrentPlayer] & BALL_1_LOCK_AVAILABLE) ) {
        if ( (PlayerLocks[CurrentPlayer] & BALL_1_LOCKED)==0 ) {
          // Announce the steal?
          QueueNotification(SOUND_EFFECT_VP_LOCK_1_STOLEN, 10);
          PlayerLocks[CurrentPlayer] |= BALL_1_LOCKED;
        } else {
          // Ball already locked but it was available? This shouldn't happen
        }
        PlayerLocks[CurrentPlayer] &= ~BALL_1_LOCK_AVAILABLE;
      }
      if ( (MachineLocks & BALL_2_LOCKED) && (PlayerLocks[CurrentPlayer] & BALL_2_LOCK_AVAILABLE) ) {
        if ( (PlayerLocks[CurrentPlayer] & BALL_2_LOCKED)==0 ) {
          // Announce the steal?
          QueueNotification(SOUND_EFFECT_VP_LOCK_2_STOLEN, 10);
          PlayerLocks[CurrentPlayer] |= BALL_2_LOCKED;
        } else {
          // Ball already locked but it was available? This shouldn't happen
        }
        PlayerLocks[CurrentPlayer] &= ~BALL_2_LOCK_AVAILABLE;
      }
    }
  }


  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Update locks after: ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
    Serial.write(buf);
  }

}

void UpdateLiftGate() {
  if (LiftGateReleaseRequestTime != 0 && CurrentTime > LiftGateReleaseRequestTime) {
    LiftGateReleaseRequestTime = 0;
    LiftGateLastReleaseTime = CurrentTime;
    UpdateLockStatus(true);
    RPU_PushToSolenoidStack(SOL_LIFT_GATE, LIFT_GATE_SOLENOID_STRENGTH, true);
    if (MB3SuperJackpotQualified<3) MB3SuperJackpotQualified = 1;
    else if (MB3SuperJackpotQualified) MB3SuperJackpotQualified = 0;
  }
}



#define SOUND_EFFECT_OM_CPC_VALUES              180
#define SOUND_EFFECT_OM_CRB_VALUES              210
#define SOUND_EFFECT_OM_DIFFICULTY_VALUES       220
#define SOUND_EFFECT_OM_LOCK_BEHAVIOR_VALUES    230

#define SOUND_EFFECT_AP_TOP_LEVEL_MENU_ENTRY    1700
#define SOUND_EFFECT_AP_TEST_MENU               1701
#define SOUND_EFFECT_AP_AUDITS_MENU             1702
#define SOUND_EFFECT_AP_BASIC_ADJUSTMENTS_MENU  1703
#define SOUND_EFFECT_AP_GAME_RULES_LEVEL        1704
#define SOUND_EFFECT_AP_GAME_SPECIFIC_ADJ_MENU  1705

#define SOUND_EFFECT_AP_TEST_LAMPS              1710
#define SOUND_EFFECT_AP_TEST_DISPLAYS           1711
#define SOUND_EFFECT_AP_TEST_SOLENOIDS          1712
#define SOUND_EFFECT_AP_TEST_SWITCHES           1713
#define SOUND_EFFECT_AP_TEST_SOUNDS             1714
#define SOUND_EFFECT_AP_TEST_EJECT_BALLS        1715

#define SOUND_EFFECT_AP_AUDIT_TOTAL_PLAYS       1720
#define SOUND_EFFECT_AP_AUDIT_CHUTE_1_COINS     1721
#define SOUND_EFFECT_AP_AUDIT_CHUTE_2_COINS     1722
#define SOUND_EFFECT_AP_AUDIT_CHUTE_3_COINS     1723
#define SOUND_EFFECT_AP_AUDIT_TOTAL_REPLAYS     1724
#define SOUND_EFFECT_AP_AUDIT_AVG_BALL_TIME     1725
#define SOUND_EFFECT_AP_AUDIT_HISCR_BEAT        1726
#define SOUND_EFFECT_AP_AUDIT_TOTAL_BALLS       1727
#define SOUND_EFFECT_AP_AUDIT_NUM_MATCHES       1728
#define SOUND_EFFECT_AP_AUDIT_MATCH_PERCENTAGE  1729
#define SOUND_EFFECT_AP_AUDIT_LIFETIME_PLAYS    1730
#define SOUND_EFFECT_AP_AUDIT_MINUTES_ON        1731
#define SOUND_EFFECT_AP_AUDIT_CLEAR_AUDITS      1732

#define OM_BASIC_ADJ_IDS_FREEPLAY               0
#define OM_BASIC_ADJ_IDS_BALL_SAVE              1
#define OM_BASIC_ADJ_IDS_TILT_WARNINGS          2
#define OM_BASIC_ADJ_IDS_MUSIC_VOLUME           3
#define OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME         4
#define OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME        5
#define OM_BASIC_ADJ_IDS_BALLS_PER_GAME         6
#define OM_BASIC_ADJ_IDS_TOURNAMENT_MODE        7
#define OM_BASIC_ADJ_IDS_EXTRA_BALL_VALUE       8
#define OM_BASIC_ADJ_IDS_SPECIAL_VALUE          9 
#define OM_BASIC_ADJ_IDS_RESET_DURING_GAME      10
#define OM_BASIC_ADJ_IDS_SCORE_LEVEL_1          11
#define OM_BASIC_ADJ_IDS_SCORE_LEVEL_2          12
#define OM_BASIC_ADJ_IDS_SCORE_LEVEL_3          13
#define OM_BASIC_ADJ_IDS_SCORE_AWARDS           14
#define OM_BASIC_ADJ_IDS_SCROLLING_SCORES       15
#define OM_BASIC_ADJ_IDS_HISCR                  16
#define OM_BASIC_ADJ_IDS_CREDITS                17
#define OM_BASIC_ADJ_IDS_CPC_1                  18
#define OM_BASIC_ADJ_IDS_CPC_2                  19
#define OM_BASIC_ADJ_IDS_CPC_3                  20
#define OM_BASIC_ADJ_IDS_MATCH_FEATURE          21
#define OM_BASIC_ADJ_FINISHED                   22
#define SOUND_EFFECT_AP_FREEPLAY                (1740 + OM_BASIC_ADJ_IDS_FREEPLAY)
#define SOUND_EFFECT_AP_BALL_SAVE_SECONDS       (1740 + OM_BASIC_ADJ_IDS_BALL_SAVE)
#define SOUND_EFFECT_AP_TILT_WARNINGS           (1740 + OM_BASIC_ADJ_IDS_TILT_WARNINGS)
#define SOUND_EFFECT_AP_MUSIC_VOLUME            (1740 + OM_BASIC_ADJ_IDS_MUSIC_VOLUME)
#define SOUND_EFFECT_AP_SOUNDFX_VOLUME          (1740 + OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME)
#define SOUND_EFFECT_AP_CALLOUTS_VOLUME         (1740 + OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME)
#define SOUND_EFFECT_AP_BALLS_PER_GAME          (1740 + OM_BASIC_ADJ_IDS_BALLS_PER_GAME)
#define SOUND_EFFECT_AP_TOURNAMENT_MODE         (1740 + OM_BASIC_ADJ_IDS_TOURNAMENT_MODE)
#define SOUND_EFFECT_AP_EXTRA_BALL_VALUE        (1740 + OM_BASIC_ADJ_IDS_EXTRA_BALL_VALUE)
#define SOUND_EFFECT_AP_SPECIAL_VALUE           (1740 + OM_BASIC_ADJ_IDS_SPECIAL_VALUE)
#define SOUND_EFFECT_AP_RESET_DURING_GAME       (1740 + OM_BASIC_ADJ_IDS_RESET_DURING_GAME)
#define SOUND_EFFECT_AP_ADJ_SCORE_LEVEL_1       (1740 + OM_BASIC_ADJ_IDS_SCORE_LEVEL_1)
#define SOUND_EFFECT_AP_ADJ_SCORE_LEVEL_2       (1740 + OM_BASIC_ADJ_IDS_SCORE_LEVEL_2)
#define SOUND_EFFECT_AP_ADJ_SCORE_LEVEL_3       (1740 + OM_BASIC_ADJ_IDS_SCORE_LEVEL_3)
#define SOUND_EFFECT_AP_SCORE_AWARDS            (1740 + OM_BASIC_ADJ_IDS_SCORE_AWARDS)
#define SOUND_EFFECT_AP_SCROLLING_SCORES        (1740 + OM_BASIC_ADJ_SCROLLING_SCORES)
#define SOUND_EFFECT_AP_ADJ_HISCR               (1740 + OM_BASIC_ADJ_IDS_HISCR)
#define SOUND_EFFECT_AP_ADJ_CREDITS             (1740 + OM_BASIC_ADJ_IDS_CREDITS)
#define SOUND_EFFECT_AP_ADJ_CPC_1               (1740 + OM_BASIC_ADJ_IDS_CPC_1)
#define SOUND_EFFECT_AP_ADJ_CPC_2               (1740 + OM_BASIC_ADJ_IDS_CPC_2)
#define SOUND_EFFECT_AP_ADJ_CPC_3               (1740 + OM_BASIC_ADJ_IDS_CPC_3)
#define SOUND_EFFECT_AP_MATCH_FEATURE           (1740 + OM_BASIC_ADJ_IDS_MATCH_FEATURE)

#define SOUND_EFFECT_OM_EASY_RULES_INSTRUCTIONS           1770
#define SOUND_EFFECT_OM_MEDIUM_RULES_INSTRUCTIONS         1771
#define SOUND_EFFECT_OM_HARD_RULES_INSTRUCTIONS           1772
#define SOUND_EFFECT_OM_PROGRESSIVE_RULES_INSTRUCTIONS    1773
#define SOUND_EFFECT_OM_CUSTOM_RULES_INSTRUCTIONS         1774

#define OM_GAME_ADJ_EASY_DIFFICULTY                 0
#define OM_GAME_ADJ_MEDIUM_DIFFICULTY               1
#define OM_GAME_ADJ_HARD_DIFFICULTY                 2
#define OM_GAME_ADJ_PROGRESSIVE_DIFFICULTY          3
#define OM_GAME_ADJ_CUSTOM_DIFFICULTY               4
#define SOUND_EFFECT_AP_DIFFICULTY                  (1790 + OM_GAME_ADJ_EASY_DIFFICULTY)

#define OM_GAME_ADJ_LOCK_BEHAVIOR                   0
#define OM_GAME_ADJ_ELWOOD_CLEARS_TO_QUALIFY        1
#define OM_GAME_ADJ_ONE_BANK_FIRST_CLEAR            2
#define OM_GAME_ADJ_JAKE_CLEARS_TO_QUALIFY          3
#define OM_GAME_ADJ_ALT_SPINNER_ACCELERATOR         4
#define OM_GAME_ADJ_RIGHT_FLIPS_FOR_ADD_A_BALL      5
#define OM_GAME_ADJ_RESET_JAKE_EACH_BALL            6
#define OM_GAME_ADJ_RESET_ELWOOD_EACH_BALL          7
#define OM_GAME_ADJ_RESET_BAND_EACH_BALL            8
#define OM_GAME_ADJ_RESET_QUALIFIED_EACH_BALL       9
#define OM_GAME_ADJ_OUTLANE_BALL_SAVES              10
#define OM_GAME_ADJ_BONUS_COLLECT_HURRY_UP_SECS     11
#define OM_GAME_ADJ_SECONDS_UNTIL_STATUS            12
#define OM_GAME_ADJ_BALL_SAVE_ON_MINIMODES          13
#define OM_GAME_ADJ_BALL_SAVE_ON_MULTIBALL          14
#define OM_GAME_ADJ_BALL_SAVE_ON_ADD_A_BALL         15
#define OM_GAME_ADJ_BALL_SAVE_ON_MINI_WIZARD        16
#define OM_GAME_ADJ_BALL_SAVE_ON_WIZARD             17
#define OM_GAME_ADJ_TROUGH_EJECT_STRENGTH           18
#define OM_GAME_ADJ_PLUNGER_FULL_STRENGTH           19
#define OM_GAME_ADJ_PLUNGER_SOFT_STRENGTH           20
#define OM_GAME_ADJ_SAUCER_EJECT_STRENGTH           21
#define OM_GAME_ADJ_SLINGSHOT_STRENGTH              22
#define OM_GAME_ADJ_POP_BUMPER_STRENGTH             23
#define OM_GAME_ADJ_FINISHED                        24
#define SOUND_EFFECT_AP_LOCK_BEHAVIOR               (1800 + OM_GAME_ADJ_LOCK_BEHAVIOR)

unsigned long SoundSettingTimeout;
unsigned long SoundTestStart;
byte SoundTestSequence;
  
void RunOperatorMenu() {
  if (!Menus.UpdateMenu(CurrentTime)) {
    // Menu is done
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    Audio.StopAllAudio();
    RPU_TurnOffAllLamps();
    if (MachineState==MACHINE_STATE_ATTRACT) {
      RPU_SetDisplayBallInPlay(0, true);
    } else {
      RPU_SetDisplayBallInPlay(CurrentBallInPlay);
    }
    SoundSettingTimeout = 0;
    return;
  }

  // It's up to this function to eject balls if requested
  if (Menus.BallEjectInProgress()) {
    if (CountBallsInTrough()) {
      if (CurrentTime > (LastTimeBallServed+1500)) {
        LastTimeBallServed = CurrentTime;
        RPU_PushToSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, true);
      }
    }
  } else {
    LastTimeBallServed = 0;
  }
  
  byte topLevel = Menus.GetTopLevel();
  byte subLevel = Menus.GetSubLevel();

  if (Menus.HasTopLevelChanged()) {
    // Play an audio prompt for the top level
    SoundTestStart = 0;
    Audio.StopAllAudio();
    Audio.PlaySound((unsigned short)topLevel + SOUND_EFFECT_AP_TOP_LEVEL_MENU_ENTRY, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
    if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_RULES_LEVEL) Menus.SetNumSubLevels(4);
    if (Menus.GetTopLevel()==OPERATOR_MENU_BASIC_ADJ_MENU) {
      GetCPCSelection(0); // make sure CPC values have been read
      Menus.SetNumSubLevels(OM_BASIC_ADJ_FINISHED);
    }
    if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_ADJ_MENU) Menus.SetNumSubLevels(OM_GAME_ADJ_FINISHED);
  }
  if (Menus.HasSubLevelChanged()) {
    SoundTestStart = 0;
    // Play an audio prompt for the sub level    
    Audio.StopAllAudio();
    if (topLevel==OPERATOR_MENU_SELF_TEST_MENU) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_TEST_LAMPS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

      if (subLevel==OPERATOR_MENU_TEST_SOUNDS) {
        SoundTestStart = CurrentTime + 1000;
        SoundTestSequence = 0;
      } else {
        SoundTestStart = 0;
      }
    } else if (topLevel==OPERATOR_MENU_AUDITS_MENU) {
      unsigned long *currentAdjustmentUL = NULL;
      byte currentAdjustmentStorageByte = 0;
      byte adjustmentType = OPERATOR_MENU_AUD_CLEARABLE;

      switch (subLevel) {
        case 0:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_TOTAL_PLAYS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_TOTAL_PLAYS_EEPROM_START_BYTE;
          break;
        case 1:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_CHUTE_1_COINS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_CHUTE_1_COINS_START_BYTE;
          break;
        case 2:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_CHUTE_2_COINS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_CHUTE_2_COINS_START_BYTE;
          break;
        case 3:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_CHUTE_3_COINS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_CHUTE_3_COINS_START_BYTE;
          break;
        case 4:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_TOTAL_REPLAYS, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_TOTAL_REPLAYS_EEPROM_START_BYTE;
          break;
        case 5:
          Audio.PlaySound(SOUND_EFFECT_AP_AUDIT_HISCR_BEAT, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
          currentAdjustmentStorageByte = RPU_TOTAL_HISCORE_BEATEN_START_BYTE;
          break;
      }

      Menus.SetAuditControls(currentAdjustmentUL, currentAdjustmentStorageByte, adjustmentType);

    } else if (topLevel==OPERATOR_MENU_BASIC_ADJ_MENU) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_FREEPLAY, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

      byte *currentAdjustmentByte = NULL;
      byte currentAdjustmentStorageByte = 0;
      byte adjustmentValues[8] = {0};
      byte numAdjustmentValues = 2;
      byte adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
      short parameterCallout = 0;
      unsigned long *currentAdjustmentUL = NULL;
      
      adjustmentValues[1] = 1;

      switch(subLevel) {
        case OM_BASIC_ADJ_IDS_FREEPLAY:
          currentAdjustmentByte = (byte *)&FreePlayMode;
          currentAdjustmentStorageByte = EEPROM_FREE_PLAY_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_BALL_SAVE:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[1] = 5;
          adjustmentValues[2] = 10;
          adjustmentValues[3] = 15;
          adjustmentValues[4] = 20;
          currentAdjustmentByte = &BallSaveNumSeconds;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_TILT_WARNINGS:
          adjustmentValues[1] = 2;
          currentAdjustmentByte = &MaxTiltWarnings;
          currentAdjustmentStorageByte = EEPROM_TILT_WARNING_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_MUSIC_VOLUME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &MusicVolume;
          currentAdjustmentStorageByte = EEPROM_MUSIC_VOLUME_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &SoundEffectsVolume;
          currentAdjustmentStorageByte = EEPROM_SFX_VOLUME_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &CalloutsVolume;
          currentAdjustmentStorageByte = EEPROM_CALLOUTS_VOLUME_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_BALLS_PER_GAME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          numAdjustmentValues = 8;
          adjustmentValues[0] = 3;
          adjustmentValues[1] = 10;
          currentAdjustmentByte = &BallsPerGame;
          currentAdjustmentStorageByte = EEPROM_BALLS_OVERRIDE_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_TOURNAMENT_MODE:
          currentAdjustmentByte = (byte *)&TournamentScoring;
          currentAdjustmentStorageByte = EEPROM_TOURNAMENT_SCORING_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_EXTRA_BALL_VALUE:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &ExtraBallValue;
          currentAdjustmentStorageByte = EEPROM_EXTRA_BALL_SCORE_UL;
          break;
        case OM_BASIC_ADJ_IDS_SPECIAL_VALUE:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &SpecialValue;
          currentAdjustmentStorageByte = EEPROM_SPECIAL_SCORE_UL;
          break;
        case OM_BASIC_ADJ_IDS_RESET_DURING_GAME:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 1;
          adjustmentValues[2] = 2;
          adjustmentValues[3] = 3;
          adjustmentValues[4] = 99;
          currentAdjustmentByte = &TimeRequiredToResetGame;
          currentAdjustmentStorageByte = EEPROM_CRB_HOLD_TIME;
          parameterCallout = SOUND_EFFECT_OM_CRB_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_LEVEL_1:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &AwardScores[0];
          currentAdjustmentStorageByte = RPU_AWARD_SCORE_1_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_LEVEL_2:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &AwardScores[1];
          currentAdjustmentStorageByte = RPU_AWARD_SCORE_2_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_LEVEL_3:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &AwardScores[2];
          currentAdjustmentStorageByte = RPU_AWARD_SCORE_3_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCORE_AWARDS:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX_DEFAULT;
          adjustmentValues[1] = 7;
          currentAdjustmentByte = &ScoreAwardReplay;
          currentAdjustmentStorageByte = EEPROM_AWARD_OVERRIDE_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_SCROLLING_SCORES:
          currentAdjustmentByte = (byte *)&ScrollingScores;
          currentAdjustmentStorageByte = EEPROM_SCROLLING_SCORES_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_HISCR:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_SCORE_WITH_DEFAULT;
          currentAdjustmentUL = &HighScore;
          currentAdjustmentStorageByte = RPU_HIGHSCORE_EEPROM_START_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_CREDITS:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 40;
          currentAdjustmentByte = &Credits;
          currentAdjustmentStorageByte = RPU_CREDITS_EEPROM_BYTE;
          break;
        case OM_BASIC_ADJ_IDS_CPC_1:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_CPC;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = (NUM_CPC_PAIRS-1);
          currentAdjustmentByte = &(CPCSelection[0]);
          currentAdjustmentStorageByte = RPU_CPC_CHUTE_1_SELECTION_BYTE;
          parameterCallout = SOUND_EFFECT_OM_CPC_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_CPC_2:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_CPC;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = (NUM_CPC_PAIRS-1);
          currentAdjustmentByte = &(CPCSelection[1]);
          currentAdjustmentStorageByte = RPU_CPC_CHUTE_2_SELECTION_BYTE;
          parameterCallout = SOUND_EFFECT_OM_CPC_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_CPC_3:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_CPC;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = (NUM_CPC_PAIRS-1);
          currentAdjustmentByte = &(CPCSelection[2]);
          currentAdjustmentStorageByte = RPU_CPC_CHUTE_3_SELECTION_BYTE;
          parameterCallout = SOUND_EFFECT_OM_CPC_VALUES;
          break;
        case OM_BASIC_ADJ_IDS_MATCH_FEATURE:
          currentAdjustmentByte = (byte *)&MatchFeature;
          currentAdjustmentStorageByte = EEPROM_MATCH_FEATURE_BYTE;
          break;
      }

      Menus.SetParameterControls(   adjustmentType, numAdjustmentValues, adjustmentValues, parameterCallout,
                                    currentAdjustmentStorageByte, currentAdjustmentByte, currentAdjustmentUL );
    } else if (topLevel==OPERATOR_MENU_GAME_RULES_LEVEL) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_DIFFICULTY, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
      byte *currentAdjustmentByte = &GameRulesSelection;
      byte adjustmentValues[8] = {0};
      adjustmentValues[0] = 0;
      // if one of the below parameters is installed, the "HasParameterChanged" 
      // check below will install Easy / Medium / Hard rules

      switch (subLevel) {
        case 0:
          adjustmentValues[1] = 1;
          break;
        case 1:
          adjustmentValues[1] = 2;
          break;
        case 2:
          adjustmentValues[1] = 3;
          break;
        case 3:
          adjustmentValues[1] = 4;
          break;
      }

      Menus.SetParameterControls(   OPERATOR_MENU_ADJ_TYPE_LIST, 2, adjustmentValues, (short)SOUND_EFFECT_OM_EASY_RULES_INSTRUCTIONS-1,
                                    EEPROM_GAME_RULES_SELECTION, currentAdjustmentByte, NULL );
                  
    } else if (topLevel==OPERATOR_MENU_GAME_ADJ_MENU) {
      Audio.PlaySound((unsigned short)subLevel + SOUND_EFFECT_AP_LOCK_BEHAVIOR, AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);

      byte *currentAdjustmentByte = NULL;
      byte currentAdjustmentStorageByte = 0;
      byte adjustmentValues[8] = {0};
      byte numAdjustmentValues = 2;
      byte adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
      short parameterCallout = 0;
      unsigned long *currentAdjustmentUL = NULL;
      
      adjustmentValues[1] = 1;

      switch (subLevel) {
        case OM_GAME_ADJ_LOCK_BEHAVIOR:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 4;
          currentAdjustmentByte = &LockHandling;
          currentAdjustmentStorageByte = EEPROM_LOCK_BEHAVIOR_BYTE;
          parameterCallout = SOUND_EFFECT_OM_LOCK_BEHAVIOR_VALUES;
          break;
        case OM_GAME_ADJ_ELWOOD_CLEARS_TO_QUALIFY:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 1;
          adjustmentValues[1] = 4;
          currentAdjustmentByte = &ElwoodClearsToQualify;
          currentAdjustmentStorageByte = EEPROM_ELWOOD_CLEARS_TO_QUALIFY_BYTE;
          break;
        case OM_GAME_ADJ_ONE_BANK_FIRST_CLEAR:
          currentAdjustmentByte = (byte *)&OneBankFirstClear;
          currentAdjustmentStorageByte = EEPROM_ONE_DROP_BANK_FIRST_CLEAR_BYTE;
          break;
        case OM_GAME_ADJ_JAKE_CLEARS_TO_QUALIFY:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 1;
          adjustmentValues[1] = 3;
          currentAdjustmentByte = &JakeClearsToQualify;
          currentAdjustmentStorageByte = EEPROM_JAKE_CLEARS_TO_QUALIFY_BYTE;
          break;
        case OM_GAME_ADJ_ALT_SPINNER_ACCELERATOR:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 6;
          adjustmentValues[0] = 1;
          adjustmentValues[1] = 5;
          adjustmentValues[2] = 10;
          adjustmentValues[3] = 15;
          adjustmentValues[4] = 20;
          adjustmentValues[5] = 25;
          currentAdjustmentByte = &AlternatingSpinnerAccelerator;
          currentAdjustmentStorageByte = EEPROM_ALT_SPINNER_ACCELERATOR_BYTE;
          break;
        case OM_GAME_ADJ_RIGHT_FLIPS_FOR_ADD_A_BALL:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 1;
          adjustmentValues[1] = 5;
          currentAdjustmentByte = &RightFlipsForAddABall;
          currentAdjustmentStorageByte = EEPROM_FLIPS_FOR_ADDABALL_BYTE;
          break;
        case OM_GAME_ADJ_RESET_JAKE_EACH_BALL:
          currentAdjustmentByte = (byte *)&ResetJakeProgressEachBall;
          currentAdjustmentStorageByte = EEPROM_RESET_JAKE_PROGRESS_EACH_BALL;
          break;
        case OM_GAME_ADJ_RESET_ELWOOD_EACH_BALL:
          currentAdjustmentByte = (byte *)&ResetElwoodProgressEachBall;
          currentAdjustmentStorageByte = EEPROM_RESET_ELWOOD_PROGRESS_EACH_BALL;
          break;
        case OM_GAME_ADJ_RESET_BAND_EACH_BALL:
          currentAdjustmentByte = (byte *)&ResetBandProgressEachBall;
          currentAdjustmentStorageByte = EEPROM_RESET_BAND_PROGRESS_EACH_BALL;
          break;
        case OM_GAME_ADJ_RESET_QUALIFIED_EACH_BALL:
          currentAdjustmentByte = (byte *)&ResetQualifiedModesEachBall;
          currentAdjustmentStorageByte = EEPROM_RESET_QUALIFIED_MODES_EACH_BALL;
          break;
        case OM_GAME_ADJ_OUTLANE_BALL_SAVES:
          currentAdjustmentByte = (byte *)&OutlaneBallSaves;
          currentAdjustmentStorageByte = EEPROM_OUTLANE_BALL_SAVES;
          break;
        case OM_GAME_ADJ_BONUS_COLLECT_HURRY_UP_SECS:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 15;
          adjustmentValues[2] = 30;
          adjustmentValues[3] = 45;
          adjustmentValues[4] = 60;
          currentAdjustmentByte = &BonusCollectHurryUp;
          currentAdjustmentStorageByte = EEPROM_BONUS_COLLECT_HURRY_UP_SECS;
          break;
        case OM_GAME_ADJ_SECONDS_UNTIL_STATUS:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 5;
          adjustmentValues[1] = 7;
          adjustmentValues[2] = 10;
          adjustmentValues[3] = 15;
          adjustmentValues[4] = 99;
          currentAdjustmentByte = &StatusModeInactivityDelay;
          currentAdjustmentStorageByte = EEPROM_SECONDS_UNTIL_STATUS;
          break;
        case OM_GAME_ADJ_BALL_SAVE_ON_MINIMODES:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 5;
          adjustmentValues[2] = 10;
          adjustmentValues[3] = 15;
          adjustmentValues[4] = 20;
          currentAdjustmentByte = &BallSaveOnMinimodes;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_ON_MINIMODES;
          break;
        case OM_GAME_ADJ_BALL_SAVE_ON_MULTIBALL:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 5;
          adjustmentValues[2] = 15;
          adjustmentValues[3] = 25;
          adjustmentValues[4] = 35;
          currentAdjustmentByte = &BallSaveOnMultiball;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_ON_MULTIBALL;
          break;
        case OM_GAME_ADJ_BALL_SAVE_ON_ADD_A_BALL:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          adjustmentValues[2] = 20;
          adjustmentValues[3] = 30;
          adjustmentValues[4] = 40;
          currentAdjustmentByte = &BallSaveOnAddABall;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_ON_ADD_A_BALL;
          break;
        case OM_GAME_ADJ_BALL_SAVE_ON_MINI_WIZARD:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          adjustmentValues[2] = 20;
          adjustmentValues[3] = 30;
          adjustmentValues[4] = 60;
          currentAdjustmentByte = &BallSaveOnMiniWizard;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_ON_MINI_WIZARD;
          break;
        case OM_GAME_ADJ_BALL_SAVE_ON_WIZARD:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 5;
          adjustmentValues[0] = 0;
          adjustmentValues[1] = 10;
          adjustmentValues[2] = 20;
          adjustmentValues[3] = 30;
          adjustmentValues[4] = 60;
          currentAdjustmentByte = &BallSaveOnWizard;
          currentAdjustmentStorageByte = EEPROM_BALL_SAVE_ON_WIZARD;
          break;
        case OM_GAME_ADJ_TROUGH_EJECT_STRENGTH:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_LIST;
          numAdjustmentValues = 6;
          adjustmentValues[0] = 10;
          adjustmentValues[1] = 20;
          adjustmentValues[2] = 30;
          adjustmentValues[3] = 40;
          adjustmentValues[4] = 50;
          adjustmentValues[5] = 60;
          currentAdjustmentByte = &BallServeSolenoidStrength;
          currentAdjustmentStorageByte = EEPROM_TROUGH_EJECT_STRENGTH;
          break;
        case OM_GAME_ADJ_PLUNGER_FULL_STRENGTH:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 3;
          adjustmentValues[1] = 8;
          currentAdjustmentByte = &ShooterKickFullStrength;
          currentAdjustmentStorageByte = EEPROM_PLUNGER_FULL_STRENGTH;
          break;
        case OM_GAME_ADJ_PLUNGER_SOFT_STRENGTH:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 1;
          adjustmentValues[1] = 5;
          currentAdjustmentByte = &ShooterKickLightStrength;
          currentAdjustmentStorageByte = EEPROM_PLUNGER_SOFT_STRENGTH;
          break;
        case OM_GAME_ADJ_SAUCER_EJECT_STRENGTH:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 5;
          adjustmentValues[1] = 15;
          currentAdjustmentByte = &SaucerSolenoidStrength;
          currentAdjustmentStorageByte = EEPROM_SAUCER_EJECT_STRENGTH;
          break;
        case OM_GAME_ADJ_SLINGSHOT_STRENGTH:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 4;
          adjustmentValues[1] = 8;
          currentAdjustmentByte = &SolenoidAssociatedSwitches[0].solenoidHoldTime;
          currentAdjustmentStorageByte = EEPROM_SLINGSHOT_STRENGTH;
          break;
        case OM_GAME_ADJ_POP_BUMPER_STRENGTH:
          adjustmentType = OPERATOR_MENU_ADJ_TYPE_MIN_MAX;
          adjustmentValues[0] = 4;
          adjustmentValues[1] = 8;
          currentAdjustmentByte = &SolenoidAssociatedSwitches[3].solenoidHoldTime;
          currentAdjustmentStorageByte = EEPROM_POP_BUMPER_STRENGTH;
          break;

      }
      
      Menus.SetParameterControls(   adjustmentType, numAdjustmentValues, adjustmentValues, parameterCallout,
                                    currentAdjustmentStorageByte, currentAdjustmentByte, currentAdjustmentUL );
    }    
  }

  if (Menus.HasParameterChanged()) {
    short parameterCallout = Menus.GetParameterCallout();
    if (parameterCallout) {
      Audio.StopAllAudio();
      Audio.PlaySound((unsigned short)parameterCallout + Menus.GetParameterID(), AUDIO_PLAY_TYPE_WAV_TRIGGER, 10);
    }
    if (Menus.GetTopLevel()==OPERATOR_MENU_GAME_RULES_LEVEL) {
      // Install the new rules level
      if (LoadRuleDefaults(GameRulesSelection)) {
        WriteParameters();
      }
    } else if (Menus.GetTopLevel()==OPERATOR_MENU_BASIC_ADJ_MENU) {
      if (Menus.GetSubLevel()==OM_BASIC_ADJ_IDS_MUSIC_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_BACKGROUND_SONG_1, AUDIO_PLAY_TYPE_WAV_TRIGGER, MusicVolume);
        Audio.SetMusicVolume(MusicVolume);
        SoundSettingTimeout = CurrentTime + 5000;
      } else if (Menus.GetSubLevel()==OM_BASIC_ADJ_IDS_SOUNDFX_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_CORRECT_MODE_SHOT, AUDIO_PLAY_TYPE_WAV_TRIGGER, SoundEffectsVolume);
        Audio.SetSoundFXVolume(SoundEffectsVolume);
        SoundSettingTimeout = CurrentTime + 5000;
      } else if (Menus.GetSubLevel()==OM_BASIC_ADJ_IDS_CALLOUTS_VOLUME) {
        if (SoundSettingTimeout) Audio.StopAllAudio();
        Audio.PlaySound(SOUND_EFFECT_VP_JACKPOT_1, AUDIO_PLAY_TYPE_WAV_TRIGGER, CalloutsVolume);
        Audio.SetNotificationsVolume(CalloutsVolume);
        SoundSettingTimeout = CurrentTime + 3000;        
      }
    }
  }

  if (SoundSettingTimeout && CurrentTime>SoundSettingTimeout) {
    SoundSettingTimeout = 0;
    Audio.StopAllAudio();
  }

  if (SoundTestStart && CurrentTime>SoundTestStart) {
    if (SoundTestSequence==0) {
      PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_SONG_1);
      SoundTestSequence = 1;
    } else if (SoundTestSequence==1 && CurrentTime>(SoundTestStart+5000)) {
      PlaySoundEffect(SOUND_EFFECT_BIG_HIT_1);
      SoundTestSequence = 2;
    } else if (SoundTestSequence==2 && CurrentTime>(SoundTestStart+10000)) {
      Audio.QueuePrioritizedNotification(SOUND_EFFECT_VP_RAWHIDE_JACKPOTS_COLLECTED, 0, 10, CurrentTime);
      SoundTestSequence = 3;
    }
  }
  
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
byte AttractLastHeadMode = 255;
boolean AttractCheckedForTrappedBall;
unsigned long AttractModeStartTime;

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
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    Display_ClearOverride(0xFF);
    Display_UpdateDisplays(0xFF);
    AttractCheckedForTrappedBall = false;
    AttractModeStartTime = CurrentTime;

    if (DEBUG_MESSAGES) {
      pinMode(20, INPUT);
      pinMode(21, INPUT);
      char buf[128];
      sprintf(buf, "D20 = %d, D21 = %d, (time=%lu)\n", digitalRead(20), digitalRead(21), CurrentTime);
      Serial.write(buf);
    }
  }

  UpdateLiftGate();

  if (CurrentTime > (AttractModeStartTime + 5000) && !AttractCheckedForTrappedBall) {
    AttractCheckedForTrappedBall = true;
    if (DEBUG_MESSAGES) {
      Serial.write("In Attract for 10 seconds - make sure there are no balls trapped\n");
    }

    byte ballInShooterLane = RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) ? 1 : 0;
    if (RPU_ReadSingleSwitchState(SW_SAUCER)) {
      RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
    }
    if (MachineLocks) {
      MachineLocks = 0;
      ReleaseLiftGate(750);
    } else if ( (CountBallsInTrough() + ballInShooterLane) < TotalBallsLoaded ) {
      ReleaseLiftGate(750);
    }    
  }

  RPU_SetLampState(LAMP_START_BUTTON, (FreePlayMode || Credits) ? true : false, 0, 200);

  // Alternate displays between high score and blank
  if (CurrentTime < 16000) {
    if (AttractLastHeadMode != 1) {
      RPU_SetDisplayCredits(Credits, !FreePlayMode);
      RPU_SetDisplayBallInPlay(0, true);
      Display_ClearOverride(0xFF);
    }
    AttractLastHeadMode = 1;
    Display_UpdateDisplays(0xFF);
    RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 0);
  } else if ((CurrentTime / 8000) % 2 == 0) {

    if (AttractLastHeadMode != 2) {
      Display_SetLastTimeScoreChanged(CurrentTime);
    }
    AttractLastHeadMode = 2;
    Display_UpdateDisplays(0xFF, false, false, false, HighScore);
    RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 1);
    for (byte count=0; count<4; count++) {
      RPU_SetLampState(PlayerUpLamps[count], 0);
    }
  } else {
    if (AttractLastHeadMode != 3) {
      if (CurrentTime < 32000) {
        for (int count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          CurrentScores[count] = 0;
        }
        CurrentNumPlayers = 0;
      }
      Display_SetLastTimeScoreChanged(CurrentTime);
    }

    RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 0);

    if (CurrentNumPlayers<3) {
      if (CurrentNumPlayers==0) {
        Display_UpdateDisplays(0xFF, false, false, false, 0);
        for (byte count=0; count<4; count++) {
          RPU_SetLampState(PlayerUpLamps[count], 0);
        }
      } else if (CurrentNumPlayers==1) {
        Display_UpdateDisplays(0, false, false, false, CurrentScores[0]);
        RPU_SetDisplayBlank(1, 0x00);
        for (byte count=0; count<4; count++) {
          RPU_SetLampState(PlayerUpLamps[count], count==0);
        }
      } else {
        Display_UpdateDisplays(0, false, false, false, CurrentScores[0]);
        Display_UpdateDisplays(1, false, false, false, CurrentScores[1]);
        for (byte count=0; count<4; count++) {
          RPU_SetLampState(PlayerUpLamps[count], count<2);
        }
      }
    } else {

      // We need to alternate between showing player 1 & 2 or 3 & 4
      boolean show1and2 = ((CurrentTime / 16000)%2)==0 ? true : false;
      if (show1and2) {
        Display_UpdateDisplays(0, false, false, false, CurrentScores[0]);
        Display_UpdateDisplays(1, false, false, false, CurrentScores[1]);
        for (byte count=0; count<4; count++) {
          RPU_SetLampState(PlayerUpLamps[count], count<2);
        }
      } else {
        Display_UpdateDisplays(0, false, false, false, CurrentScores[2]);
        if (CurrentNumPlayers==4) {
          Display_UpdateDisplays(1, false, false, false, CurrentScores[3]);
          for (byte count=0; count<4; count++) {
            RPU_SetLampState(PlayerUpLamps[count], count>1);
          }
        } else {
          RPU_SetDisplayBlank(1, 0x00);
          for (byte count=0; count<4; count++) {
            RPU_SetLampState(PlayerUpLamps[count], count==2);
          }
        } 
      }
      
    }

    AttractLastHeadMode = 3;
  }

  byte attractPlayfieldPhase = ((CurrentTime / 5000) % 6);

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
    if (switchHit == SW_SELF_TEST_ON_MATRIX) {
      Menus.EnterOperatorMenu();
    }
  }

  // If the user was holding the menu button when the game started
  // then kick the balls
  if (CurrentTime < 4000) {
    if (RPU_ReadSingleSwitchState(SW_SELF_TEST_ON_MATRIX)) {
      if (OperatorSwitchPressStarted==0) {
        OperatorSwitchPressStarted = CurrentTime;
      } else if (CurrentTime > (OperatorSwitchPressStarted+500)) {
        Menus.EnterOperatorMenu();
        Menus.BallEjectInProgress(true);
      }
    } else {
      OperatorSwitchPressStarted = 0;
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

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Game Mode = %d\n", newGameMode);
    Serial.write(buf);
  }
}

byte CountBallsInTrough() {

  byte numBalls = RPU_ReadSingleSwitchState(SW_TROUGH_1) + RPU_ReadSingleSwitchState(SW_TROUGH_2) + RPU_ReadSingleSwitchState(SW_TROUGH_3);

  return numBalls;
}



void AddToBonus(byte bonus) {
  Bonus[CurrentPlayer] += bonus;
  if (Bonus[CurrentPlayer] > MAX_DISPLAY_BONUS) {
    Bonus[CurrentPlayer] = MAX_DISPLAY_BONUS;
    if (!SpecialCollected) {
      if (SpecialAvailable) {
        AwardSpecial();  
        SpecialAvailable = false;
      } else {
        SpecialAvailable = true;
      }
    }
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

  if (curStateChanged) {
    RPU_TurnOffAllLamps();
    SetGeneralIlluminationOn(true);
    GameStartNotificationTime = CurrentTime;
    Audio.StopAllAudio();
    QueueNotification(SOUND_EFFECT_VP_ADD_PLAYER_1, 10);
    for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) RPU_SetDisplayBlank(count, 0x00);
    RPU_SetDisplayCredits(0, false);
    RPU_SetDisplayBallInPlay(1, true);
    NumberOfBallsLocked = 0;
  }

  boolean showBIP = (CurrentTime / 100) % 2;
  RPU_SetDisplayBallInPlay(1, showBIP ? true : false);

  if (RPU_ReadSingleSwitchState(SW_SAUCER)) {
    RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
  }

  if (MachineLocks) {
    ReleaseLiftGate(750);
  }

  byte ballInShooterLane = RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) ? 1 : 0;
  if ( (CountBallsInTrough() + ballInShooterLane) < TotalBallsLoaded ) {

    if (CurrentTime > (LiftGateReleaseRequestTime + 15000)) ReleaseLiftGate();

    if (CurrentTime > (GameStartNotificationTime + 15000)) {
      GameStartNotificationTime = CurrentTime;
      QueueNotification(SOUND_EFFECT_VP_BALL_MISSING, 10);
    }

    return MACHINE_STATE_INIT_GAMEPLAY;
  }

  // The start button has been hit only once to get
  // us into this mode, so we assume a 1-player game
  // at the moment
//  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

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
    SpinnerProgress[count] = 0;
    PlayerLocks[count] = 0;
    JakeShotsMade[count] = 0;
    ElwoodShotsMade[count] = 0;
    BandShotsMade[count] = 0;
    NumberOfSkillshotsMade[count] = 0;
    RawhideJackpotsCollected[count] = 0;
    ConcertJackpotsCollected[count] = 0;
    JolietJackpotsCollected[count] = 0;
    RightFlipLanes[count] = 0;
    MiniModesQualified[count] = 0;
    BuildBonusLamps[count] = 0x01;
    BuildBonusLampsCompletions[count] = 0;
    LeftOutlaneSaveSeconds[count] = 0;
    RightOutlaneSaveSeconds[count] = 0;
    ExtraBallAvailable[count] = 0;
  }

  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
  CurrentPlayer = 0;
  NumberOfBallsInPlay = 0;
  NumberOfBallsLocked = 0;
  LastTimePopHit = 0;
  LastTiltWarningTime = 0;
  Display_ClearOverride(0xFF);
  Display_UpdateDisplays(0xFF);
  RPU_EnableSolenoidStack();
  RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH);

  return MACHINE_STATE_INIT_NEW_BALL;
}



int InitNewBall(bool curStateChanged) {

  // If we're coming into this mode for the first time
  // then we have to do everything to set up the new ball
  if (curStateChanged) {
    //RPU_FireContinuousSolenoid(0x20, 5);
    RPU_TurnOffAllLamps();
    BallFirstSwitchHitTime = 0;

    if (LockHandling==LOCK_HANDLING_PURGE_LOCKS || LockHandling==LOCK_HANDLING_PURGE_LOCKS_WITH_RELOCKS) {
      if (WaitForPurgedLock) {
        RPU_SetDisableFlippers(true);
        RPU_DisableSolenoidStack();
      }
    }
    RPU_SetDisplayCredits(Credits, !FreePlayMode);
    if (CurrentNumPlayers > 1 && (CurrentBallInPlay != 1 || CurrentPlayer != 0) && !SamePlayerShootsAgain) AlertPlayerUp();
    SamePlayerShootsAgain = false;

    RPU_SetDisplayBallInPlay(CurrentBallInPlay);
    RPU_SetLampState(LAMP_HEAD_TILT, 0);
    for (byte count = 0; count < 4; count++) {
      if (count==CurrentPlayer) RPU_SetLampState(PlayerUpLamps[count], 1, 0, 250);
      else if (count<CurrentNumPlayers) RPU_SetLampState(PlayerUpLamps[count], 1);
      else RPU_SetLampState(PlayerUpLamps[count], 0);
      RPU_SetDisplayBlank(count, 0);
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

    SpecialCollected = false;
    SpecialAvailable = false;

    PlayfieldMultiplier = 1;
    PlayfieldMultiplierTimeLeft = 0;
    BonusXAnimationStart = 0;
    Bonus[CurrentPlayer] = 1;
    BonusX[CurrentPlayer] = 1;
    Display_ResetDisplayTrackingVariables();
    BuildBonusLampsCompletionTime = 0;

    SetBallSave(0);
    MiniModesRunning = 0;
    if (ResetQualifiedModesEachBall) MiniModesQualified[CurrentPlayer] = 0;
    MiniModeMultiplierExpiration = 0;

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
    if (ResetBandProgressEachBall) {
      SpinnerProgress[CurrentPlayer] = 0;
    }

    LastSpinnerSeen = 0xFF;
    LastSpinnerHitTime = 0;
    LeftGateAvailableTime = CurrentTime;
    LeftGateCloseTime = 0;
    TopGateAvailableTime = CurrentTime;
    TopGateCloseTime = 0;
    TopGateCloseSoundPlayed = false;
    TrapSwitchClosedTime = 0;
    LastCaptiveBallHit = 0;
    LeftSpinnerLitTime = 0;
    RightFlipperHoldTime = CurrentTime;
    LeftFlipperHoldTime = CurrentTime;
    RightFlipperBeingHeld = false;
    LeftFlipperBeingHeld = false;
    LastFlipperSeen = 0;
    LastFlipLaneTime = 0;
    LastDropTargetHitTime = 0;
    CollectFlipBonusTimeout = 0;
    NextBonusPayoutTime = 0;
    LeftOutlaneSaveEndTime = 0;
    RightOutlaneSaveEndTime = 0;
    LastTimeSaucerSeen = 0;
    WaitingForTrapSwitchToHold = 0;

    // Reset Drop Targets
    LeftDropTargets.ResetDropTargets(CurrentTime + 100, true, true);
    RightDropTargets.ResetDropTargets(CurrentTime + 100, true, true);

    RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH, true);

    if (!RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 1000, true);
    LastTimeBallServed = CurrentTime + 1000;
    BallLaunched = false;
    
    LastShooterKickTime = 0;
    NumberOfBallsInPlay = 1;
    SkillshotLetter = 0;
    RPU_SetLampState(LAMP_LAUNCH_BUTTON, 1, 0, 150);
    BallLaunched = false;


    // Reallocate locks
    UpdateLockStatus();

    Audio.OutputTracksPlaying();
    PlayBackgroundSong(SOUND_EFFECT_RALLY_MUSIC_1 + (CurrentBallInPlay-1));
    Audio.OutputTracksPlaying();
  }

  byte ballInShooterLane = RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) ? 1 : 0;
  
  if ( ballInShooterLane && (CountBallsInTrough() + ballInShooterLane) == (TotalBallsLoaded - NumberOfBallsLocked) ) {
    WaitForPurgedLock = false;
    RPU_SetDisableFlippers(false);
    RPU_EnableSolenoidStack();
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  } else {

    if (!ballInShooterLane) {
      if (CurrentTime > (LastTimeBallServed+3000)) {
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 1000);
        LastTimeBallServed = CurrentTime + 1000;
      }
    }
    
    return MACHINE_STATE_INIT_NEW_BALL;
  }
  
  LastTimeThroughLoop = CurrentTime;
}



void AddToBuildBonus(byte bonusLampsFlag, unsigned long baseScore) {

  boolean soundPlayed = false;

  if (CollectFlipBonusTimeout==0) {
    if (BuildBonusLamps[CurrentPlayer] & bonusLampsFlag) {
      AddToBonus(1);
      int sfxNum = SOUND_EFFECT_BONUS_1 + (Bonus[CurrentPlayer]-1);
      if (sfxNum > SOUND_EFFECT_BONUS_17) sfxNum = SOUND_EFFECT_BONUS_17;
      PlaySoundEffect(sfxNum);
      soundPlayed = true;
    } else {
      BuildBonusLamps[CurrentPlayer] |= bonusLampsFlag;
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * baseScore;
    }
  }
  
  if (BuildBonusLamps[CurrentPlayer]==BUILD_BONUS_LAMPS_COMPLETE) {
    BuildBonusLamps[CurrentPlayer] = 0;
    BuildBonusLampsCompletionTime = CurrentTime;
    BuildBonusLampsCompletions[CurrentPlayer] += 1;

    if (OutlaneBallSaves && BuildBonusLampsCompletions[CurrentPlayer]<7) {
      if (BuildBonusLampsCompletions[CurrentPlayer]%2) {
        LeftOutlaneSaveSeconds[CurrentPlayer] += 1;
        if (NumberOfBallsInPlay==1) QueueNotification(SOUND_EFFECT_VP_ONE_SECOND_ADDED_TO_LEFT, 7);
      } else {
        RightOutlaneSaveSeconds[CurrentPlayer] += 1;
        if (NumberOfBallsInPlay==1) QueueNotification(SOUND_EFFECT_VP_ONE_SECOND_ADDED_TO_RIGHT, 7);
      }
    }

    soundPlayed = true;
    CollectFlipBonusTimeout = CurrentTime + ((unsigned long)BonusCollectHurryUp * 1000);
  }

  if (!soundPlayed) PlaySoundEffect(SOUND_EFFECT_SHORT_FILL_2);

}


unsigned long SaucerClosedStart = 0;

void CheckSaucerForStuckBall() {
  if (RPU_ReadSingleSwitchState(SW_SAUCER)) {
    LastTimeSaucerSeen = CurrentTime;
    if (SaucerClosedStart == 0) {
      SaucerClosedStart = CurrentTime;
    } else {
      if (CurrentTime > (SaucerClosedStart + 1500)) {
        SaucerClosedStart = 0;
        if (MiniModesQualified[CurrentPlayer] && GameMode == GAME_MODE_UNSTRUCTURED_PLAY) {
          SetGameMode(GAME_MODE_MINIMODE_START);
        } else if (GameMode == GAME_MODE_MINI_WIZARD_READY) {
          SetGameMode(GAME_MODE_MINI_WIZARD_START);
        } else if (GameMode == GAME_MODE_MINI_WIZARD) {
          if (!ScoreWizardShot(WIZARD_SHOT_SAUCER)) {
            PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
          }
          RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
        } else if (GameMode == GAME_MODE_MULTIBALL_3) {
          if (MB3JackpotQualified) {
            MB3JackpotQualified = 0;
            JakeStatus[CurrentPlayer] = 0;
            QueueNotification(SOUND_EFFECT_VP_JACKPOT_1 + CurrentTime%3, 7);
            RawhideJackpotsCollected[CurrentPlayer] += 1;
            Display_StartScoreAnimation(MULTIBALL_3_JACKPOT_VALUE, true);
            ActivateTopper();
          } else {
            CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 1000;
          }
          RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
        } else if (GameMode != GAME_MODE_MINIMODE_START) {
          RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
          AddToBuildBonus(BUILD_BONUS_LAMP_SAUCER, 100);
        }
      }
    }
  } else {
    SaucerClosedStart = 0;
  }

}

void UpdateDropTargets() {
  LeftDropTargets.Update(CurrentTime);
  RightDropTargets.Update(CurrentTime);

  ElwoodStatus[CurrentPlayer] = LeftDropTargets.GetStatus(false);
  ElwoodStatus[CurrentPlayer] |= (RightDropTargets.GetStatus(false)) * 0x08;
}




void UpdateMachineLocks() {

  boolean lockWithoutSwitch = false;
  if (  WaitingForTrapSwitchToHold!=0 && CurrentTime>(WaitingForTrapSwitchToHold+2000) /* && 
        CurrentTime > (LastSwitchHitTime + 1000) */) {
    // it appears that a second ball has been locked, but is not sitting
    // on the SW_TRAP switch
    WaitingForTrapSwitchToHold = 0;
    lockWithoutSwitch = true;
  }
  
  if (RPU_ReadSingleSwitchState(SW_TRAP) || lockWithoutSwitch) {
    if (WaitingForTrapSwitchToHold && CurrentTime>(WaitingForTrapSwitchToHold+500)) WaitingForTrapSwitchToHold = 0;
    if (TrapSwitchClosedTime == 0) TrapSwitchClosedTime = CurrentTime;
    if (CurrentTime > (TrapSwitchClosedTime + 1500) || lockWithoutSwitch) {
      TrapSwitchClosedTime = CurrentTime;
      if (GameMode==GAME_MODE_MULTIBALL_3) {
        if (MB3SuperJackpotQualified==3) {
          MB3SuperJackpotQualified = 0;
          QueueNotification(SOUND_EFFECT_VP_MEGA_JACKPOT, 8);
          ActivateTopper(true);
          RawhideJackpotsCollected[CurrentPlayer] += 3;
          Display_StartScoreAnimation(MULTIBALL_3_MEGA_JACKPOT_VALUE, true);
          LeftDropTargets.ResetDropTargets(CurrentTime + 250, true);
          RightDropTargets.ResetDropTargets(CurrentTime + 750, true);
          ElwoodStatus[CurrentPlayer] = 0;
        }
        // Close the top gate (hide away)
        TopGateCloseTime = 0;
        RPU_PushToTimedSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH, CurrentTime + 500, true);
        if (TopGateAvailableTime == 0) TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
        ReleaseLiftGate(1000);
      } else {
        // There's a ball in the trap
        if ( (MachineLocks & BALL_2_LOCKED) == 0 ) {
          if (GameMode != GAME_MODE_MULTIBALL_3_START && GameMode != GAME_MODE_MULTIBALL_3) {
            // We haven't seen this trapped ball before
            SetGameMode(GAME_MODE_LOCK_BALL_2);
          }
        }
      }
    }
  } else {
    TrapSwitchClosedTime = 0;
  }
}



void UpdateTopGate() {
  if (GameMode == GAME_MODE_UNSTRUCTURED_PLAY || GameMode == GAME_MODE_MINIMODE) {
    if (PlayerLocks[CurrentPlayer] & BALL_LOCKS_AVAILABLE_MASK && !RPU_ReadSingleSwitchState(SW_TRAP)) {
      if (TopGateAvailableTime && (CurrentTime > TopGateAvailableTime)) {
        PlaySoundEffect(SOUND_EFFECT_SHORT_SIREN);
        TopGateCloseTime = CurrentTime + TOP_GATE_OPEN_TIME_MS;
        TopGateCloseSoundPlayed = false;
        TopGateAvailableTime = 0;
        RPU_PushToSolenoidStack(SOL_TOP_GATE_OPEN, TOP_GATE_OPEN_SOLENOID_STRENGTH);
      }
    }
  } else if (GameMode == GAME_MODE_MULTIBALL_3) {
    // We're not re-opening the gate until the player hits the captive ball again
  }
}


void AddABall(boolean releaseLockIfNecessary = false) {
  if (CountBallsInTrough()) {
    RPU_PushToSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, true);
    BallLaunched = false;
    NumberOfBallsInPlay += 1;
    if (DEBUG_MESSAGES) {
      char buf[128];
      sprintf(buf, "Add ball (one in trough), BIP=%d\n", NumberOfBallsInPlay);
      Serial.write(buf);
    }
  } else if (releaseLockIfNecessary) {
    if (MachineLocks) {
      ReleaseLiftGate();
      NumberOfBallsInPlay += 1;
      if (DEBUG_MESSAGES) {
        char buf[128];
        sprintf(buf, "Add ball (one in lock), BIP=%d\n", NumberOfBallsInPlay);
        Serial.write(buf);
      }
    }
  }
}


void CheckFlipperStatus() {
  if (RPU_ReadSingleSwitchState(SW_LEFT_FLIPPER)) {
    if (LeftFlipperHoldTime==0) {
      LeftFlipperHoldTime = CurrentTime;
    }
    if (CurrentTime > (LeftFlipperHoldTime+3000)) {
      LeftFlipperBeingHeld = true;
      LastFlipperSeen = CurrentTime;
    } 
  } else {
    LeftFlipperHoldTime = 0;
    LeftFlipperBeingHeld = false;
  }  

  if (RPU_ReadSingleSwitchState(SW_RIGHT_FLIPPER)) {
    if (RightFlipperHoldTime==0) {
      RightFlipperHoldTime = CurrentTime;
    }
    if (CurrentTime > (RightFlipperHoldTime+3000)) {
      RightFlipperBeingHeld = true;
      LastFlipperSeen = CurrentTime;
    } 
  } else {
    RightFlipperHoldTime = 0;
    RightFlipperBeingHeld = false;
  }  
}


byte GameModeStage;
boolean DisplaysNeedRefreshing = false;
unsigned long LastTimePromptPlayed = 0;
unsigned long LastTimeAwardsChecked = 0;
unsigned long LastLoopTick = 0;

void ShowPlayerScoreAcrossBothDisplays(byte playerNum) {
  byte scoreMag = Display_MagnitudeOfScore(CurrentScores[playerNum]);
  unsigned long player1Display = 100000 * ((unsigned long)playerNum + 1);
  byte player1Blank = 0x01;
  if (scoreMag > 6) {
    player1Display += CurrentScores[playerNum] / 1000000;
    for (byte count=0; count<(scoreMag-6); count++) {
      player1Blank |= (0x20>>(scoreMag-7));
    }
  }
  Display_OverrideScoreDisplay(0, player1Display, DISPLAY_OVERRIDE_ANIMATION_NONE, player1Blank);
  if (CurrentScores[playerNum]>0) {
    Display_OverrideScoreDisplay(1, (CurrentScores[playerNum])%1000000, DISPLAY_OVERRIDE_ANIMATION_NONE);
  } else {
    Display_OverrideScoreDisplay(1, 0, DISPLAY_OVERRIDE_ANIMATION_NONE, 0x30);
  }
}


// This function manages all timers, flags, and lights
int ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  boolean specialAnimationRunning = false;
  boolean statusRunning = false;

  UpdateDropTargets();

  if ((CurrentTime - LastSwitchHitTime) > 3000) TimersPaused = true;
  else TimersPaused = false;

  if (LeftGateCloseTime) {
    if (CurrentTime > LeftGateCloseTime) {
      LeftGateCloseTime = 0;
      RPU_PushToSolenoidStack(SOL_LEFT_GATE_CLOSE, LEFT_GATE_CLOSE_SOLENOID_STRENGTH);
      LeftGateAvailableTime = CurrentTime + LEFT_GATE_DELAY_TIME_MS;
    }
  }


  if (TopGateCloseTime) {
    if ( ((CurrentTime + 2700) > TopGateCloseTime) && !TopGateCloseSoundPlayed ) {
      TopGateCloseSoundPlayed = true;
      PlaySoundEffect(SOUND_EFFECT_TOP_GATE_CLOSING);
    }

    if (CurrentTime > TopGateCloseTime) {
      TopGateCloseTime = 0;
      RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH);
      TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
      if (GameMode==GAME_MODE_MULTIBALL_3 && MB3SuperJackpotQualified==2) {
        MB3SuperJackpotQualified = 1;
      }
    }
  }

  if (LeftSpinnerLitTime && CurrentTime > (LeftSpinnerLitTime + 5000)) {
    LeftSpinnerLitTime = 0;
  }

  if (CollectFlipBonusTimeout && CurrentTime > CollectFlipBonusTimeout) {
    CollectFlipBonusTimeout = 0;
  }

  if (LeftOutlaneSaveEndTime && CurrentTime > LeftOutlaneSaveEndTime) {
    LeftOutlaneSaveEndTime = 0;
  }
  if (RightOutlaneSaveEndTime && CurrentTime > RightOutlaneSaveEndTime) {
    RightOutlaneSaveEndTime = 0;
  }

  if (BuildBonusLampsCompletionTime && CurrentTime > (BuildBonusLampsCompletionTime+2000)) {
    BuildBonusLampsCompletionTime = 0;
  }

  CheckFlipperStatus();
  CheckSaucerForStuckBall();
  UpdateMachineLocks();
  UpdateTopGate();

  if (NextBonusPayoutTime && (CurrentTime > NextBonusPayoutTime)) {
    if (Bonus[CurrentPlayer]) {
      NextBonusPayoutTime = CurrentTime + 1000;
      CurrentScores[CurrentPlayer] += 10000 * PlayfieldMultiplier;
      Bonus[CurrentPlayer] -= 1;
      PlaySoundEffect(SOUND_EFFECT_BONUS_PAYOUT);
    } else {
      NextBonusPayoutTime = 0;
    }
  }

  // Ball Search
  if (  GameMode!=GAME_MODE_SKILL_SHOT && !LeftFlipperBeingHeld && !RightFlipperBeingHeld && 
        CurrentTime > (LastFlipperSeen + BALL_SEARCH_INACTIVITY_DELAY) && 
        CurrentTime > (LastSwitchHitTime + BALL_SEARCH_INACTIVITY_DELAY) && 
        BallLaunched && !RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
    byte solToFire = ((CurrentTime - (LastSwitchHitTime + BALL_SEARCH_INACTIVITY_DELAY))/1000) % BALL_SEARCH_NUMBER_OF_SOLENOIDS;

    if (solToFire!=BallSearchLastSolenoidFired) {
      BallSearchLastSolenoidFired = solToFire;
      RPU_PushToSolenoidStack(BallSearchSolenoid[BallSearchLastSolenoidFired], BallSearchSolenoidStrength[BallSearchLastSolenoidFired], true);
      if (BallSearchSolenoid[BallSearchLastSolenoidFired]==SOL_LIFT_GATE) {
        if (MachineLocks) {
          UpdateLockStatus(true);
        }
      }
      if (DEBUG_MESSAGES) {
        char buf[128];
        sprintf(buf, "Ball Search firing %d for %d\n", BallSearchSolenoid[BallSearchLastSolenoidFired], BallSearchSolenoidStrength[BallSearchLastSolenoidFired]);
        Serial.write(buf);
      }
    }
  } else {
    BallSearchLastSolenoidFired = 0xFF;
  }

  // Status Mode
  if ( (LeftFlipperBeingHeld || RightFlipperBeingHeld) && CurrentTime > (LastSwitchHitTime + ((unsigned long)StatusModeInactivityDelay*1000))) {
    if (CurrentStatusReportPage==0) {
      CurrentStatusReportPage = 1;
      LastStatusReportPage = 0;
    } else if (CurrentStatusReportPage>=2 && GameMode!=GAME_MODE_MULTIBALL_3_START) {
      statusRunning = true;
      if (CurrentStatusReportPage != LastStatusReportPage) {
        Audio.StopAllNotifications();
        if (CurrentStatusReportPage==2) {
          //RPU_TurnOffAllLamps();
        }
        int spinnerHitsRemaining;
        byte count;
        byte otherPlayerNums[3] = {1, 2, 3};
        if (CurrentStatusReportPage==2 && CurrentNumPlayers==1) {
          CurrentStatusReportPage = 5;
        } else if (CurrentStatusReportPage==3 && CurrentNumPlayers==2) {
          CurrentStatusReportPage = 5;
        } else if (CurrentStatusReportPage==4 && CurrentNumPlayers==3) {
          CurrentStatusReportPage = 5;
        }
        byte altPlayer = 0;
        for (count=0; count<(CurrentNumPlayers-1); count++) {
          if (altPlayer==CurrentPlayer) altPlayer += 1;
          otherPlayerNums[count] = altPlayer;
          altPlayer += 1;
        }
        LastStatusReportPage = CurrentStatusReportPage;
        for (byte count = 0; count < 4; count++) {
          if (count==CurrentPlayer) RPU_SetLampState(PlayerUpLamps[count], 1, 0, 500);
          else if (CurrentStatusReportPage<5 && count==otherPlayerNums[CurrentStatusReportPage-2]) RPU_SetLampState(PlayerUpLamps[count], 1, 0, 75);
          else RPU_SetLampState(PlayerUpLamps[count], 0);
        }
        
        switch ((CurrentStatusReportPage-1)) {
          case 1:
            // Show other players' scores (when applicable)
            ShowPlayerScoreAcrossBothDisplays(otherPlayerNums[0]);
            QueueNotification(SOUND_EFFECT_VP_PLAYER_ONE_SCORE + otherPlayerNums[0], 1);
            break;
          case 2:
            // Show other players' scores (when applicable)
            ShowPlayerScoreAcrossBothDisplays(otherPlayerNums[1]);
            QueueNotification(SOUND_EFFECT_VP_PLAYER_ONE_SCORE + otherPlayerNums[1], 1);
            break;
          case 3:
            // Show other players' scores (when applicable)                        
            ShowPlayerScoreAcrossBothDisplays(otherPlayerNums[2]);
            QueueNotification(SOUND_EFFECT_VP_PLAYER_ONE_SCORE + otherPlayerNums[2], 1);
            break;
          case 4:
            // show lock status
            if (PlayerLocks[CurrentPlayer]==0x00) QueueNotification(SOUND_EFFECT_VP_NO_LOCKS_NONE_QUALIFIED, 1);
            if (PlayerLocks[CurrentPlayer]==0x10) QueueNotification(SOUND_EFFECT_VP_NO_LOCKS_ONE_QUALIFIED, 1);
            if (PlayerLocks[CurrentPlayer]==0x30) QueueNotification(SOUND_EFFECT_VP_NO_LOCKS_TWO_QUALIFIED, 1);
            if (PlayerLocks[CurrentPlayer]==0x01) QueueNotification(SOUND_EFFECT_VP_ONE_LOCK_NONE_QUALIFIED, 1);
            if (PlayerLocks[CurrentPlayer]==0x21) QueueNotification(SOUND_EFFECT_VP_ONE_LOCK_ONE_QUALIFIED, 1);
            if (PlayerLocks[CurrentPlayer]==0x03) QueueNotification(SOUND_EFFECT_VP_TWO_LOCKS, 1);
            Display_OverrideScoreDisplay(0, CountBits(PlayerLocks[CurrentPlayer]&BALL_LOCKS_MASK), DISPLAY_OVERRIDE_ANIMATION_CENTER);
            Display_OverrideScoreDisplay(1, CountBits(PlayerLocks[CurrentPlayer]&BALL_LOCKS_AVAILABLE_MASK), DISPLAY_OVERRIDE_ANIMATION_CENTER);
//            RPU_SetLampState(LAMP_HIDE_AWAY, 1, 0, 75);
//            RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 1, 0, 75);
//            RPU_SetLampState(LAMP_CAPTIVE_BALL, 1, 0, 75);
            break;
          case 5:
            // show bonus multiplier
            if (BonusX[CurrentPlayer]==2) QueueNotification(SOUND_EFFECT_VP_BONUS_2X, 1);
            if (BonusX[CurrentPlayer]==3) QueueNotification(SOUND_EFFECT_VP_BONUS_3X, 1);
            if (BonusX[CurrentPlayer]==5) QueueNotification(SOUND_EFFECT_VP_BONUS_5X, 1);
            if (BonusX[CurrentPlayer]==10) QueueNotification(SOUND_EFFECT_VP_BONUS_10X, 1);
            if (BonusX[CurrentPlayer]==1) QueueNotification(SOUND_EFFECT_VP_BONUS_1X, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, BonusX[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
//            RPU_SetLampState(LAMP_HIDE_AWAY, 0);
//            RPU_SetLampState(LAMP_BEHIND_CAPTIVE, 0);
//            RPU_SetLampState(LAMP_CAPTIVE_BALL, 0);
            break;
          case 6:
            // show total bonus
            QueueNotification(SOUND_EFFECT_VP_TOTAL_BONUS, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, ((unsigned long)Bonus[CurrentPlayer] * 1000) * ((unsigned long)BonusX[CurrentPlayer]), DISPLAY_OVERRIDE_ANIMATION_NONE);
            break;
          case 7:
            for (byte count = 0; count < 6; count++) {
              RPU_SetLampState(ElwoodLampAssignments[count], 1, 0, 75);
            }          
            if (ElwoodShotsMade[CurrentPlayer]) {
              QueueNotification(SOUND_EFFECT_VP_ELWOOD_SHOTS_COLLECTED, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, ElwoodShotsMade[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            } else if (MiniModesQualified[CurrentPlayer] & ELWOOD_MINIMODE_FLAG) {
              QueueNotification(SOUND_EFFECT_VP_ELWOOD_MODE_QUALIFIED, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, 0, DISPLAY_OVERRIDE_ANIMATION_CENTER, 0x00);
            } else if (MiniModesRunning & ELWOOD_MINIMODE_FLAG) {
              QueueNotification(SOUND_EFFECT_VP_ELWOOD_MODE_RUNNING, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, 0, DISPLAY_OVERRIDE_ANIMATION_CENTER, 0x00);
            } else {
              QueueNotification(SOUND_EFFECT_VP_FINISH_ELWOOD, 1);
              if (((ElwoodCompletions[CurrentPlayer]+1) % ElwoodClearsToQualify)==4) QueueNotification(SOUND_EFFECT_VP_FIVE_MORE_TIMES, 1);
              if (((ElwoodCompletions[CurrentPlayer]+1) % ElwoodClearsToQualify)==3) QueueNotification(SOUND_EFFECT_VP_FOUR_MORE_TIMES, 1);
              if (((ElwoodCompletions[CurrentPlayer]+1) % ElwoodClearsToQualify)==2) QueueNotification(SOUND_EFFECT_VP_THREE_MORE_TIMES, 1);
              if (((ElwoodCompletions[CurrentPlayer]+1) % ElwoodClearsToQualify)==1) QueueNotification(SOUND_EFFECT_VP_TWICE_MORE, 1);
              if (((ElwoodCompletions[CurrentPlayer]+1) % ElwoodClearsToQualify)==0) QueueNotification(SOUND_EFFECT_VP_ONCE_MORE, 1);
              QueueNotification(SOUND_EFFECT_VP_TO_QUALIFY_MODE, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, ((ElwoodCompletions[CurrentPlayer]+1) % ElwoodClearsToQualify)+1, DISPLAY_OVERRIDE_ANIMATION_CENTER);
            }
            break;
          case 8:
            for (byte count = 0; count < 6; count++) {
              RPU_SetLampState(ElwoodLampAssignments[count], 0);
            }            
            for (byte count = 0; count < 4; count++) {
              RPU_SetLampState(JakeLampAssignments[count], 1, 0, 75);
            }
            if (JakeShotsMade[CurrentPlayer]) {
              QueueNotification(SOUND_EFFECT_VP_JAKE_SHOTS_COLLECTED, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, JakeShotsMade[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            } else if (MiniModesQualified[CurrentPlayer] & JAKE_MINIMODE_FLAG) {
              QueueNotification(SOUND_EFFECT_VP_JAKE_MODE_QUALIFIED, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, 0, DISPLAY_OVERRIDE_ANIMATION_CENTER, 0x00);
            } else if (MiniModesRunning & JAKE_MINIMODE_FLAG) {
              QueueNotification(SOUND_EFFECT_VP_JAKE_MODE_RUNNING, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, 0, DISPLAY_OVERRIDE_ANIMATION_CENTER, 0x00);
            } else {
              QueueNotification(SOUND_EFFECT_VP_FINISH_JAKE, 1);
              if (((JakeCompletions[CurrentPlayer]+1) % JakeClearsToQualify)==4) QueueNotification(SOUND_EFFECT_VP_FIVE_MORE_TIMES, 1);
              if (((JakeCompletions[CurrentPlayer]+1) % JakeClearsToQualify)==3) QueueNotification(SOUND_EFFECT_VP_FOUR_MORE_TIMES, 1);
              if (((JakeCompletions[CurrentPlayer]+1) % JakeClearsToQualify)==2) QueueNotification(SOUND_EFFECT_VP_THREE_MORE_TIMES, 1);
              if (((JakeCompletions[CurrentPlayer]+1) % JakeClearsToQualify)==1) QueueNotification(SOUND_EFFECT_VP_TWICE_MORE, 1);
              if (((JakeCompletions[CurrentPlayer]+1) % JakeClearsToQualify)==0) QueueNotification(SOUND_EFFECT_VP_ONCE_MORE, 1);
              QueueNotification(SOUND_EFFECT_VP_TO_QUALIFY_MODE, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, ((JakeCompletions[CurrentPlayer]+1) % JakeClearsToQualify)+1, DISPLAY_OVERRIDE_ANIMATION_CENTER);
            }
            break;
          case 9:
            for (byte count = 0; count < 4; count++) {
              RPU_SetLampState(JakeLampAssignments[count], 0);
            }            
            RPU_SetLampState(LAMP_LEFT_SPINNER, 1, 0, 75);
            if (BandShotsMade[CurrentPlayer]) {
              QueueNotification(SOUND_EFFECT_VP_BAND_SHOTS_COLLECTED, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, BandShotsMade[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            } else if (MiniModesQualified[CurrentPlayer] & BAND_MINIMODE_FLAG) {
              QueueNotification(SOUND_EFFECT_VP_BAND_MODE_QUALIFIED, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, 0, DISPLAY_OVERRIDE_ANIMATION_CENTER, 0x00);
            } else if (MiniModesRunning & BAND_MINIMODE_FLAG) {
              QueueNotification(SOUND_EFFECT_VP_BAND_MODE_RUNNING, 1);
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, 0, DISPLAY_OVERRIDE_ANIMATION_CENTER, 0x00);
            } else {
              QueueNotification(SOUND_EFFECT_VP_SPINS_NEEDED, 1);
              QueueNotification(SOUND_EFFECT_VP_TO_QUALIFY_MODE, 1);
              spinnerHitsRemaining = SpinnerHitsToQualifyBand - SpinnerProgress[CurrentPlayer];
              for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, spinnerHitsRemaining, DISPLAY_OVERRIDE_ANIMATION_CENTER);
            }
            break;
          case 10:
            QueueNotification(SOUND_EFFECT_VP_RAWHIDE_JACKPOTS_COLLECTED, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, RawhideJackpotsCollected[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            RPU_SetLampState(LAMP_LEFT_SPINNER, 0);
            break;
          case 11:
            QueueNotification(SOUND_EFFECT_VP_CONCERT_JACKPOTS_COLLECTED, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, ConcertJackpotsCollected[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            break;
          case 12:
            QueueNotification(SOUND_EFFECT_VP_JOLIET_JACKPOTS_COLLECTED, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, JolietJackpotsCollected[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            break;
          case 13:
            if ((MaxTiltWarnings - NumTiltWarnings)==3) QueueNotification(SOUND_EFFECT_VP_THREE_TILT_WARNINGS_LEFT, 1);
            else if ((MaxTiltWarnings - NumTiltWarnings)==2) QueueNotification(SOUND_EFFECT_VP_TWO_TILT_WARNINGS_LEFT, 1);
            else if ((MaxTiltWarnings - NumTiltWarnings)==1) QueueNotification(SOUND_EFFECT_VP_ONE_TILT_WARNING_LEFT, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, MaxTiltWarnings-NumTiltWarnings, DISPLAY_OVERRIDE_ANIMATION_CENTER);
            break;
          case 14:         
            // show number of flip lanes
            QueueNotification(SOUND_EFFECT_VP_NUMBER_OF_RIGHT_FLIPS, 1);
            for (count=0; count<RPU_NUMBER_OF_PLAYER_DISPLAYS; count++) Display_OverrideScoreDisplay(count, RightFlipLanes[CurrentPlayer], DISPLAY_OVERRIDE_ANIMATION_CENTER);
            break;
        }
      }
    }
  } else {
    if (LastStatusReportPage) {
      Display_ClearOverride(0xFF);
    }
    CurrentStatusReportPage = 0;
    LastStatusReportPage = 0; 
  }

  switch ( GameMode ) {
    case GAME_MODE_SKILL_SHOT:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
        LastTimePromptPlayed = CurrentTime;
        GameModeStage = 0;
        SetGeneralIlluminationOn(true);
        NumberOfShortPlunges = 0;
        MB3JackpotQualified = 0;
        MB3SuperJackpotQualified = 0;
      }

      // The switch handler will award the skill shot
      // (when applicable) and this mode will move
      // to unstructured play when any valid switch is
      // recorded

      if (CurrentTime > (LastTimePromptPlayed + 20000)) {
        //RPU_FireContinuousSolenoid(0x20, 30);
        AlertPlayerUp();
        LastTimePromptPlayed = CurrentTime;
        Audio.OutputTracksPlaying();
      }

      if (CountBallsInTrough()==3) {
        // Ball didn't kick right?
        if (!RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) && CurrentTime > (LastTimeBallServed+2000)) {
          RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime);
          LastTimeBallServed = CurrentTime;
        }

      }
      
      // If we've seen a tilt before plunge, then
      // we can show a countdown timer here
      if (LastTiltWarningTime) {
        if ( CurrentTime > (LastTiltWarningTime + 30000) ) {
          LastTiltWarningTime = 0;
        } else {
          byte secondsSinceWarning = (CurrentTime - LastTiltWarningTime) / 1000;
          for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
            if (count == CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count%RPU_NUMBER_OF_PLAYER_DISPLAYS, 30 - secondsSinceWarning, DISPLAY_OVERRIDE_ANIMATION_CENTER);
          }
          DisplaysNeedRefreshing = true;
        }
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        if (!statusRunning) Display_ClearOverride(0xFF);
      } else {
        if ( ((CurrentTime/1000)%10)>7 ) {        
          if (GameModeStage!=2) {
            for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
              if (!statusRunning && count==CurrentPlayer) Display_OverrideScoreDisplay(count%RPU_NUMBER_OF_PLAYER_DISPLAYS, ((unsigned long)CurrentPlayer + 1) * 111111, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
            }
            GameModeStage = 2;
          }
        } else {
          if (GameModeStage!=1) {
            if (!statusRunning) Display_ClearOverride(0xFF);
            GameModeStage = 1;
          }
        }
      }

      if (NumberOfShortPlunges > 3 || BallFirstSwitchHitTime != 0) {
        Display_ClearOverride(0xFF);
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      break;

    case GAME_MODE_UNSTRUCTURED_PLAY:
      // If this is the first time in this mode
      if (GameModeStartTime == 0) {
        RPU_SetContinuousSolenoidBit(true, 0x20);
        GameModeStartTime = CurrentTime;
        DisplaysNeedRefreshing = true;
        if (DEBUG_MESSAGES) {
          Serial.write("Entering unstructured play\n");
        }
        SetGeneralIlluminationOn(true);
        unsigned short songNum = SOUND_EFFECT_BACKGROUND_SONG_1;
        if (CurrentBallInPlay>1 && CurrentBallInPlay<5) songNum = SOUND_EFFECT_BACKGROUND_SONG_2 + (CurrentTime%3);
        if (CurrentBallInPlay==BallsPerGame) songNum = SOUND_EFFECT_BACKGROUND_SONG_5;
        if (Audio.GetBackgroundSong() != songNum) {
          PlayBackgroundSong(songNum);
        }
        GameModeStage = 0;
        MB3JackpotQualified = 0;
        MB3SuperJackpotQualified = 0;
        LastTimePromptPlayed = 0;
        LastTimeAwardsChecked = CurrentTime;
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 2000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 750, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
      }

      // Display Overrides in Unstructured Play
      if (MiniModeMultiplierExpiration) {
        if (DisplaysNeedRefreshing) {
          DisplaysNeedRefreshing = false;
          Display_ClearOverride(0xFF);
        }

        byte animationStep = ((CurrentTime / 20) % LAMP_ANIMATION_STEPS);
        byte lampShowPhase = ((CurrentTime / (20*LAMP_ANIMATION_STEPS)) % 3); 
        if (!statusRunning && (lampShowPhase==0)) {
          ShowLampAnimationSingleStep(3, LAMP_ANIMATION_STEPS - (animationStep + 1));
          specialAnimationRunning = true;
        }

        if (CurrentTime > MiniModeMultiplierExpiration) {
          MiniModeMultiplierExpiration = 0;
//          if (ResetQualifiedModesAtEndOfTimer) MiniModesQualified[CurrentPlayer] = 0;
          GameModeStage = 0;
          DisplaysNeedRefreshing = false;
          Display_ClearOverride(0xFF);
          PlaySoundEffect(SOUND_EFFECT_MISSED_MODE_SHOT);
        } else {
          if ( (CurrentTime + 10000) > MiniModeMultiplierExpiration ) {
            for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
              if (count != CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count, (MiniModeMultiplierExpiration - CurrentTime) / 1000, DISPLAY_OVERRIDE_ANIMATION_CENTER);
            }
            if (GameModeStage == 0) {
              GameModeStage = 1;
              PlaySoundEffect(SOUND_EFFECT_10_SECOND_WARNING);
            }
          }
        }
      } else if (LastSpinnerHitTime) {

        if (CurrentTime > (LastSpinnerHitTime + 5000)) {
          LastSpinnerHitTime = 0;
        } else {
          if (SpinnerProgress[CurrentPlayer] && SpinnerProgress[CurrentPlayer] < SpinnerHitsToQualifyBand) {
            int spinnerHitsRemaining = SpinnerHitsToQualifyBand - SpinnerProgress[CurrentPlayer];
            if (!statusRunning) Display_OverrideScoreDisplay(CurrentPlayer%RPU_NUMBER_OF_PLAYER_DISPLAYS, spinnerHitsRemaining, DISPLAY_OVERRIDE_ANIMATION_CENTER);
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
            if (count != CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_ANIMATION_BOUNCE);
          }
          DisplaysNeedRefreshing = true;
        }
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
      }

      if (LastFlipLaneTime) {
        if (CurrentTime > (LastFlipLaneTime+750)) {
          LastFlipLaneTime = 0;
        } else {
          specialAnimationRunning = true;
          byte radarPhase = ((CurrentTime - LastFlipLaneTime)/32);
          ShowLampAnimationSingleStep(0, radarPhase);
        }
      } else if (LastTimePopHit) {
        if (CurrentTime < (LastTimePopHit+480)) {
          specialAnimationRunning = true;
          byte popOutPhase = ((CurrentTime - LastTimePopHit)/20);
          ShowLampAnimationSingleStep(4, popOutPhase);
        }
      }

      // Check to see if player has earned an extra ball lamp
      if (CurrentTime > (LastTimeAwardsChecked+1000) && ExtraBallAvailable[CurrentPlayer]==0) {
        LastTimeAwardsChecked = CurrentTime;
        if (  ((MiniModesQualified[CurrentPlayer] & JAKE_MINIMODE_FLAG) || JakeShotsMade[CurrentPlayer]) && 
              ((MiniModesQualified[CurrentPlayer] & ELWOOD_MINIMODE_FLAG) || ElwoodShotsMade[CurrentPlayer]) && 
              ((MiniModesQualified[CurrentPlayer] & BAND_MINIMODE_FLAG) || BandShotsMade[CurrentPlayer]) ) {
          ExtraBallAvailable[CurrentPlayer] = 1;        
        }
      }

      // Check to see if we should launch the wizard or mini-wizard
      if (ElwoodShotsMade[CurrentPlayer] && JakeShotsMade[CurrentPlayer] && BandShotsMade[CurrentPlayer]) {
        // the player has collected shots in each of the three minimodes, so they should play the mini-wizard
        SetGameMode(GAME_MODE_MINI_WIZARD_READY);
      } else if (RawhideJackpotsCollected[CurrentPlayer] && ConcertJackpotsCollected[CurrentPlayer]) {
        SetGameMode(GAME_MODE_FULL_WIZARD_READY);
      }
        

      break;

    case GAME_MODE_MINIMODE_START:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + 2921;
        PlayBackgroundSong(SOUND_EFFECT_MINIMODE_SONG_1);
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 500);
        BallLaunched = false;
        NumberOfBallsInPlay += 1;
        GameModeStage = 0;
        AddABallUsed = false;
        RightFlipsSeen = 0;

        if (MiniModesQualified[CurrentPlayer] & ELWOOD_MINIMODE_FLAG) {
          LeftDropTargets.ResetDropTargets(CurrentTime + 250, true);
          RightDropTargets.ResetDropTargets(CurrentTime + 750, true);
          ElwoodStatus[CurrentPlayer] = 0;
        }
        ActivateTopper(true);
      }

      if (!statusRunning) {
        specialAnimationRunning = LampAnimationMinimodeStart( CurrentTime - GameModeStartTime,
                                  (MiniModesQualified[CurrentPlayer] & JAKE_MINIMODE_FLAG) ? true : false,
                                  (MiniModesQualified[CurrentPlayer] & ELWOOD_MINIMODE_FLAG) ? true : false,
                                  (MiniModesQualified[CurrentPlayer] & BAND_MINIMODE_FLAG) ? true : false );
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE) && CurrentTime > (GameModeStartTime + 1800)) {
        if (GameModeStage == 0) {
          RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
        GameModeStage = 1;
      }

      if (GameModeStage == 1 && (CurrentTime > GameModeEndTime)) {
        RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
        MiniModesRunning = MiniModesQualified[CurrentPlayer];
        MiniModesQualified[CurrentPlayer] = 0;
        ElwoodTargetLit = 0;
        JakeTargetLit = 0;
        SetGameMode(GAME_MODE_MINIMODE);
        if (BallSaveOnMinimodes) {
          SetBallSave(((unsigned long)BallSaveOnMinimodes * 1000));
        }
      }
      break;

    case GAME_MODE_MINIMODE:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        if (MiniModeMultiplierExpiration) PlayfieldMultiplier = 1 + CountBits(MiniModesRunning);
        else PlayfieldMultiplier = 1;
      }

      JakeTargetLit = (CurrentTime / 250) % 4;

      if (PlayfieldMultiplier>1) {
        for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          if (count != CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE);
        }
        DisplaysNeedRefreshing = true;
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 1000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 100, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
      }

      if (NumberOfBallsInPlay == 1) {
        SetGameMode(GAME_MODE_MINIMODE_END);
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
      }
      break;

    case GAME_MODE_MINIMODE_END:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + 1000;
        MiniModeMultiplierExpiration = 0;
        MiniModesRunning = 0;
        PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_SONG_1);
        PlayfieldMultiplier = 1;
        if (ExtraBallAvailable[CurrentPlayer]==2) ExtraBallAvailable[CurrentPlayer] = 0;
      }

      if (CurrentTime > GameModeEndTime) {
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      break;

    case GAME_MODE_LOCK_BALL_1:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
        QueueNotification(SOUND_EFFECT_VP_BALL_1_LOCKED, 10);
        GameModeStage = 0;
        GameModeEndTime = CurrentTime + 2000;
        if (LockHandling == LOCK_HANDLING_VIRTUAL_LOCKS) {
          ReleaseLiftGate();
          GameModeStage = 1;
        } else {
          NumberOfBallsLocked = 1;
          MachineLocks = BALL_1_LOCKED;
          if (NumberOfBallsInPlay==3) {
            NumberOfBallsInPlay = 2;
            GameModeStage = 1;
          } else {
            RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 500);
            BallLaunched = false;
          }
        }
        PlayerLocks[CurrentPlayer] = BALL_1_LOCKED | (PlayerLocks[CurrentPlayer] & BALL_LOCKS_AVAILABLE_MASK);
        PlayerLocks[CurrentPlayer] &= ~BALL_1_LOCK_AVAILABLE;
        if (DEBUG_MESSAGES) {
          char buf[128];
          sprintf(buf, "Ball 1 lock, ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
          Serial.write(buf);
        }
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (GameModeStage == 0) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 100, true);
          BallLaunched = true;
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
        Display_ClearOverride(0xFF);
        if (DEBUG_MESSAGES) {
          char buf[128];
          sprintf(buf, "Ball 2 lock start, ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
          Serial.write(buf);
        }
        QueueNotification(SOUND_EFFECT_VP_BALL_2_LOCKED, 10);
        PlayerLocks[CurrentPlayer] &= ~BALL_LOCKS_AVAILABLE_MASK;
        PlayerLocks[CurrentPlayer] |= BALL_2_LOCKED;
        GameModeStage = 0;
        if (LockHandling == LOCK_HANDLING_VIRTUAL_LOCKS) {
          ReleaseLiftGate();
          GameModeStage = 2;
        } else {
          if (MachineLocks==0) {
            NumberOfBallsLocked = 1;
            MachineLocks |= BALL_1_LOCKED;
            if (NumberOfBallsInPlay==1) {
              RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 500);
              BallLaunched = false;
            } else {
              NumberOfBallsInPlay = 1;
              GameModeStage = 1;
            }
          } else {
            NumberOfBallsLocked = 2;
            MachineLocks |= BALL_2_LOCKED;
            if (NumberOfBallsInPlay==1) {
              RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 500);
              BallLaunched = false;
            } else {
              NumberOfBallsInPlay = 1;
              GameModeStage = 1;
            }
          }
        }
        GameModeEndTime = CurrentTime + 2000;
        SetBallSave(2000);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (GameModeStage == 0) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 100, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
        GameModeStage = 1;
      }

      if (GameModeStage == 1 && CurrentTime > (LastShooterKickTime + 1000) && CountBallsInTrough()) {
        if ( (MachineLocks & BALL_1_LOCKED) && (MachineLocks & BALL_2_LOCKED) ) {
          // Clearing out the trough because we're in lock 2 (so all balls should be on playfield or locked)
          RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 500);
          BallLaunched = false;
          GameModeStage = 0;
        }
      }

      if (GameModeStage >= 1 && (CurrentTime > GameModeEndTime)) {
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
        MiniModesQualified[CurrentPlayer] = 0;
        MiniModeMultiplierExpiration = 0;
        GameModeStartTime = CurrentTime;
        GameModeStage = 0;
        GameModeEndTime = CurrentTime + 25000;
        Multiball3StartShotHit = false;
        Multiball3StartAward = 10000;
        SetBallSave(25000);
        ActivateTopper(true);
      }

      if (Multiball3StartShotHit) {
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
        SetGameMode(GAME_MODE_MULTIBALL_3);
      } else {
        if (CurrentTime < GameModeEndTime) Multiball3StartAward = ((((GameModeEndTime - CurrentTime) / 1000) + 1) * 1000) + 10000;
        else Multiball3StartAward = 10000;
        for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          if (count != CurrentPlayer) Display_OverrideScoreDisplay(count, Multiball3StartAward, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
        }
      }

      if (CurrentTime > GameModeEndTime) {
        // The player didn't start multiball themselves, so we have to start it now
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
        SetGameMode(GAME_MODE_MULTIBALL_3);
      }
      break;

    case GAME_MODE_MULTIBALL_3:
      if (GameModeStartTime == 0) {
        if (DEBUG_MESSAGES) {
          Serial.write("MB3 start\n");
        }
        GameModeStartTime = CurrentTime;
        Display_StartScoreAnimation(Multiball3StartAward, true);
        PlaySoundEffect(SOUND_EFFECT_JAIL_BREAK);
        ReleaseLiftGate();
        NumberOfBallsInPlay = 3;
        if (DEBUG_MESSAGES) {
          Serial.write("MB3 - number of BIP=3\n");
        }
        MachineLocks = 0;
        PlayerLocks[CurrentPlayer] = 0;
        NumberOfBallsLocked = 0;
        unsigned short songNum = SOUND_EFFECT_MULTIBALL_SONG_1;
        PlayBackgroundSong(songNum);
        SetBallSave((unsigned long)BallSaveOnMultiball * 1000);
        MB3JackpotQualified = 1;
        MB3SuperJackpotQualified = 1;
      }

      PlayfieldMultiplier = NumberOfBallsInPlay + CountBits(MiniModesRunning);

      for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
        if (count != CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 1000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 100, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
      }

      if (NumberOfBallsInPlay == 1) {
        if (MiniModesRunning) {
          SetGameMode(GAME_MODE_MINIMODE_END);
        } else {
          SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
        }
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
      }
      break;
    case GAME_MODE_MINI_WIZARD_READY:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
        // Announce Mini Wizard
        QueueNotification(SOUND_EFFECT_VP_MINI_WIZARD_INTRO, 10);
        JackpotValue = 10000;
        JackpotValue += ((unsigned long)ElwoodShotsMade[CurrentPlayer]) * 5000;
        JackpotValue += ((unsigned long)JakeShotsMade[CurrentPlayer]) * 2000;
        JackpotValue += ((unsigned long)BandShotsMade[CurrentPlayer]) * 1000;
        ElwoodShotsMade[CurrentPlayer] = 0;
        JakeShotsMade[CurrentPlayer] = 0;
        BandShotsMade[CurrentPlayer] = 0;
        Audio.StopAllMusic();

        LeftDropTargets.ResetDropTargets(CurrentTime + 250, true);
        RightDropTargets.ResetDropTargets(CurrentTime + 750, true);
        ElwoodStatus[CurrentPlayer] = 0;
        GameModeStage = 0;
        LastShooterKickTime = 0;
        PlayBackgroundSong(SOUND_EFFECT_INTRO_LOOP);
      }
      SetBallSave(1000);
      // Light show directing the player to start the mini wizard

//      if (Audio.GetBackgroundSong()==BACKGROUND_TRACK_NONE) {
//        Audio.PlayBackgroundSong(SOUND_EFFECT_INTRO_LOOP, false);
//      }

      // if they don't start it with the saucer, it will start on drain with a lower jackpot
      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (GameModeStage==0) {
          JackpotValue /= 2;
          QueueNotification(SOUND_EFFECT_VP_MINI_WIZARD_HALF_JACKPOT, 7);
          GameModeEndTime = CurrentTime + 1800;
          GameModeStage = 1;
        }

        if (CurrentTime > LastShooterKickTime + 2000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 750, true);
          LastShooterKickTime = CurrentTime;
          BallLaunched = true;
        }
        
      }

      if (GameModeEndTime && CurrentTime>GameModeEndTime) {
        SetGameMode(GAME_MODE_MINI_WIZARD_START);
      }

      break;
      
    case GAME_MODE_MINI_WIZARD_START:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + 2000;
        if (DEBUG_MESSAGES) {
          Serial.write("Mini wizard start\n");
        }
        SetBallSave(2000);

        // Announce concert multiball
        Audio.StopAllMusic();
        Audio.StopAllNotifications();
        Audio.PlayBackgroundSong(SOUND_EFFECT_MINI_WIZ_START_SONG, false);
        //QueueNotification(SOUND_EFFECT_VP_MINI_WIZARD_START, 10);
        ActivateTopper(true);
      }

      for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
        if (count != CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count, JackpotValue, DISPLAY_OVERRIDE_ANIMATION_FLUTTER);
      }      

      if (CurrentTime > GameModeEndTime) {
        if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 100, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
        if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
          RPU_PushToTimedSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, CurrentTime + 200, true);
        }
        if (MachineLocks) {
          ReleaseLiftGate();
        }
        
        SetGameMode(GAME_MODE_MINI_WIZARD);
      }
      break;
      
    case GAME_MODE_MINI_WIZARD:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        if (MachineLocks) {
          PlaySoundEffect(SOUND_EFFECT_JAIL_BREAK);
          ReleaseLiftGate();
        }
        RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH);
        SetBallSave((unsigned long)BallSaveOnMiniWizard * 1000);
        NumberOfBallsInPlay = 3;
        MachineLocks = 0;
        PlayerLocks[CurrentPlayer] = 0;
        NumberOfBallsLocked = 0;
        WizardShotsCompleted = 0;
        WizardShotsRotateTime = CurrentTime + 3100;
        if (DEBUG_MESSAGES) {
          Serial.write("Mini wizard - number of BIP=3\n");
        }
        GameModeStage = 0;
      }

      if (Audio.GetBackgroundSong()==BACKGROUND_TRACK_NONE) {
        PlayBackgroundSong(SOUND_EFFECT_MINI_WIZ_MUSIC_1);
      }

      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (CurrentTime > LastShooterKickTime + 1000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 100, true);
          BallLaunched = true;
          LastShooterKickTime = CurrentTime;
        }
      }
      
      if (CurrentTime > WizardShotsRotateTime) {
        unsigned long rotateGap = 100;
        if (CurrentTime < (GameModeStartTime + 300000)) {
          rotateGap = 3100 - (CurrentTime - GameModeStartTime) / 100;
        }
        WizardShotsRotateTime = CurrentTime + rotateGap;
        WizardShotsCompleted *= 2;
        if (WizardShotsCompleted & (WIZARD_SHOT_SAUCER*2)) {
          WizardShotsCompleted &= WIZARD_SHOTS_COMPLETE;
          WizardShotsCompleted |= WIZARD_SHOT_E1;
        }
      }

      if (WizardShotsCompleted == WIZARD_SHOTS_COMPLETE) {
        // Player has finished all the wizard shots
        WizardShotsCompleted = 0;
        unsigned short songNum = SOUND_EFFECT_MINI_WIZ_MUSIC_3;
        if (GameModeStage==0) {
          GameModeStage = 1;
          QueueNotification(SOUND_EFFECT_VP_CONCERT_1_FINISHED, 8);
          songNum = SOUND_EFFECT_MINI_WIZ_MUSIC_2;
          IncreasePlayfieldMultiplier(30000);
          Display_StartScoreAnimation(JackpotValue * PlayfieldMultiplier, true);
          JackpotValue += 25000;
          ConcertJackpotsCollected[CurrentPlayer] += 1;
          ActivateTopper();
        } else if (GameModeStage==1) {
          GameModeStage = 2;
          QueueNotification(SOUND_EFFECT_VP_CONCERT_2_FINISHED, 8);
          IncreasePlayfieldMultiplier(30000);
          Display_StartScoreAnimation(JackpotValue * PlayfieldMultiplier, true);
          JackpotValue += 25000;
          ConcertJackpotsCollected[CurrentPlayer] += 2;
          ActivateTopper(true);
        } else {
          QueueNotification(SOUND_EFFECT_VP_CONCERT_3_FINISHED, 8);
          IncreasePlayfieldMultiplier(30000);
          Display_StartScoreAnimation(JackpotValue * PlayfieldMultiplier, true);
          JackpotValue += 25000;
          ConcertJackpotsCollected[CurrentPlayer] += 3;
          ActivateTopper(true);
        }
        PlayBackgroundSong(songNum);
      }

      if (PlayfieldMultiplier>1) {
        for (byte count = 0; count < RPU_NUMBER_OF_PLAYERS_ALLOWED; count++) {
          if (count != CurrentPlayer && !statusRunning) Display_OverrideScoreDisplay(count, PlayfieldMultiplier, DISPLAY_OVERRIDE_SYMMETRIC_BOUNCE);
        }
        DisplaysNeedRefreshing = true;
      } else if (DisplaysNeedRefreshing) {
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
      }
            
      if (NumberOfBallsInPlay == 1) {
        SetGameMode(GAME_MODE_MINI_WIZARD_END);
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
      }
      break;
      
    case GAME_MODE_MINI_WIZARD_END:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + 2000;
        if (DEBUG_MESSAGES) {
          Serial.write("Mini wizard end\n");
        }
      }

      if (CurrentTime>GameModeEndTime) {
        DisplaysNeedRefreshing = false;
        Display_ClearOverride(0xFF);
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }
      break;

    case GAME_MODE_FULL_WIZARD_READY:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
        // Announce Mini Wizard
        QueueNotification(SOUND_EFFECT_VP_FULL_WIZARD_INTRO, 10);
        JackpotValue = 25000 * (unsigned long)(ConcertJackpotsCollected[CurrentPlayer] + RawhideJackpotsCollected[CurrentPlayer]);
        ElwoodShotsMade[CurrentPlayer] = 0;
        JakeShotsMade[CurrentPlayer] = 0;
        BandShotsMade[CurrentPlayer] = 0;
        Audio.StopAllMusic();

        LeftDropTargets.ResetDropTargets(CurrentTime + 250, true);
        RightDropTargets.ResetDropTargets(CurrentTime + 750, true);
        ElwoodStatus[CurrentPlayer] = 0;
        GameModeStage = 0;
        LastShooterKickTime = 0;
        PlayBackgroundSong(SOUND_EFFECT_INTRO_LOOP);
      }
      SetBallSave(1000);
      // Light show directing the player to start the full wizard

      // if they don't start it with the saucer, it will start on drain with a lower jackpot
      if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
        if (GameModeStage==0) {
          JackpotValue /= 2;
          QueueNotification(SOUND_EFFECT_VP_MINI_WIZARD_HALF_JACKPOT, 7);
          GameModeEndTime = CurrentTime + 1800;
          GameModeStage = 1;
        }

        if (CurrentTime > LastShooterKickTime + 2000) {
          RPU_PushToTimedSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, CurrentTime + 750, true);
          LastShooterKickTime = CurrentTime;
          BallLaunched = true;
        }
        
      }

      if (GameModeEndTime && CurrentTime>GameModeEndTime) {
        SetGameMode(GAME_MODE_FULL_WIZARD_START);
      }      
      break;
      
  }

  if ( !statusRunning && !specialAnimationRunning && NumTiltWarnings <= MaxTiltWarnings ) {
    ShowBonusLamps();
    ShowPopBumperAndSlingLamps();
    ShowStandupTargetLamps();
    ShowLoopSpinnerAndLockLamps();
    ShowDropTargetLamps();
    ShowLaneLamps();
    ShowShootAgainLamp();
  }
  ShowPlayerLamps();

  if (Display_UpdateDisplays(0xFF, false, (BallFirstSwitchHitTime == 0) ? true : false, (BallFirstSwitchHitTime > 0 && ((CurrentTime - Display_GetLastTimeScoreChanged()) > 2000)) ? true : false)) {
    Audio.StopSound(SOUND_EFFECT_SCORE_TICK);
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

        if (!BallLaunched || (BallFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings)) {
          // Nothing hit yet, so return the ball to the player
          RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime);
          BallLaunched = false;
          BallTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (BallSaveEndTime && CurrentTime < (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
            RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime + 100);
            BallLaunched = false;

            RPU_SetLampState(LAMP_SHOOT_AGAIN, 0);
            BallTimeInTrough = CurrentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

            if (NumberOfBallSavesRemaining && NumberOfBallSavesRemaining != 0xFF) {
              NumberOfBallSavesRemaining -= 1;
              if (NumberOfBallSavesRemaining == 0) {
                BallSaveEndTime = 0;
                if (DEBUG_MESSAGES) {
                  Serial.write("Last ball save\n");
                }
                QueueNotification(SOUND_EFFECT_VP_BALL_SAVE, 10);
              } else {
                if (DEBUG_MESSAGES) {
                  Serial.write("Not last ball save\n");
                }                
              }
            }

          } else {

            NumberOfBallsInPlay -= 1;
            if (NumberOfBallsInPlay == 0) {

              // if we don't hold locks, release them
              if (LockHandling==LOCK_HANDLING_PURGE_LOCKS || LockHandling==LOCK_HANDLING_PURGE_LOCKS_WITH_RELOCKS) {                
                if (MachineLocks) {
                  WaitForPurgedLock = true;                    
                  ReleaseLiftGate();
                }
                UpdateLockStatus(true);
              }
              
              Display_ClearOverride(0xFF);
              Audio.StopAllAudio();
              //PlaySoundEffect(SOUND_EFFECT_BALL_OVER);
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
byte BonusSoundIncrement;
boolean CountdownBonusHurryUp = false;
int LastBonusSoundPlayed = 0;

int CountDownDelayTimes[] = {130, 130, 130, 100, 100, 60, 60, 60, 60, 60};

int CountdownBonus(boolean curStateChanged) {

  // If this is the first time through the countdown loop
  if (curStateChanged) {
    // Turn off topper (if it's on)
    RPU_SetContinuousSolenoidBit(true, 0x20);

    if (LeftGateCloseTime) {
      LeftGateCloseTime = 0;
      LeftGateAvailableTime = 0;
      RPU_PushToSolenoidStack(SOL_LEFT_GATE_CLOSE, LEFT_GATE_CLOSE_SOLENOID_STRENGTH);
    }

    if (TopGateCloseTime) {
      TopGateCloseTime = 0;
      TopGateAvailableTime = 0;
      RPU_PushToSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH);
    }

    if (RPU_ReadSingleSwitchState(SW_SAUCER)) {
      RPU_PushToSolenoidStack(SOL_SAUCER, SaucerSolenoidStrength, true);
    }

    CountdownStartTime = CurrentTime;
    LastCountdownReportTime = CurrentTime;
    ShowBonusLamps();
    IncrementingBonusXCounter = 1;
    DecrementingBonusCounter = Bonus[CurrentPlayer];
    TotalBonus = Bonus[CurrentPlayer];
    TotalBonusX = BonusX[CurrentPlayer];
    CountdownBonusHurryUp = false;
    BonusSoundIncrement = 0;
    LastBonusSoundPlayed = 0;

    BonusCountDownEndTime = 0xFFFFFFFF;
    // Some sound cards have a special index
    // for a "sound" that will turn off
    // the current background drone or currently
    // playing sound
    //    PlaySoundEffect(SOUND_EFFECT_STOP_BACKGROUND);
  }

  if (RPU_ReadSingleSwitchState(SW_LEFT_FLIPPER) && RPU_ReadSingleSwitchState(SW_RIGHT_FLIPPER)) CountdownBonusHurryUp = true;

  unsigned long countdownDelayTime = (unsigned long)(CountDownDelayTimes[IncrementingBonusXCounter - 1]);
  if (CountdownBonusHurryUp && countdownDelayTime > ((unsigned long)CountDownDelayTimes[9])) countdownDelayTime = CountDownDelayTimes[9];

  if ((CurrentTime - LastCountdownReportTime) > countdownDelayTime) {

    if (DecrementingBonusCounter) {

      // Only give sound & score if this isn't a tilt
      if (NumTiltWarnings <= MaxTiltWarnings) {
        int soundToPlay = SOUND_EFFECT_BONUS_1 + (BonusSoundIncrement/4);
        if (soundToPlay>SOUND_EFFECT_BONUS_17) soundToPlay = SOUND_EFFECT_BONUS_17;
        if (LastBonusSoundPlayed!=0) Audio.StopSound(LastBonusSoundPlayed);
        PlaySoundEffect(soundToPlay);
        LastBonusSoundPlayed = soundToPlay;
        BonusSoundIncrement += 1;
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

    if (DEBUG_MESSAGES) {
      Serial.write("Count down over, moving to ball over\n");
    }
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

    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, KNOCKER_SOLENOID_STRENGTH, CurrentTime, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, KNOCKER_SOLENOID_STRENGTH, CurrentTime + 300, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, KNOCKER_SOLENOID_STRENGTH, CurrentTime + 600, true);
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

    if (MachineLocks) {
      MachineLocks = 0;
      ReleaseLiftGate(750);
    }    
  }

  if (NumMatchSpins < 40) {

    if (NumMatchSpins<10) {
      byte blankMask = (0x3F << ((NumMatchSpins)/2)) & 0x3F;
      RPU_SetDisplayBlank(0, RPU_GetDisplayBlank(0) & blankMask);
      RPU_SetDisplayBlank(1, RPU_GetDisplayBlank(1) & blankMask);      
    } else if (NumMatchSpins>16 && NumMatchSpins<40) {
      if (CurrentNumPlayers>2) {
        unsigned long scoreMatch1 = CurrentScores[0] % 100;
        unsigned long scoreMatch2 = CurrentScores[1] % 100;
        unsigned long scoreMatch3 = CurrentScores[2] % 100;
        unsigned long scoreMatch4 = CurrentScores[3] % 100;
        unsigned long display1, display2;
        byte display1Mask, display2Mask;
        byte shiftPhase = (NumMatchSpins - 16)/3;
        switch (shiftPhase) {
          case 0:
            display1 = 10*scoreMatch1;
            display1Mask = 0x18;
            display2 = 10*scoreMatch2;
            display2Mask = 0x18;
            break;
          case 1:
            display1 = 100*scoreMatch1;
            display1Mask = 0x0C;
            display2 = 100*scoreMatch2;
            display2Mask = 0x0C;
            break;
          case 2:
            display1 = 1000*scoreMatch1;
            display1Mask = 0x06;
            display2 = 1000*scoreMatch2 + scoreMatch3/10;
            display2Mask = 0x26;
            break;
          case 3:
            display1 = 10000*scoreMatch1;
            display1Mask = 0x03;
            display2 = 10000*scoreMatch2 + scoreMatch3;
            display2Mask = 0x33;
            break;
          case 4:
            display1 = 10000*scoreMatch1;
            display1Mask = 0x03;
            display2 = scoreMatch3 * 10;
            display2Mask = 0x18;
            break;
          case 5:
            display1 = 10000*scoreMatch1;
            display1Mask = 0x03;
            display2 = 100*scoreMatch3;
            display2Mask = 0x0C;
            break;
          case 6:
            display1 = 10000*scoreMatch1 + scoreMatch2/10;
            display1Mask = 0x23;
            display2 = 1000*scoreMatch3;
            display2Mask = 0x06;
            if (CurrentNumPlayers==4) {
              display2 += scoreMatch4 / 10;
              display2Mask = 0x26;
            }
            break;
          case 7:
            display1 = 10000*scoreMatch1 + scoreMatch2;
            display1Mask = 0x33;
            display2 = 10000*scoreMatch3;
            display2Mask = 0x03;
            if (CurrentNumPlayers==4) {
              display2 += scoreMatch4;
              display2Mask = 0x33;
            }            
            break;
        }
        RPU_SetDisplayBlank(0, display1Mask);
        RPU_SetDisplayBlank(1, display2Mask);
        RPU_SetDisplay(0, display1);
        RPU_SetDisplay(1, display2);        
      }
    }
    
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
    if (CurrentNumPlayers<3) {
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
    } else {
      // With more than 2 players, the scores will be compressed
      if ((ScoreMatches >> count) & 0x01) {
        byte displayMask0 = 0, displayMask1 = 0;
        if (RPU_OS_NUM_DIGITS == 7) {
          switch (count) {
            case 0: displayMask0 = 0x03; break;
            case 1: displayMask0 = 0x60; break;
            case 2: displayMask1 = 0x03; break;
            case 3: displayMask1 = 0x60; break;
          }
        } else {
          switch (count) {
            case 0: displayMask0 = 0x03; break;
            case 1: displayMask0 = 0x30; break;
            case 2: displayMask1 = 0x03; break;
            case 3: displayMask1 = 0x30; break;
          }
        }
        
        if ( (CurrentTime / 200) % 2 ) {
          RPU_SetDisplayBlank(0, RPU_GetDisplayBlank(0) & ~displayMask0);
          RPU_SetDisplayBlank(1, RPU_GetDisplayBlank(1) & ~displayMask1);
        } else {
          RPU_SetDisplayBlank(0, RPU_GetDisplayBlank(0) | displayMask0);
          RPU_SetDisplayBlank(1, RPU_GetDisplayBlank(1) | displayMask1);
        }
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
    case SW_SELF_TEST_ON_MATRIX:
      Menus.EnterOperatorMenu();
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
            if (BallSaveEndTime) {
              BallSaveEndTime = 0;
              NumberOfBallSavesRemaining = 0;
            }
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
  if (GameMode == GAME_MODE_MINIMODE || GameMode == GAME_MODE_MULTIBALL_3_START || GameMode == GAME_MODE_MULTIBALL_3) return;

  if ( (MiniModesQualified[CurrentPlayer]&miniModeFlag) == 0) {
    MiniModesQualified[CurrentPlayer] |= miniModeFlag;
    if (miniModeFlag == ELWOOD_MINIMODE_FLAG) QueueNotification(SOUND_EFFECT_VP_ELWOOD_MINIMODE_QUALIFIED, 8);
    else if (miniModeFlag == JAKE_MINIMODE_FLAG) QueueNotification(SOUND_EFFECT_VP_JAKE_MINIMODE_QUALIFIED, 8);
    else if (miniModeFlag == BAND_MINIMODE_FLAG) QueueNotification(SOUND_EFFECT_VP_BAND_MINIMODE_QUALIFIED, 8);

    if (MiniModeMultiplierExpiration == 0) {
      MiniModeMultiplierExpiration = CurrentTime + 30000;
    } else {
      MiniModeMultiplierExpiration += 15000;
    }
  }
}


boolean ScoreWizardShot(unsigned long wizShotFlag) {
  unsigned long lastWizardShots = WizardShotsCompleted;
  WizardShotsCompleted |= wizShotFlag;
  if (lastWizardShots != WizardShotsCompleted) {
    
    byte numShotsComplete = CountBits(WizardShotsCompleted);
    if (numShotsComplete<6) {
      PlaySoundEffect(SOUND_EFFECT_MINI_WIZ_SCORE_1);
      Display_StartScoreAnimation(10000 * PlayfieldMultiplier, true);
    } else if (numShotsComplete<11) {
      PlaySoundEffect(SOUND_EFFECT_MINI_WIZ_SCORE_2);
      Display_StartScoreAnimation(20000 * PlayfieldMultiplier, true);
    } else {
      PlaySoundEffect(SOUND_EFFECT_MINI_WIZ_SCORE_3);
      Display_StartScoreAnimation(30000 * PlayfieldMultiplier, true);
    }
    return true;
  }

  return false;
}


void HandleLeftDropTarget(byte switchHit) {

  byte result;
  unsigned long numTargetsDown = 0;
  result = LeftDropTargets.HandleDropTargetHit(switchHit);
  numTargetsDown = (unsigned long)CountBits(result);
  boolean soundPlayed = false;
  boolean scoreAdded = false;

  if (numTargetsDown) LastDropTargetHitTime = CurrentTime;

  if (GameMode == GAME_MODE_SKILL_SHOT) {
    if (  (SkillshotLetter == 0 && switchHit == SW_DROP_L_1) ||
          (SkillshotLetter == 1 && switchHit == SW_DROP_L_2) ||
          (SkillshotLetter == 2 && switchHit == SW_DROP_L_3) ) {
      NumberOfSkillshotsMade[CurrentPlayer] += 1;
      Display_StartScoreAnimation(50000 * ((unsigned long)NumberOfSkillshotsMade[CurrentPlayer]), true);
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      ActivateTopper();
    } else {
      PlaySoundEffect(SOUND_EFFECT_MISSED_MODE_SHOT);
    }
  } else if ((MiniModesRunning&ELWOOD_MINIMODE_FLAG)) {
    if (  (ElwoodTargetLit == 0 && switchHit == SW_DROP_L_1) ||
          (ElwoodTargetLit == 1 && switchHit == SW_DROP_L_2) ||
          (ElwoodTargetLit == 2 && switchHit == SW_DROP_L_3) ) {
      ElwoodTargetLit += 1;
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
      ElwoodShotsMade[CurrentPlayer] += 1;
      if (GameMode != GAME_MODE_MULTIBALL_3) {
        LeftDropTargets.ResetDropTargets(CurrentTime + 500, true);
        RightDropTargets.ResetDropTargets(CurrentTime + 1000, true);
        ElwoodStatus[CurrentPlayer] = 0;
      }
    }
    return;
  } else if (GameMode==GAME_MODE_MINI_WIZARD) {
    unsigned long wizFlag = WIZARD_SHOT_E1;
    if (switchHit == SW_DROP_L_2) wizFlag = WIZARD_SHOT_L;
    if (switchHit == SW_DROP_L_3) wizFlag = WIZARD_SHOT_W;
    if (!ScoreWizardShot(wizFlag)) {
      PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * numTargetsDown * 100;
    }
    LeftDropTargets.ResetDropTargets(CurrentTime + 5000, true);
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
      if (GameMode==GAME_MODE_MULTIBALL_3 && MB3SuperJackpotQualified==0) {
        MB3SuperJackpotQualified = 1;
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

  if (numTargetsDown) LastDropTargetHitTime = CurrentTime;

  if (GameMode == GAME_MODE_SKILL_SHOT) {
    if (  (SkillshotLetter == 3 && switchHit == SW_DROP_R_1) ||
          (SkillshotLetter == 4 && switchHit == SW_DROP_R_2) ||
          (SkillshotLetter == 5 && switchHit == SW_DROP_R_3) ) {
      NumberOfSkillshotsMade[CurrentPlayer] += 1;
      Display_StartScoreAnimation(50000 * ((unsigned long)NumberOfSkillshotsMade[CurrentPlayer]), true);
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
    } else {
      PlaySoundEffect(SOUND_EFFECT_MISSED_MODE_SHOT);
    }
  } else if ((MiniModesRunning&ELWOOD_MINIMODE_FLAG)) {
    if (  (ElwoodTargetLit == 3 && switchHit == SW_DROP_R_1) ||
          (ElwoodTargetLit == 4 && switchHit == SW_DROP_R_2) ||
          (ElwoodTargetLit == 5 && switchHit == SW_DROP_R_3) ) {
      ElwoodTargetLit += 1;
      if (ElwoodTargetLit > 5) {
        ElwoodTargetLit = 0;
        Display_StartScoreAnimation(ELWOOD_MINIMODE_COMPLETION_BONUS, true);
        PlaySoundEffect(SOUND_EFFECT_DROP_TARGET_BONUS_SOUND);
      } else {
        PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
        CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
      }
      ElwoodShotsMade[CurrentPlayer] += 1;
      if (GameMode != GAME_MODE_MULTIBALL_3) {
        LeftDropTargets.ResetDropTargets(CurrentTime + 500, true);
        RightDropTargets.ResetDropTargets(CurrentTime + 1000, true);
        ElwoodStatus[CurrentPlayer] = 0;
      }
    }
    return;
  } else if (GameMode==GAME_MODE_MINI_WIZARD) {
    unsigned long wizFlag = WIZARD_SHOT_O1;
    if (switchHit == SW_DROP_R_2) wizFlag = WIZARD_SHOT_O2;
    if (switchHit == SW_DROP_R_3) wizFlag = WIZARD_SHOT_D;
    if (!ScoreWizardShot(wizFlag)) {
      PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * numTargetsDown * 100;
    }
    RightDropTargets.ResetDropTargets(CurrentTime + 5000, true);
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
      if (GameMode==GAME_MODE_MULTIBALL_3 && MB3SuperJackpotQualified==0) {
        MB3SuperJackpotQualified = 1;
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
  if ( CurrentTime < (LastJakeHit[jakeNum] + 500) ) return;

  LastJakeHit[jakeNum] = CurrentTime;

  byte prevStatus = JakeStatus[CurrentPlayer];
  JakeStatus[CurrentPlayer] |= 1 << (jakeNum);
  if (prevStatus == JakeStatus[CurrentPlayer]) CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
  else CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;

  if (JakeStatus[CurrentPlayer] == JAKE_STATUS_COMPLETE) {
    // Qualify the Jake mini mode
    JakeStatus[CurrentPlayer] = 0x00;
    JakeCompletions[CurrentPlayer] += 1;

    if ((JakeCompletions[CurrentPlayer] % JakeClearsToQualify) == 0) {
      QualifyMiniMode(JAKE_MINIMODE_FLAG);
    }

    if (GameMode==GAME_MODE_MULTIBALL_3 && MB3JackpotQualified==0) {
      MB3JackpotQualified = 1;
    }
  }

  if ((MiniModesRunning & JAKE_MINIMODE_FLAG) ) {
    if ( (switchHit - SW_STANDUP_J) == JakeTargetLit ) {
      PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      CurrentScores[CurrentPlayer] += 5000 * PlayfieldMultiplier;
      JakeShotsMade[CurrentPlayer] += 1;
    } else {
      if (CurrentTime > (LastTimeModeShotMissed + 1000)) {
        PlaySoundEffect(SOUND_EFFECT_STICK_CLICK);
        LastTimeModeShotMissed = CurrentTime;
      }
    }
  } else if (GameMode==GAME_MODE_MINI_WIZARD) {
    if (!ScoreWizardShot(WIZARD_SHOT_J << jakeNum)) {
      PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
    }
  } else {
    if (JakeStatus[CurrentPlayer]==0x00) PlaySoundEffect(SOUND_EFFECT_BIG_HIT_4);
    else if (prevStatus != JakeStatus[CurrentPlayer]) PlaySoundEffect(SOUND_EFFECT_BIG_HIT_1 + CountBits(prevStatus));
    else PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
  }

}


void HandleSpinnerProgress(boolean leftSpinnerSeen = true) {
  if (LastSpinnerSeen == 0xFF) {
    SpinnerProgress[CurrentPlayer] += 1;
  } else {
    if (leftSpinnerSeen && (LastSpinnerSeen == 1)) SpinnerProgress[CurrentPlayer] += AlternatingSpinnerAccelerator;
    else if (!leftSpinnerSeen && (LastSpinnerSeen == 0)) SpinnerProgress[CurrentPlayer] += AlternatingSpinnerAccelerator;
    else SpinnerProgress[CurrentPlayer] += 1;
  }

  if (SpinnerProgress[CurrentPlayer] >= SpinnerHitsToQualifyBand) {
    QualifyMiniMode(BAND_MINIMODE_FLAG);
    SpinnerProgress[CurrentPlayer] = 0;
  }

  if (leftSpinnerSeen) LastSpinnerSeen = 0;
  else LastSpinnerSeen = 1;
  LastSpinnerHitTime = CurrentTime;

  if (GameMode == GAME_MODE_MULTIBALL_3_START && leftSpinnerSeen) {
    Multiball3StartShotHit = true;
    Multiball3StartAward += 5000;
    if (DEBUG_MESSAGES) {
      Serial.write("MB3 spinner hit\n");
    }
  }

  if (GameMode == GAME_MODE_FULL_WIZARD_READY && leftSpinnerSeen) {
    SetGameMode(GAME_MODE_FULL_WIZARD_START);
  }

  if ((MiniModesRunning&BAND_MINIMODE_FLAG) && leftSpinnerSeen) {
    CurrentScores[CurrentPlayer] += 2500 * PlayfieldMultiplier;
    BandShotsMade[CurrentPlayer] += 1;
  } else if (GameMode == GAME_MODE_MINI_WIZARD && leftSpinnerSeen) {
    if (!ScoreWizardShot(WIZARD_SHOT_SPINNER)) {
      CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
    }
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

  if (GameMode==GAME_MODE_MULTIBALL_3 || GameMode==GAME_MODE_MULTIBALL_3_START) {
    if (MB3SuperJackpotQualified == 1) {
      openGate = true;
    }
  } else if (GameMode == GAME_MODE_MINI_WIZARD) {
    CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
    PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
  } else {
    if ( (MachineLocks & BALL_LOCKS_MASK) == 0x00 ) {
      // No balls in lock, but there may be virtual, purged, or shared locks
      if ( (PlayerLocks[CurrentPlayer]&BALL_LOCKS_MASK)==0x01 ||
           (PlayerLocks[CurrentPlayer]&BALL_LOCKS_AVAILABLE_MASK)==0x01 ) {
        PlayerLocks[CurrentPlayer] |= BALL_2_LOCK_AVAILABLE;
        openGate = true;
      } else if ( PlayerLocks[CurrentPlayer]==0x00 ) {
        PlayerLocks[CurrentPlayer] |= BALL_1_LOCK_AVAILABLE;
        openGate = true;
      }
    } else if ( (MachineLocks & BALL_LOCKS_MASK) == 0x01 ) {
      // There's one machine lock, so it either needs to be stolen
      // or we can qualify lock 2
      if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK) == 0x00 ) {
        // We can steal lock 1
        if (LockHandling==LOCK_HANDLING_LOCK_STEALING) QueueNotification(SOUND_EFFECT_VP_LOCK_1_STOLEN, 10);
        // else we should have a lock shared callout
        else QueueNotification(SOUND_EFFECT_VP_BALL_1_LOCKED, 10);
        PlayerLocks[CurrentPlayer] = BALL_1_LOCKED;
      } else {
        // This player already has this lock -- qualify lock 2
        PlayerLocks[CurrentPlayer] |= BALL_2_LOCK_AVAILABLE;
        openGate = true;
      }
    } else if ( (MachineLocks & BALL_LOCKS_MASK) == 0x03 ) {
      if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK) == 0x00 ) {
        // We can steal lock 1
        QueueNotification(SOUND_EFFECT_VP_LOCK_1_STOLEN, 10);
        PlayerLocks[CurrentPlayer] = BALL_1_LOCKED;
      } else if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK) == 0x01 ) {
        // We can steal lock 2
        QueueNotification(SOUND_EFFECT_VP_LOCK_2_STOLEN, 10);
        PlayerLocks[CurrentPlayer] |= BALL_2_LOCKED;
      } else if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_MASK) == 0x03 ) {
        // Start the multiball
        Multiball3StartShotHit = true;
        Multiball3StartAward += 35000;
        if (DEBUG_MESSAGES) {
          Serial.write("MB3 Captive ball hit\n");
        }
      }
    }
  }

  if (openGate) {

    if (DEBUG_MESSAGES) {
      char buf[128];
      sprintf(buf, "Open gate: Avail=%lu, Current=%lu\n", TopGateAvailableTime, CurrentTime);
      Serial.write(buf);
    }

    if (TopGateAvailableTime && (CurrentTime > TopGateAvailableTime)) {
      MB3SuperJackpotQualified = 2;
      PlaySoundEffect(SOUND_EFFECT_SHORT_SIREN);
      TopGateCloseSoundPlayed = false;
      TopGateCloseTime = CurrentTime + TOP_GATE_OPEN_TIME_MS;
      TopGateAvailableTime = 0;
      RPU_PushToSolenoidStack(SOL_TOP_GATE_OPEN, TOP_GATE_OPEN_SOLENOID_STRENGTH);
    }
  }

  if (DEBUG_MESSAGES) {
    char buf[128];
    sprintf(buf, "Captive ball end ML=0x%02X, PL=0x%02X\n", MachineLocks, PlayerLocks[CurrentPlayer]);
    Serial.write(buf);
  }


}


void ValidateAndRegisterPlayfieldSwitch() {
  if (BallLaunched) {
    LastSwitchHitTime = CurrentTime;
    if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
  }
}


void AdvanceStatusReportPage() {
  CurrentStatusReportPage += 1;
  // Status report pages go from 2 to (STATUS_REPORT_NUM_PAGES+1)
  if (CurrentStatusReportPage>(STATUS_REPORT_NUM_PAGES+1)) {
    CurrentStatusReportPage = 0;
    LastSwitchHitTime = CurrentTime;
  }
  
}


void CycleBuildBonusLamps(boolean cycleLeft = true) {

  byte newLamps;
  if (cycleLeft) {
    newLamps = BuildBonusLamps[CurrentPlayer] / 2;
    if (BuildBonusLamps[CurrentPlayer] & 0x01) {
      newLamps |= 0x10;
    }
  } else {
    newLamps = (BuildBonusLamps[CurrentPlayer] * 2) & 0x1F;
    if (BuildBonusLamps[CurrentPlayer] & 0x10) {
      newLamps |= 0x01;
    }
  }
  BuildBonusLamps[CurrentPlayer] = newLamps;
  
}


void HandleGamePlaySwitches(byte switchHit) {

  switch (switchHit) {

    case SW_LEFT_FLIPPER:
      if (CurrentStatusReportPage) AdvanceStatusReportPage();
      CycleBuildBonusLamps(true);
      LastFlipperSeen = CurrentTime;
      break;

    case SW_RIGHT_FLIPPER:
      if (GameMode==GAME_MODE_SKILL_SHOT) {
        if (!BallLaunched) {
          SkillshotLetter += 1;
          if (SkillshotLetter > 5) SkillshotLetter = 0;
        }
      }
      CycleBuildBonusLamps(false);
      if (CurrentStatusReportPage) AdvanceStatusReportPage();
      LastFlipperSeen = CurrentTime;
      break;

    case SW_LAUNCH:
      if (GameMode!=GAME_MODE_MINI_WIZARD_READY && GameMode!=GAME_MODE_MINI_WIZARD_START) {
        if (RPU_ReadSingleSwitchState(SW_SHOOTER_LANE)) {
          RPU_SetLampState(LAMP_LAUNCH_BUTTON, 0);
          BallLaunched = true;
          if (RPU_ReadSingleSwitchState(SW_LEFT_FLIPPER)) {
            NumberOfShortPlunges += 1;
            if (NumberOfShortPlunges < 3) {
              RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, ShooterKickLightStrength, true);
            } else {
              RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, ShooterKickLightStrength + 1, true);
            }
            if (Audio.GetBackgroundSong() >= SOUND_EFFECT_RALLY_MUSIC_1 && Audio.GetBackgroundSong() <= SOUND_EFFECT_RALLY_MUSIC_5) {
              Audio.StopAllMusic();
              PlayBackgroundSong(SOUND_EFFECT_RALLY_MUSIC_1 + (CurrentBallInPlay-1));
              PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_1);
            }
          } else {

            if (DEBUG_MESSAGES) {
              Serial.write("Launching the ball\n");
            }
            RPU_PushToSolenoidStack(SOL_SHOOTER_KICK, ShooterKickFullStrength, true);
            if (Audio.GetBackgroundSong() == SOUND_EFFECT_RALLY_MUSIC_1) {
              Audio.StopAllMusic();
              PlaySoundEffect(SOUND_EFFECT_TIRES_SQUEALING);
            }
          }
        } else {
          if (NumberOfBallsInPlay==1 && BallSaveEndTime==0) {
            if (LeftOutlaneSaveSeconds[CurrentPlayer]) LeftOutlaneSaveEndTime = CurrentTime + ((unsigned long)LeftOutlaneSaveSeconds[CurrentPlayer])*1000;
            if (RightOutlaneSaveSeconds[CurrentPlayer]) RightOutlaneSaveEndTime = CurrentTime + ((unsigned long)RightOutlaneSaveSeconds[CurrentPlayer])*1000;
          }
        }
      }
      break;

    case SW_POP_BUMPER:
      LastTimePopHit = CurrentTime;
      CurrentScores[CurrentPlayer] += 100 * PlayfieldMultiplier;
      PlaySoundEffect(SOUND_EFFECT_KICK_DRUM);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_BOTTOM_LEFT:
      if (GameMode==GAME_MODE_MINI_WIZARD) {
        if (!ScoreWizardShot(WIZARD_SHOT_SAVE_GATE)) {
          CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
          PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
        }        
      } else {
        CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
        if (LeftGateAvailableTime && (CurrentTime > LeftGateAvailableTime)) {
          PlaySoundEffect(SOUND_EFFECT_CELL_DOOR);
          LeftGateCloseTime = CurrentTime + LEFT_GATE_OPEN_TIME_MS;
          LeftGateAvailableTime = 0;
          RPU_PushToSolenoidStack(SOL_LEFT_GATE_OPEN, LEFT_GATE_OPEN_SOLENOID_STRENGTH);
        }
      }
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_CAPTIVE_BALL:
      if (CurrentTime > (LastCaptiveBallHit + 1000)) {
        LastCaptiveBallHit = CurrentTime;
        CurrentScores[CurrentPlayer] += 10000 * PlayfieldMultiplier;
        HandleCaptiveBallHit();
        AddToBuildBonus(BUILD_BONUS_LAMP_CAPTIVE_BALL, 10000);
      }
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_LEFT_INLANE:
      AddToBuildBonus(BUILD_BONUS_LAMP_LEFT_INLANE, 100);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_INLANE:
      if (BallLaunched && CurrentTime > (LastTimeSaucerSeen+500)) {
        AddToBuildBonus(BUILD_BONUS_LAMP_RIGHT_INLANE, 100);
        LeftSpinnerLitTime = CurrentTime;
        ValidateAndRegisterPlayfieldSwitch();
      }
      break;

    case SW_LEFT_OUTLANE:
      if (LeftOutlaneSaveEndTime) {
        LeftOutlaneSaveEndTime = 0;
        PlaySoundEffect(SOUND_EFFECT_BIG_HIT_1);
        NumberOfBallsInPlay += 1;
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime);
        BallLaunched = false;
        LeftOutlaneSaveSeconds[CurrentPlayer] = 0;
      } else {
        SetBallSave(4000, 0, true);
        PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_4);
        CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 1000;

        if (BallSaveEndTime==0 && LeftOutlaneSaveSeconds[CurrentPlayer]) {
          // Could have saved, but didn't
          if (NumberOfBallsInPlay==1) QueueNotification(SOUND_EFFECT_VP_LAUNCH_BUTTON_TO_SAVE, 7);
        }
      }
      AddToBonus(1);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_OUTLANE:
      if (RightOutlaneSaveEndTime) {
        RightOutlaneSaveEndTime = 0;
        PlaySoundEffect(SOUND_EFFECT_BIG_HIT_1);
        NumberOfBallsInPlay += 1;
        RPU_PushToTimedSolenoidStack(SOL_SERVE_BALL, BallServeSolenoidStrength, CurrentTime);
        BallLaunched = false; 
        RightOutlaneSaveSeconds[CurrentPlayer] = 0;
      } else {
        SetBallSave(4000, 0, true);
        PlaySoundEffect(SOUND_EFFECT_ELECTRIC_PIANO_4);
        CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 1000;

        if (BallSaveEndTime==0 && RightOutlaneSaveSeconds[CurrentPlayer]) {
          // Could have saved, but didn't
          if (NumberOfBallsInPlay==1) QueueNotification(SOUND_EFFECT_VP_LAUNCH_BUTTON_TO_SAVE, 7);
        }
      }
      if (ExtraBallAvailable[CurrentPlayer]==1) {
        AwardExtraBall();
      }
      AddToBonus(1);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_FLIP_LANE:
      RightFlipLanes[CurrentPlayer] += 1;
      if (CollectFlipBonusTimeout) {
        CollectFlipBonusTimeout = 0;
        NextBonusPayoutTime = CurrentTime + 1000;
        PlaySoundEffect(SOUND_EFFECT_BONUS_PAYOUT_START);
      }
      if (GameMode == GAME_MODE_MULTIBALL_3_START) {
        Multiball3StartShotHit = true;
        Multiball3StartAward += 50000;
        if (DEBUG_MESSAGES) {
          Serial.write("MB3 Flip lane hit\n");
        }
      } else if (GameMode == GAME_MODE_MINI_WIZARD) {
        if (!ScoreWizardShot(WIZARD_SHOT_RIGHT_FLIP)) {
          CurrentScores[CurrentPlayer] += 1000 * PlayfieldMultiplier;
          PlaySoundEffect(SOUND_EFFECT_RICOCHET_0 + CurrentTime % 8);
        }
      } else if (GameMode == GAME_MODE_MINIMODE) {
        byte soundToPlay = RightFlipsSeen;
        if (soundToPlay > 2) soundToPlay = 2;
        PlaySoundEffect(SOUND_EFFECT_BAND_HIT_1 + soundToPlay);
        RightFlipsSeen += 1;
        if (!AddABallUsed) {
          if (RightFlipsSeen == RightFlipsForAddABall) {
            SetBallSave(((unsigned long)BallSaveOnAddABall) * 1000);
            AddABallUsed = true;
            AddABall(true);
          }
        } else {
          if (RightFlipsSeen > RightFlipsForAddABall) {
            Display_StartScoreAnimation(15000 * PlayfieldMultiplier, true);
            //PlaySoundEffect(SOUND_EFFECT_MINI_JACKPOT);
          }
        }
      } else {
        PlaySoundEffect(SOUND_EFFECT_LOOP_1 + CurrentTime % 2);
      }
      AddToBuildBonus(BUILD_BONUS_LAMP_LOOP_LANE, 10000);
      LastFlipLaneTime = CurrentTime;
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_MID_TOP:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 10;
      PlaySoundEffect(SOUND_EFFECT_RIM_SHOT);
      if (GameMode!=GAME_MODE_SKILL_SHOT) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_LEFT_SLING:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 10;
      PlaySoundEffect(SOUND_EFFECT_TOM_SHOT_1);
      if (GameMode!=GAME_MODE_SKILL_SHOT) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_SLING:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 10;
      PlaySoundEffect(SOUND_EFFECT_RIM_SHOT_2);
      if (GameMode!=GAME_MODE_SKILL_SHOT) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_TOP_SLING:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 10;
      PlaySoundEffect(SOUND_EFFECT_RIM_SHOT_3);
      if (GameMode!=GAME_MODE_SKILL_SHOT) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_MID_LEFT:
      CurrentScores[CurrentPlayer] += PlayfieldMultiplier * 10;
      PlaySoundEffect(SOUND_EFFECT_SNARE_AND_CYMBAL);
      if (GameMode!=GAME_MODE_SKILL_SHOT) ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_DROP_L_1:
    case SW_DROP_L_2:
    case SW_DROP_L_3:
      HandleLeftDropTarget(switchHit);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_DROP_R_1:
    case SW_DROP_R_2:
    case SW_DROP_R_3:
      HandleRightDropTarget(switchHit);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_LEFT_SPINNER:
      HandleSpinnerProgress(true);
      if (MiniModesRunning&BAND_MINIMODE_FLAG) PlaySoundEffect(SOUND_EFFECT_CORRECT_MODE_SHOT);
      else if (LeftSpinnerLitTime) PlaySoundEffect(SOUND_EFFECT_COWBELL);
      else PlaySoundEffect(SOUND_EFFECT_SPINNER_UNLIT_2);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_RIGHT_SPINNER:
      HandleSpinnerProgress(false);
      PlaySoundEffect(SOUND_EFFECT_SPINNER_UNLIT_1);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_STANDUP_J:
    case SW_STANDUP_A:
    case SW_STANDUP_K:
    case SW_STANDUP_E:
      HandleJakeStandup(switchHit);
      ValidateAndRegisterPlayfieldSwitch();
      break;

    case SW_SAUCER:
      // Saucer kitckout is now handled by debouncer
      // We'll validate the PF with this switch, unless
      // we just fired the Saucer as part of the ball search.
      LastTimeSaucerSeen = CurrentTime;
      if (BallSearchLastSolenoidFired!=SOL_SAUCER) {
        ValidateAndRegisterPlayfieldSwitch();
      }
      break;

    case SW_TRAP:
      if (MachineLocks == 0) {
        WaitingForTrapSwitchToHold = 0;
        if ( GameMode==GAME_MODE_MULTIBALL_3 ) {
          if (MB3SuperJackpotQualified==2) {
            // We're ready for a super jackpot
            QueueNotification(SOUND_EFFECT_VP_SUPER_JACKPOT, 7);
            ActivateTopper(true);
            RawhideJackpotsCollected[CurrentPlayer] += 2;
            Display_StartScoreAnimation(MULTIBALL_3_SUPER_JACKPOT_VALUE, true);
            ReleaseLiftGate(10000);
            MB3SuperJackpotQualified = 3;
            LeftDropTargets.ResetDropTargets(CurrentTime + 250, true);
            RightDropTargets.ResetDropTargets(CurrentTime + 750, true);
            ElwoodStatus[CurrentPlayer] = 0;
            TopGateCloseSoundPlayed = true;
            TopGateCloseTime = CurrentTime + 10000;
            TopGateAvailableTime = 0;
          } else if (MB3SuperJackpotQualified==3) {
            // Close the top gate (hide away)
            TopGateCloseTime = 0;
            RPU_PushToTimedSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH, CurrentTime + 500, true);
            if (TopGateAvailableTime == 0) TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
          } else {
            // Close the top gate (hide away)
            TopGateCloseTime = 0;
            RPU_PushToTimedSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH, CurrentTime + 500, true);
            if (TopGateAvailableTime == 0) TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
            ReleaseLiftGate();
          }
        } else {
          if ( (PlayerLocks[CurrentPlayer] & BALL_LOCKS_AVAILABLE_MASK) == 0 ) {
            // A ball fell in the trap when the player doesn't have a lock
            // available -- need to eject the trap
            ReleaseLiftGate();
          } else {
            // This is the first ball in the trap
            if (LockHandling == LOCK_HANDLING_VIRTUAL_LOCKS) {
              // For virtual locks, they always appear here, so we decide
              // based on the player locks
              if (PlayerLocks[CurrentPlayer] & BALL_1_LOCKED) {
                SetGameMode(GAME_MODE_LOCK_BALL_2);
              } else {
                SetGameMode(GAME_MODE_LOCK_BALL_1);
              }
            } else {
              if (PlayerLocks[CurrentPlayer] & BALL_1_LOCKED) SetGameMode(GAME_MODE_LOCK_BALL_2);
              else SetGameMode(GAME_MODE_LOCK_BALL_1);
            }
          }
          // Close the top gate (hide away)
          TopGateCloseTime = 0;
          RPU_PushToTimedSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH, CurrentTime + 500, true);
          //PlaySoundEffect(SOUND_EFFECT_TOP_GATE_CLOSING);
          if (TopGateAvailableTime == 0) TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
        }
      } else {
        // We will see this lock when the trap switch is closed
        // for an extended period
        // Close the top gate (hide away)
        TopGateCloseTime = 0;
        RPU_PushToTimedSolenoidStack(SOL_TOP_GATE_CLOSE, TOP_GATE_CLOSE_SOLENOID_STRENGTH, CurrentTime + 500, true);
        //PlaySoundEffect(SOUND_EFFECT_TOP_GATE_CLOSING);
        if (TopGateAvailableTime == 0) TopGateAvailableTime = CurrentTime + TOP_GATE_DELAY_TIME_MS;
        WaitingForTrapSwitchToHold = CurrentTime;
      }

      ValidateAndRegisterPlayfieldSwitch();
      break;

  }

}


int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long scoreAtTop = CurrentScores[CurrentPlayer];

  UpdateLiftGate();

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = InitGamePlay(curStateChanged);
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = InitNewBall(curStateChanged);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    Display_ClearOverride(0xFF);
    Display_UpdateDisplays(0xFF, true);
    returnState = CountdownBonus(curStateChanged);
//    ShowPlayerScoresOnTwoDisplays(0xFF, false, false);
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

        for (byte count = 0; count < 4; count++) {
          RPU_SetLampState(PlayerUpLamps[count], 0);
        }

        if (MatchEnabled) {
          returnState = MACHINE_STATE_MATCH_MODE;
        } else {
          ReleaseLiftGate(750);
          returnState = MACHINE_STATE_ATTRACT;
        }
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
    NumberOfBallSavesRemaining = 1;
  }
  if (CurrentTime > (BallSaveEndTime + BALL_SAVE_GRACE_PERIOD)) {
    BallSaveEndTime = 0;
    NumberOfBallSavesRemaining = 0;
  }

  if (!ScrollingScores && CurrentScores[CurrentPlayer] > RPU_OS_MAX_DISPLAY_SCORE) {
    CurrentScores[CurrentPlayer] -= RPU_OS_MAX_DISPLAY_SCORE;
    if (!TournamentScoring) AddSpecialCredit();
  }

  if (scoreAtTop != CurrentScores[CurrentPlayer]) {
    Display_SetLastTimeScoreChanged(CurrentTime);
    if (!TournamentScoring) {
      for (int awardCount = 0; awardCount < 3; awardCount++) {
        if (AwardScores[awardCount] != 0 && scoreAtTop < AwardScores[awardCount] && CurrentScores[CurrentPlayer] >= AwardScores[awardCount]) {
          // Player has just passed an award score, so we need to award it
          if (((ScoreAwardReplay >> awardCount) & 0x01)) {
            AddSpecialCredit();
          } else {
            AwardExtraBall(true);
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

#ifdef DEBUG_SHOW_LOOPS_PER_SECOND
unsigned long NumLoops = 0;
unsigned long LastLoopReportTime = 0;
#endif

void loop() {

  CurrentTime = millis();
  int newMachineState = MachineState;
  
#ifdef DEBUG_SHOW_LOOPS_PER_SECOND
  NumLoops += 1;
  if (LastLoopReportTime==0) LastLoopReportTime = CurrentTime;
  if (CurrentTime>(LastLoopReportTime+1000)) {
    LastLoopReportTime = CurrentTime;
    char buf[128];
    sprintf(buf, "Loop running at %lu Hz\n", NumLoops);
    Serial.write(buf);
    NumLoops = 0;
  }
#endif

  if (Menus.OperatorMenusActive()) {
    RunOperatorMenu();
  } else {
    if (MachineState < 0) {
      newMachineState = 0;
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
