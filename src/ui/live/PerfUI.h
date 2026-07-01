// license:GPLv3+

#pragma once

#include "core/player.h"
#include "PlotData.h"

class PerfUI final
{
public:
   PerfUI(Player* const player);
   ~PerfUI();

   void SetUIScale(float scale) { m_uiScale = scale; }

   enum PerfMode
   {
      PM_DISABLED,
      PM_FPS,
      PM_STATS
   };
   void NextPerfMode();
   PerfMode GetPerfMode() const { return m_showPerf; }
   void SetPerfMode(PerfMode mode) { m_showPerf = mode; }

   void Update();

private:
   void RenderFPS();
   void RenderStats() const;
   void RenderPlots();

   Player* const m_player;
   float m_uiScale = 1.0f;

   PerfMode m_showPerf = PerfMode::PM_DISABLED;
   bool m_showAvgFPS = true;
   bool m_showRollingFPS = true;
   
   PlotData m_plotFPS;
   PlotData m_plotFPSSmoothed;
   PlotData m_plotPhysx;
   PlotData m_plotPhysxSmoothed;
   PlotData m_plotScript;
   PlotData m_plotScriptSmoothed;

   // Latched flipper-latency display (FPS overlay): a LEFT flipper press captures a latency snapshot that
   // stays on screen until a RIGHT flipper press clears it, instead of the old 1-second auto-expiring window.
   // Each new LEFT press refreshes the latched values; RIGHT press reverts to the default "Input Latency" line.
   bool m_flipLatchActive = false;
   uint64_t m_flipLatchCapturedLeftChange = 0; // left-flip state-change time already captured (capture once per press)
   uint64_t m_flipLatchSeenRightChange = 0;    // right-flip state-change time last observed (edge-detect for clear)
   double m_flipLatchAcqToAction = 0.0;
   double m_flipLatchPrevAcqToAction = 0.0;
   bool m_flipLatchHasSdl = false;
   double m_flipLatchSdlToActMs = 0.0;
   double m_flipLatchGpuMs = 0.0;
   double m_flipLatchTotalMs = 0.0;
};
