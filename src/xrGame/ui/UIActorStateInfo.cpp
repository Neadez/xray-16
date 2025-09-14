////////////////////////////////////////////////////////////////////////////
//	Module 		: UIActorStateInfo.cpp
//	Created 	: 15.02.2008
//	Author		: Evgeniy Sokolov
//	Description : UI actor state window class implementation
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIActorStateInfo.h"
#include "xrUICore/ProgressBar/UIProgressBar.h"
#include "xrUICore/ProgressBar/UIProgressShape.h"
#include "xrUICore/ScrollView/UIScrollView.h"
#include "xrUICore/Windows/UIFrameWindow.h"
#include "xrUICore/Static/UIStatic.h"
#include "UIXmlInit.h"
#include "Common/object_broker.h"
#include "UIHelper.h"
#include "xrUICore/arrow/ui_arrow.h"
#include "UIHudStatesWnd.h"
#include "Level.h"
#include "location_manager.h"
#include "player_hud.h"
#include "UIMainIngameWnd.h"
#include "UIGameCustom.h"
#include "Actor.h"
#include "ActorCondition.h"
#include "EntityCondition.h"
#include "CustomOutfit.h"
#include "ActorHelmet.h"
#include "Inventory.h"
#include "Artefact.h"

u32 const red_clr = color_argb(255, 210, 50, 50);
u32 const green_clr = color_argb(255, 170, 170, 170);

ui_actor_state_wnd::~ui_actor_state_wnd() { delete_data(m_hint_wnd); }

void ui_actor_state_wnd::init_from_xml(CUIXml& xml)
{
    for (int i = 0; i < stt_count; ++i)
    {
        m_state[i] = xr_new<ui_actor_state_item>();
        m_state[i]->SetAutoDelete(true);
        AttachChild(m_state[i]);
        m_state[i]->set_hint_wnd(m_hint_wnd);
    }
    m_state[stt_health]->init_from_xml_plain(xml, "progress_bar_health");
    m_state[stt_psi]->init_from_xml_plain(xml, "progress_bar_psy");
    m_state[stt_radia]->init_from_xml_plain(xml, "progress_bar_radiation");
}

void ui_actor_state_wnd::init_from_xml(CUIXml& xml, LPCSTR path)
{
    XML_NODE stored_root = xml.GetLocalRoot();
    CUIXmlInit::InitWindow(xml, path, 0, this);

    XML_NODE new_root = xml.NavigateToNode(path, 0);
    xml.SetLocalRoot(new_root);

    m_hint_wnd = UIHelper::CreateHint(xml, "hint_wnd");

    for (int i = 0; i < stt_count; ++i)
    {
        m_state[i] = xr_new<ui_actor_state_item>();
        m_state[i]->SetAutoDelete(true);
        AttachChild(m_state[i]);
        m_state[i]->set_hint_wnd(m_hint_wnd);
    }
    m_state[stt_stamina]->init_from_xml(xml, "stamina_state", false);
    m_state[stt_health]->init_from_xml(xml, "health_state");
    m_state[stt_bleeding]->init_from_xml(xml, "bleeding_state", false);
    m_state[stt_radiation]->init_from_xml(xml, "radiation_state", false);
    m_state[stt_armor]->init_from_xml(xml, "armor_state", false);

    m_state[stt_main]->init_from_xml(xml, "main_sensor", false);
    m_state[stt_fire]->init_from_xml(xml, "fire_sensor");
    m_state[stt_radia]->init_from_xml(xml, "radia_sensor");
    m_state[stt_acid]->init_from_xml(xml, "acid_sensor");
    m_state[stt_psi]->init_from_xml(xml, "psi_sensor");
    m_state[stt_wound]->init_from_xml(xml, "wound_sensor", false);
    m_state[stt_fire_wound]->init_from_xml(xml, "fire_wound_sensor", false);
    m_state[stt_fire_wound_head]->init_from_xml(xml, "fire_wound_head_sensor", false);
    m_state[stt_shock]->init_from_xml(xml, "shock_sensor", false);
    m_state[stt_power]->init_from_xml(xml, "power_sensor", false);
    //
    m_state[stt_satiety]->init_from_xml(xml, "satiety_state", false);
    m_state[stt_hydration]->init_from_xml(xml, "hydration_state", false);
    m_state[stt_psy]->init_from_xml(xml, "psy_state", false);

    m_state[stt_bleed]->init_from_xml(xml, "bleed_sensor", false);
    m_state[stt_helth]->init_from_xml(xml, "helth_sensor", false);
    m_state[stt_rada]->init_from_xml(xml, "rada_sensor", false);
    m_state[stt_sati]->init_from_xml(xml, "satiety_sensor", false);
    m_state[stt_hydr]->init_from_xml(xml, "hydration_sensor", false);
    m_state[stt_weight]->init_from_xml(xml, "weight_sensor", false);
    m_state[stt_strike]->init_from_xml(xml, "strike_sensor", false);
    m_state[stt_expl]->init_from_xml(xml, "expl_sensor", false);
    //
    xml.SetLocalRoot(stored_root);
}

void ui_actor_state_wnd::UpdateActorInfo(CInventoryOwner* owner)
{
    auto actor = smart_cast<CActor*>(owner);
    if (!actor)
        return;

    float value = 0.0f;

    const auto& conditions = actor->conditions();
    // show psy icon
    value = conditions.GetPsyHealth();
    m_state[stt_psy]->set_progress(value);

    // show satiety icon
    value = conditions.GetSatietyHealth();
    m_state[stt_satiety]->set_progress(value);

    // show hydration icon
    value = conditions.GetHydrationHealth();
    m_state[stt_hydration]->set_progress(value);

    // show stamina icon
    value = conditions.GetPower();
    m_state[stt_stamina]->set_progress(value);

    value = actor->GetRestoreSpeed(ALife::ePowerRestoreSpeed);
    m_state[stt_stamina]->set_text(value); // 0..0.99

    // show health icon
    value = conditions.GetHealth();
    value = floor(value * 55) / 55; // number of sticks in progress bar
    m_state[stt_health]->set_progress(value);

    value = actor->GetRestoreSpeed(ALife::eHealthRestoreSpeed);
    m_state[stt_helth]->set_text(value);

    // show bleeding icon
    value = conditions.BleedingSpeed();
    m_state[stt_health]->show_static((value > 0.01f)); // Bleeding icon in Clear Sky

    m_state[stt_bleeding]->show_static(false, 1);
    m_state[stt_bleeding]->show_static(false, 2);
    m_state[stt_bleeding]->show_static(false, 3);

    if (!fis_zero(value, EPS))
    {
        if (value < 0.35f)
            m_state[stt_bleeding]->show_static(true, 1);
        else if (value < 0.7f)
            m_state[stt_bleeding]->show_static(true, 2);
        else
            m_state[stt_bleeding]->show_static(true, 3);
    }
    value = actor->GetRestoreSpeed(ALife::eBleedingRestoreSpeed);
    m_state[stt_bleed]->set_text(value);

    // show radiation icon
    value = conditions.GetRadiation();
    m_state[stt_radiation]->show_static(false, 1);
    m_state[stt_radiation]->show_static(false, 2);
    m_state[stt_radiation]->show_static(false, 3);

    if (!fis_zero(value, EPS))
    {
        if (value < 0.35f)
            m_state[stt_radiation]->show_static(true, 1);
        else if (value < 0.7f)
            m_state[stt_radiation]->show_static(true, 2);
        else
            m_state[stt_radiation]->show_static(true, 3);
    }
    value = actor->GetRestoreSpeed(ALife::eRadiationRestoreSpeed);
    m_state[stt_rada]->set_text(value);

    CCustomOutfit* outfit = actor->GetOutfit();
    PIItem itm = actor->inventory().ItemFromSlot(HELMET_SLOT);
    CHelmet* helmet = smart_cast<CHelmet*>(itm);

    m_state[stt_fire]->set_progress(0.0f);
    m_state[stt_radia]->set_progress(0.0f);
    m_state[stt_acid]->set_progress(0.0f);
    m_state[stt_psi]->set_progress(0.0f);
    m_state[stt_wound]->set_progress(0.0f);
    m_state[stt_fire_wound]->set_progress(0.0f);
    m_state[stt_fire_wound_head]->set_progress(0.0f);
    m_state[stt_shock]->set_progress(0.0f);
    m_state[stt_power]->set_progress(0.0f);
    //
    m_state[stt_helth]->set_progress(0.0f);
    m_state[stt_bleed]->set_progress(0.0f);
    m_state[stt_rada]->set_progress(0.0f);
    m_state[stt_sati]->set_progress(0.0f);
    m_state[stt_hydr]->set_progress(0.0f);
    m_state[stt_strike]->set_progress(0.0f);
    m_state[stt_expl]->set_progress(0.0f);

    float burn_value = 0.0f;
    float radi_value = 0.0f;
    float cmbn_value = 0.0f;
    float tele_value = 0.0f;
    float woun_value = 0.0f;
    float shoc_value = 0.0f;
    float fwou_value = 0.0f;
    float fwou2_value = 0.0f;
    //
    float stri_value = 0.0f;
    float expl_value = 0.0f;

    const auto& cur_booster_influences = conditions.GetCurBoosterInfluences();
    CEntityCondition::BOOSTER_MAP::const_iterator it;
    it = cur_booster_influences.find(eBoostRadiationProtection);
    if (it != cur_booster_influences.end())
        radi_value += it->second.fBoostValue;

    it = cur_booster_influences.find(eBoostChemicalBurnProtection);
    if (it != cur_booster_influences.end())
        cmbn_value += it->second.fBoostValue;

    it = cur_booster_influences.find(eBoostTelepaticProtection);
    if (it != cur_booster_influences.end())
        tele_value += it->second.fBoostValue;

    if (outfit)
    {
        value = outfit->GetCondition();
        m_state[stt_armor]->set_progress(value);

        burn_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeBurn);
        radi_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
        cmbn_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
        tele_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
        woun_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeWound);
        shoc_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeShock);
        stri_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeStrike);
        expl_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeExplosion);
        //fwou_value += outfit->GetDefHitTypeProtection(ALife::eHitTypeFireWound);

        IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
        VERIFY(ikv);
        const auto spine_bone = ikv->LL_BoneID("bip01_spine");

        value = outfit->GetBoneArmor(spine_bone);
        m_state[stt_armor]->set_text(value);

        fwou_value = value * outfit->GetCondition();
        const float fwou_value_max = conditions.GetMaxFireWoundProtection();
        fwou_value /= fwou_value_max;
        const auto head_bone = ikv->LL_BoneID("bip01_head");
        if (!outfit->bIsHelmetAvaliable)
        {
            fwou2_value += outfit->GetBoneArmor(head_bone) * outfit->GetCondition();
            const float fwou2_value_max = conditions.GetMaxFireWoundProtection();
            fwou2_value /= fwou2_value_max;
        }
    }
    else
    {
        m_state[stt_armor]->set_progress(0.0f);
        m_state[stt_armor]->set_text(0.0f);
    }

    if (helmet)
    {
        burn_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeBurn);
        radi_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeRadiation);
        cmbn_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeChemicalBurn);
        tele_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeTelepatic);
        woun_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeWound);
        shoc_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeShock);
        stri_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeStrike);
        expl_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeExplosion);
        //fwou_value += helmet->GetDefHitTypeProtection(ALife::eHitTypeFireWound);

        IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
        VERIFY(ikv);
        const auto head_bone = ikv->LL_BoneID("bip01_head");
        fwou2_value += helmet->GetBoneArmor(head_bone) * helmet->GetCondition();
        fwou_value += 0.0f;
    }

    const auto getProtection = [&](float& valueRef, ALife::EHitType hitType) -> float
    {
        valueRef += actor->GetProtection_ArtefactsOnBelt(hitType);
        return conditions.GetZoneMaxPower(hitType);
    };

    // fire burn protection progress bar
    {
        const float max_power = getProtection(burn_value, ALife::eHitTypeBurn);
        update_round_states(stt_fire, burn_value, max_power);
    }
    // radiation protection progress bar
    {
        const float max_power = getProtection(radi_value, ALife::eHitTypeRadiation);
        update_round_states(stt_radia, radi_value, max_power);
    }
    // chemical burn protection progress bar
    {
        const float max_power = getProtection(cmbn_value, ALife::eHitTypeChemicalBurn);
        update_round_states(stt_acid, cmbn_value, max_power);
    }
    // telepathic protection progress bar
    {
        const float max_power = getProtection(tele_value, ALife::eHitTypeTelepatic);
        update_round_states(stt_psi, tele_value, max_power);
    }
    // wound protection progress bar
    {
        //const float max_power = conditions.GetMaxWoundProtection();
        const float max_power = getProtection(woun_value, ALife::eHitTypeWound);
        update_round_states(stt_wound, woun_value, max_power);
    }
    // shock protection progress bar
    {
        const float max_power = getProtection(shoc_value, ALife::eHitTypeShock);
        update_round_states(stt_shock, shoc_value, max_power);
    }
    // fire wound protection progress bar
    {
        //const float max_power = conditions.GetMaxFireWoundProtection();
        const float max_power = getProtection(fwou_value, ALife::eHitTypeFireWound);
        update_round_states(stt_fire_wound, fwou_value, max_power);
    }
    {
        // const float max_power = conditions.GetMaxFireWoundProtection();
        const float max_power = getProtection(fwou2_value, ALife::eHitTypeFireWound);
        update_round_states(stt_fire_wound_head, fwou2_value, max_power);
    }
    // power restore speed progress bar
    {
        value = actor->GetRestoreSpeed(ALife::ePowerRestoreSpeed) / conditions.GetMaxPowerRestoreSpeed();
        update_round_states(stt_power, value, 1.f);
    }

    // health restore speed progress bar
    {
        value = actor->GetRestoreSpeed(ALife::eHealthRestoreSpeed) / conditions.GetMaxHealthRestoreSpeed();
        update_round_states(stt_helth, value, 1.f);
    }
    // bleeding restore speed progress bar
    {
        value = actor->GetRestoreSpeed(ALife::eBleedingRestoreSpeed) / conditions.GetMaxBleedingRestoreSpeed();
        update_round_states(stt_bleed, value, 1.f);
    }
    // radiation restore speed progress bar
    {
        value = actor->GetRestoreSpeed(ALife::eRadiationRestoreSpeed) / conditions.GetMaxRadiationRestoreSpeed();
        update_round_states(stt_rada, value, 1.f);
    }
    // satiety restore speed progress bar
    {
        value = actor->GetRestoreSpeed(ALife::eSatietyRestoreSpeed) / conditions.GetMaxSatietyRestoreSpeed();
        update_round_states(stt_sati, value, -1.f);
    }
    // hydration restore speed progress bar
    {
        value = actor->GetRestoreSpeed(ALife::eHydrationRestoreSpeed) / conditions.GetMaxHydrationRestoreSpeed();
        update_round_states(stt_hydr, value, -1.f);
    }
    // strike strike protection progress bar
    {
        const float max_power = getProtection(stri_value, ALife::eHitTypeStrike);
        update_round_states(stt_strike, stri_value, max_power);
    }
    // explosion wound protection progress bar
    {
        const float max_power = getProtection(expl_value, ALife::eHitTypeExplosion);
        update_round_states(stt_expl, expl_value, max_power);
    }


    // -----------------------------------------------------------------------------------
    m_state[stt_main]->set_progress_shape(conditions.GetRadiation());
    // -----------------------------------------------------------------------------------

    UpdateHitZone();
}

void ui_actor_state_wnd::update_round_states(EStateType stt_type, float initial, float max_power)
{
    auto state = m_state[stt_type];

    const float progress = floor(initial / max_power * 31) / 31; // number of sticks in progress bar
    const float arrow = initial / max_power; //  = 0..1

    if (!state->set_progress(progress))
    {
        //state->set_progress_shape(arrow);
        state->set_arrow(arrow); // 0..1
        state->set_text(arrow); // 0..1
    }
}

void ui_actor_state_wnd::UpdateHitZone()
{
    CUIHudStatesWnd* wnd = CurrentGameUI()->UIMainIngameWnd->get_hud_states(); //некрасиво слишком
    VERIFY(wnd);
    if (!wnd)
    {
        return;
    }
    wnd->UpdateZones();
    if (m_state[stt_main])
        m_state[stt_main]->set_arrow(wnd->get_main_sensor_value());

    /*
    m_state[stt_fire]->set_arrow(wnd->get_zone_cur_power(ALife::eHitTypeBurn));
    m_state[stt_radia]->set_arrow(nd->get_zone_cur_power(ALife::eHitTypeRadiation));
    m_state[stt_acid]->set_arrow(wnd->get_zone_cur_power(ALife::eHitTypeChemicalBurn));
    m_state[stt_psi]->set_arrow(wnd->get_zone_cur_power(ALife::eHitTypeTelepatic));
    */
}

void ui_actor_state_wnd::Draw()
{
    inherited::Draw();
    if (m_hint_wnd)
        m_hint_wnd->Draw();
}

void ui_actor_state_wnd::Show(bool status)
{
    inherited::Show(status);
    ShowChildren(status);
}

/// =============================================================================================
ui_actor_state_item::ui_actor_state_item() : m_magnitude(1.0f), m_sign_inverse(false), m_unit_str("") {}

void ui_actor_state_item::init_from_xml(CUIXml& xml, LPCSTR path, bool critical /*= true*/)
{
    if (!CUIXmlInit::InitWindow(xml, path, 0, this, critical))
        return;

    XML_NODE stored_root = xml.GetLocalRoot();
    XML_NODE new_root = xml.NavigateToNode(path, 0);
    xml.SetLocalRoot(new_root);

    LPCSTR hint_text = xml.Read("hint_text", 0, "no hint");
    set_hint_text_ST(hint_text);

    set_hint_delay((u32)xml.ReadAttribInt("hint_text", 0, "delay"));

    if (xml.NavigateToNode("state_progress", 0))
    {
        m_progress = UIHelper::CreateProgressBar(xml, "state_progress", this);
    }
    if (xml.NavigateToNode("progress_shape", 0))
    {
        m_sensor = xr_new<CUIProgressShape>();
        AttachChild(m_sensor);
        m_sensor->SetAutoDelete(true);
        CUIXmlInit::InitProgressShape(xml, "progress_shape", 0, m_sensor);
    }
    if (xml.NavigateToNode("arrow", 0))
    {
        m_arrow = xr_new<UI_Arrow>();
        m_arrow->init_from_xml(xml, "arrow", this);
    }
    if (xml.NavigateToNode("arrow_shadow", 0))
    {
        m_arrow_shadow = xr_new<UI_Arrow>();
        m_arrow_shadow->init_from_xml(xml, "arrow_shadow", this);
    }
    if (xml.NavigateToNode("icon", 0))
    {
        m_static = UIHelper::CreateStatic(xml, "icon", this);
        m_magnitude = xml.ReadAttribFlt("icon", 0, "magnitude", 1.0f);
        LPCSTR unit_str = xml.ReadAttrib("icon", 0, "unit_str", "");
        m_unit_str._set(StringTable().translate(unit_str));
        m_sign_inverse = (xml.ReadAttribInt("icon", 0, "sign_inverse", 0) == 1);
        m_static->TextItemControl()->SetText("");
    }
    if (xml.NavigateToNode("icon2", 0))
    {
        m_static2 = UIHelper::CreateStatic(xml, "icon2", this);
        m_magnitude = xml.ReadAttribFlt("icon2", 0, "magnitude", 1.0f);
        m_static2->TextItemControl()->SetText("");
    }
    if (xml.NavigateToNode("icon3", 0))
    {
        m_static3 = UIHelper::CreateStatic(xml, "icon3", this);
        m_magnitude = xml.ReadAttribFlt("icon3", 0, "magnitude", 1.0f);
        m_static3->TextItemControl()->SetText("");
    }
    set_arrow(0.0f);
    xml.SetLocalRoot(stored_root);
}

void ui_actor_state_item::init_from_xml_plain(CUIXml& xml, LPCSTR path)
{
    m_progress = UIHelper::CreateProgressBar(xml, path, this);
}

bool ui_actor_state_item::set_text(float value)
{
    if (!m_static)
        return false;

    int v = (int)(value * m_magnitude + 0.49f); // m_magnitude=100
    clamp(v, -999, 999);
    string32 text_res;
    xr_sprintf(text_res, sizeof(text_res), "%+d %s", v, m_unit_str.c_str());
    m_static->TextItemControl()->SetText(text_res);

    bool negative = (value < 0.0f);
    negative = (m_sign_inverse && value != 0.0f) ? !negative : negative;
    u32 color = (negative) ? red_clr : green_clr;
    m_static->SetTextColor(color);

    return true;
}

bool ui_actor_state_item::set_progress(float value)
{
    if (!m_progress)
        return false;

    m_progress->SetProgressPos(value);
    return true;
}

bool ui_actor_state_item::set_progress_shape(float value)
{
    if (!m_sensor)
        return false;

    m_sensor->SetPos(value);
    return true;
}

int ui_actor_state_item::set_arrow(float value)
{
    if (!m_arrow)
        return 0;

    m_arrow->SetNewValue(value);

    if (!m_arrow_shadow)
        return 1;

    m_arrow_shadow->SetPos(m_arrow->GetPos());
    return 2;
}

bool ui_actor_state_item::show_static(bool status, u8 number)
{
    switch (number)
    {
    case 1:
        if (!m_static)
            return false;
        m_static->Show(status);
        break;

    case 2:
        if (!m_static2)
            return false;
        m_static2->Show(status);
        break;

    case 3:
        if (!m_static3)
            return false;
        m_static3->Show(status);
        break;

    default: return false;
    }
    return true;
}
