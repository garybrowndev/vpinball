#pragma once

#ifndef __BALLHISTORY_WIN32__

struct BallHistory 
{
public:
   static bool DrawMenu;

   BallHistory(PinTable &pinTable) { }
   void Init(Player &player, int currentTimeMs, bool loadSettings) { }
   void UnInit(Player &player) { }
   void Process(Player &player, int currentTimeMsec) { }
   bool ProcessKeys(Player &player, EnumAssignKeys action, bool isPressed, int currentTimeMs, bool process) { return false; }
   void ProcessMouse(Player &player, int currentTimeMs) { }
   bool Control() { return false; }
   void SetControl(bool control) { }
   void ToggleControl() { }
   void ToggleRecall() { }
   void ResetTrainerRunStartTime() { }

   EnumAssignKeys m_PreviousProcessKeysAction;
   bool m_PreviousProcessKeyIsPressed;
};

#else

#include <set>

#include "simpleini/SimpleIni.h"
#include "renderer/typedefs3D.h"
#include "imgui/imgui.h"
#include "physics/HitBall.h"

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
      ConfigModeStateType_GameplayDifficulty,
      ConfigModeStateType_PhysicsVariance,
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

   enum GameplayDifficultyConfigModeState
   {
      GameplayDifficultyConfigModeState_Accept,
      GameplayDifficultyConfigModeState_Override,
      GameplayDifficultyConfigModeState_Disable,
      GameplayDifficultyConfigModeState_COUNT
   };

   enum PhysicsVarianceConfigModeState
   {
      PhysicsVarianceConfigModeState_Accept,
      PhysicsVarianceConfigModeState_Volatility,
      PhysicsVarianceConfigModeState_GravitySpread,
      PhysicsVarianceConfigModeState_PlayfieldFrictionSpread,
      PhysicsVarianceConfigModeState_FlipperStrengthSpread,
      PhysicsVarianceConfigModeState_FlipperFrictionSpread,
      PhysicsVarianceConfigModeState_COUNT
   };

   enum PhysicsVarianceSpreadModeType
   {
      PhysicsVarianceSpreadModeType_AboveAndBelow,
      PhysicsVarianceSpreadModeType_AboveOnly,
      PhysicsVarianceSpreadModeType_BelowOnly,
      PhysicsVarianceSpreadModeType_COUNT
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

      static const int32_t AngleMinimum = 0;
      static const int32_t AngleMaximum = 360;

      static const int32_t TotalAnglesMinimum = 1;
      static const int32_t TotalAnglesMaximum = 36;

      static const int32_t VelocityMinimum = 0;
      static const int32_t VelocityMaximum = 100;

      static const int32_t TotalVelocitiesMinimum = 1;
      static const int32_t TotalVelocitiesMaximum = 20;

      Vertex3Ds m_StartPosition;
      Vertex3Ds m_StartVelocity;
      Vertex3Ds m_StartAngularMomentum;
      float m_AngleRangeStart;
      float m_AngleRangeFinish;
      int32_t m_TotalRangeAngles;
      float m_VelocityRangeStart;
      float m_VelocityRangeFinish;
      int32_t m_TotalRangeVelocities;

      BallStartOptionsRecord();
      BallStartOptionsRecord(const Vertex3Ds &startPosition, const Vertex3Ds &startVelocity, const Vertex3Ds &startAngularMomentum, float angleRangeStart, float angleRangeFinish, int32_t totalRangeAngles, float velocityRangeStart, float velocityRangeFinish, int32_t totalRangeVelocities);
      bool IsZero();
   };

   struct BallEndOptionsRecord
   {
      static const char * ImGuiBallPassLabels[];
      static const char * ImGuiBallFailLabels[];

      static const int32_t RadiusPercentMinimum = 2;
      static const int32_t RadiusPercentMaximum = 300;
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

      static const int32_t RadiusPercentMinimum = 0;
      static const int32_t RadiusPercentMaximum = 300;

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
   GameplayDifficultyConfigModeState m_GameplayDifficultyConfigModeState;
   PhysicsVarianceConfigModeState m_PhysicsVarianceConfigModeState;
   RunOrderModeType m_RunOrderMode;
   BallKickerBehaviorModeType m_BallKickerBehaviorMode;
   SoundEffectsModeType m_SoundEffectsMode;
   bool m_SoundEffectsPassEnabled;
   bool m_SoundEffectsFailEnabled;
   bool m_SoundEffectsTimeLowEnabled;
   bool m_SoundEffectsCountdownEnabled;
   bool m_SoundEffectsMenuPlayed;

   static const int32_t GameplayDifficultyMinimum = 0;
   static const int32_t GameplayDifficultyMaximum = 100;
   static const int32_t GameplayDifficultyDisabled = -1;
   int32_t m_GameplayDifficulty;

   static const int32_t VolatilityMinimum = 0;
   static const int32_t VolatilityMaximum = 100;
   int32_t m_Volatility;

   static const int32_t GravitySpreadMinimum = 0;
   static const int32_t GravitySpreadMaximum = 500;
   int32_t m_GravitySpread;

   PhysicsVarianceSpreadModeType m_GravitySpreadMode;

   static const int32_t PlayfieldFrictionSpreadMinimum = 0;
   static const int32_t PlayfieldFrictionSpreadMaximum = 500;
   int32_t m_PlayfieldFrictionSpread;

   PhysicsVarianceSpreadModeType m_PlayfieldFrictionSpreadMode;

   static const int32_t FlipperStrengthSpreadMinimum = 0;
   static const int32_t FlipperStrengthSpreadMaximum = 500;
   int32_t m_FlipperStrengthSpread;

   PhysicsVarianceSpreadModeType m_FlipperStrengthSpreadMode;

   static const int32_t FlipperFrictionSpreadMinimum = 0;
   static const int32_t FlipperFrictionSpreadMaximum = 500;
   int32_t m_FlipperFrictionSpread;

   PhysicsVarianceSpreadModeType m_FlipperFrictionSpreadMode;

   static const std::size_t PhysicsVariancePreviewRunCount = 6;

   static const int32_t TotalRunsMinimum = 1;
   static const int32_t TotalRunsMaximum = 100;
   int32_t m_TotalRuns;

   static const int32_t MaxSecondsPerRunMinimum = 1;
   static const int32_t MaxSecondsPerRunMaximum = 30;
   int32_t m_MaxSecondsPerRun;

   static const int32_t CountdownSecondsBeforeRunMinimum = 0;
   static const int32_t CountdownSecondsBeforeRunMaximum = 5;
   int32_t m_CountdownSecondsBeforeRun;

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
   int32_t m_GameplayDifficultyTableDefault;
   float m_GravityTableDefault;
   float m_PlayfieldFrictionTableDefault;
   float m_FlipperStrengthTableDefault;
   float m_FlipperFrictionTableDefault;

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
   static const char * ImGuiBallControlVBallLabels[];

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
   bool ProcessKeys(Player &player, EnumAssignKeys action, bool isPressed, int currentTimeMs, bool process);
   void ProcessMouse(Player &player, int currentTimeMs);
   bool Control();
   void SetControl(bool control);
   void ToggleControl();
   void ToggleRecall();
   void ResetTrainerRunStartTime();

   EnumAssignKeys m_PreviousProcessKeysAction;
   bool m_PreviousProcessKeyIsPressed;

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
         uint64_t &m_ProfilerUsec;
         uint64_t m_TempUsec;

         ProfilerScope(uint64_t &usec);
         virtual ~ProfilerScope();
      };

      uint64_t m_ProcessUsec;
      uint64_t m_ShowStatusUsec;
      uint64_t m_ProcessMenuUsec;
      uint64_t m_DrawBallHistoryUsec;
      uint64_t m_ProcessModeNormalUsec;
      uint64_t m_ProcessModeTrainerUsec;
      uint64_t m_DrawTrainerBallsUsec;

      ProfilerRecord();
      void SetZero();
   };

   struct PrintScreenRecord
   {
   public:
      enum Color
      {
         Black = IM_COL32_BLACK,
         Blue = IM_COL32(0x00, 0x00, 0xFF, 0xFF),
         Green = IM_COL32(0x00, 0xFF, 0x00, 0xFF),
         Red = IM_COL32(0xFF, 0x00, 0x00, 0xFF),
         Yellow = IM_COL32(0xFF, 0xFF, 0x00, 0xFF),
         White = IM_COL32_WHITE,
         Purple = IM_COL32(0x80, 0x00, 0x80, 0xFF)
      };

      static void Init();
      static void UnInit();
      static void TransformAspectRatio(float &positionX, float &positionY);
      static void Text(const char *name, float positionX, float positionY, const std::string &message);
      static void MenuTitleText(const std::string &message);
      static void MenuText(bool selected, const std::string &message);
      static void ActiveMenuText(const std::string &message);
      static void ErrorText(const std::string &message);
      static void Results(const std::vector<std::pair<std::string, std::string>> &nameValuePairs);
      static void Status(const std::vector<std::pair<std::string, std::string>> &nameValuePairs);
      static void ActiveMenu(const std::vector<std::pair<std::string, std::string>> &nameValuePairs);

   private:
      static const char *ImGuiProcessMenuLabel;
      static const char *ImGuiActiveMenuLabel;
      static const char *ImGuiStatusLabel;
      static const char *ImGuiCurrentRunRecordLabel;
      static const char *ImGuiErrorTopLabel;
      static const char *ImGuiErrorBottomLabel;

      static ImFont *NormalSmallFont;
      static ImFont *NormalMediumFont;
      static ImFont *BoldSmallFont;
      static ImFont *BoldMediumFont;
      static ImFont *BoldLargeFont;

      static void ShowText(const char *name, ImFont *font, const ImU32 &fontColor, float positionX, float positionY, bool center, const std::string &message);
      static void ShowNameValueTable(const char *name, ImFont *rowFont, const ImU32 &rowFontColor, ImFont *headerFont, const ImU32 &headerFontColor, float positionX, float positionY, const std::vector<std::pair<std::string, std::string>> &nameValuePairs, bool overflow, bool center, float *calculatedTableWidth);
      static void TransformPosition(float &positionX, float &positionY);
      static void SetWindowPosClamped(const char * name, const ImVec2 &position, const ImVec2 &size, bool center);
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
         MenuStateType_Trainer_SelectGameplayDifficultyOptions,
         MenuStateType_Trainer_SelectGameplayDifficultyOverride,
         MenuStateType_Trainer_SelectPhysicsVarianceOptions,
         MenuStateType_Trainer_SelectPhysicsVarianceVolatility,
         MenuStateType_Trainer_SelectPhysicsVarianceGravitySpread,
         MenuStateType_Trainer_SelectPhysicsVarianceGravitySpreadType,
         MenuStateType_Trainer_SelectPhysicsVariancePlayfieldFrictionSpread,
         MenuStateType_Trainer_SelectPhysicsVariancePlayfieldFrictionSpreadType,
         MenuStateType_Trainer_SelectPhysicsVarianceFlipperStrengthSpread,
         MenuStateType_Trainer_SelectPhysicsVarianceFlipperStrengthSpreadType,
         MenuStateType_Trainer_SelectPhysicsVarianceFlipperFrictionSpread,
         MenuStateType_Trainer_SelectPhysicsVarianceFlipperFrictionSpreadType,
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

      static const int SkipKeySlowPressedMs;
      static const int SkipKeyFastPressedMs;

      static const int SkipKeySlowIntervalMs;
      static const int SkipKeyFastIntervalMs;
      static const int32_t SkipKeyStepFactor;

      static const int SkipControlSlowIntervalMs;
      static const int32_t SkipControlStepFactor;

      static const float DefaultBallRadius;

      MenuStateType m_MenuState;
      ModeType m_ModeType;
      std::string m_MenuError;

      float m_CreateZ;

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

   static const int ShouldDrawBlinkMs;

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
   static const char *TrainerModePhysicsVarianceVolatilityKeyName;
   static const char *TrainerModePhysicsVarianceGravitySpreadKeyName;
   static const char *TrainerModePhysicsVarianceGravitySpreadModeKeyName;
   static const char *TrainerModePhysicsVariancePlayfieldFrictionSpreadKeyName;
   static const char *TrainerModePhysicsVariancePlayfieldFrictionSpreadModeKeyName;
   static const char *TrainerModePhysicsVarianceFlipperStrengthSpreadKeyName;
   static const char *TrainerModePhysicsVarianceFlipperStrengthSpreadModeKeyName;
   static const char *TrainerModePhysicsVarianceFlipperFrictionSpreadKeyName;
   static const char *TrainerModePhysicsVarianceFlipperFrictionSpreadModeKeyName;
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

   bool GetSettingsFileName(Player &player, std::string &settingsFileName);
   bool GetSettingsFolderPath(std::string &settingsFolderPath);

   void LoadSettings(Player &player);
   void LoadSettingsPhysicsVarianceSpread(Player &player, CSimpleIni &iniFile, const char *sectionName, const char *spreadKeyName, int32_t &spread, const char *modeKeyName, TrainerOptions::PhysicsVarianceSpreadModeType &mode);
   bool LoadSettingsGetValue(CSimpleIni &iniFile, const char *sectionName, const char *keyName, std::istringstream &value);
   void SaveSettings(Player &player);
   void SaveSettingsPhysicsVarianceSpread(Player &player, CSimpleIni &iniFile, const char *sectionName, const char *spreadKeyName, int32_t spread, const char *modeKeyName, TrainerOptions::PhysicsVarianceSpreadModeType mode);
   void InitBallsDecreased(Player &player);
   void InitBallsIncreased(Player &player);
   void InitControlVBalls(Player &player);
   void InitActiveBallKickers(PinTable &pinTable);
   void InitFlippers(PinTable &pinTable);
   void ControlNext();
   void ControlPrev();
   void ResetBallHistoryRenderSizes();
   void DrawBallHistory(Player &player);
   void DrawFakeBall(Player &player, const std::string& name, const Vertex3Ds &m_pos, float radius, DWORD color);
   void DrawFakeBall(Player &player, const std::string& name, Vertex3Ds &position, float radius, DWORD color, const Vertex3Ds *lineEndPosition, DWORD lineColor, int lineThickness);
   void DrawFakeBall(Player &player, const std::string& name, Vertex3Ds &position, DWORD color, const Vertex3Ds *lineEndPosition, DWORD lineColor, int lineThickness);
   void DrawLineRotate(Rubber &drawnLine, const Vertex3Ds& center, const Vertex3Ds& start, const Vertex3Ds& end);
   void DrawLine(Player &player, const std::string& name, const Vertex3Ds &positionA, const Vertex3Ds &positionB, DWORD color, int thickness);
   void DrawIntersectionCircle(Player &player, const std::string& name, Vertex3Ds &position, float intersectionRadius, DWORD color);
   void DrawControlVBalls(Player &player);
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
   void DrawActiveBallKickerCheckPosition(Player &player, POINT checkPosition, int xMax, int yMax, std::string &kickerText);
   void DrawActiveBallKickers(Player &player);
   void DrawAngleVelocityPreviewHelperAdd(Player &player, TrainerOptions::BallStartOptionsRecord &bsor, float angle, float velocity, float radius);
   void DrawAngleVelocityPreviewHelper(Player &player, TrainerOptions::BallStartOptionsRecord &bsor, float angleStep, float velocityStep, float radius);
   void DrawAngleVelocityPreview(Player &player, TrainerOptions::BallStartOptionsRecord &bsor);
   void CalculateAngleVelocityStep(TrainerOptions::BallStartOptionsRecord &bsor, float &angleStep, float &velocityStep);
   void UpdateBallState(BallHistoryRecord &ballHistoryRecord);
   void ShowStatus(Player &player, int currentTimeMs);
   void ShowRecallBall(Player &player);
   void ShowAutoControlVertices(Player &player);
   void ShowRemainingRunInfo();
   void ShowPreviousRunRecord();
   void ShowCurrentRunRecord(int currentTimeMs);
   TrainerOptions::BallStartAngleVelocityModeType GetBallStartAngleVelocityMode(TrainerOptions::BallStartOptionsRecord& bsor);
   void GetBallStartOptionsConfigs(std::vector<std::pair<std::string, std::string>> &ballStartOptionsConfig);
   void GetBallStartOptionsConfig(TrainerOptions::BallStartOptionsRecord& bsor, std::size_t bsorIndex, std::vector<std::pair<std::string, std::string>> &ballStartOptionsConfig);
   void GetBallPassOptionsConfigs(std::vector<std::pair<std::string, std::string>> &ballPassOptionsConfig);
   void GetBallPassOptionsConfig(TrainerOptions::BallEndOptionsRecord &beor, std::size_t beorIndex, std::vector<std::pair<std::string, std::string>> &ballPassOptionsConfig);
   void GetBallFailOptionsConfigs(std::vector<std::pair<std::string, std::string>> &ballFailOptionsConfig);
   void GetBallFailOptionsConfig(TrainerOptions::BallEndOptionsRecord &beor, std::size_t beorIndex, std::vector<std::pair<std::string, std::string>> &ballFailOptionsConfig);
   void GetBallEndOptionsConfig(TrainerOptions::BallEndOptionsRecord &beor, std::vector<std::pair<std::string, std::string>> &ballEndOptionsConfig);
   void GetBallCorridorOptionsConfigs(std::vector<std::pair<std::string, std::string>> &ballCorridorOptionsConfig);
   void ShowSection(const char *title, const std::vector<std::string> &description);
   void GetGameplayDifficultyConfigValues(std::vector<std::pair<std::string, std::string>> &difficultySpreadConfig);
   void GetPhysicsVarianceVolatilityConfig(std::vector<std::pair<std::string, std::string>> &physicsVarianceVolatilityConfig);
   void GetPhysicsVarianceSpreadConfigMode(TrainerOptions::PhysicsVarianceSpreadModeType spreadMode, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool newlines);
   void GetPhysicsVarianceSpreadConfigRange(TrainerOptions::PhysicsVarianceSpreadModeType spreadMode, int32_t spread, float initial, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool newlines);
   void GetPhysicsVarianceSpreadConfigAll(Player &player, std::vector<std::pair<std::string, std::string>> &PhysicsVarianceSpreadConfig, bool showPreview, bool newlines);
   void GetPhysicsVarianceSpreadConfigSingle(const std::string &name, float current, int32_t spread, float initial, TrainerOptions::PhysicsVarianceSpreadModeType mode, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool showPreview, bool newlines);
   void GetPhysicsVarianceSpreadConfigGravity(Player &player, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool showPreview, bool newlines);
   void GetPhysicsVarianceSpreadConfigPlayfieldFriction(Player &player, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool showPreview, bool newlines);
   void GetPhysicsVarianceSpreadConfigFlipperStrength(Player &player, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool showPreview, bool newlines);
   void GetPhysicsVarianceSpreadConfigFlipperFriction(Player &player, std::vector<std::pair<std::string, std::string>> &physicsVarianceSpreadConfig, bool showPreview, bool newlines);
   void ShowResult(std::size_t total, std::vector<DWORD> &timesMs, const char *type, const char *subType, std::vector<std::pair<std::string, std::string>> &results);
   template <class T> float CalculateStandardDeviation(std::vector<T> &values);
   float CalculatePhysicsVarianceSpread(Player &player, float initial, float current, int32_t spread, TrainerOptions::PhysicsVarianceSpreadModeType spreadMode, bool useRand);
   void InitBallStartOptionRecords();
   std::size_t GetTotalPermutations();
   void ProcessMenu(Player &player, MenuOptionsRecord::MenuActionType menuAction, int currentTimeMs);
   void ProcessMode(Player &player, int currentTimeMs);
   void ProcessModeNormal(Player &player);
   void ProcessModeTrainer(Player &player, int currentTimeMs);
   int32_t ProcessMenuChangeValue(int32_t value, int32_t delta, int32_t min, int32_t max, bool skip);
   template <class T, class S> void ProcessMenuChangeValueInc(T &value, S min, S max);
   template <class T, class S> void ProcessMenuChangeValueIncSkip(T &value, S min, S max);
   template <class T, class S> void ProcessMenuChangeValueDec(T &value, S min, S max);
   template <class T, class S> void ProcessMenuChangeValueDecSkip(T &value, S min, S max);
   template <class T, class S> void ProcessMenuChangeValueSkip(T &value, S min, S max, int currentTimeMs);
   template <class T, class S> void ProcessMenuAction(MenuOptionsRecord::MenuActionType menuAction, MenuOptionsRecord::MenuStateType enterMenuState, T &value, S minimum, S maximum, int currentTimeMs);

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
   void InvalidEnumValue(const char *enumName, const std::string& enumValue);

   void PlaySound(UINT rcId, bool async = false);
   void StopSound();
   void StopAllSounds();

   std::string FormatFloat(float val);
};

#endif