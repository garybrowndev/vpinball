#pragma once

#include "kdtree.h"
#include "quadtree.h"
#include "Debugger.h"

#include <set>

#define DEFAULT_PLAYER_WIDTH 1024
#define DEFAULT_PLAYER_FS_WIDTH 1920
#define DEFAULT_PLAYER_FS_REFRESHRATE 60

constexpr int DBG_SPRITE_SIZE = 1024;

enum VRPreviewMode
{
   VRPREVIEW_DISABLED,
   VRPREVIEW_LEFT,
   VRPREVIEW_RIGHT,
   VRPREVIEW_BOTH
};

// NOTE that the following four definitions need to be in sync in their order!
enum EnumAssignKeys
{
   eLeftFlipperKey,
   eRightFlipperKey,
   eLeftTiltKey,
   eRightTiltKey,
   eCenterTiltKey,
   ePlungerKey,
   eFrameCount,
   eDBGBalls,
   eDebugger,
   eAddCreditKey,
   eAddCreditKey2,
   eStartGameKey,
   eMechanicalTilt,
   eRightMagnaSave,
   eLeftMagnaSave,
   eExitGame,
   eVolumeUp,
   eVolumeDown,
   eLockbarKey,
   eEnable3D,
   eEscape,
   eBallHistoryMenu,
   eBallHistoryRecall,
   eCKeys
};
static const char* regkey_string[eCKeys] = {
   "LFlipKey",
   "RFlipKey",
   "LTiltKey",
   "RTiltKey",
   "CTiltKey",
   "PlungerKey",
   "FrameCount",
   "DebugBalls",
   "Debugger",
   "AddCreditKey",
   "AddCreditKey2",
   "StartGameKey",
   "MechTilt",
   "RMagnaSave",
   "LMagnaSave",
   "ExitGameKey",
   "VolumeUp",
   "VolumeDown",
   "LockbarKey",
   "Enable3DKey",
   "EscapeKey",
   "BallHistoryMenu",
   "BallHistoryRecall"
};
static constexpr int regkey_defdik[eCKeys] = {
   DIK_LSHIFT,
   DIK_RSHIFT,
   DIK_Z,
   DIK_SLASH,
   DIK_SPACE,
   DIK_RETURN,
   DIK_F11,
   DIK_O,
   DIK_D,
   DIK_5,
   DIK_4,
   DIK_1,
   DIK_T,
   DIK_RCONTROL,
   DIK_LCONTROL,
   DIK_Q,
   DIK_EQUALS,
   DIK_MINUS,
   DIK_LALT,
   DIK_F10,
   DIK_ESCAPE,
   DIK_C,
   DIK_R
};
static constexpr int regkey_idc[eCKeys] = {
   IDC_LEFTFLIPPER,
   IDC_RIGHTFLIPPER,
   IDC_LEFTTILT,
   IDC_RIGHTTILT,
   IDC_CENTERTILT,
   IDC_PLUNGER_TEXT,
   IDC_FRAMECOUNT,
   IDC_DEBUGBALL,
   IDC_DEBUGGER,
   IDC_ADDCREDIT,
   IDC_ADDCREDITKEY2,
   IDC_STARTGAME,
   IDC_MECHTILT,
   IDC_RMAGSAVE,
   IDC_LMAGSAVE,
   IDC_EXITGAME,
   IDC_VOLUMEUP,
   IDC_VOLUMEDN,
   IDC_LOCKBAR,
   -1, //!! missing in key dialog!
   -1,
   IDC_BALLHISTORYMENU,
   IDC_BALLHISTORYRECALL,
};

#ifndef ENABLE_SDL
// Note: Nowadays the original code seems to be counter-productive, so we use the official
// pre-rendered frame mechanism instead where possible
// (e.g. all windows versions except for XP and no "EnableLegacyMaximumPreRenderedFrames" set in the registry)
/*
 * Class to limit the length of the GPU command buffer queue to at most 'numFrames' frames.
 * Excessive buffering of GPU commands creates high latency and can create stuttery overlong
 * frames when the CPU stalls due to a full command buffer ring.
 *
 * Calling Execute() within a BeginScene() / EndScene() pair creates an artificial pipeline
 * stall by locking a vertex buffer which was rendered from (numFrames-1) frames ago. This
 * forces the CPU to wait and let the GPU catch up so that rendering doesn't lag more than
 * numFrames behind the CPU. It does *NOT* limit the framerate itself, only the drawahead.
 * Note that VP is currently usually GPU-bound.
 *
 * This is similar to Flush() in later DX versions, but doesn't flush the entire command
 * buffer, only up to a certain previous frame.
 *
 * Use of this class has been observed to effectively reduce stutter at least on an NVidia/
 * Win7 64 bit setup. The queue limiting effect can be clearly seen in GPUView.
 *
 * The initial cause for the stutter may be that our command buffers are too big (two
 * packets per frame on typical tables, instead of one), so with more optimizations to
 * draw calls/state changes, none of this may be needed anymore.
 */
class FrameQueueLimiter
{
public:
   void Init(RenderDevice * const pd3dDevice, const int numFrames)
   {
      const int EnableLegacyMaximumPreRenderedFrames = LoadValueIntWithDefault(regKey[RegName::Player], "EnableLegacyMaximumPreRenderedFrames"s, 0);

      // if available, use the official RenderDevice mechanism
      if (!EnableLegacyMaximumPreRenderedFrames && pd3dDevice->SetMaximumPreRenderedFrames(numFrames))
      {
          m_buffers.resize(0);
          return;
      }

      // if not, fallback to cheating the driver
      m_buffers.resize(numFrames, nullptr);
      m_curIdx = 0;
   }

   void Shutdown()
   {
      for (size_t i = 0; i < m_buffers.size(); ++i)
         SAFE_BUFFER_RELEASE(m_buffers[i]);
   }

   void Execute(RenderDevice * const pd3dDevice)
   {
      if (m_buffers.empty())
         return;

      if (m_buffers[m_curIdx])
         pd3dDevice->DrawPrimitiveVB(RenderDevice::TRIANGLEFAN, MY_D3DFVF_NOTEX2_VERTEX, m_buffers[m_curIdx], 0, 3, true);

      m_curIdx = (m_curIdx + 1) % m_buffers.size();

      if (!m_buffers[m_curIdx])
         VertexBuffer::CreateVertexBuffer(1024, 0, MY_D3DFVF_NOTEX2_VERTEX, &m_buffers[m_curIdx], PRIMARY_DEVICE);

      // idea: locking a static vertex buffer stalls the pipeline if that VB is still
      // in the GPU render queue. In effect, this lets the GPU catch up.
      Vertex3D_NoTex2* buf;
      m_buffers[m_curIdx]->lock(0, 0, (void**)&buf, VertexBuffer::WRITEONLY);
      memset(buf, 0, 3 * sizeof(buf[0]));
      buf[0].z = buf[1].z = buf[2].z = 1e5f;      // single triangle, degenerates to point far off screen
      m_buffers[m_curIdx]->unlock();
   }

   FrameQueueLimiter()
   {
      m_curIdx = 0;
   }

private:
   vector<VertexBuffer*> m_buffers;
   size_t m_curIdx;
};
#else
class FrameQueueLimiter
{
public:
   static void Init(RenderDevice * const pd3dDevice, const int numFrames)
   {
      pd3dDevice->SetMaximumPreRenderedFrames(numFrames);
   }
};
#endif

////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_NUDGE
# define IF_DEBUG_NUDGE(code) code
#else
# define IF_DEBUG_NUDGE(code)
#endif

class NudgeFilter
{
public:
   NudgeFilter();

   // adjust an acceleration sample (m_Nudge.x or m_Nudge.y)
   void sample(float &a, const U64 frameTime);

private:
   // debug output
   IF_DEBUG_NUDGE(void dbg(const char *fmt, ...);)
   IF_DEBUG_NUDGE(virtual const char *axis() const = 0;)

   // running total of samples
   float m_sum;

   // previous sample
   float m_prv;

   // timestamp of last zero crossing in the raw acceleration data
   U64 m_tzc;

   // timestamp of last correction inserted into the data
   U64 m_tCorr;

   // timestamp of last motion == start of rest
   U64 m_tMotion;
};

class NudgeFilterX : public NudgeFilter
{
   const char *axis() const { return "x"; }
};
class NudgeFilterY : public NudgeFilter
{
   const char *axis() const { return "y"; }
};

////////////////////////////////////////////////////////////////////////////////

struct BallHistoryState
{
   Vertex3Ds m_Pos;
   Vertex3Ds m_Vel;
   Vertex3Ds m_AngMom;
   Vertex3Ds m_LastEventPos;

   Matrix3 m_Orientation;

   Vertex3Ds m_OldPos[MAX_BALL_TRAIL_POS];
   unsigned int m_RingCounter_OldPos;

   float m_DrawRadius;
   Texture *m_Texture;

   BallHistoryState();
};

struct BallHistoryRecord
{
   int m_TimeMs;
   std::vector<BallHistoryState> m_BallHistoryStates;

   BallHistoryRecord();
   BallHistoryRecord(int timeMs);
   void Set(const Ball * controlVBall, BallHistoryState &bhr);
   void Set(std::vector<Ball*> &controlVBalls, int timeMs);
   void Insert(const Ball * controlVBall, int insertIndex);
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
      ConfigModeStateType_TotalRuns,
      ConfigModeStateType_RunOrder,
      ConfigModeStateType_BallKickerBehavior,
      ConfigModeStateType_MaxSecondsPerRun,
      ConfigModeStateType_CountdownSecondsBeforeRun,
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
      BallEndFinishModeType_Stop,
      BallEndFinishModeType_Distance,
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

   enum BallKickerBehaviorModeType
   {
      BallKickerBehaviorModeType_Reset,
      BallKickerBehaviorModeType_Fail,
      BallKickerBehaviorModeType_COUNT
   };

   enum RunOrderModeType
   {
      RunOrderModeType_InOrder,
      RunOrderModeType_Random,
      RunOrderModeType_COUNT
   };

   struct BallStartOptionsRecord
   {
      static const S32 AngleMinimum = 0;
      static const S32 AngleMaximum = 360;

      static const S32 TotalAnglesMinimum = 1;
      static const S32 TotalAnglesMaximum = 36;

      static const S32 VelocityMinimum = 0;
      static const S32 VelocityMaximum = 100;

      static const S32 TotalVelocitiesMinimum = 1;
      static const S32 TotalVelocitiesMaximum = 20;

      Vertex3Ds m_Pos;
      Vertex3Ds m_Vel;
      Vertex3Ds m_AngMom;
      float m_AngleStart;
      float m_AngleFinish;
      S32 m_TotalAngles;
      float m_VelocityStart;
      float m_VelocityFinish;
      S32 m_TotalVelocities;

      BallStartOptionsRecord();
      BallStartOptionsRecord(Vertex3Ds &pos, Vertex3Ds &vel, Vertex3Ds &angMom,
         float angleStart, float angleFinish, S32 totalAngles,
         float velocityStart, float velocityFinish, S32 totalVelocities);
      bool IsZero();
   };

   struct BallEndOptionsRecord
   {
      static const S32 RadiusPercentMinimum = 1;
      static const S32 RadiusPercentMaximum = 300;
      static const float RadiusPercentDisabled;

      Vertex3Ds m_Pos;
      float m_RadiusPercent;
      std::set<std::size_t> m_AssociatedBallStartIndexes;

      std::vector<std::tuple<int, Vertex3Ds>> m_StopBallsTracker;

      BallEndOptionsRecord();
      BallEndOptionsRecord(Vertex3Ds &pos, float radiusPercent);
   };

   struct RunRecord
   {
      enum ResultType
      {
         ResultType_Passed,
         ResultType_FailedLocation,
         ResultType_FailedTimeElapsed,
         ResultType_FailedKicker
      };

      std::vector<Vertex3Ds> m_StartPositions;
      std::vector<Vertex3Ds> m_StartVelocities;
      std::vector<Vertex3Ds> m_StartAngularMomentums;
      ResultType m_Result;
      int m_TotalTimeMs;
      std::vector<std::tuple<std::size_t, std::size_t>> m_StartToPassLocationIndexes;
      std::vector<std::tuple<std::size_t, std::size_t>> m_StartToFailLocationIndexes;

      RunRecord();
   };

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
   BallKickerBehaviorModeType m_BallKickerBehaviorMode;
   RunOrderModeType m_RunOrderMode;

   S32 m_CreateBallEndZ;

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

   std::vector<RunRecord> m_RunRecords;
   std::size_t m_CurrentRunRecord;
   int m_RunStartTimeMs;
   bool m_SetupBallStarts;

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

   enum SetupRecallBallHistoryModeType
   {
      SetupRecallBallHistoryModeType_Select,
      SetupRecallBallHistoryModeType_Disable,
      SetupRecallBallHistoryModeType_COUNT
   };

   enum ClearAutoControlLocationsModeType
   {
      ClearAutoControlLocationsModeType_Clear,
      ClearAutoControlLocationsModeType_GoBack,
      ClearAutoControlLocationsModeType_COUNT
   };

   struct AutoControlVertex
   {
      Vertex3Ds m_Pos;
      bool Active;
   };

   static const std::size_t RecallControlIndexDisabled = -1;
   static const std::size_t AutoControlVerticesMax = 256;

   static const float ManageAutoControlFindFactor;

   ModeStateType m_ModeState;

   SetupRecallBallHistoryModeType m_SetupRecallBallHistoryMode;
   std::size_t m_RecallControlIndex;

   ClearAutoControlLocationsModeType m_ClearAutoControlLocationsMode;

   S32 m_CreateZ;

   std::vector<AutoControlVertex> m_AutoControlVertices;

   NormalOptions();
};

struct BallHistory
{
public:
   BallHistory();
   void Init(Player &player, int currentTimeMs, bool loadSettings);
   void UnInit(Player &player);
   void Process(Player &player, int currentTimeMsec);
   bool ProcessKeys(Player &player, const DIDEVICEOBJECTDATA * input, int currentTimeMs);
   void ProcessMouse(Player &player, int currentTimeMs);
   bool Control();
   void SetControl(bool control);
   void ToggleControl();
   void ToggleRecall();
   void ResetTrainerRunStartTime();

private:
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
         ~ProfilerScope();
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

   struct DebugFontRecord
   {
      static const char * FontTypeFace;
      ID3DXFont * m_TitleFont;
      ID3DXFont * m_NormalFont;
      ID3DXFont * m_SelectFont;
      LPD3DXSPRITE m_FontSprite;

      DebugFontRecord();
      virtual ~DebugFontRecord();
      void Init(Player &player);
      void UnInit();
   };

   struct DebugPrintRecord
   {
      static D3DCOLOR NormalMenuColor;
      static D3DCOLOR SelectedMenuColor;
      static D3DCOLOR ErrorMenuColor;

      Player &m_Player;
      DebugFontRecord &m_DebugFontRecord;

      int m_TextX;
      int m_TextY;
      int m_TextYStep;
      char m_StrBuffer[1024];
      
      DebugPrintRecord(Player &player, DebugFontRecord &debugFontRecord);
      void InitTextXY();
      void SetPosition(float x, float y);
      void SetPositionPercent(float x, float y);
      void ToggleReverse();
      void ShowText(const char * format, ...);
      void ShowTextTitle(const char * format, ...);
      void ShowMenuText(const char * format, ...);
      void ShowMenuTextPos(int x, int y, const char * format, ...);
      void ShowMenuTextTitle(const char * format, ...);
      void ShowMenuTextError(const char * format, ...);
      void ShowMenuTextSelect(bool selected, const char * format, ...);
      void SetDebugOutputPosition(const float x, const float y);
      void DebugPrint(int x, int y, LPCSTR text, bool center, D3DCOLOR color, ID3DXFont * font);
   };

   struct Vertex3DColor
   {
      D3DVALUE x;
      D3DVALUE y;
      D3DVALUE z;

      D3DCOLOR color;

      Vertex3DColor();
      Vertex3DColor(D3DVALUE x, D3DVALUE y, D3DVALUE z, D3DCOLOR color);
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
         MenuStateType_Normal_SetupRecallBallHistory,
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
         MenuStateType_Trainer_SelectBallPassAccept,
         MenuStateType_Trainer_SelectBallPassLocation,
         MenuStateType_Trainer_SelectBallPassFinishMode,
         MenuStateType_Trainer_SelectBallPassDistance,
         MenuStateType_Trainer_SelectBallPassAssociations,
         MenuStateType_Trainer_BallFailComplete,
         MenuStateType_Trainer_SelectBallFailAccept,
         MenuStateType_Trainer_SelectBallFailLocation,
         MenuStateType_Trainer_SelectBallFailFinishMode,
         MenuStateType_Trainer_SelectBallFailDistance,
         MenuStateType_Trainer_SelectBallFailAssociations,
         MenuStateType_Trainer_SelectTotalRuns,
         MenuStateType_Trainer_SelectRunOrderMode,
         MenuStateType_Trainer_SelectBallKickerBehaviorMode,
         MenuStateType_Trainer_SelectMaxSecondsPerRun,
         MenuStateType_Trainer_SelectCountdownSecondsBeforeRun,
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

      MenuStateType m_MenuState;
      ModeType m_ModeType;
      std::string m_MenuError;

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

   static const std::size_t OneSecondMs;

   static const std::size_t BallHistorySizeDefault;
   static const NextPreviousByType NextPreviousByDefault;
   static const std::size_t BallHistoryControlStepMsDefault;
   static const float BallHistoryControlStepPixelsDefault;

   static const float BallHistoryMinPointSize;
   static const float BallHistoryMaxPointSize;
   static const float ControlVerticesDistanceMax;

   std::vector<Ball*> m_ControlVBalls;
   std::vector<Ball*> m_ControlVBallsPrevious;

   std::vector<Kicker*> m_ActiveBallKickers;

   ProfilerRecord m_ProfilerRecord;

   DebugFontRecord m_DebugFontRecord;

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

   Texture * m_AutoControlBallTexture;
   Texture * m_RecallBallTexture;
   Texture * m_TrainerBallStartTexture;
   Texture * m_TrainerBallPassTexture;
   Texture * m_TrainerBallFailTexture;
   Texture * m_ActiveBallKickerTexture;
   std::map<U32, Texture*> m_ControlHistoryBallTextures;

   int m_UseTrailsForBallsInitialValue;

   MenuOptionsRecord m_MenuOptions;

   std::string m_SettingsFilePath;

   static const float DrawAngleVelocityRadiusExtraMinimum;
   static const float DrawAngleVelocityRadiusArc;
   static const float DrawAngleVelocityLengthMultiplier;
   static const float DrawAngleVelocityHeightOffset;

   static const int DrawBallBlinkMs;

   static const D3DCOLOR IntersectionCircleColor;

   static const char * SettingsFileExtension;
   static const char * SettingsFolderName;
   static const char SettingsValueDelimeter;
   static const char * TableInfoSectionName;
   static const char * FilePathKeyName;
   static const char * TableNameKeyName;
   static const char * AuthorKeyName;
   static const char * VersionKeyName;
   static const char * DateSavedKeyName;
   static const char * NormalModeSettingsSectionName;
   static const char * NormalModeAutoControlVerticesPosition3DKeyName;
   static const char * TrainerModeSettingsSectionName;
   static const char * TrainerModeTotalRunsKeyName;
   static const char * TrainerModeRunOrderModeKeyName;
   static const char * TrainerModeBallKickerBehaviorModeKeyName;
   static const char * TrainerModeMaxSecondsPerRunKeyName;
   static const char * TrainerModeCountdownSecondsBeforeRunKeyName;
   static const char * TrainerModeBallStartPositionKeyName;
   static const char * TrainerModeBallStartVelocityKeyName;
   static const char * TrainerModeBallStartAngularMomentumKeyName;
   static const char * TrainerModeBallStartAngleStartKeyName;
   static const char * TrainerModeBallStartAngleFinishKeyName;
   static const char * TrainerModeBallStartTotalAnglesKeyName;
   static const char * TrainerModeBallStartVelocityStartKeyName;
   static const char * TrainerModeBallStartVelocityFinishKeyName;
   static const char * TrainerModeBallStartTotalVelocitiesKeyName;
   static const char * TrainerModeBallPassPosition3DKeyName;
   static const char * TrainerModeBallPassRadiusPercentKeyName;
   static const char * TrainerModeBallPassAssociationsKeyName;
   static const char * TrainerModeBallFailPosition3DKeyName;
   static const char * TrainerModeBallFailRadiusPercentKeyName;
   static const char * TrainerModeBallFailAssociationsKeyName;

   bool GetSettingsFileName(Player &player, std::string &fileName);
   void LoadSettings(Player &player);
   void SaveSettings(Player &player);
   void InitBallsDecreased(Player &player);
   void InitBallsIncreased(Player &player);
   void InitControlVBalls(Player &player);
   void InitActiveBallKickers(Player &player);
   void ControlNext();
   void ControlPrev();
   void ResetBallHistoryRenderSizes();
   void DrawBallHistory(Player &player);
   void DrawLine(Player &player, const Vertex3Ds &posA, const Vertex3Ds &posB, D3DCOLOR color);
   void DrawIntersectionCircle(Player &player, Vertex3Ds &pos, float ballRadius, float intersectionRadius, D3DCOLOR color);
   void DrawAutoControlVertices(Player &player, DebugPrintRecord &dpr, int currentTimeMs);
   void DrawFakeBallAtMousePosition(Player &player, float heightZ, float intersectionRadius, Texture &texture, const Vertex3Ds * lineEndPosition, D3DCOLOR lineColor, DebugPrintRecord &dpr);
   bool ShouldDrawTrainerBallStarts(std::size_t index, int currentTimeMs);
   bool ShouldDrawTrainerBallPasses(std::size_t index, int currentTimeMs);
   bool ShouldDrawTrainerBallFails(std::size_t index, int currentTimeMs);
   bool ShouldDrawActiveBallKickers(int currentTimeMs);
   void DrawTrainerBalls(Player &player, DebugPrintRecord &dpr, int currentTimeMs);
   void DrawActiveBallKickers(Player &player, float radius, Matrix3 &orientation, DebugPrintRecord &dpr);
   void DrawAngleVelocityPreview(Player &player, TrainerOptions::BallStartOptionsRecord &bsor);
   void DrawAngleVelocityPreviewHelper(std::vector<Vertex3DColor> &testVertices, TrainerOptions::BallStartOptionsRecord &bsor, float angleStep, float velocityStep, float radius);
   void DrawAngleVelocityPreviewHelperAdd(std::vector<Vertex3DColor> &testVertices, TrainerOptions::BallStartOptionsRecord &bsor, float angle, float velocity, float radius);
   void CalculateAngleVelocityStep(TrainerOptions::BallStartOptionsRecord &bsor, float &angleStep, float &velocityStep);
   void UpdateBallState(BallHistoryRecord &ballHistoryRecord);
   void ShowStatus(Player &player, int currentTimeMs);
   void ShowAutoControlVertices(Player &player, DebugPrintRecord &dpr);
   void ShowPreviousRunRecord(DebugPrintRecord &dpr);
   void ShowCurrentRunRecord(DebugPrintRecord &dpr, int currentTimeMs);
   void ShowBallStartOptionsRecord(DebugPrintRecord &dpr, TrainerOptions::BallStartOptionsRecord &bsor);
   void ShowBallEndOptionsRecord(DebugPrintRecord &dpr, TrainerOptions::BallEndOptionsRecord &beor);
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

   void Add(std::vector<Ball *> &controlVBalls, int currentTimeMsec);
   BallHistoryRecord &Get(std::size_t index);
   std::size_t GetTailIndex();

   float DistancePixels(POINT &p1, POINT &p2);
   float DistancePixels(const Vertex3Ds &pos1, const Vertex3Ds &pos2);
   float VelocityPixels(Vertex3Ds &vel);
   POINT Get2DPointFrom3D(Player &player, const Vertex3Ds& vertex);
   Vertex3Ds GetKickerPosition(Kicker &kicker);
   bool ControlNextMove();
   bool ControlPrevMove();

   bool BallCountIncreased();
   bool BallCountDecreased();
   bool BallChanged();

   bool BallInsideAutoControlVertex(std::vector<Ball *> &controlVBalls);

   std::vector<std::string> Split(const char * str, char delimeter);
   void CenterMouse(Player &player);

   void InvalidEnumValue(const char * enumName, const int enumValue);
   void InvalidEnumValue(const char * enumName, const char * enumValue);
};

struct TimerOnOff
{
   HitTimer *m_timer;
   bool m_enabled;
};

class Player : public CWnd
{
public:
   Player(const bool cameraMode, PinTable * const ptable);
   virtual ~Player();

   void CreateWnd(HWND parent = 0);
   virtual void PreRegisterClass(WNDCLASS& wc) override;
   virtual void PreCreate(CREATESTRUCT& cs) override;
   virtual void OnInitialUpdate() override;
   virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
   void RenderStaticMirror(const bool onlyBalls);
   void RenderDynamicMirror(const bool onlyBalls);
   void RenderMirrorOverlay();
   void InitBallShader();
   void InitKeys();

   void InitStatic();

   void UpdatePhysics();

   void DrawBalls();

   void SetClipPlanePlayfield(const bool clip_orientation);

   void DrawBulbLightBuffer();
   void Bloom();
   void SSRefl();
   void StereoFXAA(const bool stereo, const bool SMAA, const bool DLAA, const bool NFAA, const bool FXAA1, const bool FXAA2, const bool FXAA3, const unsigned int sharpen, const bool depth_available);

   void UpdateHUD_IMGUI();
   void RenderHUD_IMGUI();
   void UpdateHUD();

   void PrepareVideoBuffersNormal();
   void PrepareVideoBuffersAO();
   void FlipVideoBuffers(const bool vsync);

   void PhysicsSimulateCycle(float dtime);

public:
   void LockForegroundWindow(const bool enable);
   void Render();
   void RenderDynamics();

   Ball *CreateBall(const float x, const float y, const float z, const float vx, const float vy, const float vz, const float radius = 25.0f, const float mass = 1.0f);
   void DestroyBall(Ball *pball);
   void DrawFakeBall(const Vertex3Ds &m_pos, float radius, Matrix3 m_orientation, Texture *ballColor);

   void AddCabinetBoundingHitShapes();

   void InitDebugHitStructure();
   void DoDebugObjectMenu(const int x, const int y);

   void PauseMusic();
   void UnpauseMusic();

   void RecomputePauseState();
   void RecomputePseudoPauseState();

   void NudgeUpdate();
   void FilterNudge();
   void NudgeX(const int x, const int j);
   void NudgeY(const int y, const int j);
#ifdef UNUSED_TILT
   int  NudgeGetTilt(); // returns non-zero when appropriate to set the tilt switch
#endif

   void MechPlungerUpdate();
   void MechPlungerIn(const int z);

   void SetGravity(float slopeDeg, float strength);

#ifdef PLAYBACK
   float ParseLog(LARGE_INTEGER *pli1, LARGE_INTEGER *pli2);
#endif

#ifndef ENABLE_SDL
   void DMDdraw(const float DMDposx, const float DMDposy, const float DMDwidth, const float DMDheight, const COLORREF DMDcolor, const float intensity);
#endif
   void Spritedraw(const float posx, const float posy, const float width, const float height, const COLORREF color, Texture* const tex, const float intensity, const bool backdrop=false);
   void Spritedraw(const float posx, const float posy, const float width, const float height, const COLORREF color, Sampler* const tex, const float intensity, const bool backdrop=false);

#ifdef ENABLE_SDL
   SDL_Window  *m_sdl_playfieldHwnd;
   SDL_Window  *m_sdl_backdropHwnd;
#endif
   Shader      *m_ballShader;

   IndexBuffer *m_ballIndexBuffer;
   VertexBuffer *m_ballVertexBuffer;
   VertexBuffer *m_ballTrailVertexBuffer;
   bool m_antiStretchBall;

   bool m_cameraMode;
   int m_backdropSettingActive;
   PinTable *m_ptable;

   Pin3D m_pin3d;

   U32 m_time_msec;

   Ball *m_pactiveball;      // ball the script user can get with ActiveBall
   Ball *m_pactiveballDebug; // ball the debugger will use as Activeball when firing events
   Ball *m_pactiveballBC;    // ball that the ball control UI will use
   Vertex3Ds *m_pBCTarget;   // If non-null, the target location for the ball to roll towards

   BallHistory m_BallHistory;

   std::vector<Ball*> m_vball;
   std::vector<HitFlipper*> m_vFlippers;

   vector<AnimObject*> m_vanimate; // animated objects that need frame updates

   vector<HitTimer*> m_vht;
   vector<TimerOnOff> m_changed_vht; // stores all en/disable changes to the m_vht timer list, to avoid problems with timers dis/enabling themselves

   Vertex3Ds m_gravity;

   PinInput m_pininput;

   Vertex2D m_Nudge;

   NudgeFilterX m_NudgeFilterX;
   NudgeFilterY m_NudgeFilterY;

   // new nudging
   Vertex3Ds m_tableVel;
   Vertex3Ds m_tableDisplacement;
   Vertex3Ds m_tableVelOld;
   Vertex3Ds m_tableVelDelta;
   float m_nudgeSpring;
   float m_nudgeDamping;

   // legacy/VP9 style keyboard nudging
   bool m_legacyNudge;
   float m_legacyNudgeStrength;
   Vertex2D m_legacyNudgeBack;
   int m_legacyNudgeTime;

   EnumAssignKeys m_rgKeys[eCKeys]; //Player's key assignments

   HWND m_hwndDebugOutput;

   vector<CLSID*> m_controlclsidsafe; // ActiveX control types which have already been okayed as being safe

   int m_sleeptime;          // time to sleep during each frame - can help side threads like vpinmame, but most likely outdated
   int m_minphyslooptime;    // minimum physics loop processing time in usec (0-1000), effort to reduce input latency (mainly useful if vsync is enabled, too)

   float m_globalEmissionScale;

   int m_VSync; // targeted refresh rate in Hz, if larger refresh rate it will limit FPS by uSleep() //!! currently does not work adaptively as it would require IDirect3DDevice9Ex which is not supported on WinXP
   int m_maxPrerenderedFrames;
   int m_FXAA;    // =FXAASettings
   int m_sharpen; // 0=off, 1=CAS, 2=bilateral CAS
   bool m_AA;

   bool m_dynamicAO;
   bool m_disableAO;

   bool m_ss_refl;
   bool m_pf_refl; // render playfield reflections for dynamic elements?

   bool m_useNvidiaApi;
   bool m_disableDWM;

   bool m_stereo3Denabled;
   bool m_stereo3DY;
   StereoMode m_stereo3D;
   float m_global3DContrast;
   float m_global3DDesaturation;

   int m_BWrendering; // 0=off, 1=Black&White from RedGreen, 2=B&W from Red only

   bool m_bloomOff;
   bool m_ditherOff;

   bool m_PlayMusic;
   bool m_PlaySound;
   int m_MusicVolume;
   int m_SoundVolume;

   AudioPlayer *m_audio;

   int m_lastcursorx, m_lastcursory; // used for the dumb task of seeing if the mouse has really moved when we get a WM_MOUSEMOVE message

   int m_LastKnownGoodCounter;
   int m_ModalRefCount;

   int m_closeType;                  // if 0 exit player and close application if started minimized, if 1 close application always, 2 is brute force exit
   bool m_closeDown;                 // Whether to shut down the player at the end of this frame
   bool m_closeDownDelay;
   bool m_showDebugger;

   bool m_showWindowedCaption;

   bool m_reflectionForBalls;
   bool m_trailForBalls;
   bool m_disableLightingForBalls;

   bool m_throwBalls;
   bool m_ballControl;
   int  m_debugBallSize;
   float m_debugBallMass;

   bool m_detectScriptHang;
   bool m_noTimeCorrect;                // Used so the frame after debugging does not do normal time correction

   bool m_debugMode;

   bool m_debugBalls;                   // Draw balls in the foreground.
   bool m_toggleDebugBalls;

   bool m_swap_ball_collision_handling; // Swaps the order of ball-ball collision handling around each physics cycle (in regard to the RLC comment block in quadtree.cpp (hopefully ;)))

#ifdef DEBUGPHYSICS
   U32 c_hitcnts;
   U32 c_collisioncnt;
   U32 c_contactcnt;
#ifdef C_DYNAMIC
   U32 c_staticcnt;
#endif
   U32 c_embedcnts;
   U32 c_timesearch;

   U32 c_kDObjects;
   U32 c_kDNextlevels;
   U32 c_quadObjects;
   U32 c_quadNextlevels;

   U32 c_traversed;
   U32 c_tested;
   U32 c_deepTested;
#endif

#ifdef DEBUG_BALL_SPIN
   VertexBuffer *m_ballDebugPoints;
#endif
   U32 m_movedPlunger;			// has plunger moved, must have moved at least three times
   U32 m_LastPlungerHit;		// The last time the plunger was in contact (at least the vicinity) of the ball.
   float m_curMechPlungerPos;

   int m_width, m_height;

   int m_screenwidth, m_screenheight, m_refreshrate;
   bool m_fullScreen;

   bool m_touchregion_pressed[8]; // status for each touch region to avoid multitouch double triggers (true = finger on, false = finger off)

   bool m_drawCursor;
   bool m_gameWindowActive;
   bool m_userDebugPaused;
   bool m_debugWindowActive;
   bool m_cabinetMode;
   bool m_meshAsPlayfield;
   bool m_recordContacts;             // flag for DoHitTest()
   vector< CollisionEvent > m_contacts;
   char m_ballShaderTechnique[MAX_PATH];

   int2 m_dmd;
   BaseTexture* m_texdmd;

   unsigned int m_current_renderstage; // currently only used for bulb lights
   unsigned int m_dmdstate; // used to distinguish different flasher/DMD rendering mode states

   int m_overall_frames; // amount of rendered frames since start

#ifdef LOG
   FILE *m_flog;
#else
private:
#endif
   vector<MoverObject*> m_vmover; // moving objects for physics simulation
#ifdef LOG
private:
#endif
   vector<HitObject*> m_vho;

   vector<Ball*> m_vballDelete;   // Balls to free at the end of the frame

   /*HitKD*/HitQuadtree m_hitoctree;

   vector<HitObject*> m_vdebugho;
   HitQuadtree m_debugoctree;

   vector<HitObject*> m_vho_dynamic;
#ifdef USE_EMBREE
   HitQuadtree m_hitoctree_dynamic; // should be generated from scratch each time something changes
#else
   HitKD m_hitoctree_dynamic; // should be generated from scratch each time something changes
#endif

   HitPlane m_hitPlayfield; // HitPlanes cannot be part of octree (infinite size)
   HitPlane m_hitTopGlass;

   U64 m_StartTime_usec;
   U64 m_curPhysicsFrameTime;	// Time when the last frame was drawn
   U64 m_nextPhysicsFrameTime;	// time at which the next physics update should be
   U64 m_lastFlipTime;

   // all Hitables obtained from the table's list of Editables
   vector< Hitable* > m_vhitables;
   vector< Hitable* > m_vHitNonTrans; // non-transparent hitables
   vector< Hitable* > m_vHitTrans;    // transparent hitables

   int2 m_curAccel[PININ_JOYMXCNT];

#ifdef PLAYBACK
   bool m_playback;
   FILE *m_fplaylog;
#endif

   Vertex2D m_BallStretch;

   float m_NudgeShake;         // whether to shake the screen during nudges and how much
   Vertex2D m_ScreenOffset;    // for screen shake effect during nudge

   int m_curPlunger;

   //HANDLE m_hSongCompletionEvent;

   int m_pauseRefCount;

   bool m_pseudoPause;      // Nothing is moving, but we're still redrawing

   bool m_supportsTouch;    // Display is a touchscreen?
   bool m_showTouchMessage;

   U32 m_phys_iterations;

   // all kinds of stats tracking, incl. FPS measurement
   U32 m_lastfpstime;
   U32 m_cframes;
   float m_fps;
   float m_fpsAvg;
   U32 m_fpsCount;
   U64 m_lastTime_usec;
   U32 m_lastFrameDuration;
   U32 m_max;
   U32 m_max_total;
   U64 m_count;
   U64 m_total;
   int m_lastMaxChangeTime;
   U64 m_phys_total;
   U64 m_phys_total_iterations;
   U32 m_phys_max_iterations;
   U32 m_phys_period;
   U32 m_phys_max;
   U32 m_phys_max_total;

   U32 m_script_period;
   U64 m_script_total;
   U32 m_script_max;
   U32 m_script_max_total;

   FrameQueueLimiter m_limiter;

   // only called from ctor
   HRESULT Init();
   // only called from dtor
   void Shutdown();


   void SetScreenOffset(const float x, const float y);     // set render offset in screen coordinates, e.g., for the nudge shake

   bool RenderStaticOnly() const;
   bool RenderAOOnly() const;

   void InitShader();
   void CalcBallAspectRatio();
   void GetBallAspectRatio(const Vertex3Ds &pos, float radius, Vertex2D &stretch, const float zHeight);
   //void DrawBallReflection(Ball *pball, const float zheight, const bool lowDetailBall);
   unsigned int ProfilingMode() const;

public:
   void CreateDebugFont();
   void SetDebugOutputPosition(const float x, const float y);
   void DebugPrint(int x, int y, LPCSTR text, bool center = false);

   void StopPlayer();
   void ToggleFPS();
   void InitFPS();
   bool ShowFPSonly() const;
   bool ShowStats() const;

   void UpdateBasicShaderMatrix(const Matrix3D& objectTrafo = Matrix3D(1.0f));
   void UpdateCameraModeDisplay();
   void UpdateBackdropSettings(const bool up);
   void UpdateBallShaderMatrix();

#ifdef STEPPING
   U32 m_pauseTimeTarget;
   volatile bool m_pause;
   bool m_step;
#endif

   unsigned int m_showFPS;

   bool m_scaleFX_DMD;

   bool m_toogle_DTFS;

   //bool m_low_quality_bloom;

   bool m_isRenderingStatic;

   bool m_overwriteBallImages;
   Texture *m_ballImage;
   Texture *m_decalImage;
   DebuggerDialog m_debuggerDialog;

private:
#ifdef ENABLE_SDL
   TTF_Font *m_pFont;
#else
   ID3DXFont *m_pFont;
   LPD3DXSPRITE m_fontSprite;
   RECT     m_fontRect;
#endif
};
