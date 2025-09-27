#include "StdAfx.h"
#include "UIEncyclopediaArticleWnd.h"
#include "xrUICore/Static/UIStatic.h"
#include "encyclopedia_article.h"
#include "UIXmlInit.h"
#include "UIHelper.h"

CUIEncyclopediaArticleWnd::CUIEncyclopediaArticleWnd() : CUIWindow("CUIEncyclopediaArticleWnd"), m_Article(NULL) {}

CUIEncyclopediaArticleWnd::~CUIEncyclopediaArticleWnd() {}

void CUIEncyclopediaArticleWnd::Init(LPCSTR xml_name, LPCSTR start_from)
{
    CUIXml uiXml;
    bool xml_result = uiXml.Load(CONFIG_PATH, UI_PATH, xml_name);
    R_ASSERT3(xml_result, "xml file not found", xml_name);
    CUIXmlInit xml_init;
    string512 str;

    strcpy_s(str, sizeof(str), start_from);
    xml_init.InitWindow(uiXml, str, 0, this);

    strconcat(sizeof(str), str, start_from, ":image");
    m_UIImage = UIHelper::CreateStatic(uiXml, str, this, false);

    strconcat(sizeof(str), str, start_from, ":text_cont");
    m_UIText = UIHelper::CreateStatic(uiXml, str, this, false);
}

void CUIEncyclopediaArticleWnd::SetArticle(CEncyclopediaArticle* article)
{
    //if (article->data()->image.TextureAvailable())
    //{
        m_UIImage->SetShader(article->data()->image.GetShader());
        m_UIImage->SetTextureRect(article->data()->image.GetStaticItem()->GetTextureRect());
        m_UIImage->SetWndSize(article->data()->image.GetWndSize());

        Fvector2 pos = m_UIImage->GetWndPos();
        pos.x = (GetWidth() - m_UIImage->GetWidth()) / 2.0f;
        m_UIImage->SetWndPos(pos);
    //};
    m_UIText->SetText(*CStringTable().translate(article->data()->text.c_str()));
    m_UIText->AdjustHeightToText();

    AdjustLauout();
}

void CUIEncyclopediaArticleWnd::AdjustLauout()
{
    Fvector2 pos = m_UIText->GetWndPos();
    pos.x = GetWndPos().x;
    pos.y = m_UIImage->GetWndPos().y + m_UIImage->GetHeight();
    m_UIText->SetWndPos(pos);
    SetHeight(m_UIImage->GetWndPos().y + m_UIImage->GetHeight() + m_UIText->GetHeight());
}

void CUIEncyclopediaArticleWnd::SetArticle(LPCSTR article)
{
    CEncyclopediaArticle A;
    A.Load(article);
    SetArticle(&A);
}
