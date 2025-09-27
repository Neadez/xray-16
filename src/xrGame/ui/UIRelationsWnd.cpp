#include "StdAfx.h"
#include "pch_script.h"
#include "UIRankingWnd.h"
#include "UIXmlInit.h"
#include "xrUICore/ProgressBar/UIProgressBar.h"
#include "xrUICore/Windows/UIFrameLineWnd.h"
#include "xrUICore/ScrollView/UIScrollView.h"
#include "UIHelper.h"
#include "UIInventoryUtilities.h"
#include "Actor.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "xrScriptEngine/script_engine.hpp"
#include "character_community.h"
#include "character_reputation.h"
#include "relation_registry.h"
#include "UIRankFaction.h"
#include "xrUICore/ui_base.h"

#define PDA_RELATIONS_XML "pda_relations.xml"

CUIRelationsWnd::CUIRelationsWnd()
    : CUIWindow("CUIRelationsWnd"), m_delay(3000), m_previous_time(Device.dwTimeGlobal) {}

CUIRelationsWnd::~CUIRelationsWnd() {}

void CUIRelationsWnd::Show(bool status)
{
    inherited::Show(status);
}

void CUIRelationsWnd::Update()
{
    if (Device.dwTimeGlobal - m_previous_time > m_delay)
    {
        m_previous_time = Device.dwTimeGlobal;
        update_info();
    }
    if (IsShown())
        inherited::Update();
}

bool CUIRelationsWnd::Init()
{
    Fvector2 pos;
    CUIXml xml;
    if (!xml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, PDA_RELATIONS_XML, false))
        return false;

    CUIXmlInit::InitWindow(xml, "main_wnd", 0, this);
    m_delay = (u32)xml.ReadAttribInt("main_wnd", 0, "delay", 3000);

    if (!UIHelper::CreateFrameWindow(xml, "background", this, false))
        std::ignore = UIHelper::CreateFrameLine(xml, "background", this, false);

    std::ignore = UIHelper::CreateStatic   (xml, "fraction_static", this, false);
    std::ignore = UIHelper::CreateFrameLine(xml, "fraction_line1", this, false);
    std::ignore = UIHelper::CreateFrameLine(xml, "fraction_line2", this, false);

    XML_NODE stored_root = xml.GetLocalRoot();
    XML_NODE node = xml.NavigateToNode("fraction_list", 0);

    xml.SetLocalRoot(stored_root);

    m_factions_list = UIHelper::CreateScrollView(xml, "fraction_list", this, false);

    if (m_factions_list)
    {
        m_factions_list->SetWindowName("---fraction_list");
        m_factions_list->m_sort_function = fastdelegate::MakeDelegate(this, &CUIRelationsWnd::SortingLessFunction);

        cpcstr fract_section = "pda_rank_communities";

        if (pSettings->section_exist(fract_section))
        {
            node = xml.NavigateToNode("fraction_list", 0);
            xml.SetLocalRoot(node);
            CInifile::Sect& faction_section = pSettings->r_section(fract_section);
            for (const auto& item : faction_section.Data)
            {
                add_faction(xml, item.first);
            }
            node = xml.NavigateToNode("fraction_list", 0);
            xml.SetLocalRoot(stored_root);
        }
    }

    xml.SetLocalRoot(stored_root);

    return true;
}

void CUIRelationsWnd::add_faction(CUIXml& xml, shared_str const& faction_id)
{
    CUIRankFaction* faction = xr_new<CUIRankFaction>(faction_id);
    faction->init_from_xml(xml);
    faction->SetWindowName("fraction_item");
    m_factions_list->AddWindow(faction, true);
    Register(faction);
}

void CUIRelationsWnd::clear_all_factions()
{
    m_factions_list->Clear();
}

void CUIRelationsWnd::update_info()
{
    if (m_factions_list)
    {
        bool force_rating = false;
        for (u8 i = 0; i < m_factions_list->GetSize(); ++i)
        {
            if (const auto* ui_faction = smart_cast<CUIRankFaction*>(m_factions_list->GetItem(i)))
            {
                if (ui_faction->get_cur_sn() != i + 1)
                {
                    force_rating = true;
                    break;
                }
            }
        }

        for (u8 i = 0; i < m_factions_list->GetSize(); ++i)
        {
            if (auto* ui_faction = smart_cast<CUIRankFaction*>(m_factions_list->GetItem(i)))
            {
                ui_faction->update_info(i + 1);
                ui_faction->rating(i + 1, force_rating);
            }
        }

        m_factions_list->ForceUpdate();
    }
}

bool CUIRelationsWnd::SortingLessFunction(CUIWindow* left, CUIWindow* right)
{
    CUIRankFaction* lpi = smart_cast<CUIRankFaction*>(left);
    CUIRankFaction* rpi = smart_cast<CUIRankFaction*>(right);
    VERIFY(lpi && rpi);
    return (lpi->get_faction_power() > rpi->get_faction_power());
}

void CUIRelationsWnd::ResetAll()
{
    inherited::ResetAll();
}
