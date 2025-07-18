#include "FlexDMD.h"
#include "Group.h"
#include "Label.h"
#include "Frame.h"
#include "AnimatedActor.h"
#include "Image.h"

#include <algorithm>

namespace Flex {

Group::~Group()
{
   RemoveAll();
}

void Group::OnStageStateChanged()
{
   for (Actor* child : m_children)
      child->SetOnStage(GetOnStage());
}

void Group::Update(float delta)
{
   AddRef();
   Actor::Update(delta);
   if (!GetOnStage())
   {
      Release();
      return;
   }
   vector<Actor*> children(m_children);
   for (auto child : children)
      child->Update(delta);
   Release();
}

void Group::Draw(Flex::SurfaceGraphics* pGraphics)
{
   if (GetVisible())
   {
      pGraphics->TranslateTransform(static_cast<int>(GetX()), static_cast<int>(GetY()));
      if (m_clip)
      {
         pGraphics->SetClip({ 0, 0, static_cast<int>(GetWidth()), static_cast<int>(GetHeight()) });

         Actor::Draw(pGraphics);

         for (Actor* child : m_children)
            child->Draw(pGraphics);

         pGraphics->ResetClip();
      }
      else
      {
         Actor::Draw(pGraphics);

         for (Actor* child : m_children)
            child->Draw(pGraphics);
      }
      pGraphics->TranslateTransform(-static_cast<int>(GetX()), -static_cast<int>(GetY()));
   }
}

Actor* Group::Get(const string& name)
{
   if (GetName() == name)
      return this;

   if (GetFlexDMD()->GetRuntimeVersion() <= 1008)
   {
      for (Actor* child : m_children)
      {
         if (child->GetName() == name)
         {
            child->AddRef();
            return child;
         }
         if (child->GetType() == Actor::AT_Group)
         {
            Group* group = static_cast<Group*>(child);
            Actor* found = group->Get(name);
            if (found)
            {
               found->AddRef();
               return found;
            }
         }
      }
   }
   else
   {
      size_t pos = name.find('/');
      if (pos == string::npos)
      {
         // direct child node search 'xx'
         for (Actor* child : m_children)
         {
            if (child->GetName() == name)
            {
               child->AddRef();
               return child;
            }
         }
      }
      else if (pos == 0)
      {
         // absolute path from root '/xx/yy/zz', note that stage node is named 'Stage'
         Group* root = this;
         while (root->GetParent() != nullptr)
            root = root->GetParent();
         Actor* found = root->Get(name.substr(1));
         return found;
      }
      else
      {
         // relative path from current group 'xx/yy/zz'
         string groupName = name.substr(0, pos);
         for (Actor* child : m_children)
         {
            if ((child->GetType() == Actor::AT_Group) && (child->GetName() == groupName))
               return static_cast<Group*>(child)->Get(name.substr(pos + 1));
         }
      }
   }
   
   // PLOGW << "Actor " << name << " not found in children of " << GetName();

   return NULL;
}

bool Group::HasChild(const string &name)
{
   Actor* child = Get(name);
   if (child == nullptr)
      return false;
   child->Release();
   return true;
}

Group* Group::GetGroup(const string& Name)
{
   Actor* actor = Get(Name);
   if ((actor != nullptr) && (actor->GetType() == Actor::AT_Group))
      return static_cast<Group*>(actor);
   return nullptr;
}

Frame* Group::GetFrame(const string& Name)
{
   Actor* actor = Get(Name);
   if ((actor != nullptr) && (actor->GetType() == Actor::AT_Frame))
      return static_cast<Frame*>(actor);
   return nullptr;
}

Label* Group::GetLabel(const string& Name)
{
   Actor* actor = Get(Name);
   if ((actor != nullptr) && (actor->GetType() == Actor::AT_Label))
      return static_cast<Label*>(actor);
   return nullptr;
}

AnimatedActor* Group::GetVideo(const string& Name)
{
   Actor* actor = Get(Name);
   if ((actor != nullptr) && (actor->GetType() == Actor::AT_AnimatedActor))
      return static_cast<AnimatedActor*>(actor);
   return nullptr;
}

Image* Group::GetImage(const string& Name)
{
   Actor* actor = Get(Name);
   if ((actor != nullptr) && (actor->GetType() == Actor::AT_Image))
      return static_cast<Image*>(actor);
   return nullptr;
}

void Group::AddActor(Actor* actor)
{
   if (actor == nullptr || (actor->GetParent() == this))
      return;
   actor->AddRef();
   actor->SetParent(this);
   m_children.push_back(actor);
   actor->SetOnStage(GetOnStage());
}

void Group::AddActorAt(Actor* actor, int index)
{
   if (actor == nullptr || (actor->GetParent() == this))
      return;
   actor->AddRef();
   actor->Remove();
   actor->SetParent(this);
   m_children.insert(m_children.begin() + index, actor);
   actor->SetOnStage(GetOnStage());
}

void Group::RemoveActor(Actor* actor)
{
   if (actor == nullptr || (actor->GetParent() != this))
      return;
   actor->SetParent(nullptr);
   m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [actor](Actor* p) { return p == actor; }), m_children.end());
   actor->SetOnStage(false);
   actor->Release();
}

void Group::RemoveAll()
{
   //PLOGD << "Remove all children " << GetName();
   for (Actor* child : m_children)
   {
      child->SetParent(nullptr);
      child->SetOnStage(false);
      child->Release();
   }
   m_children.clear();
}

vector<Actor *> Group::GetChildren() const
{
   for (auto child : m_children)
      child->AddRef();
   return m_children;
}

}