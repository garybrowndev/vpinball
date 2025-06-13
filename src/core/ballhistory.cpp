#pragma once

#include "core/stdafx.h"

#include <algorithm>
#include <numeric>
#include <random>
#include <array>

#include "freeimage.h"

#include "player.h"
#include "renderer/Shader.h"
#include "meshes/ballMesh.h"
#include "fonts/DroidSans.h"
#include "fonts/DroidSansBold.h"

// ================================================================================================================================================================================================================================================

BallHistoryState::BallHistoryState()
   : m_Position(0.0f, 0.0f, 0.0f)
   , m_Velocity(0.0f, 0.0f, 0.0f)
   , m_AngularMomentum(0.0f, 0.0f, 0.0f)
   , m_LastEventPos(0.0f, 0.0f, 0.0f)
   , m_Orientation(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f)
   , m_RingCounter_OldPos(0)
   , m_DrawRadius(0.0f)
   , m_Color(0x00000000)
{
   ZeroMemory(&m_OldPos, sizeof(m_OldPos));
}

// ================================================================================================================================================================================================================================================

BallHistoryRecord::BallHistoryRecord()
   : BallHistoryRecord(0)
{
}

BallHistoryRecord::BallHistoryRecord(int timeMs)
   : m_TimeMs(timeMs)
{
}

void BallHistoryRecord::Set(const HitBall* controlVBall, BallHistoryState& bhr)
{
   bhr.m_Position = controlVBall->m_d.m_pos;
   bhr.m_Velocity = controlVBall->m_d.m_vel;
   bhr.m_AngularMomentum = controlVBall->m_angularmomentum;
   bhr.m_LastEventPos = controlVBall->m_lastEventPos;
   bhr.m_Orientation = controlVBall->m_orientation;
   memcpy(bhr.m_OldPos, controlVBall->m_oldpos, sizeof(bhr.m_OldPos));
   bhr.m_RingCounter_OldPos = controlVBall->m_ringcounter_oldpos;
}

void BallHistoryRecord::Set(std::vector<HitBall*>& controlVBalls, int timeMs)
{
   m_TimeMs = timeMs;
   m_BallHistoryStates.clear();
   for (std::vector<HitBall*>::iterator it = controlVBalls.begin(); it != controlVBalls.end(); ++it)
   {
      m_BallHistoryStates.push_back(BallHistoryState());
      Set(*it, m_BallHistoryStates.back());
   }
}

void BallHistoryRecord::Insert(const HitBall* controlVBall, std::size_t insertIndex)
{
   m_BallHistoryStates.insert(m_BallHistoryStates.begin() + insertIndex, BallHistoryState());
   Set(controlVBall, m_BallHistoryStates[insertIndex]);
}

// ================================================================================================================================================================================================================================================

const char* TrainerOptions::BallStartOptionsRecord::ImGuiBallStartLabels[] = {
   "DrawTrainerBallStart00",
   "DrawTrainerBallStart01",
   "DrawTrainerBallStart02",
   "DrawTrainerBallStart03",
   "DrawTrainerBallStart04",
   "DrawTrainerBallStart05",
   "DrawTrainerBallStart06",
   "DrawTrainerBallStart07",
   "DrawTrainerBallStart08",
   "DrawTrainerBallStart09",
   "DrawTrainerBallStart10",
   "DrawTrainerBallStart11",
   "DrawTrainerBallStart12",
   "DrawTrainerBallStart13",
   "DrawTrainerBallStart14",
   "DrawTrainerBallStart15",
   "DrawTrainerBallStart16",
   "DrawTrainerBallStart17",
   "DrawTrainerBallStart18",
   "DrawTrainerBallStart19",
};

TrainerOptions::BallStartOptionsRecord::BallStartOptionsRecord()
   : BallStartOptionsRecord(Vertex3Ds(0.0f, 0.0f, 0.0f), Vertex3Ds(0.0f, 0.0f, 0.0f), Vertex3Ds(0.0f, 0.0f, 0.0f), AngleMinimum, AngleMinimum, 0, VelocityMinimum, VelocityMinimum, 0)
{
}

TrainerOptions::BallStartOptionsRecord::BallStartOptionsRecord(const Vertex3Ds& startPosition, const Vertex3Ds& startVelocity, const Vertex3Ds& startAngularMomentum, float angleRangeStart,
   float angleRangeFinish, S32 totalRangeAngles, float velocityRangeStart, float velocityRangeFinish, S32 totalRangeVelocities)
   : m_StartPosition(startPosition)
   , m_StartVelocity(startVelocity)
   , m_StartAngularMomentum(startAngularMomentum)
   , m_AngleRangeStart(angleRangeStart)
   , m_AngleRangeFinish(angleRangeFinish)
   , m_TotalRangeAngles(totalRangeAngles)
   , m_VelocityRangeStart(velocityRangeStart)
   , m_VelocityRangeFinish(velocityRangeFinish)
   , m_TotalRangeVelocities(totalRangeVelocities)
{
}

bool TrainerOptions::BallStartOptionsRecord::IsZero()
{
   return m_StartPosition.IsZero() && m_StartVelocity.IsZero() && m_StartAngularMomentum.IsZero() && m_AngleRangeStart == 0.0f && m_AngleRangeFinish == 0.0f && m_TotalRangeAngles == 0
      && m_VelocityRangeStart == 0.0f && m_VelocityRangeFinish == 0.0f && m_TotalRangeVelocities == 0;
}

// ================================================================================================================================================================================================================================================


const char* TrainerOptions::BallEndOptionsRecord::ImGuiBallPassLabels[] = {
   "DrawTrainerBallPass0",
   "DrawTrainerBallPass1",
   "DrawTrainerBallPass2",
   "DrawTrainerBallPass3",
   "DrawTrainerBallPass4",
   "DrawTrainerBallPass5",
   "DrawTrainerBallPass6",
   "DrawTrainerBallPass7",
   "DrawTrainerBallPass8",
   "DrawTrainerBallPass9",
};

const char* TrainerOptions::BallEndOptionsRecord::ImGuiBallFailLabels[] = {
   "DrawTrainerBallFail0",
   "DrawTrainerBallFail1",
   "DrawTrainerBallFail2",
   "DrawTrainerBallFail3",
   "DrawTrainerBallFail4",
   "DrawTrainerBallFail5",
   "DrawTrainerBallFail6",
   "DrawTrainerBallFail7",
   "DrawTrainerBallFail8",
   "DrawTrainerBallFail9",
};

const float TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled = -1.0f; // Stop mode is enabled

TrainerOptions::BallEndOptionsRecord::BallEndOptionsRecord()
   : BallEndOptionsRecord(Vertex3Ds(0.0f, 0.0f, 0.0f), 0.0f)
{
}

TrainerOptions::BallEndOptionsRecord::BallEndOptionsRecord(const Vertex3Ds& endPosition, float endRadiusPercent)
   : m_EndPosition(endPosition)
   , m_EndRadiusPercent(endRadiusPercent)
{
}

// ================================================================================================================================================================================================================================================

const char* TrainerOptions::BallCorridorOptionsRecord::ImGuiDrawTrainerBallCorridorPassLabel = "DrawTrainerBallCorridorPass";
const char* TrainerOptions::BallCorridorOptionsRecord::ImGuiDrawTrainerBallCorridorOpeningLeftLabel = "DrawTrainerBallCorridorOpeningLeft";
const char* TrainerOptions::BallCorridorOptionsRecord::ImGuiDrawTrainerBallCorridorOpeningRightLabel = "DrawTrainerBallCorridorOpeningRight";

TrainerOptions::BallCorridorOptionsRecord::BallCorridorOptionsRecord()
   : BallCorridorOptionsRecord(Vertex3Ds(0.0f, 0.0f, 0.0f), RadiusPercentMaximum, Vertex3Ds(0.0f, 0.0f, 0.0f), Vertex3Ds(0.0f, 0.0f, 0.0f))
{
}

TrainerOptions::BallCorridorOptionsRecord::BallCorridorOptionsRecord(const Vertex3Ds& passPosition, float passRadiusPercent, const Vertex3Ds& openingLeft, const Vertex3Ds& openingRight)
   : m_PassPosition(passPosition)
   , m_PassRadiusPercent(passRadiusPercent)
   , m_OpeningPositionLeft(openingLeft)
   , m_OpeningPositionRight(openingRight)
{
}

// ================================================================================================================================================================================================================================================

TrainerOptions::RunRecord::RunRecord()
   : m_Result(ResultType_Unknown)
   , m_TotalTimeMs(0)
   , m_StartToPassCorridorIndex(0)
   , m_StartToFailCorridorIndex(0)
{
}

// ================================================================================================================================================================================================================================================

const int TrainerOptions::CountdownSoundSeconds = 3;
const float TrainerOptions::TimeLowSoundSeconds = 4.0f;

TrainerOptions::TrainerOptions()
   : m_ModeState(ModeStateType::ModeStateType_Start)
   , m_ConfigModeState(ConfigModeStateType::ConfigModeStateType_Wizard)
   , m_BallStartMode(BallStartModeType::BallStartModeType_Accept)
   , m_BallStartAngleVelocityMode(BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Drop)
   , m_BallStartCompleteMode(BallStartCompleteModeType::BallStartCompleteModeType_Accept)
   , m_BallEndLocationMode(BallEndLocationModeType::BallEndLocationModeType_Config)
   , m_BallPassFinishMode(BallEndFinishModeType::BallEndFinishModeType_Distance)
   , m_BallFailFinishMode(BallEndFinishModeType::BallEndFinishModeType_Distance)
   , m_BallEndAssociationMode(BallEndAssociationModeType::BallEndAssociationModeType_Accept)
   , m_BallEndCompleteMode(BallEndCompleteModeType::BallEndCompleteModeType_Accept)
   , m_BallCorridorCompleteMode(BallCorridorCompleteModeType::BallCorridorCompleteModeType_Accept)
   , m_DifficultyConfigModeState(DifficultyConfigModeState::DifficultyConfigModeState_Accept)
   , m_RunOrderMode(RunOrderModeType::RunOrderModeType_InOrder)
   , m_BallKickerBehaviorMode(BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset)
   , m_SoundEffectsMode(SoundEffectsModeType::SoundEffectsModeType_Accept)
   , m_SoundEffectsPassEnabled(true)
   , m_SoundEffectsFailEnabled(true)
   , m_SoundEffectsTimeLowEnabled(true)
   , m_SoundEffectsCountdownEnabled(true)
   , m_SoundEffectsMenuPlayed(false)
   , m_GameplayDifficulty(0)
   , m_VarianceDifficulty(0)
   , m_GravityVariance(0)
   , m_GravityVarianceMode(DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow)
   , m_PlayfieldFrictionVariance(0)
   , m_PlayfieldFrictionVarianceMode(DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow)
   , m_FlipperStrengthVariance(0)
   , m_FlipperStrengthVarianceMode(DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow)
   , m_FlipperFrictionVariance(0)
   , m_FlipperFrictionVarianceMode(DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow)
   , m_TotalRuns(5)
   , m_MaxSecondsPerRun(15)
   , m_CountdownSecondsBeforeRun(3)
   , m_BallStartOptionsRecordsSize(0)
   , m_CurrentRunRecord(0)
   , m_RunStartTimeMs(0)
   , m_CountdownSoundPlayed(0)
   , m_TimeLowSoundPlaying(false)
   , m_SetupBallStarts(true)
   , m_SetupDifficulty(true)
   , m_GameplayDifficultyInitial(0)
   , m_GravityInitial(0.0f)
   , m_PlayfieldFrictionInitial(0.0f)
   , m_FlipperStrengthInitial(0.0f)
   , m_FlipperFrictionInitial(0.0f)
{
}

// ================================================================================================================================================================================================================================================

const char* NormalOptions::ImGuiDrawAutoControlVertexLabels[] = {
   "DrawAutoControlVertex0",
   "DrawAutoControlVertex1",
   "DrawAutoControlVertex2",
   "DrawAutoControlVertex3",
   "DrawAutoControlVertex4",
   "DrawAutoControlVertex5",
   "DrawAutoControlVertex6",
   "DrawAutoControlVertex7",
   "DrawAutoControlVertex8",
   "DrawAutoControlVertex9",
};

const char* NormalOptions::ImGuiDrawRecallVertexLabels[] = {
   "DrawRecallVertex00",
   "DrawRecallVertex01",
   "DrawRecallVertex02",
   "DrawRecallVertex03",
   "DrawRecallVertex04",
   "DrawRecallVertex05",
   "DrawRecallVertex06",
   "DrawRecallVertex07",
   "DrawRecallVertex08",
   "DrawRecallVertex09",
   "DrawRecallVertex10",
   "DrawRecallVertex11",
   "DrawRecallVertex12",
   "DrawRecallVertex13",
   "DrawRecallVertex14",
   "DrawRecallVertex15",
   "DrawRecallVertex16",
   "DrawRecallVertex17",
   "DrawRecallVertex18",
   "DrawRecallVertex19",
};

const float NormalOptions::ManageAutoControlFindFactor = 0.025f;

NormalOptions::NormalOptions()
   : m_ModeState(ModeStateType::ModeStateType_SelectCurrentBallHistory)
   , m_ConfigureRecallBallHistoryMode(ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Select)
   , m_RecallControlIndex(RecallControlIndexDisabled)
   , m_ClearAutoControlLocationsMode(ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_GoBack)
{
}

// ================================================================================================================================================================================================================================================

BallHistory::ProfilerRecord::ProfilerScope::ProfilerScope(U64& profilerUsec)
   : m_ProfilerUsec(profilerUsec)
{
   m_TempUsec = usec();
}

BallHistory::ProfilerRecord::ProfilerScope::~ProfilerScope() { m_ProfilerUsec = usec() - m_TempUsec; }

// ================================================================================================================================================================================================================================================

BallHistory::ProfilerRecord::ProfilerRecord() { SetZero(); }

void BallHistory::ProfilerRecord::SetZero()
{
   m_ProcessUsec = 0;
   m_ShowStatusUsec = 0;
   m_ProcessMenuUsec = 0;
   m_DrawBallHistoryUsec = 0;
   m_ProcessModeNormalUsec = 0;
   m_ProcessModeTrainerUsec = 0;
   m_DrawTrainerBallsUsec = 0;
}

// ================================================================================================================================================================================================================================================

const char* BallHistory::PrintScreenRecord::ImGuiProcessMenuLabel = "ProcessMenu";
const char* BallHistory::PrintScreenRecord::ImGuiActiveMenuLabel = "ActiveMenu";
const char* BallHistory::PrintScreenRecord::ImGuiStatusLabel = "Status";
const char* BallHistory::PrintScreenRecord::ImGuiCurrentRunRecordLabel = "CurrentRunRecord";
const char* BallHistory::PrintScreenRecord::ImGuiErrorLabel = "Error";

ImFont* BallHistory::PrintScreenRecord::Normal12Font = nullptr;
ImFont* BallHistory::PrintScreenRecord::Normal14Font = nullptr;
ImFont* BallHistory::PrintScreenRecord::Bold12Font = nullptr;
ImFont* BallHistory::PrintScreenRecord::Bold14Font = nullptr;
ImFont* BallHistory::PrintScreenRecord::Bold20Font = nullptr;

void BallHistory::PrintScreenRecord::Init()
{
   ImGuiIO& io = ImGui::GetIO();

   if (Normal12Font == nullptr)
   {
      Normal12Font = io.Fonts->AddFontFromMemoryCompressedTTF(droidsans_compressed_data, droidsans_compressed_size, 12.0f);
   }

   if (Normal14Font == nullptr)
   {
      Normal14Font = io.Fonts->AddFontFromMemoryCompressedTTF(droidsans_compressed_data, droidsans_compressed_size, 14.0f);
   }

   if (Bold12Font == nullptr)
   {
      Bold12Font = io.Fonts->AddFontFromMemoryCompressedTTF(droidsansbold_compressed_data, droidsansbold_compressed_size, 12.0f);
   }

   if (Bold14Font == nullptr)
   {
      Bold14Font = io.Fonts->AddFontFromMemoryCompressedTTF(droidsansbold_compressed_data, droidsansbold_compressed_size, 14.0f);
   }

   if (Bold20Font == nullptr)
   {
      Bold20Font = io.Fonts->AddFontFromMemoryCompressedTTF(droidsansbold_compressed_data, droidsansbold_compressed_size, 20.0f);
   }
}

void BallHistory::PrintScreenRecord::UnInit()
{
   Normal12Font = nullptr;
   Normal14Font = nullptr;
   Bold12Font = nullptr;
   Bold14Font = nullptr;
   Bold20Font = nullptr;
}

void BallHistory::PrintScreenRecord::Text(const char* name, float positionX, float positionY, const char* format, ...)
{
   char strBuffer[1024] = { 0 };
   va_list formatArgs;
   va_start(formatArgs, format);
   vsprintf_s(strBuffer, format, formatArgs);

   ShowText(name, Bold14Font, Color::White, positionX, positionY, false, strBuffer);
}

void BallHistory::PrintScreenRecord::MenuTitleText(const char* format, ...)
{
   char strBuffer[1024] = { 0 };
   va_list formatArgs;
   va_start(formatArgs, format);
   vsprintf_s(strBuffer, format, formatArgs);

   std::string tempStr = "*****" + std::string(strBuffer) + "*****";
   ShowText(ImGuiProcessMenuLabel, Bold14Font, Color::White, 0.50f, 0.25f, true, tempStr.c_str());
}

void BallHistory::PrintScreenRecord::MenuText(bool selected, const char* format, ...)
{
   char strBuffer[1024] = { 0 };
   va_list formatArgs;
   va_start(formatArgs, format);
   vsprintf_s(strBuffer, format, formatArgs);

   if (selected)
   {
      std::string tempStr = "-->" + std::string(strBuffer) + "<--";
      ShowText(ImGuiProcessMenuLabel, Bold12Font, Color::White, 0.50f, 0.25f, true, tempStr.c_str());
   }
   else
   {
      ShowText(ImGuiProcessMenuLabel, Normal12Font, Color::White, 0.50f, 0.25f, true, strBuffer);
   }

   ImGui::SetWindowFocus(ImGuiProcessMenuLabel);
}

void BallHistory::PrintScreenRecord::PrintScreenRecord::ActiveMenuText(const char* format, ...)
{
   char strBuffer[1024] = { 0 };
   va_list formatArgs;
   va_start(formatArgs, format);
   vsprintf_s(strBuffer, format, formatArgs);

   ShowText(ImGuiActiveMenuLabel, Normal12Font, Color::White, 0.50f, 0.99f, false, strBuffer);
}

void BallHistory::PrintScreenRecord::ErrorText(const char* format, ...)
{
   char strBuffer[1024] = { 0 };
   va_list formatArgs;
   va_start(formatArgs, format);
   vsprintf_s(strBuffer, format, formatArgs);

   std::string tempStr = "!!!!! ERROR:" + std::string(strBuffer) + "!!!!!";
   ShowText(ImGuiErrorLabel, Bold20Font, Color::Red, 0.50f, 0.50f, true, tempStr.c_str());
}

void BallHistory::PrintScreenRecord::Results(const std::vector<std::pair<std::string, std::string>>& nameValuePairs)
{
   ShowNameValueTable(ImGuiProcessMenuLabel, Normal12Font, Color::White, Bold12Font, Color::White, 0.00f, 0.00f, nameValuePairs, false, true);
}

void BallHistory::PrintScreenRecord::Status(const std::vector<std::pair<std::string, std::string>>& nameValuePairs)
{
   ShowNameValueTable(ImGuiStatusLabel, Normal12Font, Color::White, Bold12Font, Color::White, 0.0f, 0.0f, nameValuePairs, true, false);
}

void BallHistory::PrintScreenRecord::ActiveMenu(const std::vector<std::pair<std::string, std::string>>& nameValuePairs)
{
   ShowNameValueTable(ImGuiActiveMenuLabel, Normal12Font, Color::White, Bold12Font, Color::White, 0.80f, 1.00f, nameValuePairs, false, true);
}

void BallHistory::PrintScreenRecord::ShowText(const char* name, ImFont* font, const ImU32& fontColor, float positionX, float positionY, bool center, const char* str)
{
   if (!BallHistory::DrawMenu)
   {
      return;
   }

   TransformPosition(positionX, positionY);

   ImGui::SetNextWindowBgAlpha(0.25f);

   ImGui::Begin(name, nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

   ImGui::PushFont(font);

   ImGui::PushStyleColor(ImGuiCol_Text, fontColor);

   if (center)
   {
      ImVec2 textSize = ImGui::CalcTextSize(str);
      float windowWidth = ImGui::GetWindowSize().x;
      float textPosX = (windowWidth - textSize.x) / 2.0f;
      ImGui::SetCursorPosX(textPosX);
   }

   ImGui::TextUnformatted(str);

   ImGui::PopStyleColor();

   ImGui::PopFont();

   ImVec2 windowSize = ImGui::GetWindowSize();

   SetWindowPosClamped(name, { positionX, positionY }, windowSize, center);

   ImGui::End();
}

void BallHistory::PrintScreenRecord::ShowNameValueTable(const char* name, ImFont* rowFont, const ImU32& rowFontColor, ImFont* headerFont, const ImU32& headerFontColor, float positionX,
   float positionY, const std::vector<std::pair<std::string, std::string>>& nameValuePairs, bool overflow, bool center)
{
   if (!BallHistory::DrawMenu)
   {
      return;
   }

   TransformPosition(positionX, positionY);

   std::string windowName;
   float maxX = 0;
   bool beginWindowTable = true;
   for (std::size_t nameValuePairsIndex = 0; nameValuePairsIndex < nameValuePairs.size(); nameValuePairsIndex++)
   {
      if (beginWindowTable)
      {
         beginWindowTable = false;

         ImGui::SetNextWindowBgAlpha(0.25f);

         windowName = overflow ? name + std::to_string(nameValuePairsIndex) : name;
         ImGui::Begin(windowName.c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse
               | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

         std::string tableName = windowName + "_Table_" + std::to_string(nameValuePairsIndex);
         ImGui::BeginTable(tableName.c_str(), 2, ImGuiTableFlags_Borders);

         ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
         ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed);
      }

      auto& row = nameValuePairs[nameValuePairsIndex];

      auto cursorScreenPos = ImGui::GetCursorScreenPos();
      ImGuiIO& io = ImGui::GetIO();
      auto rowHeight = std::max(ImGui::CalcTextSize(row.first.c_str()).y, ImGui::CalcTextSize(row.second.c_str()).y);
      if (overflow && (cursorScreenPos.y + rowHeight) > io.DisplaySize.y)
      {
         ImGui::EndTable();

         ImVec2 windowSize = ImGui::GetWindowSize();

         ImGui::End();

         SetWindowPosClamped(windowName.c_str(), { positionX, positionY }, windowSize, center);

         positionX += windowSize.x;
         beginWindowTable = true;
         nameValuePairsIndex--;
      }
      else
      {
         if (row.second.empty())
         {
            ImGui::PushFont(headerFont);
            ImGui::PushStyleColor(ImGuiCol_Text, headerFontColor);
         }
         else
         {
            ImGui::PushFont(rowFont);
            ImGui::PushStyleColor(ImGuiCol_Text, rowFontColor);
         }

         ImGui::TableNextRow();
         ImGui::TableSetColumnIndex(0);
         ImGui::TextUnformatted(row.first.c_str());
         ImGui::TableSetColumnIndex(1);
         ImGui::TextUnformatted(row.second.c_str());

         if (row.second.empty())
         {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0x11, 0x11, 0x11, 0x40));
         }
         else
         {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, (nameValuePairsIndex % 2) == 0 ? IM_COL32(0xAA, 0xAA, 0xAA, 0x40) : IM_COL32(0x99, 0x99, 0x99, 0x40));
         }

         ImGui::PopStyleColor();
         ImGui::PopFont();
      }
   }

   if (!beginWindowTable)
   {
      ImGui::EndTable();

      ImVec2 windowSize = ImGui::GetWindowSize();

      ImGui::End();

      SetWindowPosClamped(windowName.c_str(), { positionX, positionY }, windowSize, center);
   }
}

void BallHistory::PrintScreenRecord::TransformPosition(float& positionX, float& positionY)
{
   ImGuiIO& io = ImGui::GetIO();
   if (positionX >= -1.0f && positionX <= 1.0f)
   {
      positionX *= io.DisplaySize.x;
   }
   if (positionY >= -1.0f && positionY <= 1.0f)
   {
      positionY *= io.DisplaySize.y;
   }
}

void BallHistory::PrintScreenRecord::SetWindowPosClamped(const char* name, const ImVec2& position, const ImVec2& size, bool center)
{
   ImVec2 adjustedPosition = { position.x - (center ? size.x / 2.0f : 0.0f), position.y };

   ImGuiIO& io = ImGui::GetIO();
   ImVec2 clampedPosition = { std::clamp(adjustedPosition.x, 0.0f, io.DisplaySize.x - size.x), std::clamp(adjustedPosition.y, 0.0f, io.DisplaySize.y - size.y) };

   ImGui::SetWindowPos(name, clampedPosition);
}

// ================================================================================================================================================================================================================================================

bool BallHistory::DrawMenu = false;

const int BallHistory::MenuOptionsRecord::SkipKeyPressHoldMs = 2000;
const int BallHistory::MenuOptionsRecord::SkipKeyIntervalMs = 250;
const S32 BallHistory::MenuOptionsRecord::SkipKeyStepFactor = 10;

const int BallHistory::MenuOptionsRecord::SkipControlIntervalMs = 300;
const S32 BallHistory::MenuOptionsRecord::SkipControlStepFactor = 3;

const float BallHistory::MenuOptionsRecord::DefaultBallRadius = 25.0f;

BallHistory::MenuOptionsRecord::MenuOptionsRecord()
   : m_MenuState(MenuStateType::MenuStateType_Root_SelectMode)
   , m_ModeType(ModeType::ModeType_Normal)
   , m_CreateZ(0)
   , m_SkipKeyPressed(false)
   , m_SkipKeyPressedMs(0)
   , m_SkipKeyLeft(false)
   , m_SkipKeyUsedMs(0)
   , m_SkipControlUsedMs(0)
   , m_CurrentBallIndex(0)
   , m_CurrentAssociationIndex(0)
   , m_CurrentCompleteIndex(0)
   , m_MousePosition2D { 0, 0 }
{
}

// ================================================================================================================================================================================================================================================

const char* BallHistory::ImGuiDrawActiveBallKickersLabels[] {
   "DrawActiveBallKicker00",
   "DrawActiveBallKicker01",
   "DrawActiveBallKicker02",
   "DrawActiveBallKicker03",
   "DrawActiveBallKicker04",
   "DrawActiveBallKicker05",
   "DrawActiveBallKicker06",
   "DrawActiveBallKicker07",
   "DrawActiveBallKicker08",
   "DrawActiveBallKicker09",
   "DrawActiveBallKicker10",
   "DrawActiveBallKicker11",
   "DrawActiveBallKicker12",
   "DrawActiveBallKicker13",
   "DrawActiveBallKicker14",
   "DrawActiveBallKicker15",
   "DrawActiveBallKicker16",
   "DrawActiveBallKicker17",
   "DrawActiveBallKicker18",
   "DrawActiveBallKicker19",
};

const char* BallHistory::ImGuiProcessModeTrainerLabel = "ProcessModeTrainer";
const char* BallHistory::ImGuiCurrentRunRecordLabel = "CurrentRunRecord";

const std::size_t BallHistory::OneSecondMs = 1000;

const std::size_t BallHistory::BallHistorySizeDefault = 8 * 1024;
const BallHistory::NextPreviousByType BallHistory::NextPreviousByDefault = BallHistory::NextPreviousByType::eDistancePixels;
const std::size_t BallHistory::BallHistoryControlStepMsDefault = 250;
const float BallHistory::BallHistoryControlStepPixelsDefault = 200.0f;

const float BallHistory::BallHistoryMinPointSize = 5.0f;
const float BallHistory::BallHistoryMaxPointSize = 20.0f;
const float BallHistory::ControlVerticesDistanceMax = 2000.0f;

const float BallHistory::DrawAngleVelocityRadiusExtraMinimum = 10.0f;
const float BallHistory::DrawAngleVelocityRadiusArc = 6.0f;
const float BallHistory::DrawAngleVelocityLengthMultiplier = 2.0f;
const float BallHistory::DrawAngleVelocityHeightOffset = 2.0f;

const int BallHistory::DrawBallBlinkMs = 100;

const char* BallHistory::DescriptionSectionTitle = "Description";
const char* BallHistory::SummarySectionTitle = "Summary";


const char* BallHistory::SettingsFileExtension = "ini";
const char* BallHistory::SettingsFolderName = "BallHistory";
const char BallHistory::SettingsValueDelimeter = ',';
const char* BallHistory::TableInfoSectionName = "TableInfo";
const char* BallHistory::FilePathKeyName = "Path";
const char* BallHistory::TableNameKeyName = "Name";
const char* BallHistory::AuthorKeyName = "Author";
const char* BallHistory::VersionKeyName = "Version";
const char* BallHistory::DateSavedKeyName = "DateSaved";
const char* BallHistory::NormalModeSettingsSectionName = "NormalModeSettings";
const char* BallHistory::NormalModeAutoControlVerticesPosition3DKeyName = "Position3D";
const char* BallHistory::TrainerModeSettingsSectionName = "TrainerModeSettings";
const char* BallHistory::TrainerModeGameplayDifficultyKeyName = "GameplayDifficulty";
const char* BallHistory::TrainerModeVarianceDifficultyKeyName = "VarianceDifficulty";
const char* BallHistory::TrainerModeGravityVarianceKeyName = "GravityVariance";
const char* BallHistory::TrainerModeGravityVarianceModeKeyName = "GravityVarianceMode";
const char* BallHistory::TrainerModePlayfieldFrictionVarianceKeyName = "PlayfieldFrictionVariance";
const char* BallHistory::TrainerModePlayfieldFrictionVarianceModeKeyName = "PlayfieldFrictionVarianceMode";
const char* BallHistory::TrainerModeFlipperStrengthVarianceKeyName = "FlipperStrengthVariance";
const char* BallHistory::TrainerModeFlipperStrengthVarianceModeKeyName = "FlipperStrengthVarianceMode";
const char* BallHistory::TrainerModeFlipperFrictionVarianceKeyName = "FlipperFrictionVariance";
const char* BallHistory::TrainerModeFlipperFrictionVarianceModeKeyName = "FlipperFrictionVarianceMode";
const char* BallHistory::TrainerModeTotalRunsKeyName = "TotalRuns";
const char* BallHistory::TrainerModeRunOrderModeKeyName = "RunOrderMode";
const char* BallHistory::TrainerModeBallKickerBehaviorModeKeyName = "BallKickerBehaviorMode";
const char* BallHistory::TrainerModeMaxSecondsPerRunKeyName = "MaxSecondsPerRun";
const char* BallHistory::TrainerModeCountdownSecondsBeforeRunKeyName = "CountdownSecondsBeforeRun";
const char* BallHistory::TrainerModeSoundEffectsPassEnabledKeyName = "SoundEffectsPassEnabled";
const char* BallHistory::TrainerModeSoundEffectsFailEnabledKeyName = "SoundEffectsFailEnabled";
const char* BallHistory::TrainerModeSoundEffectsTimeLowEnabledKeyName = "SoundEffectsTimeLowEnabled";
const char* BallHistory::TrainerModeSoundEffectsCountdownEnabledKeyName = "SoundEffectsCountdownEnabled";
const char* BallHistory::TrainerModeBallStartPositionKeyName = "StartPosition";
const char* BallHistory::TrainerModeBallStartVelocityKeyName = "StartVelocity";
const char* BallHistory::TrainerModeBallStartAngularMomentumKeyName = "StartAngularMomentum";
const char* BallHistory::TrainerModeBallStartAngleStartKeyName = "StartAngleStart";
const char* BallHistory::TrainerModeBallStartAngleFinishKeyName = "StartAngleFinish";
const char* BallHistory::TrainerModeBallStartTotalAnglesKeyName = "StartTotalAngles";
const char* BallHistory::TrainerModeBallStartVelocityStartKeyName = "StartVelocityStart";
const char* BallHistory::TrainerModeBallStartVelocityFinishKeyName = "StartVelocityFinish";
const char* BallHistory::TrainerModeBallStartTotalVelocitiesKeyName = "StartVelocities";
const char* BallHistory::TrainerModeBallPassPosition3DKeyName = "PassPosition3D";
const char* BallHistory::TrainerModeBallPassRadiusPercentKeyName = "PassRadiusPercent";
const char* BallHistory::TrainerModeBallPassAssociationsKeyName = "PassAssociations";
const char* BallHistory::TrainerModeBallFailPosition3DKeyName = "FailPosition3D";
const char* BallHistory::TrainerModeBallFailRadiusPercentKeyName = "FailRadiusPercent";
const char* BallHistory::TrainerModeBallFailAssociationsKeyName = "FailAssociations";
const char* BallHistory::TrainerModeBallCorridorPassPosition3DKeyName = "BallCorridorPassPosition3D";
const char* BallHistory::TrainerModeBallCorridorPassRadiusPercentKeyName = "BallCorridorPassRadiusPercent";
const char* BallHistory::TrainerModeBallCorridorOpeningPositionLeft3DKeyName = "BallCorridorOpeningPositionLeft3D";
const char* BallHistory::TrainerModeBallCorridorOpeningPositionRight3DKeyName = "BallCorridorOpeningPositionRight3D";

BallHistory::BallHistory(PinTable& pinTable)
   : m_PreviousProcessKeysAction(EnumAssignKeys::eCKeys)
   , m_PreviousProcessKeyIsPressed(false)
   , m_ShowStatus(false)
   , m_Control(false)
   , m_WasControlled(false)
   , m_WasRecalled(false)
   , m_CurrentControlIndex(0)
   , m_NextPreviousBy(NextPreviousByType::eDistancePixels)
   , m_BallHistoryControlStepMs(0)
   , m_BallHistoryControlStepPixels(0.0f)
   , m_BallHistoryRecordsHeadIndex(0)
   , m_BallHistoryRecordsSize(0)
   , m_MaxBallVelocityPixels(0.0f)
   , m_AutoControlBallColor(Color::Black)
   , m_RecallBallColor(Color::Blue)
   , m_TrainerBallStartColor(Color::Blue)
   , m_TrainerBallPassColor(Color::Green)
   , m_TrainerBallFailColor(Color::Red)
   , m_TrainerBallCorridorPassColor(Color::Green)
   , m_TrainerBallCorridorOpeningWallColor(Color::Red)
   , m_TrainerBallCorridorOpeningEndColor(Color::Black)
   , m_ActiveBallKickerColor(Color::Yellow)
   , m_UseTrailsForBallsInitialValue(0)
{
   m_MenuOptions.m_TrainerOptions.m_GameplayDifficultyInitial = S32(pinTable.GetGlobalDifficulty());
   pinTable.get_Gravity(&m_MenuOptions.m_TrainerOptions.m_GravityInitial);
   pinTable.get_Friction((&m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionInitial));
   InitFlippers(pinTable);
   m_MenuOptions.m_TrainerOptions.m_FlipperStrengthInitial = GetFlipperStrength();
   m_MenuOptions.m_TrainerOptions.m_FlipperFrictionInitial = GetFlipperFriction();
}

void BallHistory::Init(Player& player, int currentTimeMs, bool loadSettings)
{
   SetControl(false);
   m_WasControlled = false;
   m_WasRecalled = false;

   m_CurrentControlIndex = 0;

   m_MenuOptions.m_NormalOptions.m_RecallControlIndex = NormalOptions::RecallControlIndexDisabled;
   m_MenuOptions.m_CreateZ = S32(std::max(0.0f, std::min(float(m_MenuOptions.m_CreateZ), player.m_ptable->m_glassTopHeight)));

   m_NextPreviousBy = NextPreviousByDefault;
   m_BallHistoryControlStepMs = BallHistoryControlStepMsDefault;
   m_BallHistoryControlStepPixels = BallHistoryControlStepPixelsDefault;

   m_BallHistoryRecords.resize(BallHistorySizeDefault);
   m_BallHistoryRecordsSize = 0;
   m_BallHistoryRecordsHeadIndex = 0;
   m_MaxBallVelocityPixels = 0.0f;

   if (m_SettingsFilePath.empty() == true)
   {
      std::string settingsFolderPath;
      if (GetSettingsFolderPath(settingsFolderPath) == true)
      {
         BOOL createDir = CreateDirectory(settingsFolderPath.c_str(), NULL);
         if (createDir == TRUE || GetLastError() == ERROR_ALREADY_EXISTS)
         {
            std::string settingsFileName;
            if (GetSettingsFileName(player, settingsFileName) == true)
            {
               m_SettingsFilePath = settingsFolderPath + "\\" + settingsFileName;
            }
         }
      }
   }

   m_UseTrailsForBallsInitialValue = player.m_renderer->m_trailForBalls;

   PrintScreenRecord::Init();

   if (loadSettings)
   {
      LoadSettings(player);
   }
}

void BallHistory::UnInit(Player& player)
{
   SaveSettings(player);
   PrintScreenRecord::UnInit();
   m_ActiveBallKickers.clear();
   m_Flippers.clear();

   ClearDraws(player);

   for (auto drawnBall : m_DrawnBalls)
   {
      drawnBall.second->RenderRelease();
      drawnBall.second->Release();
   }

   for (auto drawnIntersectionCircle : m_DrawnIntersectionCircles)
   {
      drawnIntersectionCircle.second->RenderRelease();
      drawnIntersectionCircle.second->Release();
   }

   for (auto drawnLine : m_DrawnLines)
   {
      drawnLine.second->RenderRelease();
      drawnLine.second->Release();
   }
}

void BallHistory::Process(Player& player, int currentTimeMs)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_ProcessUsec);

   InitControlVBalls(player);

   InitActiveBallKickers(*player.m_ptable);

   InitFlippers(*player.m_ptable);

   ShowStatus(player, currentTimeMs);

   BallHistoryRecord currentBhr;
   currentBhr.Set(m_ControlVBalls, currentTimeMs);

   switch (m_MenuOptions.m_ModeType)
   {
   case MenuOptionsRecord::ModeType::ModeType_Normal:
   case MenuOptionsRecord::ModeType::ModeType_Trainer:

      if (BallCountIncreased())
      {
         InitBallsIncreased(player);
      }
      else if (BallCountDecreased())
      {
         InitBallsDecreased(player);
      }
      else if (BallChanged())
      {
         m_WasControlled = false;
         Init(player, currentTimeMs, false);
      }

      ProcessMode(player, currentTimeMs);

      if (m_Control)
      {
         m_WasControlled = true;

         if (m_BallHistoryRecordsSize == 0)
         {
            UpdateBallState(currentBhr);
         }
         else
         {
            BallHistoryRecord& currentControlBhr = Get(m_CurrentControlIndex);
            UpdateBallState(currentControlBhr);
         }

         ProcessMenu(player, MenuOptionsRecord::MenuActionType::MenuActionType_None, currentTimeMs);
      }
      else
      {
         if (m_WasControlled || m_WasRecalled)
         {
            m_WasControlled = false;
            m_WasRecalled = false;

            if (m_BallHistoryRecordsSize == 0)
            {
               UpdateBallState(currentBhr);
            }
            else
            {
               BallHistoryRecord& currentControlBhr = Get(m_CurrentControlIndex);
               UpdateBallState(currentControlBhr);
            }

            if (m_CurrentControlIndex > m_BallHistoryRecordsHeadIndex)
            {
               m_BallHistoryRecordsSize = m_CurrentControlIndex - m_BallHistoryRecordsHeadIndex;
            }
            else
            {
               m_BallHistoryRecordsSize = m_BallHistoryRecordsSize - m_BallHistoryRecordsHeadIndex + m_CurrentControlIndex;
            }
            m_BallHistoryRecordsHeadIndex = m_CurrentControlIndex;
         }
         else
         {
            Add(m_ControlVBalls, currentTimeMs);
         }
      }
      break;
   case MenuOptionsRecord::ModeType::ModeType_Disabled:
      ProcessMode(player, currentTimeMs);

      if (m_Control)
      {
         m_WasControlled = true;

         UpdateBallState(currentBhr);

         ProcessMenu(player, MenuOptionsRecord::MenuActionType::MenuActionType_None, currentTimeMs);
      }
      else
      {
         if (m_WasControlled)
         {
            m_WasControlled = false;

            UpdateBallState(currentBhr);

            Init(player, currentTimeMs, false);
         }
      }
      break;
   default: InvalidEnumValue("MenuOptionsRecord::ModeType", m_MenuOptions.m_ModeType); break;
   }
}

bool BallHistory::ProcessKeys(Player& player, EnumAssignKeys action, bool isPressed, int currentTimeMs, bool process)
{
   if (action != EnumAssignKeys::eCKeys)
   {
      m_PreviousProcessKeysAction = action;
      m_PreviousProcessKeyIsPressed = isPressed;
   }

   action = m_PreviousProcessKeysAction;
   isPressed = m_PreviousProcessKeyIsPressed;

   if (process)
   {
      m_PreviousProcessKeysAction = EnumAssignKeys::eCKeys;
      m_PreviousProcessKeyIsPressed = false;
   }

   if (action == EnumAssignKeys::eBallHistoryMenu)
   {
      if (isPressed)
      {
         if (process)
         {
            ToggleControl();
            Process(player, currentTimeMs);
         }
         return true;
      }
   }

   if (action == EnumAssignKeys::eBallHistoryRecall)
   {
      if (isPressed)
      {
         if (m_Control)
         {
            if (process)
            {
               m_ShowStatus = !m_ShowStatus;
            }
            return true;
         }
         else
         {
            if (process)
            {
               ToggleRecall();
               Process(player, currentTimeMs);
            }
            return true;
         }
      }
   }
   else if (m_Control && action == EnumAssignKeys::eLeftFlipperKey)
   {
      if (isPressed)
      {
         if (m_Control)
         {
            if (process)
            {
               ProcessMenu(player, MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft, currentTimeMs);
               m_MenuOptions.m_SkipKeyPressed = true;
               m_MenuOptions.m_SkipKeyPressedMs = currentTimeMs;
               m_MenuOptions.m_SkipKeyLeft = true;
            }
         }
      }
      else
      {
         if (m_Control)
         {
            if (process)
            {
               m_MenuOptions.m_SkipKeyPressed = false;
               m_MenuOptions.m_SkipKeyPressedMs = 0;
            }
         }
      }
   }
   else if (m_Control && action == EnumAssignKeys::eRightFlipperKey)
   {
      if (isPressed)
      {
         if (m_Control)
         {
            if (process)
            {
               ProcessMenu(player, MenuOptionsRecord::MenuActionType::MenuActionType_DownRight, currentTimeMs);
               m_MenuOptions.m_SkipKeyPressed = true;
               m_MenuOptions.m_SkipKeyPressedMs = currentTimeMs;
               m_MenuOptions.m_SkipKeyLeft = false;
            }
         }
      }
      else
      {
         if (m_Control)
         {
            if (process)
            {
               m_MenuOptions.m_SkipKeyPressed = false;
               m_MenuOptions.m_SkipKeyPressedMs = 0;
            }
         }
      }
   }
   else if (action == EnumAssignKeys::ePlungerKey || action == EnumAssignKeys::eRightMagnaSave)
   {
      if (isPressed)
      {
         if (m_Control)
         {
            if (process)
            {
               ProcessMenu(player, MenuOptionsRecord::MenuActionType::MenuActionType_Enter, currentTimeMs);
            }
            return true;
         }
      }
   }

   return false;
}

void BallHistory::ProcessMouse(Player& player, int currentTimeMs) { ProcessMenu(player, MenuOptionsRecord::MenuActionType::MenuActionType_Toggle, currentTimeMs); }

bool BallHistory::Control() { return m_Control; }

void BallHistory::SetControl(bool control)
{
   ClearDraws(*g_pplayer);

   if (m_Control != control)
   {
      m_Control = control;
      if (m_Control)
      {
         StopSound();
         g_pplayer->m_ptable->StopAllSounds();
         g_pplayer->PauseMusic();
         g_pplayer->m_noTimeCorrect = true;
         m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
         m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
         m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
         m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
      }
      else
      {
         SaveSettings(*g_pplayer);
         g_pplayer->UnpauseMusic();
      }
   }
}

void BallHistory::ToggleControl() { SetControl(!m_Control); }

void BallHistory::ToggleRecall()
{
   if (m_MenuOptions.m_NormalOptions.m_RecallControlIndex != NormalOptions::RecallControlIndexDisabled)
   {
      SetControl(true);
      m_CurrentControlIndex = m_MenuOptions.m_NormalOptions.m_RecallControlIndex;
      m_WasRecalled = true;
   }
}

void BallHistory::ResetTrainerRunStartTime()
{
   m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
   m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
   m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
   m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
}

bool BallHistory::GetSettingsFileName(Player& player, std::string& fileName)
{
   // TODO GARY fix potential leak of crypt context, crypt hash and file handles
   // if return false happens, the handles will not be cleaned up
   fileName.clear();

   HCRYPTPROV hCryptProv = NULL;
   HCRYPTHASH hMd5Hash = NULL;
   HANDLE hTableFile = NULL;

   // Get a handle to a cryptography provider context.
   if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT) == FALSE || CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hMd5Hash) == FALSE
      || (hTableFile = CreateFile(player.m_ptable->m_filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL)) == INVALID_HANDLE_VALUE)
   {
      return false;
   }

   static const int readBufferSize = 1024 * 1024;
   std::vector<BYTE> readBuffer(readBufferSize);
   BOOL readFileResult = FALSE;
   DWORD bytesRead = 0;
   while ((readFileResult = ReadFile(hTableFile, readBuffer.data(), readBufferSize, &bytesRead, NULL)) == TRUE)
   {
      if (bytesRead == 0)
      {
         break;
      }
      if (CryptHashData(hMd5Hash, readBuffer.data(), bytesRead, 0) == FALSE)
      {
         return false;
      }
   }

   DWORD md5HashLen = 0;

   if (readFileResult == FALSE || CryptGetHashParam(hMd5Hash, HP_HASHVAL, NULL, &md5HashLen, 0) == FALSE)
   {
      return false;
   }

   std::vector<BYTE> md5Hash(md5HashLen);
   if (CryptGetHashParam(hMd5Hash, HP_HASHVAL, md5Hash.data(), &md5HashLen, 0) == FALSE)
   {
      return false;
   }

   std::ostringstream fileNameStream;
   fileNameStream << std::hex << std::uppercase << std::setfill('0');
   for (DWORD x = 0; x < md5HashLen; x++)
   {
      fileNameStream << std::setw(2) << static_cast<int>(md5Hash[x]);
   }
   fileName = fileNameStream.str() + "." + SettingsFileExtension;

   if (CloseHandle(hTableFile) == FALSE || CryptDestroyHash(hMd5Hash) == FALSE || CryptReleaseContext(hCryptProv, 0) == FALSE)
   {
      return false;
   }

   return true;
}


bool BallHistory::GetSettingsFolderPath(std::string& settingsFolderPath)
{
   settingsFolderPath.clear();
   char nameBuffer[MAX_PATH];
   if (DWORD nameBufferLength = GetModuleFileName(NULL, nameBuffer, sizeof(nameBuffer)))
   {
      std::string exeFilePath(nameBuffer, nameBufferLength);
      std::string exeFolderPath = exeFilePath.substr(0, exeFilePath.find_last_of('\\'));
      settingsFolderPath = exeFolderPath + "\\" + SettingsFolderName;
      return true;
   }
   return false;
}

void BallHistory::LoadSettings(Player& player)
{
   std::istringstream tempStream;
   float tempXFloat = 0.0f;
   float tempYFloat = 0.0f;

   char delimeter;

   CSimpleIni iniFile;
   iniFile.LoadFile(m_SettingsFilePath.c_str());
   {
      // Normal Mode Settings

      std::istringstream autoControlVerticesPosition3D;
      if (LoadSettingsGetValue(iniFile, NormalModeSettingsSectionName, NormalModeAutoControlVerticesPosition3DKeyName, autoControlVerticesPosition3D) == true)
      {
         while (autoControlVerticesPosition3D.peek() != EOF)
         {
            m_MenuOptions.m_NormalOptions.m_AutoControlVertices.push_back({ { 0.0f, 0.0f, 0.0f }, false });
            NormalOptions::AutoControlVertex& acv = m_MenuOptions.m_NormalOptions.m_AutoControlVertices.back();
            autoControlVerticesPosition3D >> acv.m_Position.x >> delimeter >> acv.m_Position.y >> delimeter >> acv.m_Position.z >> delimeter;
         }
      }
   }

   {
      // Trainer Mode Settings

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeGameplayDifficultyKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty;
      }

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeVarianceDifficultyKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty;
      }

      LoadSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModeGravityVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_GravityVariance,
         TrainerModeGravityVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode);

      LoadSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModePlayfieldFrictionVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance,
         TrainerModePlayfieldFrictionVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode);

      LoadSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModeFlipperStrengthVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance,
         TrainerModeFlipperStrengthVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode);

      LoadSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModeFlipperFrictionVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance,
         TrainerModeFlipperFrictionVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode);

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeTotalRunsKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_TotalRuns;
      }

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeMaxSecondsPerRunKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun;
      }

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeCountdownSecondsBeforeRunKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun;
      }

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeRunOrderModeKeyName, tempStream) == true)
      {
         if (tempStream.str() == "InOrder")
         {
            m_MenuOptions.m_TrainerOptions.m_RunOrderMode = TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder;
         }
         else if (tempStream.str() == "Random")
         {
            m_MenuOptions.m_TrainerOptions.m_RunOrderMode = TrainerOptions::RunOrderModeType::RunOrderModeType_Random;
         }
         else
         {
            InvalidEnumValue("TrainerOptions::RunOrderModeType", tempStream.str().c_str());
         }
      }

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallKickerBehaviorModeKeyName, tempStream) == true)
      {
         if (tempStream.str() == "Reset")
         {
            m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode = TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset;
         }
         else if (tempStream.str() == "Fail")
         {
            m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode = TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail;
         }
         else
         {
            InvalidEnumValue("TrainerOptions::BallKickerBehaviorModeType", tempStream.str().c_str());
         }
      }

      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeSoundEffectsPassEnabledKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled;
      }
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeSoundEffectsFailEnabledKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled;
      }
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeSoundEffectsTimeLowEnabledKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled;
      }
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeSoundEffectsCountdownEnabledKeyName, tempStream) == true)
      {
         tempStream >> m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled;
      }

      std::istringstream ballStartOptionsPos;
      std::istringstream ballStartOptionsVel;
      std::istringstream ballStartOptionsAngMom;
      std::istringstream ballStartOptionsAngleStart;
      std::istringstream ballStartOptionsAngleFinish;
      std::istringstream ballStartOptionsTotalAngles;
      std::istringstream ballStartOptionsVelocityStart;
      std::istringstream ballStartOptionsVelocityFinish;
      std::istringstream ballStartOptionsTotalVelocities;
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartPositionKeyName, ballStartOptionsPos) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartVelocityKeyName, ballStartOptionsVel) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartAngularMomentumKeyName, ballStartOptionsAngMom) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartAngleStartKeyName, ballStartOptionsAngleStart) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartAngleFinishKeyName, ballStartOptionsAngleFinish) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartTotalAnglesKeyName, ballStartOptionsTotalAngles) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartVelocityStartKeyName, ballStartOptionsVelocityStart) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartVelocityFinishKeyName, ballStartOptionsVelocityFinish) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallStartTotalVelocitiesKeyName, ballStartOptionsTotalVelocities) == true)
      {
         while (ballStartOptionsPos.peek() != EOF && ballStartOptionsVel.peek() != EOF && ballStartOptionsAngMom.peek() != EOF && ballStartOptionsAngleStart.peek() != EOF
            && ballStartOptionsAngleFinish.peek() != EOF && ballStartOptionsTotalAngles.peek() != EOF && ballStartOptionsVelocityStart.peek() != EOF
            && ballStartOptionsVelocityFinish.peek() != EOF && ballStartOptionsTotalVelocities.peek() != EOF)
         {
            m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.push_back(TrainerOptions::BallStartOptionsRecord());
            TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.back();
            ballStartOptionsPos >> bsor.m_StartPosition.x >> delimeter >> bsor.m_StartPosition.y >> delimeter >> bsor.m_StartPosition.z >> delimeter;
            ballStartOptionsVel >> bsor.m_StartVelocity.x >> delimeter >> bsor.m_StartVelocity.y >> delimeter >> bsor.m_StartVelocity.z >> delimeter;
            ballStartOptionsAngMom >> bsor.m_StartAngularMomentum.x >> delimeter >> bsor.m_StartAngularMomentum.y >> delimeter >> bsor.m_StartAngularMomentum.z >> delimeter;
            ballStartOptionsAngleStart >> bsor.m_AngleRangeStart >> delimeter;
            ballStartOptionsAngleFinish >> bsor.m_AngleRangeFinish >> delimeter;
            ballStartOptionsTotalAngles >> bsor.m_TotalRangeAngles >> delimeter;
            ballStartOptionsVelocityStart >> bsor.m_VelocityRangeStart >> delimeter;
            ballStartOptionsVelocityFinish >> bsor.m_VelocityRangeFinish >> delimeter;
            ballStartOptionsTotalVelocities >> bsor.m_TotalRangeVelocities >> delimeter;
         }
      }
      m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.size();

      std::istringstream ballPassOptionsPosition3D;
      std::istringstream ballPassOptionsRadiusPercent;
      std::istringstream ballPassOptionsAssociations;
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallPassPosition3DKeyName, ballPassOptionsPosition3D) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallPassRadiusPercentKeyName, ballPassOptionsRadiusPercent) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallPassAssociationsKeyName, ballPassOptionsAssociations) == true)
      {
         while (ballPassOptionsPosition3D.peek() != EOF && ballPassOptionsRadiusPercent.peek() != EOF && ballPassOptionsAssociations.peek() != EOF)
         {
            m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.push_back(TrainerOptions::BallEndOptionsRecord());
            TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.back();
            ballPassOptionsPosition3D >> bpor.m_EndPosition.x >> delimeter >> bpor.m_EndPosition.y >> delimeter >> bpor.m_EndPosition.z >> delimeter;
            ballPassOptionsRadiusPercent >> bpor.m_EndRadiusPercent >> delimeter;
            std::size_t associatedBallStartIndexesSize;
            ballPassOptionsAssociations >> associatedBallStartIndexesSize >> delimeter;
            for (std::size_t absiIndex = 0; absiIndex < associatedBallStartIndexesSize; absiIndex++)
            {
               std::size_t associatedBallStartIndex;
               ballPassOptionsAssociations >> associatedBallStartIndex >> delimeter;
               bpor.m_AssociatedBallStartIndexes.insert(associatedBallStartIndex);
            }
         }
      }

      std::istringstream ballFailOptionsPosition3D;
      std::istringstream ballFailOptionsRadiusPercent;
      std::istringstream ballFailOptionsAssociations;
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallFailPosition3DKeyName, ballFailOptionsPosition3D) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallFailRadiusPercentKeyName, ballFailOptionsRadiusPercent) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallFailAssociationsKeyName, ballFailOptionsAssociations) == true)
      {
         while (ballFailOptionsPosition3D.peek() != EOF && ballFailOptionsRadiusPercent.peek() != EOF && ballFailOptionsAssociations.peek() != EOF)
         {
            m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.push_back(TrainerOptions::BallEndOptionsRecord());
            TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.back();
            ballFailOptionsPosition3D >> bfor.m_EndPosition.x >> delimeter >> bfor.m_EndPosition.y >> delimeter >> bfor.m_EndPosition.z >> delimeter;
            ballFailOptionsRadiusPercent >> bfor.m_EndRadiusPercent >> delimeter;
            std::size_t associatedBallStartIndexesSize;
            ballFailOptionsAssociations >> associatedBallStartIndexesSize >> delimeter;
            for (std::size_t absiIndex = 0; absiIndex < associatedBallStartIndexesSize; absiIndex++)
            {
               std::size_t associatedBallStartIndex;
               ballFailOptionsAssociations >> associatedBallStartIndex >> delimeter;
               bfor.m_AssociatedBallStartIndexes.insert(associatedBallStartIndex);
            }
         }
      }

      std::istringstream ballCorridorPassPosition3D;
      std::istringstream ballCorridorPassRadiusPercent;
      std::istringstream ballCorridorOpeningPositionLeft3D;
      std::istringstream ballCorridorOpeningPositionRight3D;
      if (LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallCorridorPassPosition3DKeyName, ballCorridorPassPosition3D) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallCorridorPassRadiusPercentKeyName, ballCorridorPassRadiusPercent) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallCorridorOpeningPositionLeft3DKeyName, ballCorridorOpeningPositionLeft3D) == true
         && LoadSettingsGetValue(iniFile, TrainerModeSettingsSectionName, TrainerModeBallCorridorOpeningPositionRight3DKeyName, ballCorridorOpeningPositionRight3D) == true)
      {
         while (ballCorridorPassPosition3D.peek() != EOF && ballCorridorPassRadiusPercent.peek() != EOF && ballCorridorOpeningPositionLeft3D.peek() != EOF
            && ballCorridorOpeningPositionRight3D.peek() != EOF)
         {
            TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;
            new (&bcor) TrainerOptions::BallCorridorOptionsRecord();
            ballCorridorPassPosition3D >> bcor.m_PassPosition.x >> delimeter >> bcor.m_PassPosition.y >> delimeter >> bcor.m_PassPosition.z >> delimeter;
            ballCorridorPassRadiusPercent >> bcor.m_PassRadiusPercent >> delimeter;
            ballCorridorOpeningPositionLeft3D >> bcor.m_OpeningPositionLeft.x >> delimeter >> bcor.m_OpeningPositionLeft.y >> delimeter >> bcor.m_OpeningPositionLeft.z >> delimeter;
            ballCorridorOpeningPositionRight3D >> bcor.m_OpeningPositionRight.x >> delimeter >> bcor.m_OpeningPositionRight.y >> delimeter >> bcor.m_OpeningPositionRight.z >> delimeter;
         }
      }
   }
}

void BallHistory::LoadSettingsDifficultyVariance(
   Player& player, CSimpleIni& iniFile, const char* sectionName, const char* varianceKeyName, S32& variance, const char* modeKeyName, TrainerOptions::DifficultyVarianceModeType& mode)
{
   std::istringstream tempStream;

   if (LoadSettingsGetValue(iniFile, sectionName, varianceKeyName, tempStream) == true)
   {
      tempStream >> variance;
   }

   if (LoadSettingsGetValue(iniFile, sectionName, modeKeyName, tempStream) == true)
   {
      if (tempStream.str() == "AboveAndBelow")
      {
         mode = TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow;
      }
      else if (tempStream.str() == "Above")
      {
         mode = TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly;
      }
      else if (tempStream.str() == "Below")
      {
         mode = TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly;
      }
      else
      {
         InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", tempStream.str().c_str());
      }
   }
}

bool BallHistory::LoadSettingsGetValue(CSimpleIni& iniFile, const char* sectionName, const char* keyName, std::istringstream& value)
{
   if (const char* settingValue = iniFile.GetValue(sectionName, keyName))
   {
      value.clear();
      value.str(settingValue);
      return true;
   }
   return false;
}

void BallHistory::SaveSettings(Player& player)
{
   std::string tempStr;

   CSimpleIni iniFile;
   {
      iniFile.SetValue(TableInfoSectionName, FilePathKeyName, player.m_ptable->m_filename.c_str());
      iniFile.SetValue(TableInfoSectionName, TableNameKeyName, player.m_ptable->m_tableName.c_str());
      iniFile.SetValue(TableInfoSectionName, AuthorKeyName, player.m_ptable->m_author.c_str());
      iniFile.SetValue(TableInfoSectionName, VersionKeyName, player.m_ptable->m_version.c_str());
      iniFile.SetValue(TableInfoSectionName, DateSavedKeyName, player.m_ptable->m_dateSaved.c_str());
   }

   {
      // Normal Mode Settings

      std::ostringstream autoControlVerticesPosition2D;
      std::ostringstream autoControlVerticesPosition3D;
      for (const NormalOptions::AutoControlVertex& acv : m_MenuOptions.m_NormalOptions.m_AutoControlVertices)
      {
         autoControlVerticesPosition3D << acv.m_Position.x << SettingsValueDelimeter << acv.m_Position.y << SettingsValueDelimeter << acv.m_Position.z << SettingsValueDelimeter;
      }

      tempStr = autoControlVerticesPosition3D.str();
      iniFile.SetValue(NormalModeSettingsSectionName, NormalModeAutoControlVerticesPosition3DKeyName, tempStr.c_str());
   }

   {
      // Trainer Mode Settings

      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeGameplayDifficultyKeyName, tempStr.c_str());

      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeVarianceDifficultyKeyName, tempStr.c_str());

      SaveSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModeGravityVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_GravityVariance,
         TrainerModeGravityVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode);

      SaveSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModePlayfieldFrictionVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance,
         TrainerModePlayfieldFrictionVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode);

      SaveSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModeFlipperStrengthVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance,
         TrainerModeFlipperStrengthVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode);

      SaveSettingsDifficultyVariance(player, iniFile, TrainerModeSettingsSectionName, TrainerModeFlipperFrictionVarianceKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance,
         TrainerModeFlipperFrictionVarianceModeKeyName, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode);

      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_TotalRuns);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeTotalRunsKeyName, tempStr.c_str());

      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeMaxSecondsPerRunKeyName, tempStr.c_str());

      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeCountdownSecondsBeforeRunKeyName, tempStr.c_str());

      switch (m_MenuOptions.m_TrainerOptions.m_RunOrderMode)
      {
      case TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder: tempStr = "InOrder"; break;
      case TrainerOptions::RunOrderModeType::RunOrderModeType_Random: tempStr = "Random"; break;
      default: InvalidEnumValue("TrainerOptions::RunOrderModeType", m_MenuOptions.m_TrainerOptions.m_RunOrderMode); break;
      }
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeRunOrderModeKeyName, tempStr.c_str());

      switch (m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode)
      {
      case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset: tempStr = "Reset"; break;
      case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail: tempStr = "Fail"; break;
      default: InvalidEnumValue("TrainerOptions::BallKickerBehaviorModeType", m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode); break;
      }
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallKickerBehaviorModeKeyName, tempStr.c_str());

      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeSoundEffectsPassEnabledKeyName, tempStr.c_str());
      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeSoundEffectsFailEnabledKeyName, tempStr.c_str());
      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeSoundEffectsTimeLowEnabledKeyName, tempStr.c_str());
      tempStr = std::to_string(m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled);
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeSoundEffectsCountdownEnabledKeyName, tempStr.c_str());

      std::ostringstream ballStartOptionsPos;
      std::ostringstream ballStartOptionsVel;
      std::ostringstream ballStartOptionsAngMom;
      std::ostringstream ballStartOptionsAngleStart;
      std::ostringstream ballStartOptionsAngleFinish;
      std::ostringstream ballStartOptionsTotalAngles;
      std::ostringstream ballStartOptionsVelocityStart;
      std::ostringstream ballStartOptionsVelocityFinish;
      std::ostringstream ballStartOptionsTotalVelocities;

      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];

         ballStartOptionsPos << bsor.m_StartPosition.x << SettingsValueDelimeter << bsor.m_StartPosition.y << SettingsValueDelimeter << bsor.m_StartPosition.z << SettingsValueDelimeter;
         ballStartOptionsVel << bsor.m_StartVelocity.x << SettingsValueDelimeter << bsor.m_StartVelocity.y << SettingsValueDelimeter << bsor.m_StartVelocity.z << SettingsValueDelimeter;
         ballStartOptionsAngMom << bsor.m_StartAngularMomentum.x << SettingsValueDelimeter << bsor.m_StartAngularMomentum.y << SettingsValueDelimeter << bsor.m_StartAngularMomentum.z
                                << SettingsValueDelimeter;
         ballStartOptionsAngleStart << bsor.m_AngleRangeStart << SettingsValueDelimeter;
         ballStartOptionsAngleFinish << bsor.m_AngleRangeFinish << SettingsValueDelimeter;
         ballStartOptionsTotalAngles << bsor.m_TotalRangeAngles << SettingsValueDelimeter;
         ballStartOptionsVelocityStart << bsor.m_VelocityRangeStart << SettingsValueDelimeter;
         ballStartOptionsVelocityFinish << bsor.m_VelocityRangeFinish << SettingsValueDelimeter;
         ballStartOptionsTotalVelocities << bsor.m_TotalRangeVelocities << SettingsValueDelimeter;
      }

      tempStr = ballStartOptionsPos.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartPositionKeyName, tempStr.c_str());
      tempStr = ballStartOptionsVel.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartVelocityKeyName, tempStr.c_str());
      tempStr = ballStartOptionsAngMom.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartAngularMomentumKeyName, tempStr.c_str());
      tempStr = ballStartOptionsAngleStart.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartAngleStartKeyName, tempStr.c_str());
      tempStr = ballStartOptionsAngleFinish.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartAngleFinishKeyName, tempStr.c_str());
      tempStr = ballStartOptionsTotalAngles.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartTotalAnglesKeyName, tempStr.c_str());
      tempStr = ballStartOptionsVelocityStart.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartVelocityStartKeyName, tempStr.c_str());
      tempStr = ballStartOptionsVelocityFinish.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartVelocityFinishKeyName, tempStr.c_str());
      tempStr = ballStartOptionsTotalVelocities.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallStartTotalVelocitiesKeyName, tempStr.c_str());

      std::ostringstream ballPassOptionsPosition3D;
      std::ostringstream ballPassOptionsRadiusPercent;
      std::ostringstream ballPassOptionsAssociations;
      for (const TrainerOptions::BallEndOptionsRecord& bpor : m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords)
      {
         ballPassOptionsPosition3D << bpor.m_EndPosition.x << SettingsValueDelimeter << bpor.m_EndPosition.y << SettingsValueDelimeter << bpor.m_EndPosition.z << SettingsValueDelimeter;
         ballPassOptionsRadiusPercent << bpor.m_EndRadiusPercent << SettingsValueDelimeter;
         ballPassOptionsAssociations << bpor.m_AssociatedBallStartIndexes.size() << SettingsValueDelimeter;
         for (const std::size_t& index : bpor.m_AssociatedBallStartIndexes)
         {
            ballPassOptionsAssociations << index << SettingsValueDelimeter;
         }
      }
      tempStr = ballPassOptionsPosition3D.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallPassPosition3DKeyName, tempStr.c_str());
      tempStr = ballPassOptionsRadiusPercent.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallPassRadiusPercentKeyName, tempStr.c_str());
      tempStr = ballPassOptionsAssociations.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallPassAssociationsKeyName, tempStr.c_str());

      std::ostringstream ballFailOptionsPos2D;
      std::ostringstream ballFailOptionsPosition3D;
      std::ostringstream ballFailOptionsRadiusPercent;
      std::ostringstream ballFailOptionsAssociations;
      for (const TrainerOptions::BallEndOptionsRecord& bfor : m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords)
      {
         ballFailOptionsPosition3D << bfor.m_EndPosition.x << SettingsValueDelimeter << bfor.m_EndPosition.y << SettingsValueDelimeter << bfor.m_EndPosition.z << SettingsValueDelimeter;
         ballFailOptionsRadiusPercent << bfor.m_EndRadiusPercent << SettingsValueDelimeter;
         ballFailOptionsAssociations << bfor.m_AssociatedBallStartIndexes.size() << SettingsValueDelimeter;
         for (const std::size_t& index : bfor.m_AssociatedBallStartIndexes)
         {
            ballFailOptionsAssociations << index << SettingsValueDelimeter;
         }
      }
      tempStr = ballFailOptionsPosition3D.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallFailPosition3DKeyName, tempStr.c_str());
      tempStr = ballFailOptionsRadiusPercent.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallFailRadiusPercentKeyName, tempStr.c_str());
      tempStr = ballFailOptionsAssociations.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallFailAssociationsKeyName, tempStr.c_str());

      std::ostringstream ballBallCorridorPassPosition3D;
      std::ostringstream ballBallCorridorPassRadiusPercent;
      std::ostringstream ballBallCorridorOpeningPositionLeft3D;
      std::ostringstream ballBallCorridorOpeningPositionRight3D;
      TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;
      ballBallCorridorPassPosition3D << bcor.m_PassPosition.x << SettingsValueDelimeter << bcor.m_PassPosition.y << SettingsValueDelimeter << bcor.m_PassPosition.z << SettingsValueDelimeter;
      ballBallCorridorPassRadiusPercent << bcor.m_PassRadiusPercent << SettingsValueDelimeter;
      ballBallCorridorOpeningPositionLeft3D << bcor.m_OpeningPositionLeft.x << SettingsValueDelimeter << bcor.m_OpeningPositionLeft.y << SettingsValueDelimeter
                                            << bcor.m_OpeningPositionLeft.z << SettingsValueDelimeter;
      ballBallCorridorOpeningPositionRight3D << bcor.m_OpeningPositionRight.x << SettingsValueDelimeter << bcor.m_OpeningPositionRight.y << SettingsValueDelimeter
                                             << bcor.m_OpeningPositionRight.z << SettingsValueDelimeter;
      tempStr = ballBallCorridorPassPosition3D.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallCorridorPassPosition3DKeyName, tempStr.c_str());
      tempStr = ballBallCorridorPassRadiusPercent.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallCorridorPassRadiusPercentKeyName, tempStr.c_str());
      tempStr = ballBallCorridorOpeningPositionLeft3D.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallCorridorOpeningPositionLeft3DKeyName, tempStr.c_str());
      tempStr = ballBallCorridorOpeningPositionRight3D.str();
      iniFile.SetValue(TrainerModeSettingsSectionName, TrainerModeBallCorridorOpeningPositionRight3DKeyName, tempStr.c_str());
   }

   iniFile.SaveFile(m_SettingsFilePath.c_str());
}

void BallHistory::SaveSettingsDifficultyVariance(
   Player& player, CSimpleIni& iniFile, const char* sectionName, const char* varianceKeyName, S32 variance, const char* modeKeyName, TrainerOptions::DifficultyVarianceModeType mode)
{
   std::string tempStr = std::to_string(variance);
   iniFile.SetValue(sectionName, varianceKeyName, tempStr.c_str());

   switch (mode)
   {
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow: tempStr = "AboveAndBelow"; break;
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly: tempStr = "Above"; break;
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly: tempStr = "Below"; break;
   default: InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", mode); break;
   }
   iniFile.SetValue(sectionName, modeKeyName, tempStr.c_str());
}

void BallHistory::InitBallsIncreased(Player& player)
{
   for (auto controlVBall : m_ControlVBalls)
   {
      std::size_t controlVBallsPreviousIndex = 0;
      std::size_t controlVBallsPreviousInsertIndex = m_ControlVBallsPrevious.size();
      for (; controlVBallsPreviousIndex < m_ControlVBallsPrevious.size(); ++controlVBallsPreviousIndex)
      {
         const HitBall* controlVBallPrevious = m_ControlVBallsPrevious[controlVBallsPreviousIndex];
         if (controlVBall < controlVBallPrevious)
         {
            controlVBallsPreviousInsertIndex = controlVBallsPreviousIndex;
         }

         if (controlVBall == controlVBallPrevious)
         {
            break;
         }
      }

      if (controlVBallsPreviousIndex >= m_ControlVBallsPrevious.size())
      {
         std::size_t tempCurrentIndex = m_CurrentControlIndex;
         std::size_t tailIndex = GetTailIndex();
         while (true)
         {
            BallHistoryRecord& currentBhr = m_BallHistoryRecords[tempCurrentIndex];
            currentBhr.Insert(controlVBall, controlVBallsPreviousInsertIndex);

            if (tempCurrentIndex == tailIndex)
            {
               break;
            }

            if (tempCurrentIndex == 0)
            {
               tempCurrentIndex = m_BallHistoryRecords.size() - 1;
            }
            else
            {
               tempCurrentIndex--;
            }
         }
      }
   }

   m_ControlVBallsPrevious.clear();
   for (auto controlVBall : m_ControlVBalls)
   {
      m_ControlVBallsPrevious.push_back(controlVBall);
   }
}

void BallHistory::InitBallsDecreased(Player& player)
{
   for (std::size_t controlVBallsPreviousIndex = 0; controlVBallsPreviousIndex < m_ControlVBallsPrevious.size();)
   {
      const HitBall* controlVBallPrevious = m_ControlVBallsPrevious[controlVBallsPreviousIndex];

      std::size_t controlVBallIndex = 0;
      for (; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
      {
         if (m_ControlVBalls[controlVBallIndex] == controlVBallPrevious)
         {
            break;
         }
      }

      if (controlVBallIndex >= m_ControlVBalls.size())
      {
         std::size_t tempCurrentIndex = m_CurrentControlIndex;
         std::size_t tailIndex = GetTailIndex();
         while (true)
         {
            BallHistoryRecord& currentBhr = m_BallHistoryRecords[tempCurrentIndex];
            currentBhr.m_BallHistoryStates.erase(currentBhr.m_BallHistoryStates.begin() + controlVBallsPreviousIndex);

            if (tempCurrentIndex == tailIndex)
            {
               break;
            }

            if (tempCurrentIndex == 0)
            {
               tempCurrentIndex = m_BallHistoryRecords.size() - 1;
            }
            else
            {
               tempCurrentIndex--;
            }
         }

         m_ControlVBallsPrevious.erase(m_ControlVBallsPrevious.begin() + controlVBallsPreviousIndex);
      }
      else
      {
         controlVBallsPreviousIndex++;
      }
   }

   m_ControlVBallsPrevious.clear();
   for (auto controlVBall : m_ControlVBalls)
   {
      m_ControlVBallsPrevious.push_back(controlVBall);
   }
}

void BallHistory::InitControlVBalls(Player& player)
{
   m_ControlVBallsPrevious.resize(m_ControlVBalls.size());
   std::copy(m_ControlVBalls.begin(), m_ControlVBalls.end(), m_ControlVBallsPrevious.begin());
   m_ControlVBalls.clear();
   for (HitBall* controlVBall : player.m_vball)
   {
      if (!controlVBall->m_d.m_lockedInKicker)
      {
         m_ControlVBalls.push_back(controlVBall);
      }
   }
   std::sort(m_ControlVBalls.begin(), m_ControlVBalls.end());

   if (player.m_vball.size() > m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
   {
      m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.resize(player.m_vball.size());
   }
}

void BallHistory::InitActiveBallKickers(PinTable& pinTable)
{
   m_ActiveBallKickers.clear();
   for (auto vedit : pinTable.m_vedit)
   {
      if (vedit && vedit->GetItemType() == ItemTypeEnum::eItemKicker)
      {
         Kicker* kicker = static_cast<Kicker*>(vedit);
         KickerHitCircle* kickerHitCircle = kicker->GetKickerHitCircle();
         if (kickerHitCircle)
         {
            switch (m_MenuOptions.m_MenuState)
            {
            case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results: kickerHitCircle->m_collideDisableCollide = true; break;
            default: kickerHitCircle->m_collideDisableCollide = false; break;
            }

            if (kicker->m_d.m_enabled && !kicker->m_d.m_fallThrough && kickerHitCircle->m_enabled)
            {
               m_ActiveBallKickers.push_back(kicker);
            }
         }
      }
   }
}

void BallHistory::InitFlippers(PinTable& pinTable)
{
   m_Flippers.clear();
   for (auto vedit : pinTable.m_vedit)
   {
      if (vedit && vedit->GetItemType() == ItemTypeEnum::eItemFlipper)
      {
         Flipper* flipper = static_cast<Flipper*>(vedit);
         {
            m_Flippers.push_back(flipper);
         }
      }
   }
}

void BallHistory::ControlNext()
{
   if (!m_Control)
   {
      return;
   }
   else if (m_CurrentControlIndex == m_BallHistoryRecordsHeadIndex)
   {
      // at end of 'next' ball history, do nothing
   }
   else
   {
      switch (m_NextPreviousBy)
      {
      case BallHistory::NextPreviousByType::eTimeMs:
      {
         int nextTimeMsec = m_BallHistoryRecords[m_CurrentControlIndex].m_TimeMs + m_BallHistoryControlStepMs;
         while (m_BallHistoryRecords[m_CurrentControlIndex].m_TimeMs <= nextTimeMsec && ControlNextMove())
            ;
      }
      break;

      case BallHistory::NextPreviousByType::eDistancePixels:
      {
         bool exceededTravel = false;
         std::size_t ballCount = m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates.size();
         std::vector<float> distancePixelsTraveled(ballCount, 0.0f);
         BallHistoryRecord* lastBallHistoryRecord = &m_BallHistoryRecords[m_CurrentControlIndex];
         while (!exceededTravel && ControlNextMove())
         {
            for (std::size_t bhsIndex = 0; bhsIndex < ballCount; bhsIndex++)
            {
               distancePixelsTraveled[bhsIndex]
                  += DistancePixels(m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates[bhsIndex].m_Position, lastBallHistoryRecord->m_BallHistoryStates[bhsIndex].m_Position);

               if (distancePixelsTraveled[bhsIndex] > m_BallHistoryControlStepPixels)
               {
                  exceededTravel = true;
                  break;
               }
            }
         }
      }
      break;

      default: InvalidEnumValue("BallHistory::NextPreviousByType", m_NextPreviousBy); break;
      }
   }
}

void BallHistory::ControlPrev()
{
   if (!m_Control)
   {
      return;
   }
   else if (m_CurrentControlIndex == GetTailIndex())
   {
      // at end of 'prev' ball history, do nothing
   }
   else
   {
      switch (m_NextPreviousBy)
      {
      case BallHistory::NextPreviousByType::eTimeMs:
      {
         int prevTimeMsec = m_BallHistoryRecords[m_CurrentControlIndex].m_TimeMs - m_BallHistoryControlStepMs;
         while (m_BallHistoryRecords[m_CurrentControlIndex].m_TimeMs >= prevTimeMsec && ControlPrevMove())
            ;
      }
      break;

      case BallHistory::NextPreviousByType::eDistancePixels:
      {
         bool exceededTravel = false;
         std::size_t ballCount = m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates.size();
         std::vector<float> distancePixelsTraveled(ballCount, 0.0f);
         BallHistoryRecord* lastBallHistoryRecord = &m_BallHistoryRecords[m_CurrentControlIndex];
         while (!exceededTravel && ControlPrevMove())
         {
            for (std::size_t bhsIndex = 0; bhsIndex < ballCount; bhsIndex++)
            {
               distancePixelsTraveled[bhsIndex]
                  += DistancePixels(m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates[bhsIndex].m_Position, lastBallHistoryRecord->m_BallHistoryStates[bhsIndex].m_Position);

               if (distancePixelsTraveled[bhsIndex] > m_BallHistoryControlStepPixels)
               {
                  exceededTravel = true;
                  break;
               }
            }
         }
      }
      break;

      default: InvalidEnumValue("BallHistory::NextPreviousByType", m_NextPreviousBy); break;
      }
   }
}

void BallHistory::ResetBallHistoryRenderSizes()
{
   for (std::size_t ballHistoryRecordIndex = 0; ballHistoryRecordIndex < m_BallHistoryRecords.size(); ++ballHistoryRecordIndex)
   {
      BallHistoryRecord& ballHistoryRecord = m_BallHistoryRecords[ballHistoryRecordIndex];
      for (std::size_t ballHistoryStateIndex = 0; ballHistoryStateIndex < ballHistoryRecord.m_BallHistoryStates.size(); ++ballHistoryStateIndex)
      {
         BallHistoryState& ballHistoryState = ballHistoryRecord.m_BallHistoryStates[ballHistoryStateIndex];
         ballHistoryState.m_DrawRadius = 0.0f;
      }
   }
}

void BallHistory::DrawBallHistory(Player& player)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_DrawBallHistoryUsec);

   struct DrawBallHistoryRecord
   {
      float TotalDistancePixelsTraveled;
      BallHistoryRecord* LastDrawnBallHistoryRecord;
      float TotalToRender;
   };

   std::size_t tempCurrentIndex = m_CurrentControlIndex;
   std::size_t tailIndex = GetTailIndex();

   std::size_t ballCount = m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates.size();
   std::vector<DrawBallHistoryRecord> drawBallHistoryRecords(ballCount, { 0.0f, nullptr, 0.0f });
   bool anyMaxDistanceTraveled = false;
   BallHistoryRecord* previousBhr = nullptr;

   ResetBallHistoryRenderSizes();

   std::size_t drawLineIndex = 0;
   // get total of ball history records to draw and fill in size
   while (tempCurrentIndex != tailIndex && !anyMaxDistanceTraveled)
   {
      BallHistoryRecord& tempCurrentBhr = m_BallHistoryRecords[tempCurrentIndex];
      for (std::size_t ballHistoryStateIndex = 0; ballHistoryStateIndex < tempCurrentBhr.m_BallHistoryStates.size(); ++ballHistoryStateIndex)
      {
         BallHistoryState& ballHistoryState = tempCurrentBhr.m_BallHistoryStates[ballHistoryStateIndex];
         HitBall* controlVBall = m_ControlVBalls[ballHistoryStateIndex];

         if (previousBhr)
         {
            float distance = DistancePixels(previousBhr->m_BallHistoryStates[ballHistoryStateIndex].m_Position, ballHistoryState.m_Position);
            if (distance < (GetDefaultBallRadius() * 3.0f))
            {
               drawBallHistoryRecords[ballHistoryStateIndex].TotalDistancePixelsTraveled += distance;
            }
         }

         BallHistoryRecord* lastDrawnBhr = drawBallHistoryRecords[ballHistoryStateIndex].LastDrawnBallHistoryRecord;
         Vertex3Ds& lastDrawnPos = lastDrawnBhr ? lastDrawnBhr->m_BallHistoryStates[ballHistoryStateIndex].m_Position
                                                : m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates[ballHistoryStateIndex].m_Position;
         float lastDrawnRadius = lastDrawnBhr ? lastDrawnBhr->m_BallHistoryStates[ballHistoryStateIndex].m_DrawRadius
                                              : m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates[ballHistoryStateIndex].m_DrawRadius;

         float ballRadius = GetDefaultBallRadius();
         float currentDrawRadius = (((VelocityPixels(ballHistoryState.m_Velocity) - 0) * (ballRadius * 1.75f - ballRadius * 0.25f)) / (m_MaxBallVelocityPixels - 0)) + ballRadius * 0.25f;

         float distanceToLastDrawnBallHistory = DistancePixels(ballHistoryState.m_Position, lastDrawnPos);

         if (distanceToLastDrawnBallHistory > (lastDrawnRadius + currentDrawRadius) && drawBallHistoryRecords[ballHistoryStateIndex].TotalDistancePixelsTraveled < ControlVerticesDistanceMax)
         {
            ballHistoryState.m_DrawRadius = currentDrawRadius;
            drawBallHistoryRecords[ballHistoryStateIndex].LastDrawnBallHistoryRecord = &tempCurrentBhr;
            drawBallHistoryRecords[ballHistoryStateIndex].TotalToRender++;
            std::string drawLineName = "DrawBallHistoryLine" + std::to_string(drawLineIndex++);
            DrawLine(player, drawLineName.c_str(), ballHistoryState.m_Position, lastDrawnPos, m_AutoControlBallColor, 10);
         }
      }

      if (tempCurrentIndex == 0)
      {
         tempCurrentIndex = m_BallHistoryRecords.size() - 1;
      }
      else
      {
         tempCurrentIndex--;
      }

      previousBhr = &tempCurrentBhr;
      for (const DrawBallHistoryRecord& dbhr : drawBallHistoryRecords)
      {
         if (dbhr.TotalDistancePixelsTraveled >= ControlVerticesDistanceMax)
         {
            anyMaxDistanceTraveled = true;
         }
      }
   }

   // fill in color and texture
   tempCurrentIndex = m_CurrentControlIndex;
   std::size_t drawBallCount = 0;

   while (tempCurrentIndex != tailIndex)
   {
      BallHistoryRecord& tempCurrentBhr = m_BallHistoryRecords[tempCurrentIndex];
      for (std::size_t ballHistoryStateIndex = 0; ballHistoryStateIndex < tempCurrentBhr.m_BallHistoryStates.size(); ++ballHistoryStateIndex)
      {
         BallHistoryState& ballHistoryState = tempCurrentBhr.m_BallHistoryStates[ballHistoryStateIndex];
         if (ballHistoryState.m_DrawRadius > 0.0f)
         {
            if (tempCurrentIndex == m_CurrentControlIndex)
            {
               ballHistoryState.m_Color = 0x00000000;
            }
            else
            {
               if (ballHistoryState.m_DrawRadius > 0.0f)
               {
                  unsigned char red = (unsigned char)(0xFF - drawBallCount * 0xFF / drawBallHistoryRecords[ballHistoryStateIndex].TotalToRender);
                  unsigned char blue = (unsigned char)(drawBallCount * 0xFF / drawBallHistoryRecords[ballHistoryStateIndex].TotalToRender);

                  ballHistoryState.m_Color = 0x00 << 24 | blue << 16 | 0x00 << 8 | red;

                  drawBallCount++;
               }
            }
         }
      }


      if (tempCurrentIndex == 0)
      {
         tempCurrentIndex = m_BallHistoryRecords.size() - 1;
      }
      else
      {
         tempCurrentIndex--;
      }
   }

   // draw the fake balls
   std::size_t ballHistoryRecordIndex = 0;
   for (auto& ballHistoryRecord : m_BallHistoryRecords)
   {
      for (auto& ballHistoryState : ballHistoryRecord.m_BallHistoryStates)
      {
         if (ballHistoryState.m_DrawRadius > 0.0f)
         {
            std::string drawBallHistoryFakeBallName = "DrawBallHistoryBall" + std::to_string(ballHistoryRecordIndex++);
            DrawFakeBall(player, drawBallHistoryFakeBallName.c_str(), ballHistoryState.m_Position, ballHistoryState.m_DrawRadius, ballHistoryState.m_Color);
         }
      }
   }
}

void BallHistory::DrawFakeBall(Player& player, const char* name, const Vertex3Ds& position, float radius, DWORD color)
{
   CComObject<Ball>* drawnBall = nullptr;
   auto findIt = m_DrawnBalls.find(name);
   if (findIt == m_DrawnBalls.end())
   {
      CComObject<Ball>::CreateInstance(&drawnBall);
      drawnBall->AddRef();
      drawnBall->Init(player.m_ptable, 0.0f, 0.0f, false, false);
      drawnBall->RenderSetup(player.m_renderer->m_renderDevice);
      m_DrawnBalls[name] = drawnBall;
   }
   else
   {
      drawnBall = m_DrawnBalls[name];
   }

   drawnBall->m_hitBall.m_d.m_pos = { position.x, position.y, position.z };
   drawnBall->m_hitBall.m_d.m_mass = 1.0f;
   drawnBall->m_hitBall.m_d.m_radius = radius;
   drawnBall->m_hitBall.m_d.m_vel.SetZero();
   drawnBall->m_d.m_useTableRenderSettings = true;
   drawnBall->put_Color(color);
   player.m_vhitables.push_back(drawnBall);
}

void BallHistory::DrawFakeBall(Player& player, const char* name, Vertex3Ds& position, float radius, DWORD ballColor, const Vertex3Ds* lineEndPosition, DWORD lineColor, int lineThickness)
{
   float ballRadius = radius == 0.0f ? GetDefaultBallRadius() : radius;

   DrawFakeBall(player, name, position, ballRadius, ballColor);
   if (lineEndPosition)
   {
      std::string drawLineName = std::string(name) + "Line";
      DrawLine(player, drawLineName.c_str(), position, *lineEndPosition, lineColor, lineThickness);
   }
}

void BallHistory::DrawFakeBall(Player& player, const char* name, Vertex3Ds& position, DWORD ballColor, const Vertex3Ds* lineEndPosition, DWORD lineColor, int lineThickness)
{
   DrawFakeBall(player, name, position, 0.0f, ballColor, lineEndPosition, lineColor, lineThickness);
}

void BallHistory::DrawLineRotate(Rubber& drawnLine, const Vertex3Ds& center, const Vertex3Ds& start, const Vertex3Ds& end)
{
   Vertex3Ds startCenter = (start - center);
   startCenter.Normalize();
   Vertex3Ds endCenter = (end - center);
   endCenter.Normalize();

   auto toSpherical = [](const Vertex3Ds& v) -> std::array<float, 2>
   {
      float theta = std::atan2(v.z, v.x);
      float phi = std::asin(v.y);
      return { theta, phi };
   };

   auto [yaw1, pitch1] = toSpherical(startCenter);
   auto [yaw2, pitch2] = toSpherical(endCenter);

   float deltaYaw = yaw2 - yaw1;
   float deltaPitch = pitch2 - pitch1;

   drawnLine.m_d.m_rotY = deltaYaw * (180.0f / M_PIf) * -1;
   drawnLine.m_d.m_rotZ = deltaPitch * (180.0f / M_PIf) * -1;
}

void BallHistory::DrawLine(Player& player, const char* name, const Vertex3Ds& posA, const Vertex3Ds& posB, DWORD color, int thickness)
{
   float lineLength = BallHistory::DistancePixels(posA, posB);
   if (lineLength == 0.0f)
   {
      return;
   }

   std::string imageColorName = "BallHistoryDrawLineRed" + std::to_string(color);
   Texture* colorTexture = player.m_ptable->GetImage(imageColorName);
   if (colorTexture == nullptr)
   {
      FIBITMAP* dib = FreeImage_Allocate(1, 1, 24);

      RGBQUAD singleColor = { 0 };
      singleColor.rgbBlue = (color >> 16) & 0xFF;
      singleColor.rgbGreen = (color >> 8) & 0xFF;
      singleColor.rgbRed = color & 0xFF;
      singleColor.rgbReserved = 0;

      FreeImage_SetPixelColor(dib, 0, 0, &singleColor);


      std::string settingsFolderPath;
      if (GetSettingsFolderPath(settingsFolderPath) == true)
      {
         std::string tempColorImageFilePath = settingsFolderPath + std::string("\\") + imageColorName + ".png";
         FreeImage_Save(FIF_PNG, dib, tempColorImageFilePath.c_str(), PNG_DEFAULT);

         player.m_ptable->ImportImage(tempColorImageFilePath, imageColorName);

         DeleteFile(tempColorImageFilePath.c_str());
      }
   }

   Vertex3Ds midpoint = { (posA.x + posB.x) / 2.0f, (posA.y + posB.y) / 2.0f, (posA.z + posB.z) / 2.0f };

   CComObject<Rubber>* drawnLine = nullptr;
   auto findIt = m_DrawnLines.find(name);
   if (findIt == m_DrawnLines.end())
   {
      CComObject<Rubber>::CreateInstance(&drawnLine);
      drawnLine->AddRef();
      drawnLine->Init(player.m_ptable, 0.0f, 0.0f, false, true);
      drawnLine->RenderSetup(player.m_renderer->m_renderDevice);
      m_DrawnLines[name] = drawnLine;
   }
   else
   {
      drawnLine = m_DrawnLines[name];
   }

   if (drawnLine)
   {
      drawnLine->RenderRelease();
      drawnLine->ClearForOverwrite();

      drawnLine->AddDragPoint({ midpoint.x - (lineLength / 2.0f), midpoint.y, midpoint.z });
      drawnLine->AddDragPoint({ midpoint.x + (lineLength / 2.0f), midpoint.y, midpoint.z });

      drawnLine->m_d.m_height = midpoint.z;
      drawnLine->m_d.m_thickness = thickness;
      drawnLine->m_d.m_staticRendering = false;
      drawnLine->m_d.m_szImage = imageColorName;

      DrawLineRotate(*drawnLine, { midpoint.x, midpoint.y, midpoint.z }, { midpoint.x - (lineLength / 2.0f), midpoint.y, midpoint.z }, { posA.x, posA.y, posA.z });

      drawnLine->RenderSetup(player.m_renderer->m_renderDevice);

      player.m_vhitables.push_back(drawnLine);
   }
}

void BallHistory::DrawIntersectionCircle(Player& player, const char* name, Vertex3Ds& position, float intersectionRadiusPercent, DWORD color)
{
   CComObject<Light>* drawnIntersectionCircle = nullptr;
   auto findIt = m_DrawnIntersectionCircles.find(name);
   if (findIt == m_DrawnIntersectionCircles.end())
   {
      CComObject<Light>::CreateInstance(&drawnIntersectionCircle);
      drawnIntersectionCircle->AddRef();
      drawnIntersectionCircle->Init(player.m_ptable, 0.0f, 0.0f, false, true);
      drawnIntersectionCircle->RenderSetup(player.m_renderer->m_renderDevice);
      m_DrawnIntersectionCircles[name] = drawnIntersectionCircle;
   }
   else
   {
      drawnIntersectionCircle = m_DrawnIntersectionCircles[name];
   }
   if (drawnIntersectionCircle)
   {
      drawnIntersectionCircle->RenderRelease();
      drawnIntersectionCircle->Init(player.m_ptable, position.x, position.y, false, true);

      drawnIntersectionCircle->ClearForOverwrite();
      float ballRadius = GetDefaultBallRadius();
      float intersectionRadius = ballRadius * intersectionRadiusPercent / 100.0f;
      drawnIntersectionCircle->m_d.m_falloff = intersectionRadius;
      drawnIntersectionCircle->InitShape();

      drawnIntersectionCircle->m_overrideSurfaceHeight = position.z - ballRadius;
      drawnIntersectionCircle->put_Color(color);

      drawnIntersectionCircle->RenderSetup(player.m_renderer->m_renderDevice);

      player.m_vhitables.push_back(drawnIntersectionCircle);
   }
}

void BallHistory::DrawNormalModeVisuals(Player& player, int currentTimeMs)
{
   Matrix3 orientation;
   orientation.SetIdentity();
   float ballRadius = MenuOptionsRecord::DefaultBallRadius;

   if (HitBall* controlVBall = m_ControlVBalls.size() ? m_ControlVBalls[0] : nullptr)
   {
      orientation = controlVBall->m_orientation;
      ballRadius = controlVBall->m_d.m_radius;
   }

   if ((currentTimeMs % OneSecondMs) >= DrawBallBlinkMs)
   {
      for (std::size_t acvIndex = 0; acvIndex < m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size(); acvIndex++)
      {
         NormalOptions::AutoControlVertex& acv = m_MenuOptions.m_NormalOptions.m_AutoControlVertices[acvIndex];
         std::string autoControlFakeBallName = "AutoControlBall" + std::to_string(acvIndex);
         DrawFakeBall(player, autoControlFakeBallName.c_str(), acv.m_Position, ballRadius, m_AutoControlBallColor);
         POINT screenPoint = Get2DPointFrom3D(player, acv.m_Position);
         PrintScreenRecord::Text(NormalOptions::ImGuiDrawAutoControlVertexLabels[acvIndex], float(screenPoint.x), float(screenPoint.y), "%zu", acvIndex + 1);
      }

      if (m_MenuOptions.m_NormalOptions.m_RecallControlIndex != NormalOptions::RecallControlIndexDisabled)
      {
         BallHistoryRecord& recallBallHistoryRecord = m_BallHistoryRecords[m_MenuOptions.m_NormalOptions.m_RecallControlIndex];
         for (std::size_t bhsIndex = 0; bhsIndex < recallBallHistoryRecord.m_BallHistoryStates.size(); bhsIndex++)
         {
            BallHistoryState& recallBallHistoryState = recallBallHistoryRecord.m_BallHistoryStates[bhsIndex];
            std::string recallBallFakeBallName = "RecallBall" + std::to_string(bhsIndex);
            DrawFakeBall(player, recallBallFakeBallName.c_str(), recallBallHistoryState.m_Position, ballRadius, m_RecallBallColor);
            POINT screenPoint = Get2DPointFrom3D(player, recallBallHistoryState.m_Position);
            PrintScreenRecord::Text(NormalOptions::ImGuiDrawRecallVertexLabels[bhsIndex], float(screenPoint.x), float(screenPoint.y), "RCL");
         }
      }
   }

   DrawActiveBallKickers(player);
}

void BallHistory::ClearDraws(Player& player)
{
   for (auto drawnBall : m_DrawnBalls)
   {
      for (std::size_t hitableIndex = 0; hitableIndex < player.m_vhitables.size();)
      {
         Hitable* hitable = player.m_vhitables[hitableIndex]->GetIHitable();
         if (hitable == drawnBall.second)
         {
            player.m_vhitables.erase(player.m_vhitables.begin() + hitableIndex);
         }
         else
         {
            hitableIndex++;
         }
      }
   }

   for (auto drawnIntersectionCircle : m_DrawnIntersectionCircles)
   {
      for (std::size_t hitableIndex = 0; hitableIndex < player.m_vhitables.size();)
      {
         Hitable* hitable = player.m_vhitables[hitableIndex]->GetIHitable();
         if (hitable == drawnIntersectionCircle.second)
         {
            player.m_vhitables.erase(player.m_vhitables.begin() + hitableIndex);
         }
         else
         {
            hitableIndex++;
         }
      }
   }

   for (auto drawnLines : m_DrawnLines)
   {
      for (std::size_t hitableIndex = 0; hitableIndex < player.m_vhitables.size();)
      {
         Hitable* hitable = player.m_vhitables[hitableIndex]->GetIHitable();
         if (hitable == drawnLines.second)
         {
            player.m_vhitables.erase(player.m_vhitables.begin() + hitableIndex);
         }
         else
         {
            hitableIndex++;
         }
      }
   }
}

bool BallHistory::ShouldDrawTrainerBallStarts(std::size_t index, int currentTimeMs)
{
   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions:
      switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
      {
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallStart: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleVelocityMode:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStart:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallStartMode:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectExistingBallStartLocation:
      switch (m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode)
      {
      case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select:
         return index != m_MenuOptions.m_CurrentCompleteIndex || (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
   default: return true;
   }
}

bool BallHistory::ShouldDrawTrainerBallPasses(std::size_t index, int currentTimeMs)
{
   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions:
      switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
      {
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallPass: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassManage:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassFinishMode:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassDistance:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassAssociations:
      return index != m_MenuOptions.m_CurrentBallIndex || (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallPassComplete:
      switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
      {
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select: return index != m_MenuOptions.m_CurrentCompleteIndex || (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
   default: return true;
   }
}

bool BallHistory::ShouldDrawTrainerBallFails(std::size_t index, int currentTimeMs)
{
   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions:
      switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
      {
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallFail: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailManage:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailFinishMode:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailDistance:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailAssociations:
      return index != m_MenuOptions.m_CurrentBallIndex || (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallFailComplete:
      switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
      {
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select: return index != m_MenuOptions.m_CurrentCompleteIndex || (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
   default: return true;
   }
}

bool BallHistory::ShouldDrawActiveBallKickers(int currentTimeMs)
{
   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions:
      switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
      {
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallKickerBehavior: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
      default: return true;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallKickerBehaviorMode: return (currentTimeMs % OneSecondMs) >= DrawBallBlinkMs;
   default: return true;
   }
}

static UINT compute_primitive_count(D3DPRIMITIVETYPE type, const int vertexCount)
{
   switch (type)
   {
   case D3DPRIMITIVETYPE::D3DPT_POINTLIST: return vertexCount;
   case D3DPRIMITIVETYPE::D3DPT_LINELIST: return vertexCount / 2;
   case D3DPRIMITIVETYPE::D3DPT_LINESTRIP: return std::max(0, vertexCount - 1);
   case D3DPRIMITIVETYPE::D3DPT_TRIANGLELIST: return vertexCount / 3;
   case D3DPRIMITIVETYPE::D3DPT_TRIANGLESTRIP:
   case D3DPRIMITIVETYPE::D3DPT_TRIANGLEFAN: return std::max(0, vertexCount - 2);
   default: return 0;
   }
}

void BallHistory::DrawTrainerBallCorridorPass(Player& player, const char* name, TrainerOptions::BallCorridorOptionsRecord& bcor, Vertex3Ds* overridePosition)
{
   Vertex3Ds* position = overridePosition;
   if (overridePosition == nullptr)
   {
      position = &bcor.m_PassPosition;
   }

   if (!position->IsZero())
   {

      float passBallRadius = GetDefaultBallRadius();
      float passWidth = passBallRadius * (bcor.m_PassRadiusPercent / 100.0f);

      DrawLine(
         player, name, { position->x - passWidth, position->y, position->z }, { position->x + passWidth, position->y, position->z }, m_TrainerBallCorridorPassColor, int(passBallRadius));

      POINT screenPoint = Get2DPointFrom3D(player, *position);
      PrintScreenRecord::Text(name, float(screenPoint.x), float(screenPoint.y), "P");
   }
}

void BallHistory::DrawTrainerBallCorridorOpeningLeft(Player& player, TrainerOptions::BallCorridorOptionsRecord& bcor)
{
   float passBallRadius = GetDefaultBallRadius();
   float passWidth = passBallRadius * (bcor.m_PassRadiusPercent / 100.0f);
   float openingBallRadius = passBallRadius / 2.0f;

   if (!bcor.m_OpeningPositionLeft.IsZero())
   {
      DrawFakeBall(player, "DrawTrainerBallCorridorOpeningLeft", bcor.m_OpeningPositionLeft, openingBallRadius, m_TrainerBallCorridorOpeningEndColor);
      POINT screenPoint = Get2DPointFrom3D(player, bcor.m_OpeningPositionLeft);
      PrintScreenRecord::Text(TrainerOptions::BallCorridorOptionsRecord::ImGuiDrawTrainerBallCorridorOpeningLeftLabel, float(screenPoint.x), float(screenPoint.y), "L");
   }

   if (!bcor.m_PassPosition.IsZero() && !bcor.m_OpeningPositionLeft.IsZero())
   {
      DrawLine(player, "BallCorridorOpeningLeftWall", { bcor.m_PassPosition.x - passWidth, bcor.m_PassPosition.y, bcor.m_PassPosition.z }, bcor.m_OpeningPositionLeft, Color::Red,
         int(passBallRadius));
   }
}

void BallHistory::DrawTrainerBallCorridorOpeningRight(Player& player, TrainerOptions::BallCorridorOptionsRecord& bcor)
{
   float passBallRadius = GetDefaultBallRadius();
   float passWidth = passBallRadius * (bcor.m_PassRadiusPercent / 100.0f);
   float openingBallRadius = passBallRadius / 2.0f;

   if (!bcor.m_OpeningPositionRight.IsZero())
   {
      DrawFakeBall(player, "DrawTrainerBallCorridorOpeningRight", bcor.m_OpeningPositionRight, openingBallRadius, m_TrainerBallCorridorOpeningEndColor);
      POINT screenPoint = Get2DPointFrom3D(player, bcor.m_OpeningPositionRight);
      PrintScreenRecord::Text(TrainerOptions::BallCorridorOptionsRecord::ImGuiDrawTrainerBallCorridorOpeningRightLabel, float(screenPoint.x), float(screenPoint.y), "R");
   }

   if (!bcor.m_PassPosition.IsZero() && !bcor.m_OpeningPositionRight.IsZero())
   {
      DrawLine(player, "BallCorridorOpeningRightWall", { bcor.m_PassPosition.x + passWidth, bcor.m_PassPosition.y, bcor.m_PassPosition.z }, bcor.m_OpeningPositionRight, Color::Red,
         int(passBallRadius));
   }
}

void BallHistory::DrawTrainerModeVisuals(Player& player, int currentTimeMs)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_DrawTrainerBallsUsec);

   float ballRadius = GetDefaultBallRadius();
   Matrix3 orientation = GetDefaultBallOrientation();

   for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
   {
      if (ShouldDrawTrainerBallStarts(bsorIndex, currentTimeMs))
      {
         TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
         if (!bsor.m_StartPosition.IsZero())
         {
            std::string ballStartFakeBallName = "BallStart" + std::to_string(bsorIndex);
            DrawFakeBall(player, ballStartFakeBallName.c_str(), bsor.m_StartPosition, ballRadius, m_TrainerBallStartColor);
            DrawAngleVelocityPreview(player, bsor);
            POINT screenPoint = Get2DPointFrom3D(player, bsor.m_StartPosition);
            PrintScreenRecord::Text(TrainerOptions::BallStartOptionsRecord::ImGuiBallStartLabels[bsorIndex], float(screenPoint.x), float(screenPoint.y), "S-%zu", bsorIndex + 1);
         }
      }
   }

   for (std::size_t bporIndex = 0; bporIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); bporIndex++)
   {
      if (ShouldDrawTrainerBallPasses(bporIndex, currentTimeMs))
      {
         TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[bporIndex];
         if (!bpor.m_EndPosition.IsZero())
         {
            std::string ballPassFakeBallName = "BallPass" + std::to_string(bporIndex);
            DrawFakeBall(player, ballPassFakeBallName.c_str(), bpor.m_EndPosition, ballRadius, m_TrainerBallPassColor);
            if (bpor.m_EndRadiusPercent != TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
            {
               std::string ballPassIntersectionCircleName = "BallPassIntersectionCircle" + std::to_string(bporIndex);
               DrawIntersectionCircle(player, ballPassIntersectionCircleName.c_str(), bpor.m_EndPosition, bpor.m_EndRadiusPercent, Color::Blue);
            }
            POINT screenPoint = Get2DPointFrom3D(player, bpor.m_EndPosition);
            PrintScreenRecord::Text(TrainerOptions::BallEndOptionsRecord::ImGuiBallPassLabels[bporIndex], float(screenPoint.x), float(screenPoint.y), "P-%zu", bporIndex + 1);
         }
      }
   }

   for (std::size_t bforIndex = 0; bforIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); bforIndex++)
   {
      if (ShouldDrawTrainerBallFails(bforIndex, currentTimeMs))
      {
         TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[bforIndex];
         if (!bfor.m_EndPosition.IsZero())
         {
            std::string ballFailFakeBallName = "BallFail" + std::to_string(bforIndex);
            DrawFakeBall(player, ballFailFakeBallName.c_str(), bfor.m_EndPosition, ballRadius, m_TrainerBallFailColor);
            if (bfor.m_EndRadiusPercent != TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
            {
               std::string ballFailIntersectionCircleName = "BallFailIntersectionCircle" + std::to_string(bforIndex);
               DrawIntersectionCircle(player, ballFailIntersectionCircleName.c_str(), bfor.m_EndPosition, bfor.m_EndRadiusPercent, Color::Blue);
            }
            POINT screenPoint = Get2DPointFrom3D(player, bfor.m_EndPosition);
            PrintScreenRecord::Text(TrainerOptions::BallEndOptionsRecord::ImGuiBallFailLabels[bforIndex], float(screenPoint.x), float(screenPoint.y), "F-%zu", bforIndex + 1);
         }
      }
   }

   DrawTrainerBallCorridor(player);

   if (ShouldDrawActiveBallKickers(currentTimeMs))
   {
      DrawActiveBallKickers(player);
   }
}

void BallHistory::DrawTrainerBallCorridor(Player& player)
{
   TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;

   DrawTrainerBallCorridorPass(player, "DrawTrainerBallCorridorPass", bcor);

   DrawTrainerBallCorridorOpeningLeft(player, bcor);

   DrawTrainerBallCorridorOpeningRight(player, bcor);
}

void BallHistory::DrawActiveBallKickers(Player& player)
{
   float ballRadius = GetDefaultBallRadius();
   for (std::size_t abkIndex = 0; abkIndex < m_ActiveBallKickers.size(); abkIndex++)
   {
      if (Kicker* kicker = m_ActiveBallKickers[abkIndex])
      {
         Vertex3Ds kickerPosition = GetKickerPosition(*kicker);
         POINT screenPoint = Get2DPointFrom3D(player, kickerPosition);

         std::string kickerText = "K-" + std::to_string(abkIndex + 1);

         int xMax = int(ImGui::GetIO().DisplaySize.x);
         int yMax = int(ImGui::GetIO().DisplaySize.y);

         if (screenPoint.x < 0)
         {
            if (screenPoint.y < 0)
            {
               kickerText = "<^ " + kickerText;
            }
            else if (screenPoint.y >= 0 && screenPoint.y < yMax)
            {
               kickerText = "<< " + kickerText;
            }
            else
            {
               kickerText = "<v " + kickerText;
            }
         }
         else if (screenPoint.x >= 0 && screenPoint.x < xMax)
         {
            if (screenPoint.y < 0)
            {
               kickerText = "^^ " + kickerText;
            }
            else if (screenPoint.y >= 0 && screenPoint.y < yMax)
            {
               // do nothing
            }
            else
            {
               kickerText = "vv " + kickerText;
            }
         }
         else
         {
            if (screenPoint.y < 0)
            {
               kickerText = kickerText + " ^>";
            }
            else if (screenPoint.y >= 0 && screenPoint.y < yMax)
            {
               kickerText = kickerText + " >>";
            }
            else
            {
               kickerText = kickerText + " v>";
            }
         }

         PrintScreenRecord::Text(ImGuiDrawActiveBallKickersLabels[abkIndex], float(screenPoint.x), float(screenPoint.y), kickerText.c_str());

         std::string activeBallKickerFakeBallName = "ActiveBallKicker" + std::to_string(abkIndex);
         DrawFakeBall(player, activeBallKickerFakeBallName.c_str(), kickerPosition, ballRadius, m_ActiveBallKickerColor);
      }
   }
}

void BallHistory::DrawAngleVelocityPreviewHelperAdd(Player& player, TrainerOptions::BallStartOptionsRecord& bsor, float angle, float velocity, float radius)
{
   float angleBefore
      = std::fabsf(std::fmodf(angle - (DrawAngleVelocityRadiusArc / 2.0f) + TrainerOptions::BallStartOptionsRecord::AngleMaximum, TrainerOptions::BallStartOptionsRecord::AngleMaximum));
   Vertex3Ds pointBefore = { bsor.m_StartPosition.x + (std::sinf((angleBefore * float(M_PI)) / 180.0f) * (radius + (velocity * 2.0f))),
      bsor.m_StartPosition.y + (std::cosf((angleBefore * float(M_PI)) / 180.0f) * -(radius + (velocity * 2.0f))), bsor.m_StartPosition.z + radius + DrawAngleVelocityHeightOffset };

   float angleAfter
      = std::fabsf(std::fmodf(angle + (DrawAngleVelocityRadiusArc / 2.0f) + TrainerOptions::BallStartOptionsRecord::AngleMaximum, TrainerOptions::BallStartOptionsRecord::AngleMaximum));
   Vertex3Ds pointAfter = { bsor.m_StartPosition.x + (std::sinf((angleAfter * float(M_PI)) / 180.0f) * (radius + (velocity * DrawAngleVelocityLengthMultiplier))),
      bsor.m_StartPosition.y + (std::cosf((angleAfter * float(M_PI)) / 180.0f) * -(radius + (velocity * DrawAngleVelocityLengthMultiplier))),
      bsor.m_StartPosition.z + radius + DrawAngleVelocityHeightOffset };

   std::string name = "AngleVelocityPreview_" + std::to_string(angle) + "_" + std::to_string(velocity);

   float velocityRange = TrainerOptions::BallStartOptionsRecord::VelocityMaximum - TrainerOptions::BallStartOptionsRecord::VelocityMinimum;
   unsigned char red = (unsigned char)(TrainerOptions::BallStartOptionsRecord::VelocityMinimum + (0xFF / float(velocityRange)) * velocity);
   unsigned char blue = 0xFF - red;
   DWORD color = (blue << 16) | (0x00 << 8) | red;

   DrawLine(player, name.c_str(), bsor.m_StartPosition, (pointBefore + pointAfter) / 2.0f, color, 10);
}

void BallHistory::DrawAngleVelocityPreviewHelper(Player& player, TrainerOptions::BallStartOptionsRecord& bsor, float angleStep, float velocityStep, float radius)
{
   for (S32 angleIndex = 0; angleIndex < bsor.m_TotalRangeAngles; angleIndex++)
   {
      float height = bsor.m_StartPosition.z;
      float angle = std::fmodf(bsor.m_AngleRangeStart + (angleStep * angleIndex), TrainerOptions::BallStartOptionsRecord::AngleMaximum);
      for (S32 velocityIndex = bsor.m_TotalRangeVelocities - 1; velocityIndex >= 0; velocityIndex--)
      {
         float velocity = std::fmodf(bsor.m_VelocityRangeStart + (velocityStep * velocityIndex), TrainerOptions::BallStartOptionsRecord::VelocityMaximum + 1);
         DrawAngleVelocityPreviewHelperAdd(player, bsor, angle, velocity, radius);
      }
   }
}

void BallHistory::DrawAngleVelocityPreview(Player& player, TrainerOptions::BallStartOptionsRecord& bsor)
{
   float ballRadius = GetDefaultBallRadius() + DrawAngleVelocityRadiusExtraMinimum;

   float angleStep = 0.0f;
   float velocityStep = 0.0f;
   CalculateAngleVelocityStep(bsor, angleStep, velocityStep);

   DrawAngleVelocityPreviewHelper(player, bsor, angleStep, velocityStep, ballRadius);
}

void BallHistory::CalculateAngleVelocityStep(TrainerOptions::BallStartOptionsRecord& bsor, float& angleStep, float& velocityStep)
{
   if (bsor.m_TotalRangeAngles == 1)
   {
      angleStep = 0.0f;
   }
   else
   {
      float angleRange = bsor.m_AngleRangeFinish - bsor.m_AngleRangeStart;
      if (angleRange > 0.0f)
      {
         angleStep = angleRange / (bsor.m_TotalRangeAngles - 1);
      }
      else if (angleRange == 0.0f)
      {
         angleStep = TrainerOptions::BallStartOptionsRecord::AngleMaximum / float(bsor.m_TotalRangeAngles);
      }
      else
      {
         angleRange = TrainerOptions::BallStartOptionsRecord::AngleMaximum - bsor.m_AngleRangeStart + bsor.m_AngleRangeFinish;
         angleStep = angleRange / (bsor.m_TotalRangeAngles - 1);
      }
   }

   if (bsor.m_VelocityRangeFinish == bsor.m_VelocityRangeStart || bsor.m_TotalRangeVelocities == 1)
   {
      velocityStep = 0.0f;
   }
   else
   {
      float velocityRange = bsor.m_VelocityRangeFinish - bsor.m_VelocityRangeStart;
      if (velocityRange < 0.0f)
      {
         velocityRange = (TrainerOptions::BallStartOptionsRecord::VelocityMaximum)-bsor.m_VelocityRangeStart + bsor.m_VelocityRangeFinish;
      }
      velocityStep = velocityRange / (bsor.m_TotalRangeVelocities - 1);
   }
}

void BallHistory::UpdateBallState(BallHistoryRecord& ballHistoryRecord)
{
   for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
   {
      m_ControlVBalls[controlVBallIndex]->m_d.m_pos = ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_Position;
      if (!m_Control)
      {
         m_ControlVBalls[controlVBallIndex]->m_d.m_vel = ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_Velocity;
         m_ControlVBalls[controlVBallIndex]->m_angularmomentum = ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_AngularMomentum;
         m_ControlVBalls[controlVBallIndex]->m_lastEventPos = ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_LastEventPos;
         m_ControlVBalls[controlVBallIndex]->m_orientation = ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_Orientation;
         memcpy(m_ControlVBalls[controlVBallIndex]->m_oldpos, ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_OldPos, sizeof(m_ControlVBalls[controlVBallIndex]->m_oldpos));
         m_ControlVBalls[controlVBallIndex]->m_ringcounter_oldpos = ballHistoryRecord.m_BallHistoryStates[controlVBallIndex].m_RingCounter_OldPos;
      }
   }
}

void BallHistory::ShowStatus(Player& player, int currentTimeMs)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_ShowStatusUsec);
   U64 showStatusUsec = usec();

   if (!m_ShowStatus)
   {
      return;
   }

   POINT mousePosition2D = { 0 };

   std::vector<std::pair<std::string, std::string>> statuses;

   statuses.push_back({ "BALL HISTORY", "" });
   statuses.push_back({ "Mouse Positions", "" });

   Get2DMousePosition(player, mousePosition2D);
   statuses.push_back({ "2D\n(Mse->Scr)", std::format("{}x\n{}y", mousePosition2D.x, mousePosition2D.y) });

   Vertex3Ds mousePosition3D = Get3DPointFromMousePosition(player, GetDefaultBallRadius());
   statuses.push_back({ "3D\n(Scr->3D)", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z) });
   POINT mousePosition2DFrom3D = Get2DPointFrom3D(player, mousePosition3D);
   statuses.push_back({ "2D\n(3D->Scr)", std::format("{}x\n{}y", mousePosition2DFrom3D.x, mousePosition2DFrom3D.y) });

   statuses.push_back({ "Render Times", "" });
   statuses.push_back({ "Process", std::format("{:.1f}ms", m_ProfilerRecord.m_ProcessUsec / 1000.0f) });
   statuses.push_back({ "Show Status", std::format("{:.1f}ms", m_ProfilerRecord.m_ShowStatusUsec / 1000.0f) });
   statuses.push_back({ "Process Menu", std::format("{:.1f}ms", m_ProfilerRecord.m_ProcessMenuUsec / 1000.0f) });
   statuses.push_back({ "Draw Ball\nHistory", std::format("{:.1f}ms", m_ProfilerRecord.m_DrawBallHistoryUsec / 1000.0f) });
   statuses.push_back({ "Process Mode\nNormal", std::format("{:.1f}ms", m_ProfilerRecord.m_ProcessModeNormalUsec / 1000.0f) });
   statuses.push_back({ "Process Mode\nTrainer", std::format("{:.1f}ms", m_ProfilerRecord.m_ProcessModeTrainerUsec / 1000.0f) });
   statuses.push_back({ "Draw Trainer\nBalls", std::format("{:.1f}ms", m_ProfilerRecord.m_DrawTrainerBallsUsec / 1000.0f) });

   m_ProfilerRecord.SetZero();

   switch (m_MenuOptions.m_ModeType)
   {
   case MenuOptionsRecord::ModeType::ModeType_Normal:
      statuses.push_back({ "Normal Options", "" });
      statuses.push_back({ "Control", std::format("{}", m_Control ? "true" : "false") });
      statuses.push_back({ "Current Control\nIndex", std::format("{}", m_CurrentControlIndex) });

      statuses.push_back({ "BH Records\nHeadIndex", std::format("{}", m_BallHistoryRecordsHeadIndex) });
      statuses.push_back({ "BH Records\nSize (Size)", std::format("{}", m_BallHistoryRecords.size()) });
      statuses.push_back({ "BH Records\nSize (Max)", std::format("{}", m_BallHistoryRecordsSize) });
      if (m_BallHistoryRecordsSize)
      {
         statuses.push_back({ "Balls", "" });
         statuses.push_back({ "Total", std::format("{}", m_BallHistoryRecordsSize ? m_BallHistoryRecords[m_CurrentControlIndex].m_BallHistoryStates.size() : 0u) });
         BallHistoryRecord& ballHistoryRecord = m_BallHistoryRecords[m_CurrentControlIndex];
         for (std::size_t ballHistoryStateIndex = 0; ballHistoryStateIndex < ballHistoryRecord.m_BallHistoryStates.size(); ++ballHistoryStateIndex)
         {
            BallHistoryState& ballHistoryState = ballHistoryRecord.m_BallHistoryStates[ballHistoryStateIndex];
            statuses.push_back({ std::format("({})-Position", ballHistoryStateIndex + 1),
               std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", ballHistoryState.m_Position.x, ballHistoryState.m_Position.y, ballHistoryState.m_Position.z) });
            statuses.push_back({ std::format("({})-Velocity", ballHistoryStateIndex + 1),
               std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", ballHistoryState.m_Velocity.x, ballHistoryState.m_Velocity.y, ballHistoryState.m_Velocity.z) });
            statuses.push_back({ std::format("({})-Momentum", ballHistoryStateIndex + 1),
               std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", ballHistoryState.m_AngularMomentum.x, ballHistoryState.m_AngularMomentum.y, ballHistoryState.m_AngularMomentum.z) });
         }
      }

      statuses.push_back({ "Auto Control\nPoints (ACPs)", "" });
      statuses.push_back({ "Total", std::format("{}", m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size()) });
      for (std::size_t autoControlVerticesIndex = 0; autoControlVerticesIndex < m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size(); ++autoControlVerticesIndex)
      {
         NormalOptions::AutoControlVertex& autoControlVertex = m_MenuOptions.m_NormalOptions.m_AutoControlVertices[autoControlVerticesIndex];
         statuses.push_back({ std::format("({})-Position", autoControlVerticesIndex + 1),
            std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", autoControlVertex.m_Position.x, autoControlVertex.m_Position.y, autoControlVertex.m_Position.z) });
         statuses.push_back({ std::format("({})-Active", autoControlVerticesIndex + 1), std::format("{}", autoControlVertex.Active ? "true" : "false") });
      }

      statuses.push_back({ "Recall Control", "" });
      if (m_MenuOptions.m_NormalOptions.m_RecallControlIndex == NormalOptions::RecallControlIndexDisabled)
      {
         statuses.push_back({ "Index", "<disabled>" });
      }
      else
      {
         statuses.push_back({ "Index", std::format("{}", m_MenuOptions.m_NormalOptions.m_RecallControlIndex) });
      }

      statuses.push_back({ "Navigation", "" });
      statuses.push_back({ "Ball Velocity\nPixels (Max)", std::format("{:.1f}wu", m_MaxBallVelocityPixels) });

      switch (m_NextPreviousBy)
      {
      case NextPreviousByType::eTimeMs: statuses.push_back({ "Next Previous By", "TimeMs" }); break;
      case NextPreviousByType::eDistancePixels: statuses.push_back({ "Next Previous By", "DistancePixel" }); break;
      default: statuses.push_back({ "Next Previous By", "**UNKNOWN**" }); break;
      };

      statuses.push_back({ "BH Control\nStep Ms", std::format("{}ms", m_BallHistoryControlStepMs) });
      statuses.push_back({ "BH Control\nStep Pixels", std::format("{:.1f}wu", m_BallHistoryControlStepPixels) });
      break;
   case MenuOptionsRecord::ModeType::ModeType_Trainer:
   {
      statuses.push_back({ "Trainer Options", "" });

      if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.size() == 0)
      {
         statuses.push_back({ "(None)", " " });
      }
      else
      {
         for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.size(); bsorIndex++)
         {
            TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
            statuses.push_back({ std::format("Ball Start {}", bsorIndex + 1), "" });
            if (bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               statuses.push_back({ "Status", "Enabled" });
            }
            else
            {
               statuses.push_back({ "Status", "Frozen" });
            }

            statuses.push_back({ "Position", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bsor.m_StartPosition.x, bsor.m_StartPosition.y, bsor.m_StartPosition.z) });
            statuses.push_back({ "Velocity", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bsor.m_StartVelocity.x, bsor.m_StartVelocity.y, bsor.m_StartVelocity.z) });
            statuses.push_back({ "Momentum", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bsor.m_StartAngularMomentum.x, bsor.m_StartAngularMomentum.y, bsor.m_StartAngularMomentum.z) });
            statuses.push_back({ "Velocity Start", std::format("{:.0f}", bsor.m_VelocityRangeStart) });
            statuses.push_back({ "Velocity Finish", std::format("{:.0f}", bsor.m_VelocityRangeFinish) });
            statuses.push_back({ "Velocity Total", std::format("{}", bsor.m_TotalRangeVelocities) });
            statuses.push_back({ "Angle Start", std::format("{:.0f}", bsor.m_AngleRangeStart) });
            statuses.push_back({ "Angle Finish", std::format("{:.0f}", bsor.m_AngleRangeFinish) });
            statuses.push_back({ "Angle Total", std::format("{}", bsor.m_TotalRangeAngles) });
         }
      }

      if (m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size() == 0)
      {
         statuses.push_back({ "Ball Pass N/A", "" });
         statuses.push_back({ " ", " " });
      }
      else
      {
         for (std::size_t bporIndex = 0; bporIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); bporIndex++)
         {
            TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[bporIndex];
            statuses.push_back({ std::format("Ball Pass {}", bporIndex + 1), "" });
            statuses.push_back({ "Position", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bpor.m_EndPosition.x, bpor.m_EndPosition.y, bpor.m_EndPosition.z) });

            if (bpor.m_EndRadiusPercent == 0.0f)
            {
               statuses.push_back({ "Finish Mode", "<Not Set>" });
            }
            else if (bpor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
            {
               statuses.push_back({ "Finish Mode", "Stop" });
            }
            else
            {
               statuses.push_back({ "Finish Mode", std::format("Distance\n{}%", bpor.m_EndRadiusPercent) });
            }

            for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
            {
               HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];
               float distance = DistancePixels(bpor.m_EndPosition, controlVBall.m_d.m_pos);
               statuses.push_back({ std::format("Distance Ball {}", controlVBallIndex + 1), std::format("{:.1f}wu", distance) });
            }

            if (bpor.m_AssociatedBallStartIndexes.size() == 0)
            {
               statuses.push_back({ "Associations", "<None>\n(Ignored!)" });
            }
            else
            {
               std::string associatedBallStartIndexesOutput;
               for (std::size_t associatedBallStartIndex : bpor.m_AssociatedBallStartIndexes)
               {
                  associatedBallStartIndexesOutput += std::to_string(associatedBallStartIndex + 1) + ",";
               }
               associatedBallStartIndexesOutput = associatedBallStartIndexesOutput.substr(0, associatedBallStartIndexesOutput.length() - 1);
               statuses.push_back({ "Associations", std::format("{}", associatedBallStartIndexesOutput.c_str()) });
            }

            if (bpor.m_StopBallsTracker.size() == 0)
            {
               statuses.push_back({ "Stop Ball", "<None>" });
            }
            else
            {
               for (std::size_t stopBallTrackerIndex = 0; stopBallTrackerIndex < bpor.m_StopBallsTracker.size(); stopBallTrackerIndex++)
               {
                  int stopBallMs = std::get<0>(bpor.m_StopBallsTracker[stopBallTrackerIndex]);
                  Vertex3Ds& stopBallPos = std::get<1>(bpor.m_StopBallsTracker[stopBallTrackerIndex]);
                  statuses.push_back(
                     { std::format("Stop Ball {}", stopBallTrackerIndex + 1), std::format("{}ms,{:.1f}x,{:.1f}y,{:.1f}z", stopBallMs, stopBallPos.x, stopBallPos.y, stopBallPos.z) });
               }
            }
         }
      }

      if (m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size() == 0)
      {
         statuses.push_back({ "Ball Fail N/A", "" });
         statuses.push_back({ " ", " " });
      }
      else
      {
         for (std::size_t bforIndex = 0; bforIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); bforIndex++)
         {
            TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[bforIndex];
            statuses.push_back({ std::format("Ball Fail {}", bforIndex + 1), "" });
            statuses.push_back({ "Position", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bfor.m_EndPosition.x, bfor.m_EndPosition.y, bfor.m_EndPosition.z) });

            if (bfor.m_EndRadiusPercent == 0.0f)
            {
               statuses.push_back({ "Finish Mode", "<Not Set>" });
            }
            else if (bfor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
            {
               statuses.push_back({ "Finish Mode", "Stop" });
            }
            else
            {
               statuses.push_back({ "Finish Mode", std::format("Distance\n{}", bfor.m_EndRadiusPercent) });
               for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
               {
                  HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];
                  float distance = DistancePixels(bfor.m_EndPosition, controlVBall.m_d.m_pos);
                  statuses.push_back({ std::format("Distance Ball {}", controlVBallIndex + 1), std::format("{:.1f}", distance) });
               }
            }

            for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
            {
               HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];
               float distance = DistancePixels(bfor.m_EndPosition, controlVBall.m_d.m_pos);
               statuses.push_back({ std::format("Distance Ball {}", controlVBallIndex + 1), std::format("{:.1f}wu", distance) });
            }

            if (bfor.m_AssociatedBallStartIndexes.size() == 0)
            {
               statuses.push_back({ "Associations", "**None - Ball End Ignored**" });
            }
            else
            {
               std::string associatedBallStartIndexesOutput;
               for (std::size_t associatedBallStartIndex : bfor.m_AssociatedBallStartIndexes)
               {
                  associatedBallStartIndexesOutput += std::to_string(associatedBallStartIndex + 1) + ",";
               }
               associatedBallStartIndexesOutput = associatedBallStartIndexesOutput.substr(0, associatedBallStartIndexesOutput.length() - 1);
               statuses.push_back({ "Associations", std::format("{}", associatedBallStartIndexesOutput.c_str()) });
            }

            if (bfor.m_StopBallsTracker.size() == 0)
            {
               statuses.push_back({ "Stop Ball", "<None>" });
            }
            else
            {
               for (std::size_t stopBallTrackerIndex = 0; stopBallTrackerIndex < bfor.m_StopBallsTracker.size(); stopBallTrackerIndex++)
               {
                  int stopBallMs = std::get<0>(bfor.m_StopBallsTracker[stopBallTrackerIndex]);
                  Vertex3Ds& stopBallPos = std::get<1>(bfor.m_StopBallsTracker[stopBallTrackerIndex]);
                  statuses.push_back(
                     { std::format("Stop Ball {}", stopBallTrackerIndex + 1), std::format("{}ms,{:.1f}x,{:.1f}y,{:.1f}z", stopBallMs, stopBallPos.x, stopBallPos.y, stopBallPos.z) });
               }
            }
         }
      }

      TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;
      statuses.push_back({ "Ball Corridor (BC)", "" });
      statuses.push_back({ "Pass", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bcor.m_PassPosition.x, bcor.m_PassPosition.y, bcor.m_PassPosition.z) });
      statuses.push_back({ "Pass Radius", std::format("{}%", bcor.m_PassRadiusPercent) });
      statuses.push_back({ "Opening Left", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bcor.m_OpeningPositionLeft.x, bcor.m_OpeningPositionLeft.y, bcor.m_OpeningPositionLeft.z) });
      statuses.push_back({ "Opening Right", std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", bcor.m_OpeningPositionRight.x, bcor.m_OpeningPositionRight.y, bcor.m_OpeningPositionRight.z) });

      Vertex3Ds passPosition1 = bcor.m_PassPosition - Vertex3Ds(GetDefaultBallRadius(), 0.0f, 0.0f);
      Vertex3Ds passPosition2 = bcor.m_PassPosition + Vertex3Ds(GetDefaultBallRadius(), 0.0f, 0.0f);
      statuses.push_back({ "Distance\nPassL<->PassR", std::format("{:.1f}wu", DistanceToLineSegment(passPosition1, passPosition2, mousePosition3D)) });
      if (bcor.m_OpeningPositionLeft.x < bcor.m_OpeningPositionRight.x)
      {
         statuses.push_back({ "Distance\nPassL<->OpeningL", std::format("{:.1f}wu", DistanceToLineSegment(passPosition1, bcor.m_OpeningPositionLeft, mousePosition3D)) });
         statuses.push_back({ "Distance\nPassR<->OpeningR", std::format("{:.1f}wu", DistanceToLineSegment(passPosition2, bcor.m_OpeningPositionRight, mousePosition3D)) });
      }
      else
      {
         statuses.push_back({ "Distance\nPassL<->OpeningR", std::format("{:.1f}wu", DistanceToLineSegment(passPosition1, bcor.m_OpeningPositionRight, mousePosition3D)) });
         statuses.push_back({ "Distance\nPassR<->OpeningL", std::format("{:.1f}wu", DistanceToLineSegment(passPosition2, bcor.m_OpeningPositionLeft, mousePosition3D)) });
      }

      statuses.push_back({ "Difficulty", "" });
      statuses.push_back({ "Gameplay", std::format("{}", m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty) });
      statuses.push_back({ "Gameplay\nInitial", std::format("{}", m_MenuOptions.m_TrainerOptions.m_GameplayDifficultyInitial) });
      statuses.push_back({ "Variance", std::format("{}", m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty) });

      ShowDifficultyVarianceStatusAll(player, statuses, true);

      statuses.push_back({ "Run Options", "" });
      statuses.push_back({ "Total", std::format("{}", m_MenuOptions.m_TrainerOptions.m_TotalRuns) });
      switch (m_MenuOptions.m_TrainerOptions.m_RunOrderMode)
      {
      case TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder: statuses.push_back({ "Run Order Mode", "In Order" }); break;
      case TrainerOptions::RunOrderModeType::RunOrderModeType_Random: statuses.push_back({ "Run Order Mode", "Random" }); break;
      default: statuses.push_back({ "Order Mode", "**UNKNOWN**" }); break;
      }

      statuses.push_back({ "Time", std::format("{}s", m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun) });
      statuses.push_back({ "Countdown", std::format("{}s", m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun) });

      switch (m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode)
      {
      case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset: statuses.push_back({ "Kicker Behavior", "Reset" }); break;
      case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail: statuses.push_back({ "Kicker Behavior", "Fail" }); break;
      default: statuses.push_back({ "Kicker Behavior", "**UNKNOWN**" }); break;
      }

      statuses.push_back({ "Sound Effects", "" });
      statuses.push_back({ "Pass", std::format("({})", m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled ? "X" : "O") });
      statuses.push_back({ "Fail", std::format("({})", m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled ? "X" : "O") });
      statuses.push_back({ "Time Low", std::format("({})", m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled ? "X" : "O") });
      statuses.push_back({ "Countdown", std::format("({})", m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled ? "X" : "O") });
      break;
   }
   case MenuOptionsRecord::ModeType::ModeType_Disabled: statuses.push_back({ "Mode", "Disabled" }); break;
   default: InvalidEnumValue("MenuOptionsRecord::ModeType", m_MenuOptions.m_ModeType); break;
   }

   statuses.push_back({ "Active Ball\nKickers (ABKs)", "" });
   statuses.push_back({ "Total", std::format("{}", m_ActiveBallKickers.size()) });
   for (std::size_t activeBallKickerIndex = 0; activeBallKickerIndex < m_ActiveBallKickers.size(); activeBallKickerIndex++)
   {
      if (Kicker* kicker = m_ActiveBallKickers[activeBallKickerIndex])
      {
         Vertex3Ds kickerPosition = GetKickerPosition(*kicker);
         statuses.push_back({ std::format("ABK {}", activeBallKickerIndex + 1), std::format("{:.1f}x\n{:.1f}y\n{:.1f}z", kickerPosition.x, kickerPosition.y, kickerPosition.z) });
      }
   }

   statuses.push_back({ "Flippers", "" });
   statuses.push_back({ "Total", std::format("{}", m_Flippers.size()) });
   for (std::size_t flipperIndex = 0; flipperIndex < m_Flippers.size(); flipperIndex++)
   {
      if (Flipper* flipper = m_Flippers[flipperIndex])
      {
         statuses.push_back({ std::format("Flipper {}", flipperIndex + 1), std::format("{}", flipper->GetName()) });
      }
   }

   PrintScreenRecord::Status(statuses);
}

void BallHistory::ShowRecallBall(Player& player)
{
   if (m_MenuOptions.m_NormalOptions.m_RecallControlIndex == NormalOptions::RecallControlIndexDisabled)
   {
      PrintScreenRecord::MenuText(false, "Recall Control Index = <disabled>");
   }
   else
   {
      PrintScreenRecord::MenuText(false, "Recall Control Index = %zu", m_MenuOptions.m_NormalOptions.m_RecallControlIndex);
      BallHistoryRecord& recallBallHistoryRecord = m_BallHistoryRecords[m_MenuOptions.m_NormalOptions.m_RecallControlIndex];
      for (auto& recallBallHistoryState : recallBallHistoryRecord.m_BallHistoryStates)
      {
         POINT screenPoint = Get2DPointFrom3D(player, recallBallHistoryState.m_Position);
         PrintScreenRecord::MenuText(false, "Recall Ball %.2f,%.2f,%.2f,%ld,%ld (3x,3y,3z,2x,2y)", recallBallHistoryState.m_Position.x, recallBallHistoryState.m_Position.y,
            recallBallHistoryState.m_Position.z, screenPoint.x, screenPoint.y);
      }
   }
}

void BallHistory::ShowAutoControlVertices(Player& player)
{
   PrintScreenRecord::MenuText(false, "Auto Control Locations (ACLs)");
   if (m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size())
   {
      for (std::size_t autoControlVerticesIndex = 0; autoControlVerticesIndex < m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size(); ++autoControlVerticesIndex)
      {
         NormalOptions::AutoControlVertex& autoControlVertex = m_MenuOptions.m_NormalOptions.m_AutoControlVertices[autoControlVerticesIndex];
         POINT screenPoint = Get2DPointFrom3D(player, autoControlVertex.m_Position);
         PrintScreenRecord::MenuText(false, "ACL %zu %.2f,%.2f,%.2f,%ld,%ld,%s (3x,3y,3z,2x,2y,active)", autoControlVerticesIndex + 1, autoControlVertex.m_Position.x,
            autoControlVertex.m_Position.y, autoControlVertex.m_Position.z, screenPoint.x, screenPoint.y, autoControlVertex.Active ? "true" : "false");
      }
   }
   else
   {
      PrintScreenRecord::MenuText(false, "<None>");
   }
}

void BallHistory::ShowRemainingRunInfo()
{
   std::vector<std::pair<std::string, std::string>> printScreenTexts;

   std::size_t runsRemaining = m_MenuOptions.m_TrainerOptions.m_RunRecords.size() - m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord;
   float runsRemainingPercent = runsRemaining / float(m_MenuOptions.m_TrainerOptions.m_RunRecords.size());

   printScreenTexts.push_back({ "Remaining", "" });
   printScreenTexts.push_back({ "Runs", std::format("{} ({:.2f}%)", runsRemaining, runsRemainingPercent * 100) });

   DWORD totalRunsMs = 0;
   for (std::size_t x = 0; x < m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord; x++)
   {
      TrainerOptions::RunRecord& rr = m_MenuOptions.m_TrainerOptions.m_RunRecords[x];
      totalRunsMs += rr.m_TotalTimeMs;
   }

   if (runsRemaining && m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord > 0)
   {
      TrainerOptions::RunRecord& rr = m_MenuOptions.m_TrainerOptions.m_RunRecords[m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord];
      float averageRunMs = totalRunsMs / float(m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord);
      float approximateRemainingMs = averageRunMs * runsRemaining;
      printScreenTexts.push_back({ "Time", std::format("{:.2f}s", approximateRemainingMs / 1000.0f) });
   }
   else
   {
      printScreenTexts.push_back({ "Time", "N/A" });
   }
   
   PrintScreenRecord::ActiveMenu(printScreenTexts);
}

void BallHistory::ShowPreviousRunRecord()
{
   std::vector<std::pair<std::string, std::string>> printScreenTexts;

   if (m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord > 0)
   {
      TrainerOptions::RunRecord& previousRunRecord = m_MenuOptions.m_TrainerOptions.m_RunRecords[m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord - 1];
      printScreenTexts.push_back({ "Previous", "" });
      printScreenTexts.push_back({ "Run #", std::format("{}", m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord) });

      switch (previousRunRecord.m_Result)
      {
      case TrainerOptions::RunRecord::ResultType::ResultType_PassedLocation:
      {
         std::stringstream startToPassLocationIndexes;
         for (const std::tuple<std::size_t, std::size_t>& startToPassLocationIndex : previousRunRecord.m_StartToPassLocationIndexes)
         {
            startToPassLocationIndexes << "Start #" << std::get<0>(startToPassLocationIndex) + 1 << "->Pass #" << std::get<1>(startToPassLocationIndex) + 1 << std::endl;
         }

         printScreenTexts.push_back({ "Result", "Pass" });
         printScreenTexts.push_back({ "Reason", startToPassLocationIndexes.str() });
      }
      break;
      case TrainerOptions::RunRecord::ResultType::ResultType_FailedLocation:
      {
         std::stringstream startToFailLocationIndexes;
         for (const std::tuple<std::size_t, std::size_t>& startToFailLocationIndex : previousRunRecord.m_StartToFailLocationIndexes)
         {
            startToFailLocationIndexes << "Start #" << std::get<0>(startToFailLocationIndex) + 1 << "->Fail #" << std::get<1>(startToFailLocationIndex) + 1 << std::endl;
         }

         printScreenTexts.push_back({ "Result", "Fail" });
         printScreenTexts.push_back({ "Reason", startToFailLocationIndexes.str() });
      }
      break;
      case TrainerOptions::RunRecord::ResultType::ResultType_PassedCorridor:
         printScreenTexts.push_back({ "Result", "Pass" });
         printScreenTexts.push_back({ "Reason", std::format("Start #{}->\nPass Corridor", previousRunRecord.m_StartToPassCorridorIndex + 1) });
         break;
      case TrainerOptions::RunRecord::ResultType::ResultType_FailedCorridorLeft:
         printScreenTexts.push_back({ "Result", "Fail" });
         printScreenTexts.push_back({ "Reason", std::format("Start #{}->\nFail Corridor Left", previousRunRecord.m_StartToFailCorridorIndex + 1) });
         break;
      case TrainerOptions::RunRecord::ResultType::ResultType_FailedCorridorRight:
         printScreenTexts.push_back({ "Result", "Fail" });
         printScreenTexts.push_back({ "Reason", std::format("Start #{}->\nFail Corridor Right", previousRunRecord.m_StartToFailCorridorIndex + 1) });
         break;
      case TrainerOptions::RunRecord::ResultType::ResultType_FailedTimeElapsed:
         printScreenTexts.push_back({ "Result", "Fail" });
         printScreenTexts.push_back({ "Reason", "Time Elapsed" });
         break;
      case TrainerOptions::RunRecord::ResultType::ResultType_FailedKicker:
         printScreenTexts.push_back({ "Result", "Fail" });
         printScreenTexts.push_back({ "Reason", "Kicker" });
         break;
      default: InvalidEnumValue("TrainerOptions::RunRecord::ResultType", previousRunRecord.m_Result); break;
      }
      printScreenTexts.push_back({ "Length", std::format("{:.2f}s", float(previousRunRecord.m_TotalTimeMs) / 1000) });

   }
   else
   {
      printScreenTexts.push_back({ "Previous", "" });
      printScreenTexts.push_back({ "Run #", "N/A" });
      printScreenTexts.push_back({ "Result", "N/A" });
      printScreenTexts.push_back({ "Reason", "N/A" });
      printScreenTexts.push_back({ "Length", "N/A" });
   }

   PrintScreenRecord::ActiveMenu(printScreenTexts);
}

void BallHistory::ShowCurrentRunRecord(int currentTimeMs)
{
   std::vector<std::pair<std::string, std::string>> printScreenTexts;

   std::size_t runsRemaining = m_MenuOptions.m_TrainerOptions.m_RunRecords.size() - m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord;
   float runsRemainingPercent = runsRemaining / float(m_MenuOptions.m_TrainerOptions.m_RunRecords.size());

   printScreenTexts.push_back({ "Current", "" });
   if (runsRemaining)
   {
      printScreenTexts.push_back({ "Run #",
         std::format("{} of {}", std::to_string(m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord + 1), m_MenuOptions.m_TrainerOptions.m_RunRecords.size()) });

      DWORD totalMsPerRun = (m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun + m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun) * OneSecondMs;
      DWORD runElapsedTimeMs = currentTimeMs - m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs;
      printScreenTexts.push_back({ "Time",
         std::format("{:.2f}s",
            std::min(float(m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun),
               (totalMsPerRun - runElapsedTimeMs) / float(OneSecondMs))) });
   }
   else
   {
      printScreenTexts.push_back({ "Run #",
         std::format("{} of {}", "N/A", m_MenuOptions.m_TrainerOptions.m_RunRecords.size()) });
      printScreenTexts.push_back({ "Time", "N/A" });
   }

   PrintScreenRecord::ActiveMenu(printScreenTexts);
}

void BallHistory::ShowBallStartOptionsRecord(TrainerOptions::BallStartOptionsRecord& bsor)
{
   PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", bsor.m_StartPosition.x, bsor.m_StartPosition.y, bsor.m_StartPosition.z);
   PrintScreenRecord::MenuText(false, "Velocity = %.2f,%.2f,%.2f (x,y,z)", bsor.m_StartVelocity.x, bsor.m_StartVelocity.y, bsor.m_StartVelocity.z);
   PrintScreenRecord::MenuText(false, "Momentum = %.2f,%.2f,%.2f (x,y,z)", bsor.m_StartAngularMomentum.x, bsor.m_StartAngularMomentum.y, bsor.m_StartAngularMomentum.z);
   PrintScreenRecord::MenuText(false, "VelocOps = %.2f,%.2f,%u (start,finish,total)", bsor.m_VelocityRangeStart, bsor.m_VelocityRangeFinish, bsor.m_TotalRangeVelocities);
   PrintScreenRecord::MenuText(false, "AngleOps = %.2f,%.2f,%u (start,finish,total)", bsor.m_AngleRangeStart, bsor.m_AngleRangeFinish, bsor.m_TotalRangeAngles);
}

void BallHistory::ShowBallEndOptionsRecord(TrainerOptions::BallEndOptionsRecord& beor)
{
   PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", beor.m_EndPosition.x, beor.m_EndPosition.y, beor.m_EndPosition.z);

   if (beor.m_EndRadiusPercent == 0.0f)
   {
      PrintScreenRecord::MenuText(false, "Finish Mode = <Not Set>");
   }
   else if (beor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
   {
      PrintScreenRecord::MenuText(false, "Finish Mode = Stop");
   }
   else
   {
      PrintScreenRecord::MenuText(false, "Finish Mode = Distance %.0f%%", beor.m_EndRadiusPercent);
   }

   for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
      float distance = DistancePixels(beor.m_EndPosition, bsor.m_StartPosition);
      PrintScreenRecord::MenuText(false, "Distance To Start %zu = %.2f", bsorIndex + 1, distance);
   }

   if (beor.m_AssociatedBallStartIndexes.size() == 0)
   {
      PrintScreenRecord::MenuText(false, "  Associations = **None - Ball End Ignored**");
   }
   else
   {
      std::string associatedBallStartIndexesOutput;
      for (std::size_t associatedBallStartIndex : beor.m_AssociatedBallStartIndexes)
      {
         associatedBallStartIndexesOutput += std::to_string(associatedBallStartIndex + 1) + ",";
      }
      associatedBallStartIndexesOutput = associatedBallStartIndexesOutput.substr(0, associatedBallStartIndexesOutput.length() - 1);
      PrintScreenRecord::MenuText(false, "Associations = %s", associatedBallStartIndexesOutput.c_str());
   }

   if (beor.m_StopBallsTracker.size() == 0)
   {
      PrintScreenRecord::MenuText(false, "Stop Ball <None>");
   }
   else
   {
      for (std::size_t stopBallTrackerIndex = 0; stopBallTrackerIndex < beor.m_StopBallsTracker.size(); stopBallTrackerIndex++)
      {
         int stopBallMs = std::get<0>(beor.m_StopBallsTracker[stopBallTrackerIndex]);
         Vertex3Ds& stopBallPos = std::get<1>(beor.m_StopBallsTracker[stopBallTrackerIndex]);
         PrintScreenRecord::MenuText(false, "Stop Ball %zu (ms,x,y,z) = %d,%.2f,%.2f,%.2f", stopBallTrackerIndex + 1, stopBallMs, stopBallPos.x, stopBallPos.y, stopBallPos.z);
      }
   }
}

void BallHistory::ShowBallCorridorOptionsRecord(TrainerOptions::BallCorridorOptionsRecord& bcor)
{
   PrintScreenRecord::MenuText(false, "Pass = %.2f,%.2f,%.2f (x,y,z)", bcor.m_PassPosition.x, bcor.m_PassPosition.y, bcor.m_PassPosition.z);
   PrintScreenRecord::MenuText(false, "Pass Radius = %.0f%%", bcor.m_PassRadiusPercent);
   PrintScreenRecord::MenuText(false, "Opening Left = %.2f,%.2f,%.2f (x,y,z)", bcor.m_OpeningPositionLeft.x, bcor.m_OpeningPositionLeft.y, bcor.m_OpeningPositionLeft.z);
   PrintScreenRecord::MenuText(false, "Opening Right = %.2f,%.2f,%.2f (x,y,z)", bcor.m_OpeningPositionRight.x, bcor.m_OpeningPositionRight.y, bcor.m_OpeningPositionRight.z);
}

void BallHistory::ShowDifficultyTableConstants(Player& player)
{
   PrintScreenRecord::MenuTitleText("Table Constants");
   PrintScreenRecord::MenuText(false, "Override Physics = %s", player.m_ptable->m_overridePhysics ? "true" : "false");
   PrintScreenRecord::MenuText(false, "Global Flipper Scatter Angle = %.4f", 0.0f);
   PrintScreenRecord::MenuText(false, "Global Playfield Scatter = %.4f", player.m_ptable->m_fOverrideScatterAngle);
   PrintScreenRecord::MenuText(false, "Global Default Elements Scatter = %.4f", c_hardScatter);
   for (size_t i = 0; i < player.m_ptable->m_vedit.size(); i++)
   {
      IEditable* const pedit = player.m_ptable->m_vedit[i];
      if (pedit->GetItemType() == eItemFlipper)
      {
         Flipper* const pflipper = static_cast<Flipper* const>(pedit);
         PhysicsSet physicsSet = PhysicsSet::Disable;
         pflipper->get_OverridePhysics(&physicsSet);
         PrintScreenRecord::MenuText(false, "Override Flipper Physics (%s) = %s", pflipper->GetName(), physicsSet == PhysicsSet::Disable ? "false" : "true");
      }
   }
   float scatter = 0.0f;
   player.m_ptable->get_Scatter(&scatter);
   PrintScreenRecord::MenuText(false, "Table Playfield Scatter Angle = %.4f", scatter);
   float defaultScatter = 0.0f;
   player.m_ptable->get_DefaultScatter(&defaultScatter);
   PrintScreenRecord::MenuText(false, "Table Default Elements Scatter = %.4f", defaultScatter);
}

void BallHistory::ShowSection(const char* title, const std::vector<std::string>& descriptions)
{
   PrintScreenRecord::MenuText(false, "");
   PrintScreenRecord::MenuTitleText(title);
   for (std::string desc : descriptions)
   {
      PrintScreenRecord::MenuText(false, desc.c_str());
   }
}
void BallHistory::ShowDifficultyVarianceStatusesMenu(std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses)
{
   for (auto& status : difficultyVarianceStatuses)
   {
      PrintScreenRecord::MenuText(false, "%s = %s", status.first.c_str(), status.second.c_str());
   }
}

void BallHistory::ShowDifficultyVarianceMode(const std::string& difficultyVarianceName, TrainerOptions::DifficultyVarianceModeType varianceMode,
   std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   std::string separator = newlines ? "\n" : " ";
   switch (varianceMode)
   {
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + separator + "Variance Mode", "Above +" + separator + "Below" });
      break;
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + separator + "Variance Mode", "Above" + separator + "Only" });
      break;
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + separator + "Variance Mode", "Below" + separator + "Only" });
      break;
   default:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + separator + "Variance Mode", "**UNKNOWN**" });
      InvalidEnumValue("TrainerModeOptions::DifficultyVarianceModeType", varianceMode);
      break;
   }
}

void BallHistory::ShowDifficultyVarianceRange(const std::string& difficultyVarianceName, TrainerOptions::DifficultyVarianceModeType varianceMode, S32 variance, float initial,
   std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   std::string nameSeparator = newlines ? "\n" : " ";
   std::string valueSeparator = newlines ? "\n" : ",";
   float varianceDiff = initial * (variance / 100.0f);
   float varianceAbove = initial + varianceDiff;
   float varianceBelow = initial - varianceDiff;
   switch (varianceMode)
   {
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + nameSeparator + "Range", std::format("[{:.3f}{}{:.3f}]", varianceBelow, valueSeparator, varianceAbove) });
      break;
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + nameSeparator + "Range", std::format("[{:.3f}{}{:.3f}]", initial, valueSeparator, varianceAbove) });
      break;
   case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + nameSeparator + "Range", std::format("[{:.3f}{}{:.3f}]", varianceBelow, valueSeparator, initial) });
      break;
   default:
      difficultyVarianceStatuses.push_back({ difficultyVarianceName + nameSeparator + "Range", "**UNKNOWN**" });
      InvalidEnumValue("TrainerModeOptions::DifficultyVarianceModeType", varianceMode);
      break;
   }
}

void BallHistory::ShowDifficultyVarianceStatusAll(Player& player, std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   ShowDifficultyVarianceStatusGravity(player, difficultyVarianceStatuses, newlines);
   ShowDifficultyVarianceStatusPlayfieldFriction(player, difficultyVarianceStatuses, newlines);
   ShowDifficultyVarianceStatusFlipperStrength(player, difficultyVarianceStatuses, newlines);
   ShowDifficultyVarianceStatusFlipperFriction(player, difficultyVarianceStatuses, newlines);
}

void BallHistory::ShowDifficultyVarianceStatusSingle(const std::string& name, float current, S32 variance, float initial, TrainerOptions::DifficultyVarianceModeType mode,
   std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   std::string separator = newlines ? "\n" : " ";
   difficultyVarianceStatuses.push_back({ name + separator + "Variance", std::format("{}%", variance) });
   ShowDifficultyVarianceMode(name, mode, difficultyVarianceStatuses, newlines);
   ShowDifficultyVarianceRange(name, mode, variance, initial, difficultyVarianceStatuses, newlines);
   difficultyVarianceStatuses.push_back({ name + separator + "Current", std::format("{:.3f}", current) });
   difficultyVarianceStatuses.push_back({ name + separator + "Initial", std::format("{:.3f}", initial) });
}

void BallHistory::ShowDifficultyVarianceStatusGravity(Player& player, std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   float gravityCurrent = 0.0f;
   player.m_ptable->get_Gravity(&gravityCurrent);
   ShowDifficultyVarianceStatusSingle("Gravity", gravityCurrent, m_MenuOptions.m_TrainerOptions.m_GravityVariance, m_MenuOptions.m_TrainerOptions.m_GravityInitial,
      m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode, difficultyVarianceStatuses, newlines);
}

void BallHistory::ShowDifficultyVarianceStatusPlayfieldFriction(Player& player, std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   float playfieldFrictionCurrent = 0.0f;
   player.m_ptable->get_Friction(&playfieldFrictionCurrent);
   ShowDifficultyVarianceStatusSingle("Playfield Friction", playfieldFrictionCurrent, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance,
      m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionInitial, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode, difficultyVarianceStatuses, newlines);
}

void BallHistory::ShowDifficultyVarianceStatusFlipperStrength(Player& player, std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   float flipperStrengthCurrent = GetFlipperStrength();
   ShowDifficultyVarianceStatusSingle("Flipper Strength", flipperStrengthCurrent, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance,
      m_MenuOptions.m_TrainerOptions.m_FlipperStrengthInitial, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode, difficultyVarianceStatuses, newlines);
}

void BallHistory::ShowDifficultyVarianceStatusFlipperFriction(Player& player, std::vector<std::pair<std::string, std::string>>& difficultyVarianceStatuses, bool newlines)
{
   float flipperFrictionCurrent = GetFlipperFriction();
   ShowDifficultyVarianceStatusSingle("Flipper Friction", flipperFrictionCurrent, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance,
      m_MenuOptions.m_TrainerOptions.m_FlipperFrictionInitial, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode, difficultyVarianceStatuses, newlines);
}

void BallHistory::ShowResult(std::size_t total, std::vector<DWORD>& timesMs, const char* type, const char* subType, std::vector<std::pair<std::string, std::string>>& results)
{
   std::size_t currentRunCount = m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord;

   results.push_back({
      std::format("{} {} Total", type, subType),
      std::format("{}", total),
   });
   if (currentRunCount)
   {
      results.push_back({ std::format("{} {} Percent", type, subType), std::format("{:.2f}%", float(total) / currentRunCount * 100.0f) });
   }
   else
   {
      results.push_back({ std::format("{} {} Percent", type, subType), "N/A" });
   }

   std::size_t totalMs = std::accumulate(timesMs.begin(), timesMs.end(), 0);
   if (totalMs > 0)
   {
      if (total)
      {
         results.push_back({ std::format("{} {} Average Time", type, subType), std::format("{:.2f}", float(totalMs) / total / 1000.0f) });
      }
      else
      {
         results.push_back({ std::format("{} {} Average Time", type, subType), "N/A" });
      }

      if (total)
      {
         results.push_back({ std::format("{} {} StdDev Time", type, subType), std::format("{:.2f}", CalculateStandardDeviation(timesMs) / 1000.f) });
      }
      else
      {
         results.push_back({ std::format("{} {} StdDev Time", type, subType), "N/A" });
      }
   }
}

template <class T> float BallHistory::CalculateStandardDeviation(std::vector<T>& values)
{
   if (values.size() == 0)
   {
      return 0.0f;
   }

   float mean = std::accumulate(values.begin(), values.end(), 0.0f) / float(values.size());
   float variance = 0.0f;
   for (T value : values)
   {
      variance += (value - mean) * (value - mean);
   }
   variance /= values.size();
   return std::sqrtf(variance);
}

float BallHistory::CalculateDifficultyVariance(Player& player, float initial, float current, S32 variance, TrainerOptions::DifficultyVarianceModeType varianceMode)
{
   if (variance > 0)
   {
      float direction = 0.0f;
      float min = initial;
      float max = initial;
      float diff = initial * variance / 100.0f;
      switch (varianceMode)
      {
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
         direction = rand_mt_m11() >= 0.0f ? 0.5f : -0.5f;
         min -= diff;
         max += diff;
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
         direction = 1.0f;
         max += diff;
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
         direction = -1.0f;
         min -= diff;
         break;
      default: InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", varianceMode); break;
      }

      float range = max - min;

      current += (float(m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty) / TrainerOptions::VarianceDifficultyMaximum) * range * direction;
      current = std::max(min, std::min(current, max));
      return current;
   }
   return current;
}

void BallHistory::InitBallStartOptionRecords()
{
   BallHistoryRecord& ballHistoryRecord = m_BallHistoryRecords[m_CurrentControlIndex];
   m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize = 0;
   for (std::size_t ballHistoryStateIndex = 0; ballHistoryStateIndex < ballHistoryRecord.m_BallHistoryStates.size(); ++ballHistoryStateIndex)
   {
      BallHistoryState& ballHistoryState = ballHistoryRecord.m_BallHistoryStates[ballHistoryStateIndex];

      PrintScreenRecord::MenuText(
         false, "Ball %zu Pos = %.2f,%.2f,%.2f (x,y,z)", ballHistoryStateIndex + 1, ballHistoryState.m_Position.x, ballHistoryState.m_Position.y, ballHistoryState.m_Position.z);
      PrintScreenRecord::MenuText(
         false, "Ball %zu Vel = %.2f,%.2f,%.2f (x,y,z)", ballHistoryStateIndex + 1, ballHistoryState.m_Velocity.x, ballHistoryState.m_Velocity.y, ballHistoryState.m_Velocity.z);
      PrintScreenRecord::MenuText(false, "Ball %zu Mom = %.2f,%.2f,%.2f (x,y,z)", ballHistoryStateIndex + 1, ballHistoryState.m_AngularMomentum.x, ballHistoryState.m_AngularMomentum.y,
         ballHistoryState.m_AngularMomentum.z);

      TrainerOptions::BallStartOptionsRecord bsor(ballHistoryState.m_Position, ballHistoryState.m_Velocity, ballHistoryState.m_AngularMomentum,
         TrainerOptions::BallStartOptionsRecord::AngleMinimum, TrainerOptions::BallStartOptionsRecord::AngleMinimum, 0, TrainerOptions::BallStartOptionsRecord::VelocityMinimum,
         TrainerOptions::BallStartOptionsRecord::VelocityMinimum, 0);

      if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize == m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.size())
      {
         m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords.push_back(bsor);
      }
      else
      {
         m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize] = bsor;
      }
      m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize++;
   }
}

// yes i know this is a huuuuge function, so sue me
// in my defense, this is menu/ui handling and a huge switch statement, which is kinda like many functions
void BallHistory::ProcessMenu(Player& player, MenuOptionsRecord::MenuActionType menuAction, int currentTimeMs)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_ProcessMenuUsec);

   ClearDraws(player);

   if (!m_MenuOptions.m_MenuError.empty())
   {
      PrintScreenRecord::ErrorText("%s", m_MenuOptions.m_MenuError.c_str());
   }

   POINT mousePosition2D = { 0 };
   Get2DMousePosition(player, mousePosition2D);
   Vertex3Ds mousePosition3D = Get3DPointFromMousePosition(player, GetDefaultBallRadius());

   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Root_SelectMode:
      PrintScreenRecord::MenuTitleText("Ball History Mode");
      PrintScreenRecord::MenuText(m_MenuOptions.m_ModeType == MenuOptionsRecord::ModeType::ModeType_Normal, "Normal");
      PrintScreenRecord::MenuText(m_MenuOptions.m_ModeType == MenuOptionsRecord::ModeType::ModeType_Trainer, "Trainer");
      PrintScreenRecord::MenuText(m_MenuOptions.m_ModeType == MenuOptionsRecord::ModeType::ModeType_Disabled, "Disabled");

      switch (m_MenuOptions.m_ModeType)
      {
      case MenuOptionsRecord::ModeType::ModeType_Normal:
         ShowSection(DescriptionSectionTitle, { "Use plunger to configure 'Normal' options", "Navigate backward/forward through Ball History", "Setup recall and auto control locations" });
         break;
      case MenuOptionsRecord::ModeType::ModeType_Trainer:
         ShowSection(DescriptionSectionTitle,
            {
               "Use plunger to configure 'Trainer' options",
               "Setup training runs for practice and improving skill",
            });
         break;
      case MenuOptionsRecord::ModeType::ModeType_Disabled: ShowSection(DescriptionSectionTitle, { "Disable Ball History" }); break;
      default: InvalidEnumValue("MenuOptionsRecord::ModeType", m_MenuOptions.m_ModeType); break;
      }

      ShowSection(SummarySectionTitle,
         { "Welcome to Ball History"
           "Press Ball History Menu key '"
               + std::string(1, GetBallHistoryKey(player, eBallHistoryMenu)) + "' to show this menu",
            "Press Ball History Recall key '" + std::string(1, GetBallHistoryKey(player, eBallHistoryRecall)) + "' to show verbose status",
            "Use plunger and flippers to select/configure options" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<MenuOptionsRecord::ModeType>(m_MenuOptions.m_ModeType, 0, MenuOptionsRecord::ModeType::ModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<MenuOptionsRecord::ModeType>(m_MenuOptions.m_ModeType, 0, MenuOptionsRecord::ModeType::ModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_ModeType)
         {
         case MenuOptionsRecord::ModeType::ModeType_Normal: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions; break;
         case MenuOptionsRecord::ModeType::ModeType_Trainer: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectModeOptions; break;
         case MenuOptionsRecord::ModeType::ModeType_Disabled:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Disabled_Disabled;
            Init(player, currentTimeMs, false);
            ToggleControl();
            break;
         default: InvalidEnumValue("MenuOptionsRecord::ModeType", m_MenuOptions.m_ModeType); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions:
      PrintScreenRecord::MenuTitleText("Normal Mode Options");
      PrintScreenRecord::MenuText(m_MenuOptions.m_NormalOptions.m_ModeState == NormalOptions::ModeStateType::ModeStateType_SelectCurrentBallHistory, "Select Current Ball History");
      PrintScreenRecord::MenuText(m_MenuOptions.m_NormalOptions.m_ModeState == NormalOptions::ModeStateType::ModeStateType_SelectRecallBallHistory, "Select Recall Ball History");
      PrintScreenRecord::MenuText(m_MenuOptions.m_NormalOptions.m_ModeState == NormalOptions::ModeStateType::ModeStateType_ManageAutoControlLocations, "Manage Auto Control Locations");
      PrintScreenRecord::MenuText(m_MenuOptions.m_NormalOptions.m_ModeState == NormalOptions::ModeStateType::ModeStateType_ClearAutoControlLocations, "Clear Auto Control Locations");
      PrintScreenRecord::MenuText(m_MenuOptions.m_NormalOptions.m_ModeState == NormalOptions::ModeStateType::ModeStateType_GoBack, "Go Back");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowRecallBall(player);
      ShowAutoControlVertices(player);

      switch (m_MenuOptions.m_NormalOptions.m_ModeState)
      {
      case NormalOptions::ModeStateType::ModeStateType_SelectCurrentBallHistory:
         ShowSection(DescriptionSectionTitle,
            { "Allows navigation backward/forward through Ball History",
               "Ball History is reset when any of the following happen:", "ball created, ball destroyed, any ball hits any kicker" });
         break;
      case NormalOptions::ModeStateType::ModeStateType_SelectRecallBallHistory:
         ShowSection(DescriptionSectionTitle,
            { "Configure Recall Ball along the Ball History",
               "Press Ball History Recall key '" + std::string(1, GetBallHistoryKey(player, eBallHistoryRecall)) + "' to recall balls back to predefined position" });
         break;
      case NormalOptions::ModeStateType::ModeStateType_ManageAutoControlLocations:
         ShowSection(DescriptionSectionTitle, { "Configure Auto Control Locations (ACLs)", "Ball Control activates automatically when any ball intersects an ACL" });
         break;
      case NormalOptions::ModeStateType::ModeStateType_ClearAutoControlLocations: ShowSection(DescriptionSectionTitle, { "Clear existing Auto Control Locations" }); break;
      case NormalOptions::ModeStateType::ModeStateType_GoBack: ShowSection(DescriptionSectionTitle, { "Go back to previous menu" }); break;
      default: InvalidEnumValue("NormalOptions::ModeStateType", m_MenuOptions.m_NormalOptions.m_ModeState); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<NormalOptions::ModeStateType>(m_MenuOptions.m_NormalOptions.m_ModeState, 0, NormalOptions::ModeStateType::ModeStateType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<NormalOptions::ModeStateType>(m_MenuOptions.m_NormalOptions.m_ModeState, 0, NormalOptions::ModeStateType::ModeStateType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_NormalOptions.m_ModeState)
         {
         case NormalOptions::ModeStateType::ModeStateType_SelectCurrentBallHistory:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectCurrentBallHistory;
            break;
         case NormalOptions::ModeStateType::ModeStateType_SelectRecallBallHistory:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ConfigureRecallBallHistory;
            break;
         case NormalOptions::ModeStateType::ModeStateType_ManageAutoControlLocations:
            CenterMouse(player);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ManageAutoControlLocations;
            break;
         case NormalOptions::ModeStateType::ModeStateType_ClearAutoControlLocations:
            m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode = NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_GoBack;
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ClearAutoControlLocations;
            break;
         case NormalOptions::ModeStateType::ModeStateType_GoBack:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Root_SelectMode;
            m_MenuOptions.m_NormalOptions.m_ModeState = NormalOptions::ModeStateType::ModeStateType_SelectCurrentBallHistory;
            break;
         default: InvalidEnumValue("NormalOptions::ModeStateType", m_MenuOptions.m_NormalOptions.m_ModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectCurrentBallHistory:
      PrintScreenRecord::MenuTitleText("Select Current Ball History");
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowAutoControlVertices(player);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to navigate backward/forward through current Ball History",
            ("Press Ball History Menu key '" + std::string(1, GetBallHistoryKey(player, eBallHistoryMenu)) + "' to continue play").c_str(),
            "Plunger accepts and resets ball positions to selected Ball History location" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
      {
         std::size_t controlCount = 1;
         if ((currentTimeMs - m_MenuOptions.m_SkipControlUsedMs) < MenuOptionsRecord::SkipControlIntervalMs)
         {
            controlCount = MenuOptionsRecord::SkipControlStepFactor;
         }
         for (std::size_t x = 0; x < controlCount; x++)
         {
            switch (menuAction)
            {
            case MenuOptionsRecord::MenuActionType::MenuActionType_None:
            case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
               // do nothing;
               break;
            case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ControlPrev(); break;
            case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ControlNext(); break;
            case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
               // do nothing
               break;
            default: InvalidEnumValue("NormalOptions::ModeStateType", m_MenuOptions.m_NormalOptions.m_ModeState); break;
            }
         }
         m_MenuOptions.m_SkipControlUsedMs = currentTimeMs;
      }
      break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ConfigureRecallBallHistory:
      PrintScreenRecord::MenuTitleText("Setup Recall Ball History");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode == NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Select, "Select");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode == NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Disable, "Disable");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowAutoControlVertices(player);

      switch (m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode)
      {
      case NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Select:
         ShowSection(DescriptionSectionTitle, { "Setup and select Recall position", "Plunger accepts configuration" });
         break;
      case NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Disable:
         ShowSection(DescriptionSectionTitle, { "Disable Recall position", "Plunger accepts configuration" });
         break;
      default: InvalidEnumValue("NormalOptions::ConfigureRecallBallHistoryModeType", m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<NormalOptions::ConfigureRecallBallHistoryModeType>(
            m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode, 0, NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<NormalOptions::ConfigureRecallBallHistoryModeType>(
            m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode, 0, NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode)
         {
         case NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Select:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectRecallBallHistory;
            break;
         case NormalOptions::ConfigureRecallBallHistoryModeType::ConfigureRecallBallHistoryModeType_Disable:
            m_MenuOptions.m_NormalOptions.m_RecallControlIndex = NormalOptions::RecallControlIndexDisabled;
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions;
            break;
         default: InvalidEnumValue("NormalOptions::ConfigureRecallBallHistoryModeType", m_MenuOptions.m_NormalOptions.m_ConfigureRecallBallHistoryMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectRecallBallHistory:
      PrintScreenRecord::MenuTitleText("Select Recall Ball History");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowAutoControlVertices(player);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to navigate backward/forward through current Ball History",
            ("Press Ball History Menu key '" + std::string(1, GetBallHistoryKey(player, eBallHistoryMenu)) + "' to continue play").c_str(),
            "Plunger accepts sets Recall position to selected Ball History location" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
      {
         std::size_t controlCount = 1;
         if ((currentTimeMs - m_MenuOptions.m_SkipControlUsedMs) < MenuOptionsRecord::SkipControlIntervalMs)
         {
            controlCount = MenuOptionsRecord::SkipControlStepFactor;
         }
         for (std::size_t x = 0; x < controlCount; x++)
         {
            switch (menuAction)
            {
            case MenuOptionsRecord::MenuActionType::MenuActionType_None:
            case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
               // do nothing;
               break;
            case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ControlPrev(); break;
            case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ControlNext(); break;
            case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
               // do nothing
               break;
            default: InvalidEnumValue("NormalOptions::ModeStateType", m_MenuOptions.m_NormalOptions.m_ModeState); break;
            }
         }
         m_MenuOptions.m_SkipControlUsedMs = currentTimeMs;
      }
      break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_NormalOptions.m_RecallControlIndex = m_CurrentControlIndex;
         m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ManageAutoControlLocations:
   {
      PrintScreenRecord::MenuTitleText("Manage Auto Control Locations");

      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      PrintScreenRecord::MenuText(false, "Point %zu %.2f,%.2f,%.2f,%ld,%ld (3x,3y,3z,2x,2y)", m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size() + 1, mousePosition3D.x,
         mousePosition3D.y, mousePosition3D.z, mousePosition2D.x, mousePosition2D.y);

      float ballRadius = GetDefaultBallRadius();
      S32 heightMinimum = S32(ballRadius);
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight - ballRadius);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Ball Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowAutoControlVertices(player);

      ShowSection(DescriptionSectionTitle,
         { "Use mouse (move/click) to manage Auto Control Locations (ACLs)", "Use flippers to set height when creating new ACL", "Create new ACL by clicking mouse in empty area",
            "Delete existing ACL by clicking existing (blinking) ACL", "Plunger returns to previous menu" });

      DrawFakeBall(player, "ManageAutoControlLocations", mousePosition3D, m_AutoControlBallColor, nullptr, 0, 0);

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, S32(heightMinimum), heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
      {
         bool removedExisting = false;

         for (std::vector<NormalOptions::AutoControlVertex>::iterator autoControlVerticesIt = m_MenuOptions.m_NormalOptions.m_AutoControlVertices.begin();
            autoControlVerticesIt < m_MenuOptions.m_NormalOptions.m_AutoControlVertices.end(); autoControlVerticesIt++)
         {
            POINT screenPoint = Get2DPointFrom3D(player, autoControlVerticesIt->m_Position);
            float checkRadius = std::min(g_pplayer->m_playfieldWnd->GetWidth(), g_pplayer->m_playfieldWnd->GetHeight()) * NormalOptions::ManageAutoControlFindFactor;
            if (DistancePixels(screenPoint, mousePosition2D) < checkRadius)
            {
               m_MenuOptions.m_NormalOptions.m_AutoControlVertices.erase(autoControlVerticesIt);
               removedExisting = true;
               break;
            }
         }
         if (!removedExisting && m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size() < NormalOptions::AutoControlVerticesMax)
         {
            m_MenuOptions.m_NormalOptions.m_AutoControlVertices.push_back({ mousePosition3D, false });
         }
      }
      break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ClearAutoControlLocations:
   {
      PrintScreenRecord::MenuTitleText("Clear Auto Control Locations");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode == NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_Clear, "Clear");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode == NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_GoBack, "Go Back");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowAutoControlVertices(player);

      switch (m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode)
      {
      case NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_Clear:
         ShowSection(DescriptionSectionTitle,
            {
               "Plunger clears all existing Auto Control Locations (ACLs)",
            });
         break;
      case NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_GoBack:
         ShowSection(DescriptionSectionTitle,
            {
               "Go back to previous menu",
            });
         break;
      default: InvalidEnumValue("NormalOptions::ClearAutoControlLocationsModeType", m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<NormalOptions::ClearAutoControlLocationsModeType>(
            m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode, 0, NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<NormalOptions::ClearAutoControlLocationsModeType>(
            m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode, 0, NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode)
         {
         case NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_Clear:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions;
            m_MenuOptions.m_NormalOptions.m_AutoControlVertices.clear();
            break;
         case NormalOptions::ClearAutoControlLocationsModeType::ClearAutoControlLocationsModeType_GoBack:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions;
            break;
         default: InvalidEnumValue("NormalOptions::ClearAutoControlLocationsModeType", m_MenuOptions.m_NormalOptions.m_ClearAutoControlLocationsMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectModeOptions:
      PrintScreenRecord::MenuTitleText("Trainer Mode Options");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ModeState == TrainerOptions::ModeStateType::ModeStateType_Config, "Config");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ModeState == TrainerOptions::ModeStateType::ModeStateType_Start, "Start");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ModeState == TrainerOptions::ModeStateType::ModeStateType_Resume, "Resume");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ModeState == TrainerOptions::ModeStateType::ModeStateType_Results, "Results");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ModeState == TrainerOptions::ModeStateType::ModeStateType_GoBack, "Go Back");

      switch (m_MenuOptions.m_TrainerOptions.m_ModeState)
      {
      case TrainerOptions::ModeStateType::ModeStateType_Config: ShowSection(DescriptionSectionTitle, { "Configure options for training" }); break;
      case TrainerOptions::ModeStateType::ModeStateType_Start: ShowSection(DescriptionSectionTitle, { "Start training session" }); break;
      case TrainerOptions::ModeStateType::ModeStateType_Resume: ShowSection(DescriptionSectionTitle, { "Resume previously paused training session" }); break;
      case TrainerOptions::ModeStateType::ModeStateType_Results: ShowSection(DescriptionSectionTitle, { "Show results for current training session" }); break;
      case TrainerOptions::ModeStateType::ModeStateType_GoBack: ShowSection(DescriptionSectionTitle, { "Go back to previous menu" }); break;
      default: InvalidEnumValue("TrainerOptions::ModeStateType", m_MenuOptions.m_TrainerOptions.m_ModeState); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::ModeStateType>(m_MenuOptions.m_TrainerOptions.m_ModeState, 0, TrainerOptions::ModeStateType::ModeStateType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::ModeStateType>(m_MenuOptions.m_TrainerOptions.m_ModeState, 0, TrainerOptions::ModeStateType::ModeStateType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_ModeState)
         {
         case TrainerOptions::ModeStateType::ModeStateType_Start:
            if (BallsReadyForTrainer())
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results;

               ToggleControl();

               m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord = 0;
               m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
               m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
               m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
               m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
            }
            break;
         case TrainerOptions::ModeStateType::ModeStateType_Resume:
            if (BallsReadyForTrainer())
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results;

               ToggleControl();

               m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
               m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
               m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
            }
            break;
         case TrainerOptions::ModeStateType::ModeStateType_Results:
            if (BallsReadyForTrainer())
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results;
            }
            break;
         case TrainerOptions::ModeStateType::ModeStateType_Config: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions; break;
         case TrainerOptions::ModeStateType::ModeStateType_GoBack:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Root_SelectMode;
            m_MenuOptions.m_TrainerOptions.m_ModeState = TrainerOptions::ModeStateType::ModeStateType_Config;
            break;
         default: InvalidEnumValue("TrainerOptions::ModeStateType", m_MenuOptions.m_TrainerOptions.m_ModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions:
   {
      PrintScreenRecord::MenuTitleText("Config Mode Options");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard, "Wizard");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallStart, "Ball Start");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallPass, "Ball Pass");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallFail, "Ball Fail");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallCorridor, "Ball Corridor");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_Difficulty, "Difficulty");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_TotalRuns, "Total Runs");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_RunOrder, "Run Order");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallKickerBehavior, "Ball Kicker Behavior");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_MaxSecondsPerRun, "Time Per Run");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_CountdownSecondsBeforeRun, "Countdown Before Run");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_SoundEffects, "Sound Effects");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_ConfigModeState == TrainerOptions::ConfigModeStateType::ConfigModeStateType_GoBack, "Go Back");

      PrintScreenRecord::MenuText(false, "");
      switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
      {
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard: ShowSection(DescriptionSectionTitle, { "Configure all settings in order" }); break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallStart:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
         {
            for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
            {
               PrintScreenRecord::MenuText(false, "Ball Start %zu", bsorIndex + 1);
               ShowBallStartOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex]);
            }
         }
         else
         {
            PrintScreenRecord::MenuText(false, "Ball Starts <None>");
         }

         ShowSection(DescriptionSectionTitle,
            { "Configure behavior of balls when a training run starts", "Start position, velocity and angle can be configured per ball",
               "Multiple velocities and angles can be configured per ball" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallPass:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         if (!m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size())
         {
            PrintScreenRecord::MenuText(false, "Ball Passes <None>");
         }
         for (std::size_t beorIndex = 0; beorIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); beorIndex++)
         {
            PrintScreenRecord::MenuText(false, "Ball Pass %zu", beorIndex + 1);
            ShowBallEndOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[beorIndex]);
         }

         ShowSection(DescriptionSectionTitle,
            { "Configure requirements for Ball Starts in order to 'pass' a training run", "Training run will 'pass' when all Ball Passes satisfy all Ball Starts",
               "Distance from a position and velocity can be configured per Ball Pass", "Specific Ball Starts can be associated with Ball Passes" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallFail:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         if (!m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size())
         {
            PrintScreenRecord::MenuText(false, "Ball Fails <None>");
         }
         for (std::size_t beorIndex = 0; beorIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); beorIndex++)
         {
            PrintScreenRecord::MenuText(false, "Ball Fail %zu", beorIndex + 1);
            ShowBallEndOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[beorIndex]);
         }

         ShowSection(DescriptionSectionTitle,
            { "Configure requirements for Ball Starts in order to 'fail' a training run", "Training run will 'fail' when any ball satisfies any Ball Fail",
               "Distance from a position and velocity can be configured per Ball Fail", "Specific Ball Starts can be associated with Ball Fails" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallCorridor:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         ShowBallCorridorOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord);

         ShowSection(DescriptionSectionTitle,
            { "Configure requirements for Ball Corridor", "Creates a Corridor with three walls: Pass, Fail Left and Fail Right",
               "Training run will 'pass' when any ball intersects the Pass wall", "Training run will 'fail' when any ball intersects any Fail wall" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Difficulty:
      {
         PrintScreenRecord::MenuTitleText("Current Configuration");

         std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;

         difficultyVarianceStatuses.push_back({ "Gameplay Difficulty", std::format("{}", m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty) });
         difficultyVarianceStatuses.push_back({ "Gameplay Difficulty Initial", std::format("{}", m_MenuOptions.m_TrainerOptions.m_GameplayDifficultyInitial) });
         difficultyVarianceStatuses.push_back({ "Variance Difficulty", std::format("{}", m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty) });

         ShowDifficultyVarianceStatusAll(player, difficultyVarianceStatuses, false);
         ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

         ShowSection(DescriptionSectionTitle,
            { "Configure Difficulty and Variance", "Controls scatter (difficulty) lowering randomness creating consistent play",
               "Varies gravity, playfield friction, flipper strength and flipper friction", "Increase skills of adaptation to real-world variances" });

         PrintScreenRecord::MenuText(false, "");
         ShowDifficultyTableConstants(player);
      }
      break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_TotalRuns:
      {
         bool anyCustom = false;
         std::size_t totalPermutations = m_MenuOptions.m_TrainerOptions.m_TotalRuns;
         for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
         {
            TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
            if (bsor.m_TotalRangeAngles > 0)
            {
               anyCustom = true;
               totalPermutations *= bsor.m_TotalRangeAngles;
            }
            if (bsor.m_TotalRangeVelocities > 0)
            {
               anyCustom = true;
               totalPermutations *= bsor.m_TotalRangeVelocities;
            }
         }

         PrintScreenRecord::MenuTitleText("Current Configuration");
         PrintScreenRecord::MenuText(false, "Total Runs = %d", m_MenuOptions.m_TrainerOptions.m_TotalRuns);
         if (anyCustom)
         {
            PrintScreenRecord::MenuText(false, "Total Permutations = %d", totalPermutations);
         }
      }

         ShowSection(DescriptionSectionTitle, { "Configure total number of training runs", "A training run is one pass/fail result" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_RunOrder:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         switch (m_MenuOptions.m_TrainerOptions.m_RunOrderMode)
         {
         case TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder: PrintScreenRecord::MenuText(false, "Run Order = In Order"); break;
         case TrainerOptions::RunOrderModeType::RunOrderModeType_Random: PrintScreenRecord::MenuText(false, "Run Order = Random"); break;
         default: PrintScreenRecord::MenuText(false, "Run Order = **UNKNOWN**"); break;
         }

         ShowSection(DescriptionSectionTitle, { "Configure order of training runs", "For when Ball Starts have multiple velocity/angle configurations" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallKickerBehavior:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         switch (m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode)
         {
         case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset: PrintScreenRecord::MenuText(false, "Ball Kicker Behavior = Reset"); break;
         case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail: PrintScreenRecord::MenuText(false, "Ball Kicker Behavior = Fail"); break;
         default: PrintScreenRecord::MenuText(false, "Ball Kicker Behavior = **UNKNOWN**"); break;
         }

         ShowSection(DescriptionSectionTitle,
            { "Configure ball kicker behavior", "Kickers often change table state causing inconsistent training runs",
               "Instead of triggering a kicker, the training run will 'Reset' or 'Fail'" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_MaxSecondsPerRun:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         PrintScreenRecord::MenuText(false, "Time Per Run = %d (seconds)", m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun);

         ShowSection(
            DescriptionSectionTitle, { "Configure maxmimum allowed time, in seconds, to 'pass' a training run", "A 'pass' state is when all Ball Passes are satisfied by Ball Starts" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_CountdownSecondsBeforeRun:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         PrintScreenRecord::MenuText(false, "Countdown Before Run = %d (seconds)", m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun);

         ShowSection(DescriptionSectionTitle, { "Configure time, in seconds, before the next training run starts", "Countdown is indicated with visual and audible effects" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_SoundEffects:
         PrintScreenRecord::MenuTitleText("Current Configuration");
         PrintScreenRecord::MenuText(false, "(%s) Pass", m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled ? "X" : "O");
         PrintScreenRecord::MenuText(false, "(%s) Fail", m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled ? "X" : "O");
         PrintScreenRecord::MenuText(false, "(%s) Time Low", m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled ? "X" : "O");
         PrintScreenRecord::MenuText(false, "(%s) Countdown", m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled ? "X" : "O");

         ShowSection(DescriptionSectionTitle, { "Configure various sounds effects" });
         break;
      case TrainerOptions::ConfigModeStateType::ConfigModeStateType_GoBack: ShowSection(DescriptionSectionTitle, { "Go back to previous menu" }); break;
      default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::ConfigModeStateType>(
            m_MenuOptions.m_TrainerOptions.m_ConfigModeState, 0, TrainerOptions::ConfigModeStateType::ConfigModeStateType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::ConfigModeStateType>(
            m_MenuOptions.m_TrainerOptions.m_ConfigModeState, 0, TrainerOptions::ConfigModeStateType::ConfigModeStateType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
         {
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallStartMode; break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallStart:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallStartMode;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallPass:
            if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallPassComplete;
            }
            else
            {
               m_MenuOptions.m_MenuError = "At least one Ball Start must exist to configure Ball Passes";
            }
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallFail:
            if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallFailComplete;
            }
            else
            {
               m_MenuOptions.m_MenuError = "At least one Ball Start must exist to configure Ball Fails";
            }
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallCorridor:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorComplete;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Difficulty:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_TotalRuns: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectTotalRuns; break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_RunOrder:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectRunOrderMode;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallKickerBehavior:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallKickerBehaviorMode;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_MaxSecondsPerRun:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectMaxSecondsPerRun;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_CountdownSecondsBeforeRun:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCountdownSecondsBeforeRun;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_SoundEffects:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectSoundEffects;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_GoBack:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectModeOptions;
            m_MenuOptions.m_TrainerOptions.m_ModeState = TrainerOptions::ModeStateType::ModeStateType_Start;
            break;
         default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;

   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results:
   {
      // TODO GARY if you start a table and do nothing other than go into control menu,
      // select Trainer Mode, Results and then toggle out of control menu
      // Simply being on the results view causes this and it should not work this way
      ShowRemainingRunInfo();

      ShowPreviousRunRecord();

      PrintScreenRecord::MenuText(
         false, "%s:", m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord == m_MenuOptions.m_TrainerOptions.m_RunRecords.size() ? "Final Run Results" : "Current Run Results");
      if (m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord > 0)
      {
         std::size_t totalPassLocation = 0;
         std::size_t totalFailLocation = 0;
         std::size_t totalPassCorridor = 0;
         std::size_t totalFailCorridorLeft = 0;
         std::size_t totalFailCorridorRight = 0;
         std::size_t totalFailTimeElapsed = 0;
         std::size_t totalFailKicker = 0;
         std::vector<DWORD> totalPassLocationTimesMs;
         std::vector<DWORD> totalFailLocationTimesMs;
         std::vector<DWORD> totalPassCorridorTimesMs;
         std::vector<DWORD> totalFailCorridorLeftTimesMs;
         std::vector<DWORD> totalFailCorridorRightTimesMs;
         std::vector<DWORD> totalFailTimeElapsedTimeMs;
         std::vector<DWORD> totalFailKickerTimesMs;

         for (std::size_t x = 0; x <= m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord - 1; x++)
         {
            TrainerOptions::RunRecord& rr = m_MenuOptions.m_TrainerOptions.m_RunRecords[x];
            switch (rr.m_Result)
            {
            case TrainerOptions::RunRecord::ResultType::ResultType_PassedLocation:
               totalPassLocation++;
               totalPassLocationTimesMs.push_back(rr.m_TotalTimeMs);
               break;
            case TrainerOptions::RunRecord::ResultType::ResultType_FailedLocation:
               totalFailLocation++;
               totalFailLocationTimesMs.push_back(rr.m_TotalTimeMs);
               break;
            case TrainerOptions::RunRecord::ResultType::ResultType_PassedCorridor:
               totalPassCorridor++;
               totalPassCorridorTimesMs.push_back(rr.m_TotalTimeMs);
               break;
            case TrainerOptions::RunRecord::ResultType::ResultType_FailedCorridorLeft:
               totalFailCorridorLeft++;
               totalFailCorridorLeftTimesMs.push_back(rr.m_TotalTimeMs);
               break;
            case TrainerOptions::RunRecord::ResultType::ResultType_FailedCorridorRight:
               totalFailCorridorRight++;
               totalFailCorridorRightTimesMs.push_back(rr.m_TotalTimeMs);
               break;
            case TrainerOptions::RunRecord::ResultType::ResultType_FailedTimeElapsed: totalFailTimeElapsed++; break;
            case TrainerOptions::RunRecord::ResultType::ResultType_FailedKicker:
               totalFailKicker++;
               totalFailKickerTimesMs.push_back(rr.m_TotalTimeMs);
               break;
            default: InvalidEnumValue("TrainerOptions::RunRecord::ResultType", rr.m_Result); break;
            }
         }

         std::vector<std::pair<std::string, std::string>> results;
         results.push_back({ "Pass Results", "" });
         if (m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size() > 0)
         {
            ShowResult(totalPassLocation, totalPassLocationTimesMs, "Pass", "Location", results);
         }
         if (!m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord.m_PassPosition.IsZero())
         {
            ShowResult(totalPassCorridor, totalPassCorridorTimesMs, "Pass", "Corridor", results);
         }

         results.push_back({ "Fail Results", "" });
         if (m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size() > 0)
         {
            ShowResult(totalFailLocation, totalFailLocationTimesMs, "Fail", "Location", results);
         }
         if (!m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord.m_OpeningPositionLeft.IsZero())
         {
            ShowResult(totalFailCorridorLeft, totalFailCorridorLeftTimesMs, "Fail", "Corridor Left", results);
         }
         if (!m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord.m_OpeningPositionRight.IsZero())
         {
            ShowResult(totalFailCorridorRight, totalFailCorridorRightTimesMs, "Fail", "Corridor Right", results);
         }
         ShowResult(totalFailTimeElapsed, totalFailTimeElapsedTimeMs, "Fail", "Time", results);
         if (m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode == TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail)
         {
            ShowResult(totalFailKicker, totalFailKickerTimesMs, "Fail", "Kicker", results);
         }
         PrintScreenRecord::Results(results);
      }
      else
      {
         PrintScreenRecord::MenuText(false, "Results Not Available");
      }

      ShowSection(DescriptionSectionTitle, { "Use Plunger to go back to Trainer menu" });

      ShowCurrentRunRecord(currentTimeMs);

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectModeOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallStartMode:
      if (!m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
      {
         InitBallStartOptionRecords();
      }

      PrintScreenRecord::MenuTitleText("Ball Start Mode");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallStartMode == TrainerOptions::BallStartModeType::BallStartModeType_Accept, "Accept");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallStartMode == TrainerOptions::BallStartModeType::BallStartModeType_Existing, "Configure Existing");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallStartMode == TrainerOptions::BallStartModeType::BallStartModeType_Custom, "Configure Custom");

      PrintScreenRecord::MenuText(false, "");

      PrintScreenRecord::MenuTitleText("Current Configuration");
      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         PrintScreenRecord::MenuText(false, "Ball Start %zu", bsorIndex + 1);
         ShowBallStartOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex]);
      }

      switch (m_MenuOptions.m_TrainerOptions.m_BallStartMode)
      {
      case TrainerOptions::BallStartModeType::BallStartModeType_Accept:
         ShowSection(DescriptionSectionTitle,
            {
               "Accept current configuration",
            });
         break;
      case TrainerOptions::BallStartModeType::BallStartModeType_Existing: ShowSection(DescriptionSectionTitle, { "Configure Ball Starts using position/history of existing balls" }); break;
      case TrainerOptions::BallStartModeType::BallStartModeType_Custom:
         ShowSection(DescriptionSectionTitle, { "Configure Ball Starts using custom positions, velocities and angles" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallStartModeType", m_MenuOptions.m_TrainerOptions.m_BallStartMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallStartModeType>(m_MenuOptions.m_TrainerOptions.m_BallStartMode, 0, TrainerOptions::BallStartModeType::BallStartModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallStartModeType>(m_MenuOptions.m_TrainerOptions.m_BallStartMode, 0, TrainerOptions::BallStartModeType::BallStartModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallStartMode)
         {
         case TrainerOptions::BallStartModeType::BallStartModeType_Accept:
         {
            std::size_t ballNotSetNumber = 0;
            for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
            {
               TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
               if (!ballNotSetNumber && bsor.m_StartPosition.IsZero())
               {
                  ballNotSetNumber = bsorIndex + 1;
               }
            }

            m_MenuOptions.m_MenuError.clear();
            if (ballNotSetNumber)
            {
               std::ostringstream strStream;
               strStream << "Ball " << ballNotSetNumber << " must be set";
               m_MenuOptions.m_MenuError = strStream.str();
            }
            else
            {
               switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
               {
               case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
                  m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallPassComplete;
                  break;
               case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallStart:
                  m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
                  break;
               default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
               }
               m_MenuOptions.m_CurrentBallIndex = 0;
               m_MenuOptions.m_CurrentAssociationIndex = 0;
               m_MenuOptions.m_CurrentCompleteIndex = 0;
               m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode = TrainerOptions::BallEndLocationModeType_Config;
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
               m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
            }
         }
         break;
         case TrainerOptions::BallStartModeType::BallStartModeType_Existing:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectExistingBallStartLocation;
            m_MenuOptions.m_TrainerOptions.m_BallStartMode = TrainerOptions::BallStartModeType::BallStartModeType_Accept;
            break;
         case TrainerOptions::BallStartModeType::BallStartModeType_Custom:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStart;
            m_MenuOptions.m_TrainerOptions.m_BallStartMode = TrainerOptions::BallStartModeType::BallStartModeType_Accept;
            break;
         default: InvalidEnumValue("TrainerOptions::BallStartModeType", m_MenuOptions.m_TrainerOptions.m_BallStartMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectExistingBallStartLocation:
      PrintScreenRecord::MenuTitleText("Existing Ball Start Location");

      if (m_BallHistoryRecordsSize)
      {
         InitBallStartOptionRecords();
      }

      ShowSection(DescriptionSectionTitle, { "Use flippers to navigate backward/forward through existing Ball History", "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ControlPrev(); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ControlNext(); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallStartMode;
         m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode = TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStart:
   {
      PrintScreenRecord::MenuTitleText("Custom Ball Start Location");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode == TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept, "Accept");

      std::size_t ballNotSetNumber = 0;
      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
         PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode == TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select
               && m_MenuOptions.m_CurrentCompleteIndex == bsorIndex,
            "Edit Ball %zu%s", bsorIndex + 1, bsor.m_StartPosition.IsZero() ? " (Not Set)" : "");
         if (!ballNotSetNumber && bsor.m_StartPosition.IsZero())
         {
            ballNotSetNumber = bsorIndex + 1;
         }
      }

      switch (m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode)
      {
      case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept: ShowSection(DescriptionSectionTitle, { "Accept current configuration" }); break;
      case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select:
         PrintScreenRecord::MenuText(false, "");
         PrintScreenRecord::MenuTitleText("Current Configuration");
         ShowBallStartOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentCompleteIndex]);

         ShowSection(DescriptionSectionTitle, { "Configure custom position, velocities and angles" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallStartCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         switch (m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode)
         {
         case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode = TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select;
            m_MenuOptions.m_CurrentCompleteIndex = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize - 1;
            break;
         case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select:
            if (m_MenuOptions.m_CurrentCompleteIndex == 0)
            {
               m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode = TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept;
            }
            else
            {
               m_MenuOptions.m_CurrentCompleteIndex--;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallStartCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         switch (m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode)
         {
         case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode = TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select;
            m_MenuOptions.m_CurrentCompleteIndex = 0;
            break;
         case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select:
            m_MenuOptions.m_CurrentCompleteIndex++;
            if (m_MenuOptions.m_CurrentCompleteIndex == m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode = TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept;
               m_MenuOptions.m_CurrentCompleteIndex = 0;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallStartCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode)
         {
         case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept:
            m_MenuOptions.m_MenuError.clear();
            if (ballNotSetNumber)
            {
               std::ostringstream strStream;
               strStream << "Ball " << ballNotSetNumber << " must be set";
               m_MenuOptions.m_MenuError = strStream.str();
            }
            else
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallStartMode;
            }
            break;
         case TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Select:
            CenterMouse(player);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartLocation;
            m_MenuOptions.m_CurrentBallIndex = m_MenuOptions.m_CurrentCompleteIndex;
            m_MenuOptions.m_CreateZ = S32(m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex].m_StartPosition.z);
            break;
         default: InvalidEnumValue("TrainerOptions::BallStartCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode); break;
         }
         m_MenuOptions.m_CurrentCompleteIndex = 0;
         m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartLocation:
   {
      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Location", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "Position %.2f,%.2f,%.2f (3x,3y,3z)", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z);

      float ballRadius = GetDefaultBallRadius();
      S32 heightMinimum = S32(ballRadius);
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight - ballRadius);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuText(false, "Ball Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      DrawFakeBall(player, "CustomBallStartLocation", mousePosition3D, m_TrainerBallStartColor, &bsor.m_StartPosition, Color::Blue, 10);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(DescriptionSectionTitle, { "Use mouse (move/click) to set position", "Use flippers to set Ball Start height", "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle: bsor.m_StartPosition = mousePosition3D; break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex].m_StartPosition.IsZero())
         {
            std::ostringstream strStream;
            strStream << "Ball " << (m_MenuOptions.m_CurrentBallIndex + 1) << " must be set";
            m_MenuOptions.m_MenuError = strStream.str();
         }
         else
         {
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleVelocityMode;
            m_MenuOptions.m_TrainerOptions.m_BallStartCompleteMode = TrainerOptions::BallStartCompleteModeType::BallStartCompleteModeType_Accept;
            if (bsor.m_TotalRangeAngles == 0)
            {
               m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode = TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Drop;
            }
            else
            {
               m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode = TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Custom;
            }
            bsor.m_StartVelocity.SetZero();
            bsor.m_StartAngularMomentum.SetZero();
            m_MenuOptions.m_CurrentCompleteIndex = 0;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleVelocityMode:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Angle/Velocity Mode", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode == TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Drop, "Drop");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode == TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Custom, "Custom");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode)
      {
      case TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Drop:
         ShowSection(DescriptionSectionTitle, { "Configure ball to drop at position", "Plunger accepts configuration" });
         break;
      case TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Custom:
         ShowSection(DescriptionSectionTitle,
            { "Configure ball to start at custom velocities and/or angles", "Plunger accepts configuration"

            });
         break;
      default: InvalidEnumValue("TrainerOptions::BallStartAngleVelocityModeType", m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallStartAngleVelocityModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode, 0, TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallStartAngleVelocityModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode, 0, TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode)
         {
         case TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Drop:
            bsor.m_AngleRangeStart = 0.0f;
            bsor.m_AngleRangeFinish = 0.0f;
            bsor.m_TotalRangeAngles = 0;
            bsor.m_VelocityRangeStart = 0.0f;
            bsor.m_VelocityRangeFinish = 0.0f;
            bsor.m_TotalRangeVelocities = 0;
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStart;
            break;
         case TrainerOptions::BallStartAngleVelocityModeType::BallStartAngleVelocityModeType_Custom:
            bsor.m_AngleRangeStart
               = float(std::max(std::min(S32(bsor.m_AngleRangeStart), TrainerOptions::BallStartOptionsRecord::AngleMaximum), TrainerOptions::BallStartOptionsRecord::AngleMinimum));
            bsor.m_AngleRangeFinish
               = float(std::max(std::min(S32(bsor.m_AngleRangeFinish), TrainerOptions::BallStartOptionsRecord::AngleMaximum), TrainerOptions::BallStartOptionsRecord::AngleMinimum));
            bsor.m_TotalRangeAngles
               = std::max(std::min(S32(bsor.m_TotalRangeAngles), TrainerOptions::BallStartOptionsRecord::TotalAnglesMaximum), TrainerOptions::BallStartOptionsRecord::TotalAnglesMinimum);
            bsor.m_VelocityRangeStart
               = float(std::max(std::min(S32(bsor.m_VelocityRangeStart), TrainerOptions::BallStartOptionsRecord::VelocityMaximum), TrainerOptions::BallStartOptionsRecord::VelocityMinimum));
            bsor.m_VelocityRangeFinish
               = float(std::max(std::min(S32(bsor.m_VelocityRangeFinish), TrainerOptions::BallStartOptionsRecord::VelocityMaximum), TrainerOptions::BallStartOptionsRecord::VelocityMinimum));
            bsor.m_TotalRangeVelocities = std::max(
               std::min(S32(bsor.m_TotalRangeVelocities), TrainerOptions::BallStartOptionsRecord::TotalVelocitiesMaximum), TrainerOptions::BallStartOptionsRecord::TotalVelocitiesMinimum);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartVelocityStart;
            break;
         default: InvalidEnumValue("TrainerOptions::BallStartAngleVelocityModeType", m_MenuOptions.m_TrainerOptions.m_BallStartAngleVelocityMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartVelocityStart:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Start Velocity", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::BallStartOptionsRecord::VelocityMinimum, static_cast<S32>(bsor.m_VelocityRangeStart),
         TrainerOptions::BallStartOptionsRecord::VelocityMaximum);

      if (bsor.m_TotalRangeVelocities == 1)
      {
         bsor.m_VelocityRangeFinish = bsor.m_VelocityRangeStart;
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(DescriptionSectionTitle,
         { "Configure velocity of balls when training run starts", "Also acts as 'start' of an optional range of multiple velocities", "Plunger accepts configuration" });


      ProcessMenuAction<float>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartVelocityFinish, bsor.m_VelocityRangeStart,
         TrainerOptions::BallStartOptionsRecord::VelocityMinimum, TrainerOptions::BallStartOptionsRecord::VelocityMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartVelocityFinish:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Finish Velocity", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::BallStartOptionsRecord::VelocityMinimum, static_cast<S32>(bsor.m_VelocityRangeFinish),
         TrainerOptions::BallStartOptionsRecord::VelocityMaximum);

      if (bsor.m_VelocityRangeStart != bsor.m_VelocityRangeFinish && bsor.m_TotalRangeVelocities == 1)
      {
         bsor.m_TotalRangeVelocities = 2;
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(DescriptionSectionTitle,
         { "Configure velocity of balls when training run starts", "Also acts as 'end' of an optional range of multiple velocities", "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
         ProcessMenuChangeValueSkip<float>(
            bsor.m_VelocityRangeFinish, TrainerOptions::BallStartOptionsRecord::VelocityMinimum, TrainerOptions::BallStartOptionsRecord::VelocityMaximum, currentTimeMs);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<float>(bsor.m_VelocityRangeFinish, TrainerOptions::BallStartOptionsRecord::VelocityMinimum, TrainerOptions::BallStartOptionsRecord::VelocityMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<float>(bsor.m_VelocityRangeFinish, TrainerOptions::BallStartOptionsRecord::VelocityMinimum, TrainerOptions::BallStartOptionsRecord::VelocityMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         if (bsor.m_VelocityRangeStart == bsor.m_VelocityRangeFinish)
         {
            bsor.m_TotalRangeVelocities = 1;
         }
         else
         {
            if (bsor.m_TotalRangeVelocities == 1)
            {
               bsor.m_TotalRangeVelocities = 2;
            }
         }
         m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartVelocityTotal;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartVelocityTotal:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      S32 minimum = TrainerOptions::BallStartOptionsRecord::TotalVelocitiesMinimum + (bsor.m_VelocityRangeStart == bsor.m_VelocityRangeFinish ? 0 : 1);
      S32 maximum = (bsor.m_VelocityRangeStart == bsor.m_VelocityRangeFinish ? 1 : TrainerOptions::BallStartOptionsRecord::TotalVelocitiesMaximum);

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Total Velocities", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", minimum, static_cast<S32>(bsor.m_TotalRangeVelocities), maximum);
      if (minimum == 1 && maximum == 1)
      {
         PrintScreenRecord::MenuText(false, "(Start=Finish, Total must be 1)", minimum, static_cast<S32>(bsor.m_TotalRangeVelocities), maximum);
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(DescriptionSectionTitle,
         { "Configure total velocities to use for training runs", "When start/finish are different, evenly divided velocities between range will be used",
            "For example, when start=10, finish=20 and total = 3, velocities will be 10, 15, 20", "Plunger accepts configuration" });

      ProcessMenuAction<S32>(
         menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleStart, bsor.m_TotalRangeVelocities, minimum, maximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleStart:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Start Angle", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::BallStartOptionsRecord::AngleMinimum, static_cast<S32>(bsor.m_AngleRangeStart),
         TrainerOptions::BallStartOptionsRecord::AngleMaximum - 1);

      if (bsor.m_TotalRangeAngles == 1)
      {
         bsor.m_AngleRangeFinish = bsor.m_AngleRangeStart;
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(
         DescriptionSectionTitle, { "Configure angle of balls when training run starts", "Also acts as 'start' of an optional range of multiple angles", "Plunger accepts configuration" });

      ProcessMenuAction<float>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleFinish, bsor.m_AngleRangeStart,
         TrainerOptions::BallStartOptionsRecord::AngleMinimum, TrainerOptions::BallStartOptionsRecord::AngleMaximum - 1, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleFinish:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Finish Angle", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::BallStartOptionsRecord::AngleMinimum, static_cast<S32>(bsor.m_AngleRangeFinish),
         TrainerOptions::BallStartOptionsRecord::AngleMaximum - 1);

      if (bsor.m_AngleRangeStart != bsor.m_AngleRangeFinish && bsor.m_TotalRangeAngles == 1)
      {
         bsor.m_TotalRangeAngles = 2;
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(
         DescriptionSectionTitle, { "Configure angle of balls when training run starts", "Also acts as 'end' of an optional range of multiple angles", "Plunger accepts configuration" });

      ProcessMenuAction<float>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleTotal, bsor.m_AngleRangeFinish,
         TrainerOptions::BallStartOptionsRecord::AngleMinimum, TrainerOptions::BallStartOptionsRecord::AngleMaximum - 1, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStartAngleTotal:
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      S32 minimum = TrainerOptions::BallStartOptionsRecord::TotalAnglesMinimum + (bsor.m_AngleRangeStart == bsor.m_AngleRangeFinish ? 0 : 1);

      PrintScreenRecord::MenuTitleText("Custom Ball Start %zu Total Angles", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(
         false, "(minimum)%d <-- %d --> %d(maximum)", minimum, static_cast<S32>(bsor.m_TotalRangeAngles), TrainerOptions::BallStartOptionsRecord::TotalAnglesMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuText(false, "Current Configuration");
      ShowBallStartOptionsRecord(bsor);

      ShowSection(DescriptionSectionTitle,
         { "Configure total angles to use for training runs", "When start/finish are different, evenly divided angles between range will be used",
            "For example, when start=90, finish=180 and total = 3, velocities will be 90, 135, 180", "Plunger accepts configuration" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCustomBallStart, bsor.m_TotalRangeAngles, minimum,
         TrainerOptions::BallStartOptionsRecord::TotalAnglesMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallPassComplete:
   {
      PrintScreenRecord::MenuTitleText("Ball Passes Complete");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode == TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept, "Accept");
      for (std::size_t bporIndex = 0; bporIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); bporIndex++)
      {
         PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode == TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select
               && m_MenuOptions.m_CurrentCompleteIndex == bporIndex,
            "Edit Ball %zu", bporIndex + 1);
      }
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode == TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select
            && m_MenuOptions.m_CurrentCompleteIndex == m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(),
         "Create Ball %zu", m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size() + 1);

      switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
      {
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
         PrintScreenRecord::MenuText(false, "");
         PrintScreenRecord::MenuTitleText("Current Configuration");
         if (!m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size())
         {
            PrintScreenRecord::MenuText(false, "Ball Passes <None>");
         }
         for (std::size_t beorIndex = 0; beorIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); beorIndex++)
         {
            PrintScreenRecord::MenuText(false, "Ball Pass %zu", beorIndex + 1);
            ShowBallEndOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[beorIndex]);
         }
         ShowSection(DescriptionSectionTitle, { "Accept current configuration" });
         break;
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
         if (m_MenuOptions.m_CurrentCompleteIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size())
         {
            PrintScreenRecord::MenuText(false, "");
            PrintScreenRecord::MenuTitleText("Current Configuration");
            ShowBallEndOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[m_MenuOptions.m_CurrentCompleteIndex]);

            ShowSection(DescriptionSectionTitle, { "Configure existing Ball Pass" });
         }
         else
         {
            ShowSection(DescriptionSectionTitle, { "Configure new Ball Pass" });
         }

         break;
      default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
         {
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select;
            m_MenuOptions.m_CurrentCompleteIndex = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size();
            break;
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
            if (m_MenuOptions.m_CurrentCompleteIndex == 0)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
            }
            else
            {
               m_MenuOptions.m_CurrentCompleteIndex--;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
         {
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select;
            m_MenuOptions.m_CurrentCompleteIndex = 0;
            break;
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
            m_MenuOptions.m_CurrentCompleteIndex++;
            if (m_MenuOptions.m_CurrentCompleteIndex == (m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size() + 1))
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
               m_MenuOptions.m_CurrentCompleteIndex = 0;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
         {
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
            switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
            {
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallFailComplete; break;
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallPass:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
               break;
            default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
            }
            m_MenuOptions.m_CurrentBallIndex = 0;
            break;
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
            if (m_MenuOptions.m_CurrentCompleteIndex == m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size())
            {
               CenterMouse(player);
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassLocation;
               m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.push_back(TrainerOptions::BallEndOptionsRecord());
               for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
               {
                  m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.back().m_AssociatedBallStartIndexes.insert(bsorIndex);
               }
               m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.back().m_StopBallsTracker.resize(
                  m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize, std::make_tuple<int, Vertex3Ds>(0, { 0.0f, 0.0f, 0.0f }));
            }
            else
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassManage;
            }
            m_MenuOptions.m_CurrentBallIndex = m_MenuOptions.m_CurrentCompleteIndex;
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
         }
         m_MenuOptions.m_CurrentAssociationIndex = 0;
         m_MenuOptions.m_CurrentCompleteIndex = 0;
         m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode = TrainerOptions::BallEndLocationModeType_Config;
         m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
         m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassManage:
   {
      TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Pass %zu Location", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode == TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Config, "Config");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode == TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Delete, "Delete");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bpor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode)
      {
      case TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Config:
         ShowSection(DescriptionSectionTitle,
            {
               "Configure currently selected Ball Pass",
            });
         break;
      case TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Delete:
         ShowSection(DescriptionSectionTitle,
            {
               "Delete currently selected Ball Pass",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndLocationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallEndLocationModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode, 0, TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallEndLocationModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode, 0, TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode)
         {
         case TrainerOptions::BallEndLocationModeType_Config:
            CenterMouse(player);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassLocation;
            if (bpor.m_EndRadiusPercent == 0.0f)
            {
               // do nothing
            }
            else if (bpor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
            {
               m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode = TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop;
            }
            else
            {
               m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode = TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance;
            }
            break;
         case TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Delete:
            m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.erase(m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.begin() + m_MenuOptions.m_CurrentBallIndex);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallPassComplete;
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndLocationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassLocation:
   {
      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Pass %zu Location", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z);

      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
         float distance = DistancePixels(mousePosition3D, bsor.m_StartPosition);
         PrintScreenRecord::MenuText(false, "Distance to Start %zu = %.2f", bsorIndex + 1, distance);
      }

      float ballRadius = GetDefaultBallRadius();
      S32 heightMinimum = S32(ballRadius);
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight - ballRadius);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuText(false, "Ball Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      float intersectionRadiusPercent = bpor.m_EndRadiusPercent != TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled ? bpor.m_EndRadiusPercent : 0.0f;
      DrawFakeBall(player, "BallPassLocation", mousePosition3D, m_TrainerBallPassColor, &bpor.m_EndPosition, Color::Green, 10);
      DrawIntersectionCircle(player, "BallPassIntersection", mousePosition3D, intersectionRadiusPercent, Color::Blue);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bpor);

      ShowSection(DescriptionSectionTitle, { "Use mouse (move/click) to set position", "Use flippers to set Ball Pass height", "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle: bpor.m_EndPosition = mousePosition3D; break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         if (bpor.m_EndPosition.IsZero())
         {
            std::ostringstream strStream;
            strStream << "Use mouse (move/click) to set position for Ball Pass " << (m_MenuOptions.m_CurrentBallIndex + 1);
            m_MenuOptions.m_MenuError = strStream.str();
         }
         else
         {
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassFinishMode;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassFinishMode:
   {
      TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Pass %zu Finish Mode", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode == TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance, "Distance");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode == TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop, "Stop");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bpor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode)
      {
      case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance:
         ShowSection(DescriptionSectionTitle, { "Ball Pass satisfied when associated ball comes within this distance" });
         break;
      case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop:
         ShowSection(DescriptionSectionTitle, { "Ball Pass satisfied when associated ball stops (comes to rest)" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndFinishModeType", m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallEndFinishModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode, 0, TrainerOptions::BallEndFinishModeType::BallEndStopModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallEndFinishModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode, 0, TrainerOptions::BallEndFinishModeType::BallEndStopModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode)
         {
         case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassDistance;
            if (m_ControlVBalls.size())
            {
               if (bpor.m_EndRadiusPercent == 0.0f || bpor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
               {
                  bpor.m_EndRadiusPercent = 100.0f;
               }
            }
            else
            {
               bpor.m_EndRadiusPercent = TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled;
            }
            break;
         case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassAssociations;
            bpor.m_EndRadiusPercent = TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled;
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndFinishModeType", m_MenuOptions.m_TrainerOptions.m_BallPassFinishMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassDistance:
   {
      TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Pass Distance (%% of ball radius)");
      PrintScreenRecord::MenuText(false, "(minimum)%d%% <-- %d%% --> %d%%(maximum)", TrainerOptions::BallEndOptionsRecord::RadiusPercentMinimum, static_cast<S32>(bpor.m_EndRadiusPercent),
         TrainerOptions::BallEndOptionsRecord::RadiusPercentMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bpor);

      ShowSection(
         DescriptionSectionTitle, { "Use flippers to set 'pass' distance as a percentage of ball radius", "Ball Pass satisfied when associated ball intersects blue disk shown visually" });

      ProcessMenuAction<float>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassAssociations, bpor.m_EndRadiusPercent,
         TrainerOptions::BallEndOptionsRecord::RadiusPercentMinimum, TrainerOptions::BallEndOptionsRecord::RadiusPercentMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallPassAssociations:
   {
      TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Pass Associations");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode == TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept, "Accept");
      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         if (bpor.m_AssociatedBallStartIndexes.find(bsorIndex) == bpor.m_AssociatedBallStartIndexes.end())
         {
            PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode == TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select
                  && m_MenuOptions.m_CurrentAssociationIndex == bsorIndex,
               "Ball %zu", bsorIndex + 1);
         }
         else
         {
            PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode == TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select
                  && m_MenuOptions.m_CurrentAssociationIndex == bsorIndex,
               "(X) Ball %zu", bsorIndex + 1);
         }
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bpor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
      {
      case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept: ShowSection(DescriptionSectionTitle, { "Accept current configuration" }); break;
      case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
         ShowSection(DescriptionSectionTitle, { "Use plunger to enable/disable association with Ball Starts", "Allows requiring specific Ball Starts to end up at specific Ball Passes" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
         {
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept:
            if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select;
               m_MenuOptions.m_CurrentAssociationIndex = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize - 1;
            }
            break;
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
            if (m_MenuOptions.m_CurrentAssociationIndex == 0)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
            }
            else
            {
               m_MenuOptions.m_CurrentAssociationIndex--;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
         {
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept:
            if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select;
               m_MenuOptions.m_CurrentAssociationIndex = 0;
            }
            break;
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
            m_MenuOptions.m_CurrentAssociationIndex++;
            if (m_MenuOptions.m_CurrentAssociationIndex == m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
               m_MenuOptions.m_CurrentAssociationIndex = 0;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:

         switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
         {
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallPassComplete;
            break;
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
            if (bpor.m_AssociatedBallStartIndexes.find(m_MenuOptions.m_CurrentAssociationIndex) == bpor.m_AssociatedBallStartIndexes.end())
            {
               bpor.m_AssociatedBallStartIndexes.insert(m_MenuOptions.m_CurrentAssociationIndex);
            }
            else
            {
               bpor.m_AssociatedBallStartIndexes.erase(m_MenuOptions.m_CurrentAssociationIndex);
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallFailComplete:
   {
      PrintScreenRecord::MenuTitleText("Ball Fails Complete");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode == TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept, "Accept");
      for (std::size_t bforIndex = 0; bforIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); bforIndex++)
      {
         PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode == TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select
               && m_MenuOptions.m_CurrentCompleteIndex == bforIndex,
            "Edit Ball %zu", bforIndex + 1);
      }
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode == TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select
            && m_MenuOptions.m_CurrentCompleteIndex == m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(),
         "Create Ball %zu", m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size() + 1);

      switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
      {
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
         PrintScreenRecord::MenuText(false, "");
         PrintScreenRecord::MenuTitleText("Current Configuration");
         if (!m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size())
         {
            PrintScreenRecord::MenuText(false, "Ball Fails <None>");
         }
         for (std::size_t beorIndex = 0; beorIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); beorIndex++)
         {
            PrintScreenRecord::MenuText(false, "Ball Fail %zu", beorIndex + 1);
            ShowBallEndOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[beorIndex]);
         }
         ShowSection(DescriptionSectionTitle, { "Accept current configuration" });
         break;
      case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
         if (m_MenuOptions.m_CurrentCompleteIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size())
         {
            PrintScreenRecord::MenuText(false, "");
            PrintScreenRecord::MenuTitleText("Current Configuration");
            ShowBallEndOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[m_MenuOptions.m_CurrentCompleteIndex]);
            ShowSection(DescriptionSectionTitle, { "Configure existing Ball Fail" });
         }
         else
         {
            ShowSection(DescriptionSectionTitle, { "Configure new Ball Fail" });
         }
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
         {
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select;
            m_MenuOptions.m_CurrentCompleteIndex = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size();
            break;
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
            if (m_MenuOptions.m_CurrentCompleteIndex == 0)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
            }
            else
            {
               m_MenuOptions.m_CurrentCompleteIndex--;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
         {
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select;
            m_MenuOptions.m_CurrentCompleteIndex = 0;
            break;
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
            m_MenuOptions.m_CurrentCompleteIndex++;
            if (m_MenuOptions.m_CurrentCompleteIndex == (m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size() + 1))
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
               m_MenuOptions.m_CurrentCompleteIndex = 0;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode)
         {
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept:
            switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
            {
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorComplete;
               break;
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallFail:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
               break;
            default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
            }
            m_MenuOptions.m_CurrentBallIndex = 0;
            break;
         case TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Select:
            if (m_MenuOptions.m_CurrentCompleteIndex == m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size())
            {
               CenterMouse(player);
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailLocation;
               m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.push_back(TrainerOptions::BallEndOptionsRecord());
               for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
               {
                  m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.back().m_AssociatedBallStartIndexes.insert(bsorIndex);
               }
               m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.back().m_StopBallsTracker.resize(
                  m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize, std::make_tuple<int, Vertex3Ds>(0, { 0.0f, 0.0f, 0.0f }));
            }
            else
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailManage;
            }
            m_MenuOptions.m_CurrentBallIndex = m_MenuOptions.m_CurrentCompleteIndex;
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode); break;
         }
         m_MenuOptions.m_CurrentAssociationIndex = 0;
         m_MenuOptions.m_CurrentCompleteIndex = 0;
         m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode = TrainerOptions::BallEndLocationModeType_Config;
         m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
         m_MenuOptions.m_TrainerOptions.m_BallEndCompleteMode = TrainerOptions::BallEndCompleteModeType::BallEndCompleteModeType_Accept;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailManage:
   {
      TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Fail %zu Location", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode == TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Config, "Config");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode == TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Delete, "Delete");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bfor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode)
      {
      case TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Config:
         ShowSection(DescriptionSectionTitle,
            {
               "Configure currently selected Ball Fail",
            });
         break;
      case TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Delete:
         ShowSection(DescriptionSectionTitle,
            {
               "Delete currently selected Ball Fail",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndLocationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallEndLocationModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode, 0, TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallEndLocationModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode, 0, TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode)
         {
         case TrainerOptions::BallEndLocationModeType_Config:
            CenterMouse(player);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailLocation;
            if (bfor.m_EndRadiusPercent == 0.0f)
            {
               // do nothing
            }
            else if (bfor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
            {
               m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode = TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop;
            }
            else
            {
               m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode = TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance;
            }
            break;
         case TrainerOptions::BallEndLocationModeType::BallEndLocationModeType_Delete:
            m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.erase(m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.begin() + m_MenuOptions.m_CurrentBallIndex);
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallFailComplete;
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndLocationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndLocationMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailLocation:
   {
      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Fail %zu Location", m_MenuOptions.m_CurrentBallIndex + 1);

      PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z);

      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
         float distance = DistancePixels(mousePosition3D, bsor.m_StartPosition);
         PrintScreenRecord::MenuText(false, "Distance to Start %zu = %.2f", bsorIndex + 1, distance);
      }

      float ballRadius = GetDefaultBallRadius();
      S32 heightMinimum = S32(ballRadius);
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight - ballRadius);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Ball Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      float intersectionRadius = bfor.m_EndRadiusPercent != TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled ? bfor.m_EndRadiusPercent : 0.0f;
      DrawFakeBall(player, "BallFailLocation", mousePosition3D, m_TrainerBallFailColor, &bfor.m_EndPosition, Color::Red, 10);
      DrawIntersectionCircle(player, "BallFailIntersection", mousePosition3D, intersectionRadius, Color::Blue);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bfor);

      ShowSection(DescriptionSectionTitle, { "Use mouse (move/click) to set position", "Use flippers to set Ball Fail height", "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle: bfor.m_EndPosition = mousePosition3D; break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         if (bfor.m_EndPosition.IsZero())
         {
            std::ostringstream strStream;
            strStream << "Use mouse (move/click) to set position for Ball Fail " << (m_MenuOptions.m_CurrentBallIndex + 1);
            m_MenuOptions.m_MenuError = strStream.str();
         }
         else
         {
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailFinishMode;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailFinishMode:
   {
      TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Fail %zu Finish Mode", m_MenuOptions.m_CurrentBallIndex + 1);
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode == TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance, "Distance");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode == TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop, "Stop");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bfor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode)
      {
      case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance:
         ShowSection(DescriptionSectionTitle, { "Ball Fail satisfied when associated ball comes within this distance" });
         break;
      case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop:
         ShowSection(DescriptionSectionTitle, { "Ball Fail satisfied when associated ball stops (comes to rest)" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndFinishModeType", m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallEndFinishModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode, 0, TrainerOptions::BallEndFinishModeType::BallEndStopModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallEndFinishModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode, 0, TrainerOptions::BallEndFinishModeType::BallEndStopModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode)
         {
         case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Distance:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailDistance;
            if (m_ControlVBalls.size())
            {
               if (bfor.m_EndRadiusPercent == 0.0f || bfor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
               {
                  bfor.m_EndRadiusPercent = 100.0f;
               }
            }
            else
            {
               bfor.m_EndRadiusPercent = TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled;
            }
            break;
         case TrainerOptions::BallEndFinishModeType::BallEndFinishModeType_Stop:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailAssociations;
            bfor.m_EndRadiusPercent = TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled;
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndFinishModeType", m_MenuOptions.m_TrainerOptions.m_BallFailFinishMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailDistance:
   {
      TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Fail Distance (%% of ball radius)");
      PrintScreenRecord::MenuText(false, "(minimum)%d%% <-- %d%%--> %d%%(maximum)", TrainerOptions::BallEndOptionsRecord::RadiusPercentMinimum, static_cast<S32>(bfor.m_EndRadiusPercent),
         TrainerOptions::BallEndOptionsRecord::RadiusPercentMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bfor);

      ShowSection(
         DescriptionSectionTitle, { "Use flippers to set 'fail' distance as a percentage of ball radius", "Ball Fail satisfied when associated ball intersects blue disk shown visually" });

      ProcessMenuAction<float>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailAssociations, bfor.m_EndRadiusPercent,
         TrainerOptions::BallEndOptionsRecord::RadiusPercentMinimum, TrainerOptions::BallEndOptionsRecord::RadiusPercentMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallFailAssociations:
   {
      TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[m_MenuOptions.m_CurrentBallIndex];

      PrintScreenRecord::MenuTitleText("Ball Fail Associations");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode == TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept, "Accept");
      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         if (bfor.m_AssociatedBallStartIndexes.find(bsorIndex) == bfor.m_AssociatedBallStartIndexes.end())
         {
            PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode == TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select
                  && m_MenuOptions.m_CurrentAssociationIndex == bsorIndex,
               "Ball %zu", bsorIndex + 1);
         }
         else
         {
            PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode == TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select
                  && m_MenuOptions.m_CurrentAssociationIndex == bsorIndex,
               "(X) Ball %zu", bsorIndex + 1);
         }
      }

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallEndOptionsRecord(bfor);

      switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
      {
      case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept: ShowSection(DescriptionSectionTitle, { "Accept current configuration" }); break;
      case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
         ShowSection(DescriptionSectionTitle, { "Use plunger to enable/disable association with Ball Starts", "Allows requiring specific Ball Starts to end up at specific Ball Fails" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
         {
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select;
            m_MenuOptions.m_CurrentAssociationIndex = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize - 1;
            break;
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
            if (m_MenuOptions.m_CurrentAssociationIndex == 0)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
            }
            else
            {
               m_MenuOptions.m_CurrentAssociationIndex--;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
         {
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept:
            m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select;
            m_MenuOptions.m_CurrentAssociationIndex = 0;
            break;
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
            m_MenuOptions.m_CurrentAssociationIndex++;
            if (m_MenuOptions.m_CurrentAssociationIndex == m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
            {
               m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode = TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept;
               m_MenuOptions.m_CurrentAssociationIndex = 0;
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
         }
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:

         switch (m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode)
         {
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Accept:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_BallFailComplete;
            break;
         case TrainerOptions::BallEndAssociationModeType::BallEndAssociationModeType_Select:
            if (bfor.m_AssociatedBallStartIndexes.find(m_MenuOptions.m_CurrentAssociationIndex) == bfor.m_AssociatedBallStartIndexes.end())
            {
               bfor.m_AssociatedBallStartIndexes.insert(m_MenuOptions.m_CurrentAssociationIndex);
            }
            else
            {
               bfor.m_AssociatedBallStartIndexes.erase(m_MenuOptions.m_CurrentAssociationIndex);
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallEndAssociationModeType", m_MenuOptions.m_TrainerOptions.m_BallEndAssociationMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorComplete:
      PrintScreenRecord::MenuTitleText("Ball Corridor Complete");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode == TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Accept, "Accept");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode == TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Config, "Config");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode == TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Reset, "Reset");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallCorridorOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord);

      switch (m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode)
      {
      case TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Accept: ShowSection(DescriptionSectionTitle, { "Accept current configuration" }); break;
      case TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Config:
         ShowSection(DescriptionSectionTitle,
            {
               "Configure Ball Corridor requirements",
            });
         break;
      case TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Reset:
         ShowSection(DescriptionSectionTitle,
            {
               "Reset/Clear Ball Corridor requirements",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::BallCorridorCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallCorridorCompleteModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode, 0, TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallCorridorCompleteModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode, 0, TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode)
         {
         case TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Accept:
            switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
            {
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions;
               break;
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallCorridor:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
               break;
            default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode); break;
            }
            break;
         case TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Config:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorPassLocation;
            break;
         case TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Reset:
            new (&m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord) TrainerOptions::BallCorridorOptionsRecord();
            m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode = TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Accept;
            break;
         default: InvalidEnumValue("TrainerOptions::BallCorridorCompleteModeType", m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorPassLocation:
   {
      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;

      PrintScreenRecord::MenuTitleText("Ball Corridor Pass Location");

      PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z);

      float ballRadius = GetDefaultBallRadius();
      S32 heightMinimum = S32(ballRadius);
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight - ballRadius);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Ball Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      float passBallRadius = GetDefaultBallRadius();
      float passWidth = passBallRadius * (bcor.m_PassRadiusPercent / 100.0f);
      Vertex3Ds passPositionLeft = { mousePosition3D.x - passWidth, mousePosition3D.y, mousePosition3D.z };
      Vertex3Ds passPositionRight = { mousePosition3D.x + passWidth, mousePosition3D.y, mousePosition3D.z };
      DrawLine(player, "BallCorridorPassLocationLeft", passPositionLeft, bcor.m_OpeningPositionLeft, Color::Red, int(passBallRadius));
      DrawLine(player, "BallCorridorPassLocationRight", passPositionRight, bcor.m_OpeningPositionRight, Color::Red, int(passBallRadius));
      DrawTrainerBallCorridorPass(player, "BallCorridorPassLocation", bcor, &mousePosition3D);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallCorridorOptionsRecord(bcor);

      ShowSection(DescriptionSectionTitle,
         { "Use mouse (move/click) to set position", "Use flippers to set Ball Corridor Pass height", "Plunger accepts configuration",
            "Trainer run will 'pass' when any ball intersects green Pass Wall" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle: bcor.m_PassPosition = mousePosition3D; break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         if (bcor.m_PassPosition.IsZero())
         {
            std::ostringstream strStream;
            strStream << "Use mouse (move/click) to set position for Ball Corridor Pass";
            m_MenuOptions.m_MenuError = strStream.str();
         }
         else
         {
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorPassWidth;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorPassWidth:
   {
      TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;

      PrintScreenRecord::MenuTitleText("Ball Corridor Pass Width (%% of ball radius)");
      PrintScreenRecord::MenuText(false, "(minimum)%d%% <-- %d%% --> %d%%(maximum)", TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMinimum,
         static_cast<S32>(bcor.m_PassRadiusPercent), TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallCorridorOptionsRecord(m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord);

      ShowSection(DescriptionSectionTitle, { "Use flippers to set 'pass' width as a percentage of ball radius", "Ball Corridor Pass satisfied when any ball intersects green Pass Wall" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
         ProcessMenuChangeValueSkip<float>(
            bcor.m_PassRadiusPercent, TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMinimum, TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMaximum, currentTimeMs);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<float>(
            bcor.m_PassRadiusPercent, TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMinimum, TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<float>(
            bcor.m_PassRadiusPercent, TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMinimum, TrainerOptions::BallCorridorOptionsRecord::RadiusPercentMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorOpeningLeftLocation;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorOpeningLeftLocation:
   {
      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;

      PrintScreenRecord::MenuTitleText("Ball Corridor Opening Left");

      PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z);

      S32 heightMinimum = 0;
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Opening Left Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      float passBallRadius = GetDefaultBallRadius();
      float passWidth = passBallRadius * (bcor.m_PassRadiusPercent / 100.0f);
      Vertex3Ds passPositionLeft = { bcor.m_PassPosition.x - passWidth, bcor.m_PassPosition.y, bcor.m_PassPosition.z };
      DrawFakeBall(
         player, "BallCorridorOpeningLeft", mousePosition3D, GetDefaultBallRadius() / 2.0f, m_TrainerBallCorridorOpeningEndColor, &passPositionLeft, Color::Red, int(GetDefaultBallRadius()));

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallCorridorOptionsRecord(bcor);

      ShowSection(DescriptionSectionTitle,
         { "Use mouse (move/click) to set position", "Use flippers to set Ball Corridor Opening Left height", "Plunger accepts configuration",
            "Trainer run will 'fail' when any ball intersects red Opening Left wall" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle: bcor.m_OpeningPositionLeft = mousePosition3D; break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         if (bcor.m_OpeningPositionLeft.IsZero())
         {
            std::ostringstream strStream;
            strStream << "Use mouse (move/click) to set position for Ball Corridor Opening Left";
            m_MenuOptions.m_MenuError = strStream.str();
         }
         else
         {
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorOpeningRightLocation;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorOpeningRightLocation:
   {
      mousePosition3D = Get3DPointFromMousePosition(player, float(m_MenuOptions.m_CreateZ));
      TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;

      PrintScreenRecord::MenuTitleText("Ball Corridor Opening Right");

      PrintScreenRecord::MenuText(false, "Position = %.2f,%.2f,%.2f (x,y,z)", mousePosition3D.x, mousePosition3D.y, mousePosition3D.z);

      S32 heightMinimum = 0;
      S32 heightMaximum = S32(player.m_ptable->m_glassTopHeight);
      m_MenuOptions.m_CreateZ = std::max(std::min(S32(m_MenuOptions.m_CreateZ), heightMaximum), heightMinimum);
      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Opening Right Height");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", heightMinimum, m_MenuOptions.m_CreateZ, heightMaximum);

      float passBallRadius = GetDefaultBallRadius();
      float passWidth = passBallRadius * (bcor.m_PassRadiusPercent / 100.0f);
      Vertex3Ds passPositionRight = { bcor.m_PassPosition.x + passWidth, bcor.m_PassPosition.y, bcor.m_PassPosition.z };
      DrawFakeBall(player, "BallCorridorOpeningRight", mousePosition3D, GetDefaultBallRadius() / 2.0f, m_TrainerBallCorridorOpeningEndColor, &passPositionRight, Color::Red,
         int(GetDefaultBallRadius()));

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      ShowBallCorridorOptionsRecord(bcor);

      ShowSection(DescriptionSectionTitle,
         { "Use mouse (move/click) to set position", "Use flippers to set Ball Corridor Opening Right height", "Plunger accepts configuration",
            "Trainer run will 'fail' when any ball intersects red Opening Right wall" });


      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum, currentTimeMs); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle: bcor.m_OpeningPositionRight = mousePosition3D; break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_CreateZ, heightMinimum, heightMaximum); break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         m_MenuOptions.m_MenuError.clear();
         if (bcor.m_OpeningPositionRight.IsZero())
         {
            std::ostringstream strStream;
            strStream << "Use mouse (move/click) to set position for Ball Corridor Opening Right";
            m_MenuOptions.m_MenuError = strStream.str();
         }
         else
         {
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallCorridorComplete;
            m_MenuOptions.m_TrainerOptions.m_BallCorridorCompleteMode = TrainerOptions::BallCorridorCompleteModeType::BallCorridorCompleteModeType_Accept;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions:
   {
      PrintScreenRecord::MenuTitleText("Difficulty Options");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_Accept, "Accept");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_GameplayDifficulty, "Gameplay Difficulty");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_VarianceDifficulty, "Variance Difficulty");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_GravityVariance, "Gravity Variance");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_PlayfieldFrictionVariance,
         "Playfield Friction Variance");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_FlipperStrengthVariance,
         "Flipper Strength Variance");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_FlipperFrictionVariance,
         "Flipper Friction Variance");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      bool acceptMode = m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState == TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_Accept;
      switch (m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState)
      {
      // default at beginning due to purposeful fall through
      default: InvalidEnumValue("TrainerOptions::DifficultyConfigModeState", m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState); break;
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_Accept:
         // purposeful fall through
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_GameplayDifficulty:
         PrintScreenRecord::MenuText(false, "Gameplay Difficulty = %d", m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty);
         PrintScreenRecord::MenuText(false, "Gameplay Difficulty Initial = %d", m_MenuOptions.m_TrainerOptions.m_GameplayDifficultyInitial);
         if (!acceptMode)
         {
            ShowSection(DescriptionSectionTitle,
               { "Gameplay Difficulty applies randomness to ball interactions", "Lowering/Raising causes more consistent/inconsistent play", "'Initial' provided for reference" });
            break;
         }
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_VarianceDifficulty:
         PrintScreenRecord::MenuText(false, "Variance Difficulty = %d", m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty);
         if (!acceptMode)
         {
            ShowSection(DescriptionSectionTitle,
               { "Variance Difficulty controls speed of variance within specified range", "Lowering/Raising causes slow/fast variance between baseline and target range" });
            break;
         }
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_GravityVariance:
         difficultyVarianceStatuses.push_back({ "Gravity Variance", std::format("{}", m_MenuOptions.m_TrainerOptions.m_GravityVariance) });
         ShowDifficultyVarianceStatusGravity(player, difficultyVarianceStatuses, false);
         ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

         if (!acceptMode)
         {
            ShowSection(DescriptionSectionTitle,
               { "Gravity Variance sets range above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
                  "'Initial' shows starting table value" });
            break;
         }
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_PlayfieldFrictionVariance:
         ShowDifficultyVarianceStatusPlayfieldFriction(player, difficultyVarianceStatuses, false);
         ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

         if (!acceptMode)
         {
            ShowSection(DescriptionSectionTitle,
               { "Playfield Friction Variance sets range above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
                  "'Initial' shows starting table value" });
            break;
         }
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_FlipperStrengthVariance:
         ShowDifficultyVarianceStatusFlipperStrength(player, difficultyVarianceStatuses, false);
         ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

         if (!acceptMode)
         {
            ShowSection(DescriptionSectionTitle,
               { "Flipper Strength Variance sets range above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
                  "'Initial' shows starting table value" });
            break;
         }
      case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_FlipperFrictionVariance:
         ShowDifficultyVarianceStatusFlipperFriction(player, difficultyVarianceStatuses, false);
         ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

         if (!acceptMode)
         {
            ShowSection(DescriptionSectionTitle,
               { "Flipper Friction Variance sets range above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
                  "'Initial' shows starting table value" });
            break;
         }
      }

      if (acceptMode)
      {
         ShowSection(DescriptionSectionTitle, { "Accept current configuration" });

         PrintScreenRecord::MenuText(false, "");
         ShowDifficultyTableConstants(player);
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::DifficultyConfigModeState>(
            m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState, 0, TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::DifficultyConfigModeState>(
            m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState, 0, TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState)
         {
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_Accept:
            switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
            {
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectTotalRuns; break;
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Difficulty:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
               break;
            default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState); break;
            }
            break;
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_GameplayDifficulty:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyGameplayDifficulty;
            break;
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_VarianceDifficulty:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyVarianceDifficulty;
            break;
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_GravityVariance:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyGravityVariance;
            break;
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_PlayfieldFrictionVariance:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyPlayfieldFrictionVariance;
            break;
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_FlipperStrengthVariance:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperStrengthVariance;
            break;
         case TrainerOptions::DifficultyConfigModeState::DifficultyConfigModeState_FlipperFrictionVariance:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperFrictionVariance;
            break;
         default: InvalidEnumValue("TrainerOptions::DifficultyConfigModeState", m_MenuOptions.m_TrainerOptions.m_DifficultyConfigModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyGameplayDifficulty:
      PrintScreenRecord::MenuTitleText("Gameplay Difficulty");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::GameplayDifficultyMinimum, m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty,
         TrainerOptions::GameplayDifficultyMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");
      PrintScreenRecord::MenuText(false, "Gameplay Difficulty Initial = %d", m_MenuOptions.m_TrainerOptions.m_GameplayDifficultyInitial);

      ShowSection(DescriptionSectionTitle,
         { "Gameplay Difficulty applies randomness to ball interactions", "Lowering/Raising causes more consistent/inconsistent play", "'Initial' provided for reference" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions, m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty,
         TrainerOptions::GameplayDifficultyMinimum, TrainerOptions::GameplayDifficultyMaximum, currentTimeMs);
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyVarianceDifficulty:
      PrintScreenRecord::MenuTitleText("Variance Difficulty");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::VarianceDifficultyMinimum, m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty,
         TrainerOptions::VarianceDifficultyMaximum);

      ShowSection(DescriptionSectionTitle, { "Variance Difficulty controls speed of variation", "Lowering/Raising causes variance to move slower/faster along range" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions, m_MenuOptions.m_TrainerOptions.m_VarianceDifficulty,
         TrainerOptions::VarianceDifficultyMinimum, TrainerOptions::VarianceDifficultyMaximum, currentTimeMs);
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyGravityVariance:
   {
      PrintScreenRecord::MenuTitleText("Gravity Variance");
      PrintScreenRecord::MenuText(
         false, "(minimum)%d%% <-- %d%% --> %d%%(maximum)", TrainerOptions::GravityVarianceMinimum, m_MenuOptions.m_TrainerOptions.m_GravityVariance, TrainerOptions::GravityVarianceMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusGravity(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to set Gravity Variance above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
            "'Initial' shows starting table value" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyGravityVarianceType, m_MenuOptions.m_TrainerOptions.m_GravityVariance,
         TrainerOptions::GravityVarianceMinimum, TrainerOptions::GravityVarianceMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyGravityVarianceType:
   {
      PrintScreenRecord::MenuTitleText("Gravity Variance Mode");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow, "Above and Below");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly, "Above Only");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly, "Below Only");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusGravity(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      switch (m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode)
      {
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Gravity Variance range to ['Variance Below', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Gravity Variance range to ['Initial', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Gravity Variance range to ['Variance Below', 'Initial']",
               "Plunger accepts configuration",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyPlayfieldFrictionVariance:
   {
      PrintScreenRecord::MenuTitleText("Playfield Friction Variance");
      PrintScreenRecord::MenuText(false, "(minimum)%d%% <-- %d%% --> %d%%(maximum)", TrainerOptions::PlayfieldFrictionVarianceMinimum,
         m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance, TrainerOptions::PlayfieldFrictionVarianceMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusPlayfieldFriction(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to set Playfield Friction Variance above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
            "'Initial' shows starting table value" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyPlayfieldFrictionVarianceType,
         m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance, TrainerOptions::PlayfieldFrictionVarianceMinimum, TrainerOptions::PlayfieldFrictionVarianceMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyPlayfieldFrictionVarianceType:
   {
      PrintScreenRecord::MenuTitleText("Playfield Friction Variance Mode");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow, "Above and Below");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly, "Above Only");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly, "Below Only");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusPlayfieldFriction(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      switch (m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode)
      {
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Playfield Friction Variance range to ['Variance Below', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Playfield Friction Variance range to ['Initial', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Playfield Friction Variance range to ['Variance Below', 'Initial']",
               "Plunger accepts configuration",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperStrengthVariance:
   {
      PrintScreenRecord::MenuTitleText("Flipper Strength Variance");
      PrintScreenRecord::MenuText(false, "(minimum)%d%% <-- %d%% --> %d%%(maximum)", TrainerOptions::FlipperStrengthVarianceMinimum, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance,
         TrainerOptions::FlipperStrengthVarianceMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusFlipperStrength(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to set Flipper Strength Variance above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
            "'Initial' shows starting table value" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperStrengthVarianceType,
         m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance, TrainerOptions::FlipperStrengthVarianceMinimum, TrainerOptions::FlipperStrengthVarianceMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperStrengthVarianceType:
   {
      PrintScreenRecord::MenuTitleText("Flipper Strength Variance Mode");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow, "Above and Below");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly, "Above Only");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly, "Below Only");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusFlipperStrength(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      switch (m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode)
      {
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Flipper Strength Variance range to ['Variance Below', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Flipper Strength Variance range to ['Initial', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Flipper Strength Variance range to ['Variance Below', 'Initial']",
               "Plunger accepts configuration",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperFrictionVariance:
   {
      PrintScreenRecord::MenuTitleText("Flipper Friction Variance");
      PrintScreenRecord::MenuText(false, "(minimum)%d%% <-- %d%% --> %d%%(maximum)", TrainerOptions::FlipperFrictionVarianceMinimum, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance,
         TrainerOptions::FlipperFrictionVarianceMaximum);

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusFlipperFriction(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to set Flipper Friction Variance above/below 'Initial'", "'Current'/'Initial' provided for reference", "'Current' shows active table value",
            "'Initial' shows starting table value" });

      ProcessMenuAction<S32>(menuAction, MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperFrictionVarianceType,
         m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance, TrainerOptions::FlipperFrictionVarianceMinimum, TrainerOptions::FlipperFrictionVarianceMaximum, currentTimeMs);
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyFlipperFrictionVarianceType:
   {
      PrintScreenRecord::MenuTitleText("Flipper Friction Variance Mode");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow, "Above and Below");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly, "Above Only");
      PrintScreenRecord::MenuText(
         m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode == TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly, "Below Only");

      PrintScreenRecord::MenuText(false, "");
      PrintScreenRecord::MenuTitleText("Current Configuration");

      std::vector<std::pair<std::string, std::string>> difficultyVarianceStatuses;
      ShowDifficultyVarianceStatusFlipperFriction(player, difficultyVarianceStatuses, false);
      ShowDifficultyVarianceStatusesMenu(difficultyVarianceStatuses);

      switch (m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode)
      {
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveAndBelow:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Flipper Friction Variance range to ['Variance Below', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_AboveOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Flipper Friction Variance range to ['Initial', 'Variance Above']",
               "Plunger accepts configuration",
            });
         break;
      case TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_BelowOnly:
         ShowSection(DescriptionSectionTitle,
            {
               "Configures Flipper Friction Variance range to ['Variance Below', 'Initial']",
               "Plunger accepts configuration",
            });
         break;
      default: InvalidEnumValue("TrainerOptions::DifficultyVarianceModeType", m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::DifficultyVarianceModeType>(
            m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode, 0, TrainerOptions::DifficultyVarianceModeType::DifficultyVarianceModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectDifficultyOptions; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectTotalRuns:
   {
      std::size_t totalPermutations = 1;
      for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
      {
         TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
         if (bsor.m_TotalRangeAngles > 0)
         {
            totalPermutations *= bsor.m_TotalRangeAngles;
         }
         if (bsor.m_TotalRangeVelocities > 0)
         {
            totalPermutations *= bsor.m_TotalRangeVelocities;
         }
      }

      PrintScreenRecord::MenuTitleText("Total Runs");
      if (m_MenuOptions.m_TrainerOptions.m_TotalRuns == totalPermutations)
      {
         PrintScreenRecord::MenuText(
            false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::TotalRunsMinimum, m_MenuOptions.m_TrainerOptions.m_TotalRuns, TrainerOptions::TotalRunsMaximum);
         PrintScreenRecord::MenuText(false, "(Runs)");
         ShowSection(DescriptionSectionTitle, { "Use flippers to set Total Runs", "Plunger accepts configuration" });
      }
      else
      {

         PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::TotalRunsMinimum, totalPermutations * m_MenuOptions.m_TrainerOptions.m_TotalRuns,
            TrainerOptions::TotalRunsMaximum * totalPermutations);
         PrintScreenRecord::MenuText(false, "(Permutations)");
         ShowSection(DescriptionSectionTitle, { "Use flippers to set Total Runs", "Ball Starts with velocities or angles ranges will multiply Total Runs", "Plunger accepts configuration" });
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
         ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_TrainerOptions.m_TotalRuns, TrainerOptions::TotalRunsMinimum, TrainerOptions::TotalRunsMaximum, currentTimeMs);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_TrainerOptions.m_TotalRuns, TrainerOptions::TotalRunsMinimum, TrainerOptions::TotalRunsMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_TrainerOptions.m_TotalRuns, TrainerOptions::TotalRunsMinimum, TrainerOptions::TotalRunsMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
      {
         bool anyCustom = false;
         for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
         {
            TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
            if (bsor.m_TotalRangeAngles > 0 || bsor.m_TotalRangeVelocities > 0)
            {
               anyCustom = true;
               break;
            }
         }

         switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
         {
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
            if (anyCustom)
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectRunOrderMode;
            }
            else
            {
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallKickerBehaviorMode;
            }
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_TotalRuns:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
            break;
         default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
         }
      }
      break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
   }
   break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectRunOrderMode:
      PrintScreenRecord::MenuTitleText("Run Order Mode");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_RunOrderMode == TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder, "In Order");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_RunOrderMode == TrainerOptions::RunOrderModeType::RunOrderModeType_Random, "Random");

      switch (m_MenuOptions.m_TrainerOptions.m_RunOrderMode)
      {
      case TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder:
         ShowSection(DescriptionSectionTitle, { "Configure training run permuations to run in ascending order", "Plunger accepts configuration" });
         break;
      case TrainerOptions::RunOrderModeType::RunOrderModeType_Random:
         ShowSection(DescriptionSectionTitle, { "Configure training run permutations to run in random order", "Plunger accepts configuration" });
         break;
      default: InvalidEnumValue("TrainerOptions::RunOrderModeType", m_MenuOptions.m_TrainerOptions.m_RunOrderMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::RunOrderModeType>(m_MenuOptions.m_TrainerOptions.m_RunOrderMode, 0, TrainerOptions::RunOrderModeType::RunOrderModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::RunOrderModeType>(m_MenuOptions.m_TrainerOptions.m_RunOrderMode, 0, TrainerOptions::RunOrderModeType::RunOrderModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
         {
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallKickerBehaviorMode;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_RunOrder:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
            break;
         default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectBallKickerBehaviorMode:
      PrintScreenRecord::MenuTitleText("Ball Kicker Behavior Mode");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode == TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset, "Reset");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode == TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail, "Fail");

      switch (m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode)
      {
      case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset:
         ShowSection(DescriptionSectionTitle, { "Configures training run to reset (start over) when any ball hits a kicker", "Plunger accepts configuration" });
         break;
      case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail:
         ShowSection(DescriptionSectionTitle, { "Configures training run will 'fail' when any ball hits a kicker", "Plunger accepts configuration" });
         break;
      default: InvalidEnumValue("TrainerOptions::BallKickerBehaviorModeType", m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode); break;
      }

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::BallKickerBehaviorModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode, 0, TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::BallKickerBehaviorModeType>(
            m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode, 0, TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_COUNT - 1);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
         {
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectMaxSecondsPerRun;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_BallKickerBehavior:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
            break;
         default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectMaxSecondsPerRun:
      PrintScreenRecord::MenuTitleText("Time Per Run (Seconds)");
      PrintScreenRecord::MenuText(
         false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::MaxSecondsPerRunMinimum, m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun, TrainerOptions::MaxSecondsPerRunMaximum);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to set maximum allowed time, in seconds, per training run", "Training run will 'fail' when time runs out", "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
         ProcessMenuChangeValueSkip<S32>(m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun, TrainerOptions::MaxSecondsPerRunMinimum, TrainerOptions::MaxSecondsPerRunMaximum, currentTimeMs);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<S32>(m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun, TrainerOptions::MaxSecondsPerRunMinimum, TrainerOptions::MaxSecondsPerRunMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<S32>(m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun, TrainerOptions::MaxSecondsPerRunMinimum, TrainerOptions::MaxSecondsPerRunMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
         {
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCountdownSecondsBeforeRun;
            break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_MaxSecondsPerRun:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
            break;
         default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectCountdownSecondsBeforeRun:
      PrintScreenRecord::MenuTitleText("Countdown Before Run (Seconds)");
      PrintScreenRecord::MenuText(false, "(minimum)%d <-- %d --> %d(maximum)", TrainerOptions::CountdownSecondsBeforeRunMinimum, m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun,
         TrainerOptions::CountdownSecondsBeforeRunMaximum);

      ShowSection(DescriptionSectionTitle,
         { "Use flippers to set countdown time, in seconds, before each training run",
            "Countdown is indicated with visual and audible effects"
            "Plunger accepts configuration" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
         ProcessMenuChangeValueSkip<S32>(
            m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun, TrainerOptions::CountdownSecondsBeforeRunMinimum, TrainerOptions::CountdownSecondsBeforeRunMaximum, currentTimeMs);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<S32>(
            m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun, TrainerOptions::CountdownSecondsBeforeRunMinimum, TrainerOptions::CountdownSecondsBeforeRunMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<S32>(
            m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun, TrainerOptions::CountdownSecondsBeforeRunMinimum, TrainerOptions::CountdownSecondsBeforeRunMaximum);
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
         {
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectSoundEffects; break;
         case TrainerOptions::ConfigModeStateType::ConfigModeStateType_CountdownSecondsBeforeRun:
            m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
            break;
         default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
         }
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectSoundEffects:
      PrintScreenRecord::MenuTitleText("Sound Effects");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode == TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Accept, "Accept");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode == TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Pass, "(%s) Pass",
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled ? "X" : "O");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode == TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Fail, "(%s) Fail",
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled ? "X" : "O");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode == TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Countdown, "(%s) Countdown",
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled ? "X" : "O");
      PrintScreenRecord::MenuText(m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode == TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_TimeLow, "(%s) Time Low",
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled ? "X" : "O");

      switch (m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode)
      {
      case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Accept:
         ShowSection(DescriptionSectionTitle, { "Plunger accepts configuration" });
         StopSound();
         break;
      case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Pass:
         ShowSection(DescriptionSectionTitle, { "Use plunger to toggle sound", "Sound is played when training run results in a 'pass'" });
         if (!m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed)
         {
            PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_PASS, true);
         }
         break;
      case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Fail:
         ShowSection(DescriptionSectionTitle, { "Use plunger to toggle sound", "Sound is played when training run results in a 'Fail'" });
         if (!m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed)
         {
            PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_FAIL, true);
         }
         break;
      case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Countdown:
         ShowSection(DescriptionSectionTitle, { "Use plunger to toggle sound", "Sound is played when training run is about to start" });
         if (!m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed)
         {
            PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_COUNTDOWN_READYSET, true);
         }
         break;
      case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_TimeLow:
         ShowSection(DescriptionSectionTitle, { "Use plunger to toggle sound", "Sound is played when training run remaining time approaches zero" });
         if (!m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed)
         {
            PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_TIME_LOW, true);
         }
         break;
      default: InvalidEnumValue("TrainerOptions::SoundEffectsModeType", m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode); break;
      }
      m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed = true;

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
         ProcessMenuChangeValueDec<TrainerOptions::SoundEffectsModeType>(
            m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode, 0, TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_COUNT - 1);
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed = false;
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         ProcessMenuChangeValueInc<TrainerOptions::SoundEffectsModeType>(
            m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode, 0, TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_COUNT - 1);
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed = false;
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter:
         switch (m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode)
         {
         case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Accept:
            switch (m_MenuOptions.m_TrainerOptions.m_ConfigModeState)
            {
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_Wizard:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
               m_MenuOptions.m_TrainerOptions.m_ConfigModeState = TrainerOptions::ConfigModeStateType::ConfigModeStateType_GoBack;
               break;
            case TrainerOptions::ConfigModeStateType::ConfigModeStateType_SoundEffects:
               m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectConfigModeOptions;
               break;
            default: InvalidEnumValue("TrainerOptions::ConfigModeStateType", m_MenuOptions.m_TrainerOptions.m_ConfigModeState); break;
            }
            break;
         case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Pass:
            m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled = !m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled;
            break;
         case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Fail:
            m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled = !m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled;
            break;
         case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_TimeLow:
            m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled = !m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled;
            break;
         case TrainerOptions::SoundEffectsModeType::SoundEffectsModeType_Countdown:
            m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled = !m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled;
            break;
         default: InvalidEnumValue("TrainerOptions::SoundEffectsModeType", m_MenuOptions.m_TrainerOptions.m_SoundEffectsMode); break;
         }
         m_MenuOptions.m_TrainerOptions.m_SoundEffectsMenuPlayed = false;
         break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   case MenuOptionsRecord::MenuStateType::MenuStateType_Disabled_Disabled:
      PrintScreenRecord::MenuTitleText("Ball Control Disabled");

      ShowSection(DescriptionSectionTitle,
         { ("Press Ball History Menu key '" + std::string(1, GetBallHistoryKey(player, eBallHistoryMenu)) + "' to continue play").c_str(), "Plunger returns to previous menu" });

      switch (menuAction)
      {
      case MenuOptionsRecord::MenuActionType::MenuActionType_None:
      case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
      case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft:
      case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight:
         // do nothing
         break;
      case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Root_SelectMode; break;
      default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
      }
      break;
   default: InvalidEnumValue("MenuOptionsRecord::MenuStateType", m_MenuOptions.m_MenuState); break;
   }

   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Root_SelectMode:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectModeOptions:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectCurrentBallHistory:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ConfigureRecallBallHistory:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectRecallBallHistory:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Normal_ManageAutoControlLocations:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_SelectExistingBallStartLocation:
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results: DrawBallHistory(player); break;
   default:
      // do nothing;
      break;
   }

   switch (m_MenuOptions.m_ModeType)
   {
   case MenuOptionsRecord::ModeType::ModeType_Normal: DrawNormalModeVisuals(player, currentTimeMs); break;
   case MenuOptionsRecord::ModeType::ModeType_Trainer: DrawTrainerModeVisuals(player, currentTimeMs); break;
   case MenuOptionsRecord::ModeType::ModeType_Disabled:
      // do nothing
      break;
   default: InvalidEnumValue("MenuOptionsRecord::ModeType", m_MenuOptions.m_ModeType); break;
   }
}

void BallHistory::ProcessMode(Player& player, int currentTimeMs)
{
   if (!m_Control)
   {
      switch (m_MenuOptions.m_ModeType)
      {
      case MenuOptionsRecord::ModeType::ModeType_Normal: ProcessModeNormal(player); break;
      case MenuOptionsRecord::ModeType::ModeType_Trainer: ProcessModeTrainer(player, currentTimeMs); break;
      case MenuOptionsRecord::ModeType::ModeType_Disabled:
         // do nothing
         break;
      default: InvalidEnumValue("MenuOptionsRecord::ModeType", m_MenuOptions.m_ModeType); break;
      }
   }
}

void BallHistory::ProcessModeNormal(Player& player)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_ProcessModeNormalUsec);
   if (BallInsideAutoControlVertex(m_ControlVBalls))
   {
      SetControl(true);
      m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Normal_SelectCurrentBallHistory;
      ToggleRecall();
   }
}

void BallHistory::ProcessModeTrainer(Player& player, int currentTimeMs)
{
   ProfilerRecord::ProfilerScope profilerScope(m_ProfilerRecord.m_ProcessModeTrainerUsec);
   switch (m_MenuOptions.m_MenuState)
   {
   case MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results: break;
   default:
      // everything else
      return;
   }

   std::string errorMessage;
   bool cancelRun = false;

   for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
   {
      TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
      if (bsor.IsZero())
      {
         errorMessage = "Trainer run cancelled - Ball Start " + std::to_string(bsorIndex + 1) + " must be set";
         cancelRun = true;
      }
   }

   if (m_MenuOptions.m_TrainerOptions.m_RunRecords.size() > 0 && m_MenuOptions.m_TrainerOptions.m_RunRecords.size() < m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord)
   {
      TrainerOptions::RunRecord& currentRunRecord = m_MenuOptions.m_TrainerOptions.m_RunRecords[m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord];
      if (currentRunRecord.m_StartPositions.size() == 0 || currentRunRecord.m_StartVelocities.size() == 0 || currentRunRecord.m_StartAngularMomentums.size() == 0)
      {
         cancelRun = true;
      }
   }

   if (cancelRun)
   {
      m_MenuOptions.m_MenuError = errorMessage;

      ToggleControl();
      m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results;
      m_MenuOptions.m_TrainerOptions.m_ModeState = TrainerOptions::ModeStateType::ModeStateType_Config;
      return;
   }

   if (m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs == 0)
   {
      if (m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord == 0)
      {
         m_MenuOptions.m_TrainerOptions.m_RunRecords.clear();

         std::vector<std::vector<float>> angleAndVelocityPairs;
         for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
         {
            TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];

            float angleStep = 0.0f;
            float velocityStep = 0.0f;
            CalculateAngleVelocityStep(bsor, angleStep, velocityStep);

            angleAndVelocityPairs.emplace_back();
            if (bsor.m_TotalRangeAngles == 0)
            {
               angleAndVelocityPairs.back().push_back(0.0f);
            }
            else
            {
               for (S32 angleIndex = 0; angleIndex < bsor.m_TotalRangeAngles; angleIndex++)
               {
                  angleAndVelocityPairs.back().push_back(std::fmodf(bsor.m_AngleRangeStart + (angleStep * angleIndex), TrainerOptions::BallStartOptionsRecord::AngleMaximum));
               }
            }

            angleAndVelocityPairs.emplace_back();
            if (bsor.m_TotalRangeVelocities == 0)
            {
               angleAndVelocityPairs.back().push_back(0.0f);
            }
            else
            {
               for (S32 velocityIndex = 0; velocityIndex < bsor.m_TotalRangeVelocities; velocityIndex++)
               {
                  angleAndVelocityPairs.back().push_back(std::fmodf(bsor.m_VelocityRangeStart + (velocityStep * velocityIndex), TrainerOptions::BallStartOptionsRecord::VelocityMaximum + 1));
               }
            }
         }

         // following code is adaptation of algorithm code found here:
         // https://www.geeksforgeeks.org/combinations-from-n-arrays-picking-one-element-from-each-array/
         std::vector<std::size_t> indexes(angleAndVelocityPairs.size(), 0);
         while (true)
         {
            m_MenuOptions.m_TrainerOptions.m_RunRecords.push_back(TrainerOptions::RunRecord());
            for (std::size_t bsorIndex = 0; bsorIndex < m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize; bsorIndex++)
            {
               TrainerOptions::BallStartOptionsRecord& bsor = m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecords[bsorIndex];
               m_MenuOptions.m_TrainerOptions.m_RunRecords.back().m_StartPositions.push_back(bsor.m_StartPosition);
               m_MenuOptions.m_TrainerOptions.m_RunRecords.back().m_StartVelocities.push_back(bsor.m_StartVelocity);

               if (bsor.m_TotalRangeAngles > 0)
               {
                  std::size_t angleAndVelocityPairsIndex = bsorIndex * 2; // angles and velocities
                  // TODO GARY do some ad-hoc testing to make sure the velocity translation into the
                  // other vector direction keeps the expected velocity
                  Vertex3Ds& startVelocity = m_MenuOptions.m_TrainerOptions.m_RunRecords.back().m_StartVelocities.back();
                  startVelocity.x = std::sinf(
                     (std::fmodf(angleAndVelocityPairs[angleAndVelocityPairsIndex][indexes[angleAndVelocityPairsIndex]], TrainerOptions::BallStartOptionsRecord::AngleMaximum) * float(M_PI))
                     / 180.0f);
                  startVelocity.x *= angleAndVelocityPairs[angleAndVelocityPairsIndex + 1][indexes[angleAndVelocityPairsIndex + 1]];
                  startVelocity.y = std::cosf(
                     (std::fmodf(angleAndVelocityPairs[angleAndVelocityPairsIndex][indexes[angleAndVelocityPairsIndex]], TrainerOptions::BallStartOptionsRecord::AngleMaximum) * float(M_PI))
                     / 180.0f);
                  startVelocity.y *= angleAndVelocityPairs[angleAndVelocityPairsIndex + 1][indexes[angleAndVelocityPairsIndex + 1]] * -1;
               }

               m_MenuOptions.m_TrainerOptions.m_RunRecords.back().m_StartAngularMomentums.push_back(bsor.m_StartAngularMomentum);
            }

            S32 next = S32(indexes.size() - 1);
            while (next >= 0 && (indexes[next] + 1 >= angleAndVelocityPairs[next].size()))
            {
               next--;
            }

            if (next < 0)
            {
               break;
            }

            indexes[next]++;

            for (std::size_t x = next + 1; x < indexes.size(); x++)
            {
               indexes[x] = 0;
            }
         }

         std::size_t totalRunRecords = m_MenuOptions.m_TrainerOptions.m_RunRecords.size();
         for (S32 totalRuns = 1; totalRuns < m_MenuOptions.m_TrainerOptions.m_TotalRuns; totalRuns++) // start at 1 to skip first
         {
            m_MenuOptions.m_TrainerOptions.m_RunRecords.resize(m_MenuOptions.m_TrainerOptions.m_RunRecords.size() + totalRunRecords);
            std::copy(m_MenuOptions.m_TrainerOptions.m_RunRecords.begin(), m_MenuOptions.m_TrainerOptions.m_RunRecords.begin() + totalRunRecords,
               m_MenuOptions.m_TrainerOptions.m_RunRecords.begin() + (totalRuns * totalRunRecords));
         }

         switch (m_MenuOptions.m_TrainerOptions.m_RunOrderMode)
         {
         case TrainerOptions::RunOrderModeType::RunOrderModeType_InOrder:
            // do nothing;
            break;
         case TrainerOptions::RunOrderModeType::RunOrderModeType_Random:
         {
            std::random_device randomDevice;
            std::mt19937 randomGenerator(randomDevice());
            std::shuffle(m_MenuOptions.m_TrainerOptions.m_RunRecords.begin(), m_MenuOptions.m_TrainerOptions.m_RunRecords.end(), randomGenerator);
         }
         break;
         default: InvalidEnumValue("TrainerOptions::RunOrderModeType", m_MenuOptions.m_TrainerOptions.m_RunOrderMode); break;
         }

         player.m_ptable->put_Gravity(m_MenuOptions.m_TrainerOptions.m_GravityInitial);
         player.m_ptable->put_Friction(m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionInitial);
         SetFlipperStrength(m_MenuOptions.m_TrainerOptions.m_FlipperStrengthInitial);
         SetFlipperFriction(m_MenuOptions.m_TrainerOptions.m_FlipperFrictionInitial);
      }

      m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = currentTimeMs;

      for (std::size_t bporIndex = 0; bporIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); bporIndex++)
      {
         TrainerOptions::BallEndOptionsRecord& bpor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[bporIndex];
         bpor.m_StopBallsTracker.resize(m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize, std::make_tuple<int, Vertex3Ds>(0, { 0.0f, 0.0f, 0.0f }));
      }
      for (std::size_t bforIndex = 0; bforIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); bforIndex++)
      {
         TrainerOptions::BallEndOptionsRecord& bfor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[bforIndex];
         bfor.m_StopBallsTracker.resize(m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize, std::make_tuple<int, Vertex3Ds>(0, { 0.0f, 0.0f, 0.0f }));
      }
   }

   if (m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord == m_MenuOptions.m_TrainerOptions.m_RunRecords.size())
   {
      ToggleControl();
      m_MenuOptions.m_MenuState = MenuOptionsRecord::MenuStateType::MenuStateType_Trainer_Results;
      m_MenuOptions.m_TrainerOptions.m_ModeState = TrainerOptions::ModeStateType::ModeStateType_Results;
      return;
   }

   ShowRemainingRunInfo();

   ShowPreviousRunRecord();

   ShowCurrentRunRecord(currentTimeMs);

   TrainerOptions::RunRecord& currentRunRecord = m_MenuOptions.m_TrainerOptions.m_RunRecords[m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord];
   S32 runElapsedTimeMs = currentTimeMs - m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs;
   if (runElapsedTimeMs == 0 || runElapsedTimeMs < (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * S32(OneSecondMs)))
   {
      for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
      {
         m_ControlVBalls[controlVBallIndex]->m_d.m_pos = currentRunRecord.m_StartPositions[controlVBallIndex];
         m_ControlVBalls[controlVBallIndex]->m_d.m_vel.SetZero();
         m_ControlVBalls[controlVBallIndex]->m_angularmomentum.SetZero();
      }

      if (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun > 0)
      {
         // countdown before run starts
         float secondsBeforeStart = ((m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs) - runElapsedTimeMs) / 1000.0f;
         if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled)
         {
            if (secondsBeforeStart < TrainerOptions::CountdownSoundSeconds && secondsBeforeStart < m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed)
            {
               PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_COUNTDOWN_READYSET, true);
               m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = int(secondsBeforeStart);
            }
         }

         PrintScreenRecord::MenuTitleText(
            "Run %zu (of %zu) starts in %.2f seconds", m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord + 1, m_MenuOptions.m_TrainerOptions.m_RunRecords.size(), secondsBeforeStart);
         if (!m_MenuOptions.m_MenuError.empty())
         {
            PrintScreenRecord::ErrorText("%s", m_MenuOptions.m_MenuError.c_str());
         }
      }

      player.m_renderer->m_trailForBalls = 0;
   }
   else if (runElapsedTimeMs < ((m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * S32(OneSecondMs)) + (m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun * S32(OneSecondMs))))
   {
      if (m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed != -1)
      {
         if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsCountdownEnabled)
         {
            PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_COUNTDOWN_GO, true);
            m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = -1;
         }
      }

      m_MenuOptions.m_MenuError.clear();
      if (m_MenuOptions.m_TrainerOptions.m_SetupBallStarts)
      {
         Init(player, currentTimeMs, false);

         bool anyVelocityAngularMomentumSet = false;
         for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); ++controlVBallIndex)
         {
            m_ControlVBalls[controlVBallIndex]->m_d.m_pos = currentRunRecord.m_StartPositions[controlVBallIndex];
            m_ControlVBalls[controlVBallIndex]->m_d.m_vel = currentRunRecord.m_StartVelocities[controlVBallIndex];
            m_ControlVBalls[controlVBallIndex]->m_angularmomentum = currentRunRecord.m_StartAngularMomentums[controlVBallIndex];

            if (m_ControlVBalls[controlVBallIndex]->m_d.m_vel.IsZero() == false || m_ControlVBalls[controlVBallIndex]->m_angularmomentum.IsZero() == false)
            {
               m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = false;
            }

            if (currentRunRecord.m_StartVelocities[controlVBallIndex].IsZero() == false || currentRunRecord.m_StartAngularMomentums[controlVBallIndex].IsZero() == false)
            {
               anyVelocityAngularMomentumSet = true;
            }
         }

         if (anyVelocityAngularMomentumSet == false)
         {
            m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = false;
            player.m_renderer->m_trailForBalls = m_UseTrailsForBallsInitialValue;
         }
      }

      if (m_MenuOptions.m_TrainerOptions.m_SetupDifficulty)
      {
         player.m_ptable->SetGlobalDifficulty(float(m_MenuOptions.m_TrainerOptions.m_GameplayDifficulty));

         if (m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord > 0)
         {
            if (m_MenuOptions.m_TrainerOptions.m_GravityVariance > 0)
            {
               float currentGravity = 0.0f;
               player.m_ptable->get_Gravity(&currentGravity);
               float newGravity = CalculateDifficultyVariance(player, m_MenuOptions.m_TrainerOptions.m_GravityInitial, currentGravity, m_MenuOptions.m_TrainerOptions.m_GravityVariance,
                  m_MenuOptions.m_TrainerOptions.m_GravityVarianceMode);

               player.m_ptable->put_Gravity(newGravity);
            }

            if (m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance > 0)
            {
               float currentPlayfieldFriction = 0.0f;
               player.m_ptable->get_Friction(&currentPlayfieldFriction);
               float newPlayfieldFriction = CalculateDifficultyVariance(player, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionInitial, currentPlayfieldFriction,
                  m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVariance, m_MenuOptions.m_TrainerOptions.m_PlayfieldFrictionVarianceMode);

               player.m_ptable->put_Friction(newPlayfieldFriction);
            }

            if (m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance > 0)
            {
               float currentFlipperStrength = GetFlipperStrength();
               float newFlipperStrength = CalculateDifficultyVariance(player, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthInitial, currentFlipperStrength,
                  m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVariance, m_MenuOptions.m_TrainerOptions.m_FlipperStrengthVarianceMode);

               SetFlipperStrength(newFlipperStrength);
            }

            if (m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance > 0)
            {
               float currentFlipperFriction = GetFlipperFriction();
               float newFlipperFriction = CalculateDifficultyVariance(player, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionInitial, currentFlipperFriction,
                  m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVariance, m_MenuOptions.m_TrainerOptions.m_FlipperFrictionVarianceMode);

               SetFlipperFriction(newFlipperFriction);
            }
         }

         m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = false;
      }

      float remainingRunTime
         = ((m_MenuOptions.m_TrainerOptions.m_MaxSecondsPerRun * OneSecondMs) - runElapsedTimeMs + (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs)) / 1000.0f;
      if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsTimeLowEnabled && remainingRunTime < TrainerOptions::TimeLowSoundSeconds && !m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying)
      {
         PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_TIME_LOW, true);
         m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = true;
      }

      if (m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size() > 0)
      {
         bool allPass = true;
         std::vector<std::tuple<std::size_t, std::size_t>> startToPassLocationIndexes;
         for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); controlVBallIndex++)
         {
            HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];
            bool controlVBallAssociated = false;
            bool anyPassBeor = false;

            for (std::size_t passBeorIndex = 0; passBeorIndex < m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size(); passBeorIndex++)
            {
               TrainerOptions::BallEndOptionsRecord& passBeor = m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords[passBeorIndex];
               if (passBeor.m_AssociatedBallStartIndexes.find(controlVBallIndex) != passBeor.m_AssociatedBallStartIndexes.end())
               {
                  controlVBallAssociated = true;
                  float distance = DistancePixels(passBeor.m_EndPosition, controlVBall.m_d.m_pos);
                  if (passBeor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
                  {
                     int& stopBallMs = std::get<0>(passBeor.m_StopBallsTracker[controlVBallIndex]);
                     Vertex3Ds& stopBallPos = std::get<1>(passBeor.m_StopBallsTracker[controlVBallIndex]);
                     if (distance < controlVBall.m_d.m_radius)
                     {
                        if (stopBallMs == 0)
                        {
                           stopBallMs = currentTimeMs;
                        }

                        if ((controlVBall.m_d.m_pos - stopBallPos).Length() < 1.0f)
                        {
                           if ((currentTimeMs - stopBallMs) > 200)
                           {
                              startToPassLocationIndexes.push_back(std::tuple<std::size_t, std::size_t>(controlVBallIndex, passBeorIndex));
                              anyPassBeor = true;
                              break;
                           }
                        }
                        else
                        {
                           stopBallMs = currentTimeMs;
                           stopBallPos = controlVBall.m_d.m_pos;
                        }
                     }
                     else
                     {
                        stopBallMs = 0;
                        stopBallPos.SetZero();
                     }
                  }
                  else if (distance < (controlVBall.m_d.m_radius * passBeor.m_EndRadiusPercent / 100.0f))
                  {
                     startToPassLocationIndexes.push_back(std::tuple<std::size_t, std::size_t>(controlVBallIndex, passBeorIndex));
                     anyPassBeor = true;
                     break;
                  }
               }
            }

            if (controlVBallAssociated && !anyPassBeor)
            {
               allPass = false;
               break;
            }
         }

         if (allPass == true)
         {
            currentRunRecord.m_Result = TrainerOptions::RunRecord::ResultType::ResultType_PassedLocation;
            currentRunRecord.m_TotalTimeMs = runElapsedTimeMs - (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs);
            currentRunRecord.m_StartToPassLocationIndexes = startToPassLocationIndexes;
            m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
            m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
            m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
            m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
            m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
            m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord++;
            if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled)
            {
               PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_PASS);
            }
         }
      }

      bool anyFail = false;
      std::vector<std::tuple<std::size_t, std::size_t>> startToFailLocationIndexes;
      for (std::size_t failBeorIndex = 0; failBeorIndex < m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords.size(); failBeorIndex++)
      {
         TrainerOptions::BallEndOptionsRecord& failBeor = m_MenuOptions.m_TrainerOptions.m_BallFailOptionsRecords[failBeorIndex];

         for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); controlVBallIndex++)
         {
            HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];

            if (failBeor.m_AssociatedBallStartIndexes.find(controlVBallIndex) != failBeor.m_AssociatedBallStartIndexes.end())
            {
               float distance = DistancePixels(failBeor.m_EndPosition, controlVBall.m_d.m_pos);
               if (failBeor.m_EndRadiusPercent == TrainerOptions::BallEndOptionsRecord::RadiusPercentDisabled)
               {
                  int& stopBallMs = std::get<0>(failBeor.m_StopBallsTracker[controlVBallIndex]);
                  Vertex3Ds& stopBallPos = std::get<1>(failBeor.m_StopBallsTracker[controlVBallIndex]);
                  if (distance < controlVBall.m_d.m_radius)
                  {
                     if (stopBallMs == 0)
                     {
                        stopBallMs = currentTimeMs;
                     }

                     if ((controlVBall.m_d.m_pos - stopBallPos).Length() < 1.0f)
                     {
                        if ((currentTimeMs - stopBallMs) > 200)
                        {
                           startToFailLocationIndexes.push_back(std::tuple<std::size_t, std::size_t>(controlVBallIndex, failBeorIndex));
                           anyFail = true;
                        }
                     }
                     else
                     {
                        stopBallMs = currentTimeMs;
                        stopBallPos = controlVBall.m_d.m_pos;
                     }
                  }
                  else
                  {
                     stopBallMs = 0;
                     stopBallPos.SetZero();
                  }
               }
               else if (distance < (controlVBall.m_d.m_radius * failBeor.m_EndRadiusPercent / 100.0f))
               {
                  startToFailLocationIndexes.push_back(std::tuple<std::size_t, std::size_t>(controlVBallIndex, failBeorIndex));
                  anyFail = true;
               }
            }
         }

         if (anyFail == true)
         {
            break;
         }
      }
      if (anyFail == true)
      {
         currentRunRecord.m_Result = TrainerOptions::RunRecord::ResultType::ResultType_FailedLocation;
         currentRunRecord.m_TotalTimeMs = runElapsedTimeMs - (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs);
         currentRunRecord.m_StartToFailLocationIndexes = startToFailLocationIndexes;
         m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
         m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
         m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
         m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
         m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
         m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord++;
         if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled)
         {
            PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_FAIL);
         }
      }

      if (BallCorridorReadyForTrainer())
      {
         TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;
         float passWidth = GetDefaultBallRadius() * (bcor.m_PassRadiusPercent / 100.0f);
         Vertex3Ds passPositionLeft = bcor.m_PassPosition - Vertex3Ds(passWidth, 0.0f, 0.0f);
         Vertex3Ds passPositionRight = bcor.m_PassPosition + Vertex3Ds(passWidth, 0.0f, 0.0f);
         {
            std::size_t startToPassCorridorIndex = 0;
            TrainerOptions::RunRecord::ResultType resultType = TrainerOptions::RunRecord::ResultType::ResultType_Unknown;
            for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); controlVBallIndex++)
            {
               HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];
               float distanceToPassLine = DistanceToLineSegment(passPositionLeft, passPositionRight, controlVBall.m_d.m_pos);
               if (distanceToPassLine < controlVBall.m_d.m_radius)
               {
                  startToPassCorridorIndex = controlVBallIndex;
                  resultType = TrainerOptions::RunRecord::ResultType::ResultType_PassedCorridor;
                  break;
               }
            }

            if (resultType != TrainerOptions::RunRecord::ResultType::ResultType_Unknown)
            {
               currentRunRecord.m_Result = TrainerOptions::RunRecord::ResultType::ResultType_PassedCorridor;
               currentRunRecord.m_TotalTimeMs = runElapsedTimeMs - (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs);
               currentRunRecord.m_StartToPassCorridorIndex = startToPassCorridorIndex;
               m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
               m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
               m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
               m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
               m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
               m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord++;
               if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled)
               {
                  PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_PASS);
               }
            }
         }

         {
            std::size_t startToFailCorridorIndex = 0;
            TrainerOptions::RunRecord::ResultType resultType = TrainerOptions::RunRecord::ResultType::ResultType_Unknown;
            for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); controlVBallIndex++)
            {
               HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];

               float distanceToFailLeft = 0.0f;
               float distanceToFailRight = 0.0f;

               if (bcor.m_OpeningPositionLeft.x < bcor.m_OpeningPositionRight.x)
               {
                  distanceToFailLeft = DistanceToLineSegment(passPositionLeft, bcor.m_OpeningPositionLeft, controlVBall.m_d.m_pos);
                  distanceToFailRight = DistanceToLineSegment(passPositionRight, bcor.m_OpeningPositionRight, controlVBall.m_d.m_pos);
               }
               else
               {
                  distanceToFailLeft = DistanceToLineSegment(passPositionLeft, bcor.m_OpeningPositionRight, controlVBall.m_d.m_pos);
                  distanceToFailRight = DistanceToLineSegment(passPositionRight, bcor.m_OpeningPositionLeft, controlVBall.m_d.m_pos);
               }

               if (distanceToFailLeft < controlVBall.m_d.m_radius)
               {
                  resultType = TrainerOptions::RunRecord::ResultType::ResultType_FailedCorridorLeft;
                  startToFailCorridorIndex = controlVBallIndex;
                  break;
               }
               else if (distanceToFailRight < controlVBall.m_d.m_radius)
               {
                  resultType = TrainerOptions::RunRecord::ResultType::ResultType_FailedCorridorRight;
                  startToFailCorridorIndex = controlVBallIndex;
                  break;
               }
            }

            if (resultType != TrainerOptions::RunRecord::ResultType::ResultType_Unknown)
            {
               currentRunRecord.m_Result = resultType;
               currentRunRecord.m_TotalTimeMs = runElapsedTimeMs - (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs);
               currentRunRecord.m_StartToFailCorridorIndex = startToFailCorridorIndex;
               m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
               m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
               m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
               m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
               m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
               m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord++;
               if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsPassEnabled)
               {
                  PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_FAIL);
               }
            }
         }
      }

      bool oneKicker = false;
      for (std::size_t controlVBallIndex = 0; controlVBallIndex < m_ControlVBalls.size(); controlVBallIndex++)
      {
         HitBall& controlVBall = *m_ControlVBalls[controlVBallIndex];
         for (std::size_t activeBallKickerIndex = 0; activeBallKickerIndex < m_ActiveBallKickers.size(); activeBallKickerIndex++)
         {
            Vertex3Ds kickerPosition = GetKickerPosition(*m_ActiveBallKickers[activeBallKickerIndex]);
            float distance = DistancePixels(kickerPosition, controlVBall.m_d.m_pos);

            //PrintScreenRecord::MenuText(false, "%.2f", distance);
            // TODO GARY this calculation is not correct, the distance should be
            // the radius of the ball plus the radius of the kick circle OR,
            // center of 3drect/box to edge like radius of a box
            if (distance < (controlVBall.m_d.m_radius * 2.0f))
            {
               oneKicker = true;
               break;
            }
         }
         if (oneKicker == true)
         {
            break;
         }
      }
      if (oneKicker == true)
      {
         switch (m_MenuOptions.m_TrainerOptions.m_BallKickerBehaviorMode)
         {
         case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Reset:
            m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
            m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
            m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
            m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
            m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
            m_MenuOptions.m_MenuError = "Trainer run reset - ball hit active kicker";
            break;
         case TrainerOptions::BallKickerBehaviorModeType::BallKickerBehaviorModeType_Fail:
            currentRunRecord.m_Result = TrainerOptions::RunRecord::ResultType::ResultType_FailedKicker;
            currentRunRecord.m_TotalTimeMs = runElapsedTimeMs - (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs);
            m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
            m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
            m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
            m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
            m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
            m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord++;
            if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled)
            {
               PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_FAIL);
            }
            break;
         default: InvalidEnumValue("TrainerOptions::BallKickerBehaviorModeType", m_MenuOptions.m_ModeType); break;
         }
      }
   }
   else
   {
      currentRunRecord.m_Result = TrainerOptions::RunRecord::ResultType::ResultType_FailedTimeElapsed;
      currentRunRecord.m_TotalTimeMs = runElapsedTimeMs - (m_MenuOptions.m_TrainerOptions.m_CountdownSecondsBeforeRun * OneSecondMs);
      m_MenuOptions.m_TrainerOptions.m_SetupBallStarts = true;
      m_MenuOptions.m_TrainerOptions.m_SetupDifficulty = true;
      m_MenuOptions.m_TrainerOptions.m_RunStartTimeMs = 0;
      m_MenuOptions.m_TrainerOptions.m_CountdownSoundPlayed = TrainerOptions::CountdownSoundSeconds;
      m_MenuOptions.m_TrainerOptions.m_TimeLowSoundPlaying = false;
      m_MenuOptions.m_TrainerOptions.m_CurrentRunRecord++;
      if (m_MenuOptions.m_TrainerOptions.m_SoundEffectsFailEnabled)
      {
         PlaySound(ID_BALL_HISTORY_SOUND_EFFECT_FAIL);
      }
   }
}

S32 BallHistory::ProcessMenuChangeValue(S32 value, S32 delta, S32 min, S32 max, bool skip)
{
   value += delta;

   if (value > max)
   {
      value = min + (value - max) - 1;
   }
   else if (value < min)
   {
      value = max - (min - value) + 1;
   }

   if (skip && value % 10 != 0)
   {
      if (delta > 0)
      {
         value -= value % 10;
      }
      else if (delta < 0)
      {
         value += 10 - (value % 10);
      }
   }

   return value;
}

template <class T> void BallHistory::ProcessMenuChangeValueInc(T& value, S32 min, S32 max)
{
   S32 tempValue = S32(value);
   if (tempValue == max)
   {
      tempValue = min;
   }
   else
   {
      tempValue++;
   }
   value = T(tempValue);
}

template <class T> void BallHistory::ProcessMenuChangeValueIncSkip(T& value, S32 min, S32 max)
{
   S32 tempValue = S32(value);
   if (tempValue == max)
   {
      tempValue = min;
   }
   else
   {
      tempValue += m_MenuOptions.SkipKeyStepFactor;
      if (tempValue > max)
      {
         tempValue = max;
      }
      else
      {
         tempValue = tempValue - (tempValue % m_MenuOptions.SkipKeyStepFactor);
      }
   }
   value = T(tempValue);
}

template <class T> void BallHistory::ProcessMenuChangeValueDec(T& value, S32 min, S32 max)
{
   S32 tempValue = S32(value);
   if (tempValue == min)
   {
      tempValue = max;
   }
   else
   {
      tempValue--;
   }
   value = T(tempValue);
}

template <class T> void BallHistory::ProcessMenuChangeValueDecSkip(T& value, S32 min, S32 max)
{
   S32 tempValue = S32(value);
   if (tempValue == min)
   {
      tempValue = max;
   }
   else
   {
      tempValue -= m_MenuOptions.SkipKeyStepFactor;
      if (tempValue < min)
      {
         tempValue = min;
      }
      else if (tempValue % m_MenuOptions.SkipKeyStepFactor != 0)
      {
         tempValue = tempValue + (m_MenuOptions.SkipKeyStepFactor - (tempValue % m_MenuOptions.SkipKeyStepFactor));
      }
   }
   value = T(tempValue);
}

template <class T> void BallHistory::ProcessMenuChangeValueSkip(T& value, S32 min, S32 max, int currentTimeMs)
{
   if (m_MenuOptions.m_SkipKeyPressed && (currentTimeMs - m_MenuOptions.m_SkipKeyPressedMs) > MenuOptionsRecord::SkipKeyIntervalMs)
   {
      if ((currentTimeMs - m_MenuOptions.m_SkipKeyUsedMs) > MenuOptionsRecord::SkipKeyIntervalMs)
      {
         if (m_MenuOptions.m_SkipKeyLeft == true)
         {
            ProcessMenuChangeValueDecSkip(value, min, max);
         }
         else
         {
            ProcessMenuChangeValueIncSkip(value, min, max);
         }
         m_MenuOptions.m_SkipKeyUsedMs = currentTimeMs;
      }
   }
}

template <class T>
void BallHistory::ProcessMenuAction(MenuOptionsRecord::MenuActionType menuAction, MenuOptionsRecord::MenuStateType enterMenuState, T& value, S32 minimum, S32 maximum, int currentTimeMs)
{
   switch (menuAction)
   {
   case MenuOptionsRecord::MenuActionType::MenuActionType_None: ProcessMenuChangeValueSkip<T>(value, minimum, maximum, currentTimeMs); break;
   case MenuOptionsRecord::MenuActionType::MenuActionType_Toggle:
      // do nothing
      break;
   case MenuOptionsRecord::MenuActionType::MenuActionType_UpLeft: ProcessMenuChangeValueDec<T>(value, minimum, maximum); break;
   case MenuOptionsRecord::MenuActionType::MenuActionType_DownRight: ProcessMenuChangeValueInc<T>(value, minimum, maximum); break;
   case MenuOptionsRecord::MenuActionType::MenuActionType_Enter: m_MenuOptions.m_MenuState = enterMenuState; break;
   default: InvalidEnumValue("MenuOptionsRecord::MenuActionType", menuAction); break;
   }
}

void BallHistory::Add(std::vector<HitBall*>& controlVBalls, int currentTimeMs)
{
   if (!controlVBalls.empty())
   {
      std::size_t prevBallHistoryRecordsSize = m_BallHistoryRecordsSize;
      std::size_t prevBallHistoryRecordsHeadIndex = m_BallHistoryRecordsHeadIndex;
      std::size_t prevCurrentControlIndex = m_CurrentControlIndex;
      std::size_t prevRecallControlIndex = m_MenuOptions.m_NormalOptions.m_RecallControlIndex;
      bool recallIndexIsLast = m_MenuOptions.m_NormalOptions.m_RecallControlIndex == GetTailIndex() && m_BallHistoryRecordsSize == m_BallHistoryRecords.size();

      if (m_BallHistoryRecordsSize)
      {
         m_BallHistoryRecordsHeadIndex++;
         if (m_BallHistoryRecordsHeadIndex >= m_BallHistoryRecords.size())
         {
            m_BallHistoryRecordsHeadIndex = 0;
         }
      }
      if (m_BallHistoryRecordsSize < m_BallHistoryRecords.size())
      {
         m_BallHistoryRecordsSize++;
      }
      m_CurrentControlIndex = m_BallHistoryRecordsHeadIndex;
      if (recallIndexIsLast)
      {
         m_MenuOptions.m_NormalOptions.m_RecallControlIndex = GetTailIndex();
      }

      BallHistoryRecord& ballHistoryRecordHead = m_BallHistoryRecords[m_BallHistoryRecordsHeadIndex];
      ballHistoryRecordHead.Set(controlVBalls, currentTimeMs);

      for (std::vector<HitBall*>::iterator it = controlVBalls.begin(); it != controlVBalls.end(); ++it)
      {
         m_MaxBallVelocityPixels = max(VelocityPixels((*it)->m_d.m_vel), m_MaxBallVelocityPixels);
      }

      if (m_BallHistoryRecordsSize > 1)
      {
         bool differenceFound = false;
         BallHistoryRecord& prevBallHistoryRecordHead = m_BallHistoryRecords[prevBallHistoryRecordsHeadIndex];
         for (std::size_t ballHistoryStateIndex = 0; ballHistoryStateIndex < prevBallHistoryRecordHead.m_BallHistoryStates.size(); ballHistoryStateIndex++)
         {
            if (DistancePixels(ballHistoryRecordHead.m_BallHistoryStates[ballHistoryStateIndex].m_Position, prevBallHistoryRecordHead.m_BallHistoryStates[ballHistoryStateIndex].m_Position)
               > 1.0f)
            {
               differenceFound = true;
               break;
            }
         }

         if (!differenceFound)
         {
            m_BallHistoryRecordsSize = prevBallHistoryRecordsSize;
            m_BallHistoryRecordsHeadIndex = prevBallHistoryRecordsHeadIndex;
            m_CurrentControlIndex = prevCurrentControlIndex;
            m_MenuOptions.m_NormalOptions.m_RecallControlIndex = prevRecallControlIndex;
         }
      }
   }
}

BallHistoryRecord& BallHistory::Get(std::size_t index) { return m_BallHistoryRecords[index]; }

std::size_t BallHistory::GetTailIndex()
{
   std::size_t tailIndex = m_BallHistoryRecordsHeadIndex;
   std::size_t backStep = std::min(m_BallHistoryRecordsHeadIndex, m_BallHistoryRecordsSize);
   tailIndex = tailIndex - backStep;
   std::size_t backStepRemaining = m_BallHistoryRecordsSize - backStep;
   if (backStepRemaining > 1)
   {
      tailIndex = m_BallHistoryRecords.size() - backStepRemaining + 1;
   }
   return tailIndex;
}

float BallHistory::GetDefaultBallRadius()
{
   HitBall* controlVBall = m_ControlVBalls.size() ? m_ControlVBalls[0] : nullptr;
   return (controlVBall ? controlVBall->m_d.m_radius : MenuOptionsRecord::DefaultBallRadius);
}

Matrix3 BallHistory::GetDefaultBallOrientation()
{
   Matrix3 orientation;
   orientation.SetIdentity();
   if (HitBall* controlVBall = m_ControlVBalls.size() ? m_ControlVBalls[0] : nullptr)
   {
      orientation = controlVBall->m_orientation;
   }

   return orientation;
}

float BallHistory::DistancePixels(POINT& p1, POINT& p2) { return sqrtf(float(powl(p1.x - p2.x, 2)) + float(powl(p1.y - p2.y, 2))); }

float BallHistory::DistancePixels(const Vertex3Ds& pos1, const Vertex3Ds& pos2) { return sqrtf(powf((pos1.x - pos2.x), 2) + powf((pos1.y - pos2.y), 2) + powf((pos1.z - pos2.z), 2)); }

float BallHistory::DistanceToLineSegment(const Vertex3Ds& lineA, const Vertex3Ds& lineB, const Vertex3Ds& point)
{
   Vertex3Ds lineAB = lineB - lineA;
   Vertex3Ds lineAP = point - lineA;
   Vertex3Ds lineBP = point - lineB;

   float lineABSquared = powf(lineAB.Length(), 2.0f);
   if (lineABSquared == 0.0f)
   {
      return lineAP.Length();
   }

   float projectionFactor = lineAP.Dot(lineAB) / lineABSquared;
   if (projectionFactor < 0.0f)
   {
      return lineAP.Length();
   }
   else if (projectionFactor > 1.0f)
   {
      return lineBP.Length();
   }
   else
   {
      Vertex3Ds closest = { lineA.x + projectionFactor * lineAB.x, lineA.y + projectionFactor * lineAB.y, lineA.z + projectionFactor * lineAB.z };
      Vertex3Ds vectorToClosestPoint = { point.x - closest.x, point.y - closest.y, point.z - closest.z };
      return vectorToClosestPoint.Length();
   }
}

float BallHistory::VelocityPixels(const Vertex3Ds& vel) { return sqrtf(powf(vel.x, 2) + powf(vel.y, 2) + powf(vel.z, 2)); }

char BallHistory::GetBallHistoryKey(Player& player, EnumAssignKeys enumAssignKey) { return get_vk(player.m_rgKeys[enumAssignKey]); }

bool BallHistory::BallsReadyForTrainer()
{
   m_MenuOptions.m_MenuError.clear();
   std::ostringstream strStream;
   if (m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize == 0)
   {
      strStream << "Ball Start Positions for all available non-frozen balls have not been configured";
   }
   else if (m_ControlVBalls.size() != m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize)
   {
      strStream << "Available non-frozen balls (" << m_ControlVBalls.size() << ") must equal Ball Start Positions (" << m_MenuOptions.m_TrainerOptions.m_BallStartOptionsRecordsSize << ")";
   }
   else if (m_MenuOptions.m_TrainerOptions.m_BallPassOptionsRecords.size() == 0 && !BallCorridorReadyForTrainer())
   {
      strStream << "Ball Pass and/or Ball Corridor must be configured";
   }

   if (strStream.str().size() > 0)
   {
      m_MenuOptions.m_MenuError = strStream.str();
      return false;
   }
   else
   {
      return true;
   }
}

bool BallHistory::BallCorridorReadyForTrainer()
{
   TrainerOptions::BallCorridorOptionsRecord& bcor = m_MenuOptions.m_TrainerOptions.m_BallCorridorOptionsRecord;
   return !bcor.m_PassPosition.IsZero() && !bcor.m_OpeningPositionLeft.IsZero() && !bcor.m_OpeningPositionRight.IsZero();
}

POINT BallHistory::Get2DPointFrom3D(Player& player, const Vertex3Ds& vertex)
{
   return player.m_renderer->Get2DPointFrom3D(player.m_playfieldWnd->GetWidth(), player.m_playfieldWnd->GetHeight(), vertex);
}

Vertex3Ds BallHistory::Get3DPointFrom2D(const POINT& p, float heightZ)
{
   // TODO GARY This is not correct, need to check width/height and add heightZ
   return g_pplayer->m_renderer->Get3DPointFrom2D(g_pplayer->m_playfieldWnd->GetWidth(), g_pplayer->m_playfieldWnd->GetHeight(), p, heightZ);
}

Vertex3Ds BallHistory::Get3DPointFromMousePosition(Player& player, float heightZ)
{
   Vertex3Ds point3D = { 0.0f, 0.0f, 0.0f };
   POINT mousePosition2D = { 0 };
   if (Get2DMousePosition(player, mousePosition2D, false))
   {
      point3D = Get3DPointFrom2D(mousePosition2D, heightZ);
   }
   return point3D;
}

bool BallHistory::Get2DMousePosition(Player& player, POINT& mousePosition2D, bool correct)
{
   bool retVal = false;
   if (GetCursorPos(&mousePosition2D) == TRUE && ScreenToClient(player.m_pininput.GetFocusHWnd(), &mousePosition2D) == TRUE)
   {
      if (correct && player.m_ptable->mViewSetups[player.m_ptable->m_BG_current_set].mViewportRotation == 180.0f)
      {
         mousePosition2D.x = g_pplayer->m_playfieldWnd->GetWidth() - mousePosition2D.x;
         mousePosition2D.y = g_pplayer->m_playfieldWnd->GetHeight() - mousePosition2D.y;
      }

      retVal = true;
   }
   return retVal;
}

Vertex3Ds BallHistory::GetKickerPosition(Kicker& kicker)
{
   Vertex3Ds kickerPosition(0.0f, 0.0f, 0.0f);
   if (KickerHitCircle* kickerHitCircle = kicker.GetKickerHitCircle())
   {
      kickerPosition = { kickerHitCircle->m_hitBBox.left + (kickerHitCircle->m_hitBBox.right - kickerHitCircle->m_hitBBox.left) / 2.0f,
         kickerHitCircle->m_hitBBox.bottom + (kickerHitCircle->m_hitBBox.top - kickerHitCircle->m_hitBBox.bottom) / 2.0f,
         kickerHitCircle->m_hitBBox.zlow + (kickerHitCircle->m_hitBBox.zhigh - kickerHitCircle->m_hitBBox.zlow) / 2.0f };
   }
   return kickerPosition;
}

void BallHistory::SetFlipperStrength(float flipperStrength)
{
   for (Flipper* flipper : m_Flippers)
   {
      flipper->put_Strength(flipperStrength);
   }
}

float BallHistory::GetFlipperStrength()
{
   float flipperStrength = 0.0f;
   if (m_Flippers.size())
   {
      Flipper* flipper = static_cast<Flipper*>(m_Flippers[0]);
      flipper->get_Strength(&flipperStrength);
   }
   return flipperStrength;
}

void BallHistory::SetFlipperFriction(float flipperFriction)
{
   for (Flipper* flipper : m_Flippers)
   {
      flipper->put_Friction(flipperFriction);
   }
}

float BallHistory::GetFlipperFriction()
{
   float flipperFriction = 0.0f;
   if (m_Flippers.size())
   {
      Flipper* flipper = static_cast<Flipper*>(m_Flippers[0]);
      flipper->get_Friction(&flipperFriction);
   }
   return flipperFriction;
}

bool BallHistory::ControlNextMove()
{
   m_CurrentControlIndex++;
   if (m_CurrentControlIndex >= m_BallHistoryRecords.size())
   {
      m_CurrentControlIndex = 0;
   }

   return m_CurrentControlIndex != m_BallHistoryRecordsHeadIndex;
}

bool BallHistory::ControlPrevMove()
{
   if (m_CurrentControlIndex == 0)
   {
      m_CurrentControlIndex = m_BallHistoryRecords.size() - 1;
   }
   else
   {
      m_CurrentControlIndex--;
   }

   if (m_CurrentControlIndex == GetTailIndex())
   {
      return false;
   }
   return true;
}

bool BallHistory::BallCountIncreased() { return m_ControlVBalls.size() > m_ControlVBallsPrevious.size(); }

bool BallHistory::BallCountDecreased() { return m_ControlVBalls.size() < m_ControlVBallsPrevious.size(); }

bool BallHistory::BallChanged() { return !std::equal(m_ControlVBalls.begin(), m_ControlVBalls.end(), m_ControlVBallsPrevious.begin()); }

bool BallHistory::BallInsideAutoControlVertex(std::vector<HitBall*>& controlVBalls)
{
   if (controlVBalls.size())
   {
      for (const HitBall* controlVBall : controlVBalls)
      {
         for (std::size_t autoControlVertexIndex = 0; autoControlVertexIndex < m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size(); autoControlVertexIndex++)
         {
            NormalOptions::AutoControlVertex& autoControlVertex = m_MenuOptions.m_NormalOptions.m_AutoControlVertices[autoControlVertexIndex];
            if (autoControlVertex.Active)
            {
               if (DistancePixels(autoControlVertex.m_Position, controlVBall->m_d.m_pos) < (controlVBall->m_d.m_radius * 1.5f))
               {
                  autoControlVertex.Active = false;
                  return true;
               }
            }
         }
      }

      for (std::size_t autoControlVertexIndex = 0; autoControlVertexIndex < m_MenuOptions.m_NormalOptions.m_AutoControlVertices.size(); autoControlVertexIndex++)
      {
         NormalOptions::AutoControlVertex& autoControlVertex = m_MenuOptions.m_NormalOptions.m_AutoControlVertices[autoControlVertexIndex];
         if (!autoControlVertex.Active)
         {
            autoControlVertex.Active = true;
            for (const HitBall* ball : controlVBalls)
            {
               if (DistancePixels(autoControlVertex.m_Position, ball->m_d.m_pos) < (ball->m_d.m_radius * 3.5f))
               {
                  autoControlVertex.Active = false;
                  break;
               }
            }
         }
      }
   }
   return false;
}

std::vector<std::string> BallHistory::Split(const char* str, char delimeter)
{
   std::vector<std::string> result;
   if (str != nullptr)
   {
      std::stringstream ss(str);
      std::string item;

      while (std::getline(ss, item, delimeter))
      {
         result.push_back(item);
      }
   }

   return result;
}

void BallHistory::CenterMouse(Player& player)
{
   POINT p = { g_pplayer->m_playfieldWnd->GetWidth() / 2, g_pplayer->m_playfieldWnd->GetHeight() / 2 };
   ClientToScreen(player.m_pininput.GetFocusHWnd(), &p);
   SetCursorPos(p.x, p.y);
}

void BallHistory::InvalidEnumValue(const char* enumName, const int enumValue)
{
   std::stringstream errorMessage;
   errorMessage << "Ball History: Invalid value '" << enumValue << "' for enum type " << enumName;
   m_MenuOptions.m_MenuError = errorMessage.str();
}

void BallHistory::InvalidEnumValue(const char* enumName, const char* enumValue)
{
   std::stringstream errorMessage;
   errorMessage << "Ball History: Invalid value '" << enumValue << "' for enum type " << enumName;
   m_MenuOptions.m_MenuError = errorMessage.str();
}

void BallHistory::PlaySound(UINT rcId, bool async) { ::PlaySound(MAKEINTRESOURCE(rcId), ::GetModuleHandle(NULL), SND_RESOURCE | (async ? SND_ASYNC : SND_SYNC)); }

void BallHistory::StopSound() { ::PlaySound(NULL, NULL, 0); }

// ================================================================================================================================================================================================================================================
