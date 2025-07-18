#include "Video.h"

namespace Flex {

Video::Video(FlexDMD* pFlexDMD, const string& name) : AnimatedActor(pFlexDMD, name)
{
}

Video* Video::Create(FlexDMD* pFlexDMD, const string& path, const string& name, bool loop)
{
   Video* pVideo = new Video(pFlexDMD, name);
   pVideo->SetVisible(true);
   pVideo->m_seek = -1;
   pVideo->SetLoop(loop);

   //PLOGW << "Video not supported " << path;

   return pVideo;
}

Video::~Video()
{
}

void Video::OnStageStateChanged()
{
}

void Video::Seek(float posInSeconds)
{
   m_seek = posInSeconds;
   SetTime(posInSeconds);
   //if (_audioReader != null)
   //{
   //   _audioReader.CurrentTime = TimeSpan.FromSeconds(position);
   //}
   //if (_videoReader != null)
   //{
      ReadNextFrame();
      SetTime(GetFrameTime());
   //}

}

void Video::Advance(float delta)
{
   // Remove when video is supported
}

void Video::Rewind()
{
   AnimatedActor::Rewind();
   Seek(0);
}

void Video::ReadNextFrame()
{
   SetFrameTime(GetFrameTime() + GetFrameDuration());
}

void Video::Draw(Flex::SurfaceGraphics* pGraphics)
{
   if (GetVisible()) {
      float w = 0;
      float h = 0;
      Layout::Scale(GetScaling(), GetPrefWidth(), GetPrefHeight(), GetWidth(), GetHeight(), w, h);
      float x = 0;
      float y = 0;
      Layout::Align(GetAlignment(), w, h, GetWidth(), GetHeight(), x, y);
      //SDL_Rect rect = { (int)(GetX() + x), (int)(GetY() + y), (int)w, (int)h };
      //pGraphics->DrawImage(m_pFrame, NULL, &rect);
   }
}

}