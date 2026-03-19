#pragma once
#include "xrUICore/Windows/UIWindow.h"
#include "encyclopedia_article_defs.h"

class CEncyclopediaArticle;
class CUIFrameWindow;
class CUIFrameLineWnd;
class CUIStatic;
class CUIListWnd;
class CUIEncyclopediaCore;
class CUIScrollView;
class CUITreeViewItem;

class CUIEncyclopediaWnd : public CUIWindow
{
private:
    typedef CUIWindow inherited;
    enum
    {
        eNeedReload = (1 << 0),
    };
    Flags16 m_flags;

public:
    CUIEncyclopediaWnd();
    virtual ~CUIEncyclopediaWnd();

    virtual bool Init();
    virtual void Show(bool status);
    virtual void SendMessage(CUIWindow* pWnd, s16 msg, void* pData = NULL);
    virtual void Draw();

    void AddArticle(shared_str, bool bReaded);
    void DeleteArticles();
    bool HasArticle(shared_str);

    void ReloadArticles();
    virtual void Reset();

protected:
    u32 prevArticlesCount;
    // Элементы графического оформления
    CUIFrameWindow* UIEncyclopediaIdxBkg;
    CUIFrameWindow* UIEncyclopediaInfoBkg;
    CUIStatic* UIEncyclopediaIdxHeader;
    CUIStatic* UIEncyclopediaInfoHeader;
    CUIStatic* UIArticleHeader;

    // Хранилище статей
    typedef xr_vector<CEncyclopediaArticle*> ArticlesDB;
    typedef ArticlesDB::iterator ArticlesDB_it;

    ArticlesDB m_ArticlesDB;
    CGameFont* m_pTreeRootFont;
    u32 m_uTreeRootColor;
    CGameFont* m_pTreeItemFont;
    u32 m_uTreeItemColor;

    CUIListWnd* UIIdxList;
    CUIScrollView* UIInfoList;

    void SetCurrentArtice(CUITreeViewItem* pTVItem);
};
