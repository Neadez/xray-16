//=============================================================================
//  Filename:   UIEncyclopediaWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#include "StdAfx.h"
#include "UIEncyclopediaWnd.h"
#include "xrUICore/Windows/UIFrameWindow.h"
#include "UIXmlInit.h"
#include "xrUICore/ScrollView/UIScrollView.h"
#include "xrUICore/ListWnd/UIListWnd.h"
#include "xrUICore/ListWnd/UITreeViewItem.h"
#include "UIEncyclopediaArticleWnd.h"
#include "Actor.h"
#include "alife_registry_wrappers.h"
#include "encyclopedia_article.h"
#include "UIHelper.h"
#include "Common/object_broker.h"

constexpr pcstr PDA_ENCYCLOPEDIA_XML = "pda_encyclopedia.xml";

CUIEncyclopediaWnd::CUIEncyclopediaWnd() : CUIWindow(CUIEncyclopediaWnd::GetDebugType())
{
    prevArticlesCount = 0;
}

CUIEncyclopediaWnd::~CUIEncyclopediaWnd() { DeleteArticles(); }

bool CUIEncyclopediaWnd::Init()
{
    CUIXml uiXml;
    if (!uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, PDA_ENCYCLOPEDIA_XML, false))
        return false;

    CUIXmlInit::InitWindow(uiXml, "main_wnd", 0, this);

    // Load xml data
    UIEncyclopediaIdxBkg = UIHelper::CreateFrameWindow(uiXml, "left_frame_window", this, false);

    UIEncyclopediaInfoBkg = UIHelper::CreateFrameWindow(uiXml, "right_frame_window", this, false);

    CUIXmlInit::InitFont(uiXml, "tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
    R_ASSERT(m_pTreeItemFont);
    CUIXmlInit::InitFont(uiXml, "tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
    R_ASSERT(m_pTreeRootFont);

    std::ignore = UIHelper::CreateFrameLine(uiXml, "left_frame_line", UIEncyclopediaIdxBkg, false);
    std::ignore = UIHelper::CreateFrameLine(uiXml, "right_frame_line", UIEncyclopediaInfoBkg, false);

    UIEncyclopediaIdxHeader = UIHelper::CreateStatic(uiXml, "left_frame_line_text", UIEncyclopediaIdxBkg, false);

    UIEncyclopediaInfoHeader = UIHelper::CreateStatic(uiXml, "right_frame_line_text", UIEncyclopediaInfoBkg, false);
    
    UIArticleHeader = UIHelper::CreateStatic(uiXml, "article_header_static", UIEncyclopediaInfoBkg, false);
    xr_string caption = "FFD@32-45";
    UIEncyclopediaInfoHeader->SetTextST(caption.c_str());

    UIInfoList = UIHelper::CreateScrollView(uiXml, "info_list", UIEncyclopediaInfoBkg, false);

    UIIdxList = xr_new<CUIListWnd>();
    UIIdxList->SetAutoDelete(true);
    UIEncyclopediaIdxBkg->AttachChild(UIIdxList);
    CUIXmlInit::InitListWnd(uiXml, "idx_list", 0, UIIdxList);
    UIIdxList->SetMessageTarget(this);
    UIIdxList->EnableScrollBar(true);

    return true;
}

void CUIEncyclopediaWnd::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    if (UIIdxList == pWnd && LIST_ITEM_CLICKED == msg)
    {
        xr_string caption = "FFD@32-45";
        CUITreeViewItem* pTVItem = (CUITreeViewItem*)(pData);
        R_ASSERT(pTVItem);

        if (pTVItem->vSubItems.size())
        {
            CEncyclopediaArticle* A = m_ArticlesDB[pTVItem->vSubItems[0]->GetValue()];

            caption += "//";
            caption += CStringTable().translate(A->data()->group).c_str();

            UIEncyclopediaInfoHeader->SetTextST(caption.c_str());
            UIArticleHeader->SetTextST(CStringTable().translate(A->data()->group).c_str());
            SetCurrentArtice(NULL);
        }
        else
        {
            CEncyclopediaArticle* A = m_ArticlesDB[pTVItem->GetValue()];
            caption += "//";
            caption += CStringTable().translate(A->data()->group).c_str();
            caption += "//";
            caption += CStringTable().translate(A->data()->name).c_str();

            UIEncyclopediaInfoHeader->SetTextST(caption.c_str());
            SetCurrentArtice(pTVItem);
            UIArticleHeader->SetTextST(CStringTable().translate(A->data()->name).c_str());
        }
    }

    inherited::SendMessage(pWnd, msg, pData);
}

void CUIEncyclopediaWnd::Draw()
{
    if (m_flags.test(eNeedReload))
    {
        if (Actor()->encyclopedia_registry->registry().objects_ptr() &&
            Actor()->encyclopedia_registry->registry().objects_ptr()->size() > prevArticlesCount)
        {
            ARTICLE_VECTOR::const_iterator it = Actor()->encyclopedia_registry->registry().objects_ptr()->begin();
            std::advance(it, prevArticlesCount);
            for (; it != Actor()->encyclopedia_registry->registry().objects_ptr()->end(); it++)
            {
                if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type)
                {
                    AddArticle(it->article_id, it->readed);
                }
            }
            prevArticlesCount = Actor()->encyclopedia_registry->registry().objects_ptr()->size();
        }

        m_flags.set(eNeedReload, FALSE);
    }

    inherited::Draw();
}

void CUIEncyclopediaWnd::ReloadArticles() { m_flags.set(eNeedReload, TRUE); }

void CUIEncyclopediaWnd::Show(bool status)
{
    if (status)
        ReloadArticles();

    inherited::Show(status);
}

bool CUIEncyclopediaWnd::HasArticle(shared_str id)
{
    ReloadArticles();
    for (std::size_t i = 0; i < m_ArticlesDB.size(); ++i)
    {
        if (m_ArticlesDB[i]->Id() == id)
            return true;
    }
    return false;
}

void CUIEncyclopediaWnd::DeleteArticles()
{
    UIIdxList->RemoveAll();
    delete_data(m_ArticlesDB);
}

void CUIEncyclopediaWnd::SetCurrentArtice(CUITreeViewItem* pTVItem)
{
    UIInfoList->ScrollToBegin();
    UIInfoList->Clear();

    if (!pTVItem)
        return;

    // для начала проверим, что нажатый элемент не рутовый
    if (!pTVItem->IsRoot())
    {
        CUIEncyclopediaArticleWnd* article_info = xr_new<CUIEncyclopediaArticleWnd>();
        article_info->Init("encyclopedia_item.xml", "encyclopedia_wnd:objective_item");
        article_info->SetArticle(m_ArticlesDB[pTVItem->GetValue()]);
        UIInfoList->AddWindow(article_info, true);

        // Пометим как прочитанную
        if (!pTVItem->IsArticleReaded())
        {
            if (Actor()->encyclopedia_registry->registry().objects_ptr())
            {
                for (ARTICLE_VECTOR::iterator it = Actor()->encyclopedia_registry->registry().objects().begin();
                     it != Actor()->encyclopedia_registry->registry().objects().end(); it++)
                {
                    if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type &&
                        m_ArticlesDB[pTVItem->GetValue()]->Id() == it->article_id)
                    {
                        it->readed = true;
                        break;
                    }
                }
            }
        }
    }
}

void CUIEncyclopediaWnd::AddArticle(shared_str article_id, bool bReaded)
{
    for (std::size_t i = 0; i < m_ArticlesDB.size(); i++)
    {
        if (m_ArticlesDB[i]->Id() == article_id)
            return;
    }

    // Добавляем элемент
    m_ArticlesDB.resize(m_ArticlesDB.size() + 1);
    CEncyclopediaArticle*& a = m_ArticlesDB.back();
    a = xr_new<CEncyclopediaArticle>();
    a->Load(article_id);

    // Теперь создаем иерархию вещи по заданному пути

    auto b = xr_new<CUITreeBranch>();
    b->CreateTreeBranch(a->data()->group, a->data()->name, UIIdxList, m_ArticlesDB.size() - 1, m_pTreeRootFont,
        m_uTreeRootColor, m_pTreeItemFont, m_uTreeItemColor, bReaded);
}

void CUIEncyclopediaWnd::Reset()
{
    inherited::Reset();
    ReloadArticles();
}
