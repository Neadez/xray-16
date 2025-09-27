//=============================================================================
//  Filename:   UITreeViewItem.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  TreeView Item class
//=============================================================================

#include "pch.hpp"
#include "UITreeViewItem.h"
#include "UIListWnd.h"
#include "xrUICore/XML/UIXmlInitBase.h"

constexpr auto UNREAD_COLOR = 0xff00ff00;
constexpr auto READ_COLOR = 0xffffffff;

//////////////////////////////////////////////////////////////////////////

// Смещение относительно родителя
const int subShift = 1;
// const char* const treeItemBackgroundTexture = "ui\\ui_pda_over_list";
//  Цвет непрочитанного элемента
static const u32 unreadColor = 0xff00ff00;
constexpr pcstr PDA_ENCYCLOPEDIA_XML = "pda_encyclopedia.xml";

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem::CUITreeViewItem()
    : isRoot(false), isOpened(false), iTextShift(0), pOwner(NULL), m_uUnreadedColor(UNREAD_COLOR),
      m_uReadedColor(READ_COLOR)
{
    CUIXml uiXml;
    uiXml.Load(CONFIG_PATH, UI_PATH, UI_PATH_DEFAULT, PDA_ENCYCLOPEDIA_XML, false);
    UIBkg = xr_new<CUIStatic>("UIBkg");
    UIBkg->SetAutoDelete(true);
    AttachChild(UIBkg);
    // UIBkg->InitTexture(treeItemBackgroundTexture);
    // UIBkg = UIHelper::CreateStatic(uiXml, "ui_pda_over_list", this, false);
    CUIXmlInitBase::InitStatic(uiXml, "ui_pda_over_list", 0, UIBkg);
    UIBkg->TextureOff();
    m_bManualSetColor = false;
    SetTextAlignment(CGameFont::alLeft);
    SetHighlightText(false);
}
//////////////////////////////////////////////////////////////////////////

CUITreeViewItem::~CUITreeViewItem() { DeleteAllSubItems(); }

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::OnRootChanged()
{
    xr_string str;
    if (isRoot)
    {
        // Вставляем после последнего пробела перед текстом знак + или -
        str = GetText();

        xr_string::size_type pos = str.find_first_not_of(" ");
        if (xr_string::npos == pos)
            pos = 0;

        if (pos == 0)
        {
            ++iTextShift;
            str.insert(0, " ");
        }
        else
            --pos;

        if (isOpened)
            // Add minus sign
            str.replace(pos, 1, "-");
        else
            // Add plus sign
            str.replace(pos, 1, "+");

        inherited::SetText(str.c_str());
    }
    else
    {
        str = GetText();
        // Remove "+/-" sign
        xr_string::size_type pos = str.find_first_of("+-");

        if (pos == 0)
        {
            for (int i = 0; i < iTextShift; ++i)
                str.insert(pos, " ");
        }
        else
            str.replace(pos, 1, " ");

        inherited::SetText(str.c_str());
    }
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::OnOpenClose()
{
    // Если мы не являемся узлом дерева, значит ничего не делаем
    if (!isRoot)
        return;

    xr_string str;

    str = GetText();
    xr_string::size_type pos = str.find_first_of("+-");

    if (xr_string::npos != pos)
    {
        if (isOpened)
            // Change minus sign to plus
            str.replace(pos, 1, "-");
        else
            // Change plus sign to minus
            str.replace(pos, 1, "+");
    }

    inherited::SetText(str.c_str());
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::Open()
{
    // Если не рут или уже открыты, то ничего не делаем
    if (!isRoot || isOpened)
        return;
    isOpened = true;

    // Изменяем состояние
    OnOpenClose();

    // Аттачим все подэлементы к родтельскому листбоксу
    CUIListWnd* pList = smart_cast<CUIListWnd*>(GetParent());

    R_ASSERT(pList);
    if (!pList)
        return;

    int pos = pList->GetItemPos(this);

    for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        pList->AddItem(*it, ++pos);
    }
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::Close()
{
    // Если не рут или уже открыты, то ничего не делаем
    if (!isRoot || !isOpened)
        return;
    isOpened = false;

    // Изменяем состояние
    OnOpenClose();

    // Детачим все подэлементы
    CUIListWnd* pList = smart_cast<CUIListWnd*>(GetParent());

    R_ASSERT(pList);
    if (!pList)
        return;

    int pos;

    // Сначала все закрыть
    for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        (*it)->Close();
    }

    // Затем все датачим
    for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        pos = pList->GetItemPos(*it);
        pList->RemoveItem(pos);
    }
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::AddItem(CUITreeViewItem* pItem)
{
    R_ASSERT(pItem);
    if (!pItem)
        return;

    pItem->SetTextShift(subShift + iTextShift);

    vSubItems.push_back(pItem);
    pItem->SetAutoDelete(false);

    pItem->SetOwner(this);
    pItem->SetText(pItem->GetText());
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::DeleteAllSubItems()
{
    for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        CUIWindow* pWindow = (*it)->GetParent();

        if (pWindow)
            pWindow->DetachChild(*it);

        xr_delete(*it);
    }

    vSubItems.clear();
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::SetRoot(bool set)
{
    if (isRoot)
        return;

    isRoot = set;
    OnRootChanged();
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::SetText(LPCSTR str)
{
    xr_string s = str;
    xr_string::size_type pos = s.find_first_not_of(" +-");

    if (pos < static_cast<xr_string::size_type>(iTextShift))
    {
        for (u32 i = 0; i < iTextShift - pos; ++i)
            s.insert(0, " ");
    }
    else if (pos > static_cast<xr_string::size_type>(iTextShift))
    {
        s.erase(0, pos - iTextShift);
    }

    inherited::SetText(s.c_str());
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
    static CUITreeViewItem* pPrevFocusedItem = NULL;

    if (pWnd == this)
    {
        switch (msg)
        {
        case BUTTON_CLICKED: {
            if (IsRoot())
            {
                IsOpened() ? Close() : Open();
            }
            else
            {
                MarkArticleAsRead(true);
            }
            break;
        }
        case WINDOW_FOCUS_RECEIVED: {
            UIBkg->TextureOn();
            if (pPrevFocusedItem)
            {
                pPrevFocusedItem->UIBkg->TextureOff();
            }
            pPrevFocusedItem = this;
            break;
        }
        case WINDOW_FOCUS_LOST: {
            UIBkg->TextureOff();
            pPrevFocusedItem = NULL;
            break;
        }
        };
    }
    else
        inherited::SendMessage(pWnd, msg, pData);
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem* CUITreeViewItem::Find(LPCSTR text) const
{
    // Пробегаемся по списку подчиненных элементов, и ищем элемент с заданным текстом
    // Если среди подч. эл-тов есть root'ы, то ищем рекурсивно в них
    CUITreeViewItem* pResult = NULL;
    xr_string caption;

    for (SubItems::const_iterator it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        caption = (*it)->GetText();
        xr_string::size_type pos = caption.find_first_not_of(" +-");
        if (pos != xr_string::npos)
        {
            caption.erase(0, pos);
        }

        if (xr_strcmp(caption.c_str(), text) == 0)
            pResult = *it;

        if ((*it)->IsRoot() && !pResult)
            pResult = (*it)->Find(text);

        if (pResult)
            break;
    }

    return pResult;
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem* CUITreeViewItem::Find(int value) const
{
    CUITreeViewItem* pResult = NULL;

    for (SubItems::const_iterator it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        if ((*it)->GetValue() == value)
            pResult = *it;

        if ((*it)->IsRoot() && !pResult)
            pResult = (*it)->Find(value);

        if (pResult)
            break;
    }

    return pResult;
}

//////////////////////////////////////////////////////////////////////////

CUITreeViewItem* CUITreeViewItem::Find(CUITreeViewItem* pItem) const
{
    CUITreeViewItem* pResult = NULL;

    for (SubItems::const_iterator it = vSubItems.begin(); it != vSubItems.end(); ++it)
    {
        if ((*it)->IsRoot() && !pResult)
            pResult = (*it)->Find(pItem);
        else if (pItem == *it)
            pResult = *it;

        if (pResult)
            break;
    }

    return pResult;
}

//////////////////////////////////////////////////////////////////////////

xr_string CUITreeViewItem::GetHierarchyAsText() const
{
    xr_string name;

    if (GetOwner())
    {
        name = GetOwner()->GetHierarchyAsText();
    }

    xr_string::size_type prevPos = name.size() + 1;
    name += static_cast<xr_string>("/") + static_cast<xr_string>(GetText());

    // Удаляем мусор: [ +-]
    xr_string::size_type pos = name.find_first_not_of("/ +-", prevPos);
    if (xr_string::npos != pos)
    {
        name.erase(prevPos, pos - prevPos);
    }

    return name;
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::MarkArticleAsRead(bool value)
{
    // Если элемент рутовый, то мы его маркаем его, и все чилды
    if (IsRoot())
    {
        m_bArticleRead = value;
        if (!m_bManualSetColor)
            SetItemColor();

        for (SubItems_it it = vSubItems.begin(); it != vSubItems.end(); ++it)
        {
            (*it)->m_bArticleRead = value;
            (*it)->SetItemColor();
            if ((*it)->IsRoot())
                (*it)->MarkArticleAsRead(value);
        }
    }
    else
    {
        // Если же нет, то маркаем себя и говорим проверить свой парентовый элемент
        m_bArticleRead = value;
        if (!m_bManualSetColor)
            SetItemColor();
        CheckParentMark(GetOwner());
    }
}

//////////////////////////////////////////////////////////////////////////

void CUITreeViewItem::CheckParentMark(CUITreeViewItem* pOwner)
{
    // Берем рута, смотрим на его чилдов, и если среди них есть хоть 1
    // непрочитанный, то маркаем себя как непрочитанный, и  говорим провериться выше.
    bool f = false;
    if (pOwner && pOwner->IsRoot())
    {
        for (SubItems_it it = pOwner->vSubItems.begin(); it != pOwner->vSubItems.end(); ++it)
        {
            if (!(*it)->IsArticleReaded())
            {
                pOwner->m_bArticleRead = false;
                pOwner->SetItemColor();
                f = true;
            }
        }

        if (!f)
        {
            // Если мы тут, то все артиклы прочитанны, и можно маркнуть себя как прочитанная ветвь
            pOwner->m_bArticleRead = true;
            pOwner->SetItemColor();
        }

        pOwner->CheckParentMark(pOwner->GetOwner());
    }
}
