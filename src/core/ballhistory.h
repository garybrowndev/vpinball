#pragma once

#include <set>

#include "simpleini/SimpleIni.h"
#include "src\renderer\typedefs3D.h"

struct BallHistoryState
{
   Vertex3Ds m_Position;
   Vertex3Ds m_Velocity;
   Vertex3Ds m_AngularMomentum;
   Vertex3Ds m_LastEventPos;

   Matrix3 m_Orientation;

   Vertex3Ds m_OldPos[MAX_BALL_TRAIL_POS];
   unsigned int m_RingCounter_OldPos;

   float m_DrawRadius;
   DWORD m_Color;

   BallHistoryState();
};

struct BallHistoryRecord
{
   int m_TimeMs;
   std::vector<BallHistoryState> m_BallHistoryStates;

   BallHistoryRecord();
   BallHistoryRecord(int timeMs);
   void Set(const HitBall *controlVBall, BallHistoryState &bhr);
   void Set(std::vector<HitBall *> &controlVBalls, int timeMs);
   void Insert(const HitBall *controlVBall, std::size_t insertIndex);
};

class TrainerOptions
{
public:
   enum ModeStateType
   {
      ModeStateType_Config,
      ModeStateType_Start,
      ModeStateType_Resume,
      ModeStateType_Results,
      ModeStateType_GoBack,
      ModeStateType_COUNT
   };

   enum ConfigModeStateType
   {
      ConfigModeStateType_Wizard,
      ConfigModeStateType_BallStart,
      ConfigModeStateType_BallPass,
      ConfigModeStateType_BallFail,
      ConfigModeStateType_BallCorridor,
      ConfigModeStateType_Difficulty,
      ConfigModeStateType_TotalRuns,
      ConfigModeStateType_RunOrder,
      ConfigModeStateType_BallKickerBehavior,
      ConfigModeStateType_MaxSecondsPerRun,
      ConfigModeStateType_CountdownSecondsBeforeRun,
      ConfigModeStateType_SoundEffects,
      ConfigModeStateType_GoBack,
      ConfigModeStateType_COUNT
   };

   enum BallStartModeType
   {
      BallStartModeType_Accept,
      BallStartModeType_Existing,
      BallStartModeType_Custom,
      BallStartModeType_COUNT
   };

   enum BallStartAngleVelocityModeType
   {
      BallStartAngleVelocityModeType_Drop,
      BallStartAngleVelocityModeType_Custom,
      BallStartAngleVelocityModeType_COUNT
   };

   enum BallStartCompleteModeType
   {
      BallStartCompleteModeType_Accept,
      BallStartCompleteModeType_Select,
      BallStartCompleteModeType_COUNT
   };

   enum BallEndLocationModeType
   {
      BallEndLocationModeType_Config,
      BallEndLocationModeType_Delete,
      BallEndLocationModeType_COUNT
   };

   enum BallEndFinishModeType
   {
      BallEndFinishModeType_Distance,
      BallEndFinishModeType_Stop,
      BallEndStopModeType_COUNT
   };

   enum BallEndAssociationModeType
   {
      BallEndAssociationModeType_Accept,
      BallEndAssociationModeType_Select,
      BallEndAssociationModeType_COUNT
   };

   enum BallEndCompleteModeType
   {
      BallEndCompleteModeType_Accept,
      BallEndCompleteModeType_Select,
      BallEndCompleteModeType_COUNT
   };

   enum BallCorridorCompleteModeType
   {
      BallCorridorCompleteModeType_Accept,
      BallCorridorCompleteModeType_Config,
      BallCorridorCompleteModeType_Reset,
      BallCorridorCompleteModeType_COUNT
   };

   enum DifficultyConfigModeState
   {
      DifficultyConfigModeState_Accept,
      DifficultyConfigModeState_GameplayDifficulty,
      DifficultyConfigModeState_VarianceDifficulty,
      DifficultyConfigModeState_GravityVariance,
      DifficultyConfigModeState_PlayfieldFrictionVariance,
      DifficultyConfigModeState_FlipperStrengthVariance,
      DifficultyConfigModeState_FlipperFrictionVariance,
      DifficultyConfigModeState_COUNT
   };

   enum DifficultyVarianceModeType
   {
      DifficultyVarianceModeType_AboveAndBelow,
      DifficultyVarianceModeType_AboveOnly,
      DifficultyVarianceModeType_BelowOnly,
      DifficultyVarianceModeType_COUNT
   };

   enum RunOrderModeType
   {
      RunOrderModeType_InOrder,
      RunOrderModeType_Random,
      RunOrderModeType_COUNT
   };

   enum BallKickerBehaviorModeType
   {
      BallKickerBehaviorModeType_Reset,
      BallKickerBehaviorModeType_Fail,
      BallKickerBehaviorModeType_COUNT
   };

   enum SoundEffectsModeType
   {
      SoundEffectsModeType_Accept,
      SoundEffectsModeType_Pass,
      SoundEffectsModeType_Fail,
      SoundEffectsModeType_Countdown,
      SoundEffectsModeType_TimeLow,
      SoundEffectsModeType_COUNT
   };

   struct BallStartOptionsRecord
   {
      static const char * ImGuiBallStartLabels[];

      static const S32 AngleMinimum = 0;
      static const S32 AngleMaximum = 360;

      static const S32 TotalAnglesMinimum = 1;
      static const S32 TotalAnglesMaximum = 36;

      static const S32 VelocityMinimum = 0;
      static const S32 VelocityMaximum = 100;

      static const S32 TotalVelocitiesMinimum = 1;
      static const S32 TotalVelocitiesMaximum = 20;

      Vertex3Ds m_StartPosition;
      Vertex3Ds m_StartVelocity;
      Vertex3Ds m_StartAngularMomentum;
      float m_AngleRangeStart;
      float m_AngleRangeFinish;
      S32 m_TotalRangeAngles;
      float m_VelocityRangeStart;
      float m_VelocityRangeFinish;
      S32 m_TotalRangeVelocities;

      BallStartOptionsRecord();
      BallStartOptionsRecord(const Vertex3Ds &startPosition, const Vertex3Ds &startVelocity, const Vertex3Ds &startAngularMomentum, float angleRangeStart, float angleRangeFinish, S32 totalRangeAngles, float velocityRangeStart, float velocityRangeFinish, S32 totalRangeVelocities);
      bool IsZero();
   };

   struct BallEndOptionsRecord
   {
      static const char * ImGuiBallPassLabels[];
      static const char * ImGuiBallFailLabels[];

      static const S32 RadiusPercentMinimum = 1;
      static const S32 RadiusPercentMaximum = 300;
      static const float RadiusPercentDisabled;

      Vertex3Ds m_EndPosition;
      float m_EndRadiusPercent;
      std::set<std::size_t> m_AssociatedBallStartIndexes;

      std::vector<std::tuple<int, Vertex3Ds>> m_StopBallsTracker;

      BallEndOptionsRecord();
      BallEndOptionsRecord(const Vertex3Ds &endPosition, float endRadiusPercent);
   };

   struct BallCorridorOptionsRecord
   {
      static const char * ImGuiDrawTrainerBallCorridorPassLabel;
      static const char * ImGuiDrawTrainerBallCorridorOpeningLeftLabel;
      static const char * ImGuiDrawTrainerBallCorridorOpeningRightLabel;

      static const S32 RadiusPercentMinimum = 1;
      static const S32 RadiusPercentMaximum = 300;

      Vertex3Ds m_PassPosition;
      float m_PassRadiusPercent;

      Vertex3Ds m_OpeningPositionLeft;
      Vertex3Ds m_OpeningPositionRight;

      BallCorridorOptionsRecord();
      BallCorridorOptionsRecord(const Vertex3Ds &passPosition, float passRadiusPercent, const Vertex3Ds &openingLeft, const Vertex3Ds &openingRight);
   };

   struct RunRecord
   {
      enum ResultType
      {
         ResultType_PassedLocation,
         ResultType_FailedLocation,
         ResultType_PassedCorridor,
         ResultType_FailedCorridorLeft,
         ResultType_FailedCorridorRight,
         ResultType_FailedTimeElapsed,
         ResultType_FailedKicker,
         ResultType_Unknown
      };

      std::vector<Vertex3Ds> m_StartPositions;
      std::vector<Vertex3Ds> m_StartVelocities;
      std::vector<Vertex3Ds> m_StartAngularMomentums;

      ResultType m_Result;
      int m_TotalTimeMs;

      std::vector<std::tuple<std::size_t, std::size_t>> m_StartToPassLocationIndexes;
      std::vector<std::tuple<std::size_t, std::size_t>> m_StartToFailLocationIndexes;
      std::size_t m_StartToPassCorridorIndex;
      std::size_t m_StartToFailCorridorIndex;

      RunRecord();
   };

   static const int CountdownSoundSeconds;
   static const float TimeLowSoundSeconds;

   ModeStateType m_ModeState;
   ConfigModeStateType m_ConfigModeState;
   BallStartModeType m_BallStartMode;
   BallStartAngleVelocityModeType m_BallStartAngleVelocityMode;
   BallStartCompleteModeType m_BallStartCompleteMode;
   BallEndLocationModeType m_BallEndLocationMode;
   BallEndFinishModeType m_BallPassFinishMode;
   BallEndFinishModeType m_BallFailFinishMode;
   BallEndAssociationModeType m_BallEndAssociationMode;
   BallEndCompleteModeType m_BallEndCompleteMode;
   BallCorridorCompleteModeType m_BallCorridorCompleteMode;
   DifficultyConfigModeState m_DifficultyConfigModeState;
   RunOrderModeType m_RunOrderMode;
   BallKickerBehaviorModeType m_BallKickerBehaviorMode;
   SoundEffectsModeType m_SoundEffectsMode;
   bool m_SoundEffectsPassEnabled;
   bool m_SoundEffectsFailEnabled;
   bool m_SoundEffectsTimeLowEnabled;
   bool m_SoundEffectsCountdownEnabled;
   bool m_SoundEffectsMenuPlayed;

   static const S32 GameplayDifficultyMinimum = 0;
   static const S32 GameplayDifficultyMaximum = 100;
   S32 m_GameplayDifficulty;

   static const S32 VarianceDifficultyMinimum = 0;
   static const S32 VarianceDifficultyMaximum = 100;
   S32 m_VarianceDifficulty;

   static const S32 GravityVarianceMinimum = 0;
   static const S32 GravityVarianceMaximum = 500;
   S32 m_GravityVariance;

   DifficultyVarianceModeType m_GravityVarianceMode;

   static const S32 PlayfieldFrictionVarianceMinimum = 0;
   static const S32 PlayfieldFrictionVarianceMaximum = 500;
   S32 m_PlayfieldFrictionVariance;

   DifficultyVarianceModeType m_PlayfieldFrictionVarianceMode;

   static const S32 FlipperStrengthVarianceMinimum = 0;
   static const S32 FlipperStrengthVarianceMaximum = 500;
   S32 m_FlipperStrengthVariance;

   DifficultyVarianceModeType m_FlipperStrengthVarianceMode;

   static const S32 FlipperFrictionVarianceMinimum = 0;
   static const S32 FlipperFrictionVarianceMaximum = 500;
   S32 m_FlipperFrictionVariance;

   DifficultyVarianceModeType m_FlipperFrictionVarianceMode;

   static const S32 TotalRunsMinimum = 1;
   static const S32 TotalRunsMaximum = 100;
   S32 m_TotalRuns;

   static const S32 MaxSecondsPerRunMinimum = 1;
   static const S32 MaxSecondsPerRunMaximum = 30;
   S32 m_MaxSecondsPerRun;

   static const S32 CountdownSecondsBeforeRunMinimum = 0;
   static const S32 CountdownSecondsBeforeRunMaximum = 5;
   S32 m_CountdownSecondsBeforeRun;

   std::vector<BallStartOptionsRecord> m_BallStartOptionsRecords;
   std::size_t m_BallStartOptionsRecordsSize;
   std::vector<BallEndOptionsRecord> m_BallPassOptionsRecords;
   std::vector<BallEndOptionsRecord> m_BallFailOptionsRecords;

   BallCorridorOptionsRecord m_BallCorridorOptionsRecord;

   std::vector<RunRecord> m_RunRecords;
   std::size_t m_CurrentRunRecord;
   int m_RunStartTimeMs;

   int m_CountdownSoundPlayed;
   bool m_TimeLowSoundPlaying;

   bool m_SetupBallStarts;

   bool m_SetupDifficulty;
   S32 m_GameplayDifficultyInitial;
   float m_GravityInitial;
   float m_PlayfieldFrictionInitial;
   float m_FlipperStrengthInitial;
   float m_FlipperFrictionInitial;

   TrainerOptions();
};

class NormalOptions
{
public:
   enum ModeStateType
   {
      ModeStateType_SelectCurrentBallHistory,
      ModeStateType_SelectRecallBallHistory,
      ModeStateType_ManageAutoControlLocations,
      ModeStateType_ClearAutoControlLocations,
      ModeStateType_GoBack,
      ModeStateType_COUNT
   };

   enum ConfigureRecallBallHistoryModeType
   {
      ConfigureRecallBallHistoryModeType_Select,
      ConfigureRecallBallHistoryModeType_Disable,
      ConfigureRecallBallHistoryModeType_COUNT
   };

   enum ClearAutoControlLocationsModeType
   {
      ClearAutoControlLocationsModeType_Clear,
      ClearAutoControlLocationsModeType_GoBack,
      ClearAutoControlLocationsModeType_COUNT
   };

   struct AutoControlVertex
   {
      Vertex3Ds m_Position;
      bool Active;
   };

   static const char * ImGuiDrawAutoControlVertexLabels[];
   static const char * ImGuiDrawRecallVertexLabels[];

   static const std::size_t RecallControlIndexDisabled = -1;
   static const std::size_t AutoControlVerticesMax = 256;

   static const float ManageAutoControlFindFactor;

   ModeStateType m_ModeState;

   ConfigureRecallBallHistoryModeType m_ConfigureRecallBallHistoryMode;
   std::size_t m_RecallControlIndex;

   ClearAutoControlLocationsModeType m_ClearAutoControlLocationsMode;

   std::vector<AutoControlVertex> m_AutoControlVertices;

   NormalOptions();
};

struct BallHistory
{
public:
   //TODO GARY Remove magic/global value
   static bool DrawMenu;

   BallHistory(PinTable &pinTable);
   void Init(Player &player, int currentTimeMs, bool loadSettings);
   void UnInit(Player &player);
   void Process(Player &player, int currentTimeMsec);
   bool ProcessKeys(Player &player, const DIDEVICEOBJECTDATA *input, int currentTimeMs, bool process);
   void ProcessMouse(Player &player, int currentTimeMs);
   bool Control();
   void SetControl(bool control);
   void ToggleControl();
   void ToggleRecall();
   void ResetTrainerRunStartTime();

   DWORD m_PreviousProcessKeysOfs;
   DWORD m_PreviousProcessKeysData;


private:

   enum Color
   {
      Black = 0x00000000,
      Blue = 0x00FF0000,
      Green = 0x0000FF00,
      Red = 0x0000000FF,
      Yellow = 0x0000FFFF,
      White = 0x00FFFFFF,
      Purple = 0x00800080
   };

   enum NextPreviousByType
   {
      eTimeMs,
      eDistancePixels
   };

   struct ProfilerRecord
   {
      struct ProfilerScope
      {
         U64 &m_ProfilerUsec;
         U64 m_TempUsec;

         ProfilerScope(U64 &usec);
         virtual ~ProfilerScope();
      };

      U64 m_ProcessUsec;
      U64 m_ShowStatusUsec;
      U64 m_ProcessMenuUsec;
      U64 m_DrawBallHistoryUsec;
      U64 m_ProcessModeNormalUsec;
      U64 m_ProcessModeTrainerUsec;
      U64 m_DrawTrainerBallsUsec;

      ProfilerRecord();
      void SetZero();
   };

   struct PrintScreenRecord
   {
   public:
      static void Init();
      static void ShowTable(const char *name, float positionX, float positionY, const std::vector<std::pair<std::string, std::string>> &nameValuePairs);
      static void ShowText(const char *name, float positionX, float positionY, const char *format, ...);
      static void MenuTitleText(const char *format, ...);
      static void MenuText(bool selected, const char *format, ...);
      static void ActiveMenuText(const char *format, ...);
      static void StatusText(const char *format, ...);
      static void ErrorText(const char *format, ...);

   private:
      static const char *ImGuiShowStatusLabel;
      static const char *ImGuiProcessMenuLabel;
      static const char *ImGuiActiveMenuLabel;
      static const char *ImGuiErrorLabel;

      static const float ShowTextFontSize;
      static const float MenuTitleTextFontSize;
      static const float MenuTextFontSize;
      static const float MenuSelectedTextFontSize;
      static const float ActiveMenuTextFontSize;
      static const float StatusTextFontSize;
      static const float ErrorTextFontSize;

      static const ImU32 ShowTextColor;
      static const ImU32 MenuTitleTextColor;
      static const ImU32 MenuTextColor;
      static const ImU32 MenuSelectedTextColor;
      static const ImU32 ActiveMenuTextColor;
      static const ImU32 StatusTextColor;
      static const ImU32 ErrorTextColor;

      static ImFont *m_ShowTextFont;
      static ImFont *m_MenuTitleTextFont;
      static ImFont *m_MenuTextFont;
      static ImFont *m_MenuSelectedTextFont;
      static ImFont *m_ActiveMenuTextFont;
      static ImFont *m_StatusTextFont;
      static ImFont *m_ErrorTextFont;

      static void Show(const char *name, ImFont *font, ImU32 color, float positionX, float positionY, bool center, const char *str);
   };

   struct MenuOptionsRecord
   {
      enum MenuActionType
      {
         MenuActionType_None,
         MenuActionType_Toggle,
         MenuActionType_UpLeft,
         MenuActionType_DownRight,
         MenuActionType_Enter,
         MenuActionType_COUNT
      };

      enum MenuStateType
      {
         MenuStateType_None,
         MenuStateType_Root_SelectMode,
         MenuStateType_Normal_SelectModeOptions,
         MenuStateType_Normal_SelectCurrentBallHistory,
         MenuStateType_Normal_ConfigureRecallBallHistory,
         MenuStateType_Normal_SelectRecallBallHistory,
         MenuStateType_Normal_ManageAutoControlLocations,
         MenuStateType_Normal_ClearAutoControlLocations,
         MenuStateType_Trainer_SelectModeOptions,
         MenuStateType_Trainer_SelectConfigModeOptions,
         MenuStateType_Trainer_Results,
         MenuStateType_Trainer_SelectBallStartMode,
         MenuStateType_Trainer_SelectExistingBallStartLocation,
         MenuStateType_Trainer_SelectCustomBallStart,
         MenuStateType_Trainer_SelectCustomBallStartLocation,
         MenuStateType_Trainer_SelectCustomBallStartAngleVelocityMode,
         MenuStateType_Trainer_SelectCustomBallStartVelocityStart,
         MenuStateType_Trainer_SelectCustomBallStartVelocityFinish,
         MenuStateType_Trainer_SelectCustomBallStartVelocityTotal,
         MenuStateType_Trainer_SelectCustomBallStartAngleStart,
         MenuStateType_Trainer_SelectCustomBallStartAngleFinish,
         MenuStateType_Trainer_SelectCustomBallStartAngleTotal,
         MenuStateType_Trainer_BallPassComplete,
         MenuStateType_Trainer_SelectBallPassManage,
         MenuStateType_Trainer_SelectBallPassLocation,
         MenuStateType_Trainer_SelectBallPassFinishMode,
         MenuStateType_Trainer_SelectBallPassDistance,
         MenuStateType_Trainer_SelectBallPassAssociations,
         MenuStateType_Trainer_BallFailComplete,
         MenuStateType_Trainer_SelectBallFailManage,
         MenuStateType_Trainer_SelectBallFailLocation,
         MenuStateType_Trainer_SelectBallFailFinishMode,
         MenuStateType_Trainer_SelectBallFailDistance,
         MenuStateType_Trainer_SelectBallFailAssociations,
         MenuStateType_Trainer_SelectBallCorridorComplete,
         MenuStateType_Trainer_SelectBallCorridorPassLocation,
         MenuStateType_Trainer_SelectBallCorridorPassWidth,
         MenuStateType_Trainer_SelectBallCorridorOpeningLeftLocation,
         MenuStateType_Trainer_SelectBallCorridorOpeningRightLocation,
         MenuStateType_Trainer_SelectDifficultyOptions,
         MenuStateType_Trainer_SelectDifficultyGameplayDifficulty,
         MenuStateType_Trainer_SelectDifficultyVarianceDifficulty,
         MenuStateType_Trainer_SelectDifficultyGravityVariance,
         MenuStateType_Trainer_SelectDifficultyGravityVarianceType,
         MenuStateType_Trainer_SelectDifficultyPlayfieldFrictionVariance,
         MenuStateType_Trainer_SelectDifficultyPlayfieldFrictionVarianceType,
         MenuStateType_Trainer_SelectDifficultyFlipperStrengthVariance,
         MenuStateType_Trainer_SelectDifficultyFlipperStrengthVarianceType,
         MenuStateType_Trainer_SelectDifficultyFlipperFrictionVariance,
         MenuStateType_Trainer_SelectDifficultyFlipperFrictionVarianceType,
         MenuStateType_Trainer_SelectTotalRuns,
         MenuStateType_Trainer_SelectRunOrderMode,
         MenuStateType_Trainer_SelectBallKickerBehaviorMode,
         MenuStateType_Trainer_SelectMaxSecondsPerRun,
         MenuStateType_Trainer_SelectCountdownSecondsBeforeRun,
         MenuStateType_Trainer_SelectSoundEffects,
         MenuStateType_Disabled_Disabled,
         MenuStateType_COUNT
      };

      enum ModeType
      {
         ModeType_Normal,
         ModeType_Trainer,
         ModeType_Disabled,
         ModeType_COUNT
      };

      static const int SkipKeyPressHoldMs;
      static const int SkipKeyIntervalMs;
      static const S32 SkipKeyStepFactor;

      static const int SkipControlIntervalMs;
      static const S32 SkipControlStepFactor;

      static const float DefaultBallRadius;

      MenuStateType m_MenuState;
      ModeType m_ModeType;
      std::string m_MenuError;

      S32 m_CreateZ;

      NormalOptions m_NormalOptions;
      TrainerOptions m_TrainerOptions;

      bool m_SkipKeyPressed;
      int m_SkipKeyPressedMs;
      bool m_SkipKeyLeft;
      int m_SkipKeyUsedMs;
      int m_SkipControlUsedMs;
      std::size_t m_CurrentBallIndex;
      std::size_t m_CurrentAssociationIndex;
      std::size_t m_CurrentCompleteIndex;
      POINT m_MousePosition2D;

      MenuOptionsRecord();
   };

   static const char * ImGuiDrawActiveBallKickersLabels[];
   static const char * ImGuiProcessModeTrainerLabel;
   static const char * ImGuiCurrentRunRecordLabel;

   static const std::size_t OneSecondMs;

   static const std::size_t BallHistorySizeDefault;
   static const NextPreviousByType NextPreviousByDefault;
   static const std::size_t BallHistoryControlStepMsDefault;
   static const float BallHistoryControlStepPixelsDefault;

   static const float BallHistoryMinPointSize;
   static const float BallHistoryMaxPointSize;
   static const float ControlVerticesDistanceMax;

   static const float DrawAngleVelocityRadiusExtraMinimum;
   static const float DrawAngleVelocityRadiusArc;
   static const float DrawAngleVelocityLengthMultiplier;
   static const float DrawAngleVelocityHeightOffset;

   static const int DrawBallBlinkMs;

   static const char *DescriptionSectionTitle;
   static const char *SummarySectionTitle;

   static const char *SettingsFileExtension;
   static const char *SettingsFolderName;
   static const char SettingsValueDelimeter;
   static const char *TableInfoSectionName;
   static const char *FilePathKeyName;
   static const char *TableNameKeyName;
   static const char *AuthorKeyName;
   static const char *VersionKeyName;
   static const char *DateSavedKeyName;
   static const char *NormalModeSettingsSectionName;
   static const char *NormalModeAutoControlVerticesPosition3DKeyName;
   static const char *TrainerModeSettingsSectionName;
   static const char *TrainerModeGameplayDifficultyKeyName;
   static const char *TrainerModeVarianceDifficultyKeyName;
   static const char *TrainerModeGravityVarianceKeyName;
   static const char *TrainerModeGravityVarianceModeKeyName;
   static const char *TrainerModePlayfieldFrictionVarianceKeyName;
   static const char *TrainerModePlayfieldFrictionVarianceModeKeyName;
   static const char *TrainerModeFlipperStrengthVarianceKeyName;
   static const char *TrainerModeFlipperStrengthVarianceModeKeyName;
   static const char *TrainerModeFlipperFrictionVarianceKeyName;
   static const char *TrainerModeFlipperFrictionVarianceModeKeyName;
   static const char *TrainerModeTotalRunsKeyName;
   static const char *TrainerModeRunOrderModeKeyName;
   static const char *TrainerModeBallKickerBehaviorModeKeyName;
   static const char *TrainerModeMaxSecondsPerRunKeyName;
   static const char *TrainerModeCountdownSecondsBeforeRunKeyName;
   static const char *TrainerModeSoundEffectsPassEnabledKeyName;
   static const char *TrainerModeSoundEffectsFailEnabledKeyName;
   static const char *TrainerModeSoundEffectsTimeLowEnabledKeyName;
   static const char *TrainerModeSoundEffectsCountdownEnabledKeyName;
   static const char *TrainerModeBallStartPositionKeyName;
   static const char *TrainerModeBallStartVelocityKeyName;
   static const char *TrainerModeBallStartAngularMomentumKeyName;
   static const char *TrainerModeBallStartAngleStartKeyName;
   static const char *TrainerModeBallStartAngleFinishKeyName;
   static const char *TrainerModeBallStartTotalAnglesKeyName;
   static const char *TrainerModeBallStartVelocityStartKeyName;
   static const char *TrainerModeBallStartVelocityFinishKeyName;
   static const char *TrainerModeBallStartTotalVelocitiesKeyName;
   static const char *TrainerModeBallPassPosition3DKeyName;
   static const char *TrainerModeBallPassRadiusPercentKeyName;
   static const char *TrainerModeBallPassAssociationsKeyName;
   static const char *TrainerModeBallFailPosition3DKeyName;
   static const char *TrainerModeBallFailRadiusPercentKeyName;
   static const char *TrainerModeBallFailAssociationsKeyName;
   static const char *TrainerModeBallCorridorPassPosition3DKeyName;
   static const char *TrainerModeBallCorridorPassRadiusPercentKeyName;
   static const char *TrainerModeBallCorridorOpeningPositionLeft3DKeyName;
   static const char *TrainerModeBallCorridorOpeningPositionRight3DKeyName;
   
   std::vector<HitBall *> m_ControlVBalls;
   std::vector<HitBall *> m_ControlVBallsPrevious;

   std::vector<Kicker *> m_ActiveBallKickers;

   std::vector<Flipper *> m_Flippers;

   ProfilerRecord m_ProfilerRecord;

   bool m_ShowStatus;
   bool m_Control;
   bool m_WasControlled;
   bool m_WasRecalled;
   std::size_t m_CurrentControlIndex;

   NextPreviousByType m_NextPreviousBy;
   int m_BallHistoryControlStepMs;
   float m_BallHistoryControlStepPixels;

   std::vector<BallHistoryRecord> m_BallHistoryRecords;
   std::size_t m_BallHistoryRecordsHeadIndex;
   std::size_t m_BallHistoryRecordsSize;
   float m_MaxBallVelocityPixels;

   std::map<std::string, CComObject<Ball>*> m_DrawnBalls;
   std::map<std::string, CComObject<Light>*> m_DrawnIntersectionCircles;
   std::map<std::string, CComObject<Rubber>*> m_DrawnLines;

   IDirect3DVertexDeclaration9 *m_VertexColorDeclaration;

   DWORD m_AutoControlBallColor;
   DWORD m_RecallBallColor;
   DWORD m_TrainerBallStartColor;
   DWORD m_TrainerBallPassColor;
   DWORD m_TrainerBallFailColor;
   DWORD m_TrainerBallCorridorPassColor;
   DWORD m_TrainerBallCorridorOpeningWallColor;
   DWORD m_TrainerBallCorridorOpeningEndColor;
   DWORD m_ActiveBallKickerColor;

   int m_UseTrailsForBallsInitialValue;

   MenuOptionsRecord m_MenuOptions;

   std::string m_SettingsFilePath;

   bool GetSettingsFileName(Player &player, std::string &fileName);
   void LoadSettings(Player &player);
   void LoadSettingsDifficultyVariance(Player &player, CSimpleIni &iniFile, const char *sectionName, const char *varianceKeyName, S32 &variance, const char *modeKeyName, TrainerOptions::DifficultyVarianceModeType &mode);
   bool LoadSettingsGetValue(CSimpleIni &iniFile, const char *sectionName, const char *keyName, std::istringstream &value);
   void SaveSettings(Player &player);
   void SaveSettingsDifficultyVariance(Player &player, CSimpleIni &iniFile, const char *sectionName, const char *varianceKeyName, S32 variance, const char *modeKeyName, TrainerOptions::DifficultyVarianceModeType mode);
   void InitBallsDecreased(Player &player);
   void InitBallsIncreased(Player &player);
   void InitControlVBalls(Player &player);
   void InitActiveBallKickers(PinTable &pinTable);
   void InitFlippers(PinTable &pinTable);
   void ControlNext();
   void ControlPrev();
   void ResetBallHistoryRenderSizes();
   void DrawBallHistory(Player &player);
   void DrawFakeBall(Player &player, const char * name, const Vertex3Ds &m_pos, float radius, DWORD color);
   void DrawFakeBall(Player &player, const char * name, Vertex3Ds &position, float radius, DWORD color, const Vertex3Ds *lineEndPosition, DWORD lineColor, int lineThickness);
   void DrawFakeBall(Player &player, const char * name, Vertex3Ds &position, DWORD color, const Vertex3Ds *lineEndPosition, DWORD lineColor, int lineThickness);
   void DrawLineRotate(Rubber &drawnLine, const Vertex3Ds& center, const Vertex3Ds& start, const Vertex3Ds& end);
   void DrawLine(Player &player, const char * name, const Vertex3Ds &positionA, const Vertex3Ds &positionB, DWORD color, int thickness);
   void DrawIntersectionCircle(Player &player, const char * name, Vertex3Ds &position, float intersectionRadius, DWORD color);
   void DrawNormalModeVisuals(Player &player, int currentTimeMs);
   void ClearDraws(Player &player);
   bool ShouldDrawTrainerBallStarts(std::size_t index, int currentTimeMs);
   bool ShouldDrawTrainerBallPasses(std::size_t index, int currentTimeMs);
   bool ShouldDrawTrainerBallFails(std::size_t index, int currentTimeMs);
   bool ShouldDrawTrainerBallCorridor(int currentTimeMs);
   bool ShouldDrawActiveBallKickers(int currentTimeMs);
   void DrawTrainerBallCorridorPass(Player &player, const char * name, TrainerOptions::BallCorridorOptionsRecord &bcor, Vertex3Ds *overridePosition = nullptr);
   void DrawTrainerBallCorridorOpeningLeft(Player &player, TrainerOptions::BallCorridorOptionsRecord &bcor);
   void DrawTrainerBallCorridorOpeningRight(Player &player, TrainerOptions::BallCorridorOptionsRecord &bcor);
   void DrawTrainerModeVisuals(Player &player, int currentTimeMs);
   void DrawTrainerBallCorridor(Player &player);
   void DrawActiveBallKickers(Player &player);
   void DrawAngleVelocityPreviewHelperAdd(Player &player, TrainerOptions::BallStartOptionsRecord &bsor, float angle, float velocity, float radius);
   void DrawAngleVelocityPreviewHelper(Player &player, TrainerOptions::BallStartOptionsRecord &bsor, float angleStep, float velocityStep, float radius);
   void DrawAngleVelocityPreview(Player &player, TrainerOptions::BallStartOptionsRecord &bsor);
   void CalculateAngleVelocityStep(TrainerOptions::BallStartOptionsRecord &bsor, float &angleStep, float &velocityStep);
   void UpdateBallState(BallHistoryRecord &ballHistoryRecord);
   void ShowStatus(Player &player, int currentTimeMs);
   void ShowRecallBall(Player &player);
   void ShowAutoControlVertices(Player &player);
   void ShowPreviousRunRecord();
   void ShowCurrentRunRecord(int currentTimeMs);
   void ShowBallStartOptionsRecord(TrainerOptions::BallStartOptionsRecord &bsor);
   void ShowBallEndOptionsRecord(TrainerOptions::BallEndOptionsRecord &beor);
   void ShowBallCorridorOptionsRecord(TrainerOptions::BallCorridorOptionsRecord &bcor);
   void ShowSection(const char *title, const std::vector<std::string> &description);
   void ShowDifficultyTableConstants(Player &player);
   void ShowDifficultyVarianceStatusesMenu(std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceMode(const std::string &difficultyVarianceName, TrainerOptions::DifficultyVarianceModeType varianceMode, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceRange(const std::string &difficultyVarianceName, TrainerOptions::DifficultyVarianceModeType varianceMode, S32 variance, float initial, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceStatusAll(Player &player, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceStatusSingle(const std::string &name, float current, S32 variance, float initial, TrainerOptions::DifficultyVarianceModeType mode, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceStatusGravity(Player &player, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceStatusPlayfieldFriction(Player &player, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceStatusFlipperStrength(Player &player, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowDifficultyVarianceStatusFlipperFriction(Player &player, std::vector<std::pair<std::string, std::string>> &difficultyVarianceStatuses);
   void ShowResult(std::size_t total, std::vector<DWORD> &timesMs, const char *type, const char *subType, std::vector<std::pair<std::string, std::string>> &results);
   template <class T> float CalculateStandardDeviation(std::vector<T> &values);
   float CalculateDifficultyVariance(Player &player, float initial, float current, S32 variance, TrainerOptions::DifficultyVarianceModeType varianceMode);
   void InitBallStartOptionRecords();
   void ProcessMenu(Player &player, MenuOptionsRecord::MenuActionType menuAction, int currentTimeMs);
   void ProcessMode(Player &player, int currentTimeMs);
   void ProcessModeNormal(Player &player);
   void ProcessModeTrainer(Player &player, int currentTimeMs);
   S32 ProcessMenuChangeValue(S32 value, S32 delta, S32 min, S32 max, bool skip);
   template <class T> void ProcessMenuChangeValueInc(T &value, S32 min, S32 max);
   template <class T> void ProcessMenuChangeValueIncSkip(T &value, S32 min, S32 max);
   template <class T> void ProcessMenuChangeValueDec(T &value, S32 min, S32 max);
   template <class T> void ProcessMenuChangeValueDecSkip(T &value, S32 min, S32 max);
   template <class T> void ProcessMenuChangeValueSkip(T &value, S32 min, S32 max, int currentTimeMs);
   template <class T> void ProcessMenuAction(MenuOptionsRecord::MenuActionType menuAction, MenuOptionsRecord::MenuStateType enterMenuState, T &value, S32 minimum, S32 maximum, int currentTimeMs);

   void Add(std::vector<HitBall *> &controlVBalls, int currentTimeMsec);
   BallHistoryRecord &Get(std::size_t index);
   std::size_t GetTailIndex();

   float GetDefaultBallRadius();
   Matrix3 GetDefaultBallOrientation();
   float DistancePixels(POINT &p1, POINT &p2);
   float DistancePixels(const Vertex3Ds &pos1, const Vertex3Ds &pos2);
   float DistanceToLineSegment(const Vertex3Ds &lineA, const Vertex3Ds &lineB, const Vertex3Ds &point);
   float VelocityPixels(const Vertex3Ds &vel);
   char GetBallHistoryKey(Player &player, EnumAssignKeys enumAssignKey);
   bool BallsReadyForTrainer();
   bool BallCorridorReadyForTrainer();
   POINT Get2DPointFrom3D(Player &player, const Vertex3Ds &vertex);
   Vertex3Ds Get3DPointFrom2D(const POINT &p, float heightZ);
   Vertex3Ds Get3DPointFromMousePosition(Player &player, float heightZ);
   bool Get2DMousePosition(Player &player, POINT &mousePosition2D, bool correct = true);
   Vertex3Ds GetKickerPosition(Kicker &kicker);
   void SetFlipperStrength(float flipperStrength);
   float GetFlipperStrength();
   void SetFlipperFriction(float flipperFriction);
   float GetFlipperFriction();
   bool ControlNextMove();
   bool ControlPrevMove();

   bool BallCountIncreased();
   bool BallCountDecreased();
   bool BallChanged();

   bool BallInsideAutoControlVertex(std::vector<HitBall *> &controlVBalls);

   std::vector<std::string> Split(const char *str, char delimeter);
   void CenterMouse(Player &player);

   void InvalidEnumValue(const char *enumName, const int enumValue);
   void InvalidEnumValue(const char *enumName, const char *enumValue);

   void PlaySound(UINT rcId, bool async = false);
   void StopSound();
};
