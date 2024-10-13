// These example lamp definitions come from Supersonic
#define LAMP_SHOOT_AGAIN            35    // Q56
#define LAMP_LEFT_OUTLANE           2     // Q36
#define LAMP_LEFT_INLANE            46    // Q39
#define LAMP_RIGHT_INLANE           7     // Q50
#define LAMP_RIGHT_OUTLANE_EB       11    // Q51
#define LAMP_OPEN_SAVE_GATE         38    // Q41
#define LAMP_SAUCER                 34    // Q42
#define LAMP_LOOP_BONUS             39    // Q46
#define LAMP_BONUS_1                18    // Q48
#define LAMP_BONUS_2                3     // Q57
#define LAMP_BONUS_3                26    // Q32
#define LAMP_BONUS_4                47    // Q53
#define LAMP_BONUS_5                42    // Q40
#define LAMP_BONUS_6                14    // Q49
#define LAMP_SPECIAL                10    // Q44
#define LAMP_LEFT_SLING             6     // Q38
#define LAMP_RIGHT_SLING            43    // Q52
#define LAMP_CAPTIVE_BALL           45    // Q21
#define LAMP_E_1                    44    // Q7
#define LAMP_L                      36    // Q2
#define LAMP_W                      4     // Q12
#define LAMP_LEFT_SPINNER           0     // Q14
#define LAMP_O_1                    37    // Q17
#define LAMP_O_2                    9     // Q28
#define LAMP_D                      13    // Q35
#define LAMP_E_2                    41    // Q23
#define LAMP_K                      33    // Q24
#define LAMP_A                      1     // Q29
#define LAMP_J                      5     // Q27
#define LAMP_HIDE_AWAY              17    // Q34
#define LAMP_POP_BLUE_UP            28    // Q4
#define LAMP_POP_WHITE_DOWN         12    // Q8
#define LAMP_POP_SMALL_BLUE         20    // Q10
#define LAMP_BEHIND_CAPTIVE         8     // Q13
#define LAMP_START_BUTTON           51    // Q47
#define LAMP_LAUNCH_BUTTON          55    // Q43
#define LAMP_HEAD_TILT              57    // Q19
#define LAMP_HEAD_GAME_OVER         56    // Q6
#define LAMP_HEAD_BALL_IN_PLAY      54    // Q30
#define LAMP_HEAD_MATCH             50    // Q33
#define LAMP_HEAD_PLAYER_1_UP       49    // Q15
#define LAMP_HEAD_PLAYER_2_UP       52    // Q5
#define LAMP_HEAD_PLAYER_3_UP       53    // Q18
#define LAMP_HEAD_PLAYER_4_UP       48    // Q16



#define SW_LEFT_SPINNER             0
#define SW_RIGHT_SPINNER            1
#define SW_TROUGH_1                 2
#define SW_TROUGH_2                 3
#define SW_TROUGH_3                 4
#define SW_SELF_TEST_ON_MATRIX      5
#define SW_OUTHOLE                  7
#define SW_CREDIT_RESET             8
#define SW_COIN_1                   9
#define SW_LAUNCH                   10
#define SW_TILT                     11
#define SW_PLUMB_TILT               11
#define SW_ROLL_TILT                11
#define SW_MID_TOP                  12
#define SW_CAPTIVE_BALL             13
#define SW_MID_LEFT                 14
#define SW_BOTTOM_LEFT              15
#define SW_DROP_R_1                 16
#define SW_DROP_R_2                 17
#define SW_DROP_R_3                 18
#define SW_LEFT_SLING               19
#define SW_RIGHT_SLING              20
#define SW_TOP_SLING                21
#define SW_LEFT_FLIPPER             22
#define SW_RIGHT_FLIPPER            23
#define SW_DROP_L_1                 24
#define SW_DROP_L_2                 25
#define SW_DROP_L_3                 26
#define SW_STANDUP_J                27
#define SW_STANDUP_A                28
#define SW_STANDUP_K                29
#define SW_STANDUP_E                30
#define SW_POP_BUMPER               31
#define SW_SAUCER                   32
#define SW_SHOOTER_LANE             33
#define SW_LEFT_OUTLANE             34
#define SW_LEFT_INLANE              35
#define SW_RIGHT_OUTLANE            37
#define SW_RIGHT_INLANE             36
#define SW_RIGHT_FLIP_LANE          38
#define SW_TRAP                     39

#define SOL_TOP_GATE_OPEN           1
#define SOL_TOP_GATE_CLOSE          0
#define SOL_LEFT_GATE_OPEN          9
#define SOL_POP_BUMPER              6
#define SOL_DROP_BANK_R_RESET       2
#define SOL_DROP_BANK_L_RESET       3
#define SOL_KNOCKER                 4
#define SOL_SAUCER                  7
#define SOL_LEFT_SLING              10
#define SOL_RIGHT_SLING             11
#define SOL_TOP_SLING               13
#define SOL_SHOOTER_KICK            12
#define SOL_SERVE_BALL              8
#define SOL_LIFT_GATE               5
#define SOL_LEFT_GATE_CLOSE         14


#define NUM_SWITCHES_WITH_TRIGGERS          4 // total number of solenoid/switch pairs
#define NUM_PRIORITY_SWITCHES_WITH_TRIGGERS 4 // This number should match the define above

struct PlayfieldAndCabinetSwitch SolenoidAssociatedSwitches[] = {
  { SW_RIGHT_SLING, SOL_RIGHT_SLING, 4},
  { SW_LEFT_SLING, SOL_LEFT_SLING, 4},
  { SW_TOP_SLING, SOL_TOP_SLING, 4},
  { SW_POP_BUMPER, SOL_POP_BUMPER, 4}
};
