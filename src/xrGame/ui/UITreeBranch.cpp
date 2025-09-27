//////////////////////////////////////////////////////////////////////////
// Standalone function for tree hierarchy creation
//////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "UITreeBranch.h"
#include "xrUIcore/ListWnd/UIListWnd.h"

void CreateTreeBranch(shared_str nesting, shared_str leafName, CUIListWnd* pListToAdd, int leafProperty,
    CGameFont* pRootFont, u32 rootColor, CGameFont* pLeafFont, u32 leafColor, bool markRead)
{
    // Nested function emulation
    class AddTreeTail_
    {
    private:
        CGameFont* pRootFnt;
        u32 rootItemColor;

    public:
        AddTreeTail_(CGameFont* f, u32 cl) : pRootFnt(f), rootItemColor(cl) {}

        CUITreeViewItem* operator()(GroupTree_it it, GroupTree& cont, CUITreeViewItem* pItemToIns)
        {
            // Вставляем иерархию разделов в энциклопедию
            CUITreeViewItem* pNewItem = NULL;

            for (GroupTree_it it2 = it; it2 != cont.end(); ++it2)
            {
                pNewItem = xr_new<CUITreeViewItem>();
                pItemToIns->AddItem(pNewItem);
                pNewItem->SetFont(pRootFnt);
                pNewItem->SetText(*(*it2));
                pNewItem->SetReadedColor(rootItemColor);
                pNewItem->SetRoot(true);
                pItemToIns = pNewItem;
            }

            return pNewItem;
        }
    }AddTreeTail(pRootFont, rootColor);

    //-----------------------------------------------------------------------------
    //  Function body
    //-----------------------------------------------------------------------------

    // Начинаем алгоритм определения группы вещи в иерархии энциклопедии
    R_ASSERT(*nesting);
    R_ASSERT(pListToAdd);
    R_ASSERT(pLeafFont);
    R_ASSERT(pRootFont);
    xr_string group = *nesting;

    // Парсим строку группы для определения вложенности
    GroupTree groupTree;

    xr_string::size_type pos;
    xr_string oneLevel;

    while (true)
    {
        pos = group.find('/');
        if (pos != xr_string::npos)
        {
            oneLevel.assign(group, 0, pos);
            shared_str str(oneLevel.c_str());
            groupTree.push_back(CStringTable().translate(str));
            group.erase(0, pos + 1);
        }
        else
        {
            groupTree.push_back(CStringTable().translate(group.c_str()));
            break;
        }
    }

    // Теперь ищем нет ли затребованных групп уже в наличии
    CUITreeViewItem *pTVItem = NULL, *pTVItemChilds = NULL;
    bool status = false;

    // Для всех рутовых элементов
    for (u32 i = 0; i < pListToAdd->GetItemsCount(); ++i)
    {
        pTVItem = smart_cast<CUITreeViewItem*>(pListToAdd->GetItem(i));
        R_ASSERT(pTVItem);

        pTVItem->Close();

        xr_string caption = pTVItem->GetText();
        // Remove "+" sign
        caption.erase(0, 1);

        // Ищем не содержит ли он данной иерархии и добавляем новые элементы если не найдено
        if (0 == xr_strcmp(caption.c_str(), *groupTree.front()))
        {
            // Уже содержит. Надо искать глубже
            pTVItemChilds = pTVItem;
            for (GroupTree_it it = groupTree.begin() + 1; it != groupTree.end(); ++it)
            {
                pTVItem = pTVItemChilds->Find(*(*it));
                // Не нашли, надо вставлять хвост списка вложенности
                if (!pTVItem)
                {
                    pTVItemChilds = AddTreeTail(it, groupTree, pTVItemChilds);
                    status = true;
                    break;
                }
                pTVItemChilds = pTVItem;
            }
        }

        pTVItem->Open();
        pTVItem->Close();

        if (status)
            break;
    }

    // Прошли все существующее дерево, и не нашли? Тогда добавляем новую иерархию
    if (!pTVItemChilds)
    {
        pTVItemChilds = xr_new<CUITreeViewItem>();
        pTVItemChilds->SetFont(pRootFont);
        pTVItemChilds->SetText(*groupTree.front());
        pTVItemChilds->SetReadedColor(rootColor);
        pTVItemChilds->SetRoot(true);
        pListToAdd->AddItem<CUITreeViewItem>(pTVItemChilds);

        // Если в списке вложенности 1 элемент, то хвоста нет, и соответственно ничего не добавляем
        if (groupTree.size() > 1)
            pTVItemChilds = AddTreeTail(groupTree.begin() + 1, groupTree, pTVItemChilds);
    }

    // К этому моменту pTVItemChilds обязательно должна быть не NULL
    R_ASSERT(pTVItemChilds);

    // Cначала проверяем нет ли записи с таким названием, и добавляем если нет
    //	if (!pTVItemChilds->Find(*name))
    //	{
    pTVItem = xr_new<CUITreeViewItem>();
    pTVItem->SetFont(pLeafFont);
    pTVItem->SetReadedColor(leafColor);
    pTVItem->SetText(*CStringTable().translate(*leafName));
    pTVItem->SetValue(leafProperty);
    pTVItemChilds->AddItem(pTVItem);
    pTVItem->MarkArticleAsRead(markRead);
    //	}
}
