#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "xrUICore/ProgressBar/UIDoubleProgressBar.h"
#include "xrServerEntities/alife_space.h"

class CCustomOutfit;
class CHelmet;
class CBackpack;
class CUIStatic;
class CUIDoubleProgressBar;
class CUIXml;

class CUIOutfitImmunity final : public CUIWindow
{
public:
    CUIOutfitImmunity();

    bool InitFromXml(CUIXml& xml_doc, pcstr base_str, pcstr immunity, pcstr immunity_text);
    void SetProgressValue(float cur, float comp);
    void Set2ProgressValue(float cur, float cur2, float comp, float comp2);

    pcstr GetDebugType() override { return "CUIOutfitImmunity"; }

protected:
    CUIStatic m_name; // texture + name
    CUIDoubleProgressBar m_progress;
    CUIStatic m_value; // 100%
    float m_magnitude;

}; // class CUIOutfitImmunity

// -------------------------------------------------------------------------------------

class CUIOutfitInfo final : public CUIWindow
{
public:
    CUIOutfitInfo() : CUIWindow("CUIOutfitInfo") {}

    void InitFromXml(CUIXml& xml_doc);
    void UpdateInfo(CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit = nullptr);
    void UpdateInfo(CHelmet* cur_helmet, CHelmet* slot_helmet = nullptr);
    void UpdateInfo(CBackpack* cur_backpack, CBackpack* slot_backpack = nullptr);

    pcstr GetDebugType() override { return "CUIOutfitInfo"; }

protected:
    CUIStatic* m_caption{};
    CUIStatic* m_Prop_line{};
    xr_unordered_map<ALife::EHitType, CUIOutfitImmunity*> m_items;
}; // class CUIOutfitInfo


class CUIWeightInfo final : public CUIWindow
{
public:
    CUIWeightInfo();

    bool InitFromXml(CUIXml& xml_doc);
    void SetInfo(CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit);
    void SetInfo(CBackpack* cur_backpack, CBackpack* slot_backpack);

    pcstr GetDebugType() override { return "CUIWeightInfo"; }

protected:
    CUIStatic* m_icon; // texture
    CUIStatic m_name; // name
    shared_str m_unit_str; //kg
    CUIStatic* m_value; // xxx
};
