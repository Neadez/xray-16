//////////////////////////////////////////////////////////////////////////
//  Function for automatic tree hierarchy creation
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "xrUIcore/ListWnd/UITreeViewItem.h"
    DEF_VECTOR(GroupTree, shared_str);

void CreateTreeBranch(shared_str nestingTree, shared_str leafName, CUIListWnd* pListToAdd, int leafProperty,
    CGameFont* pRootFont, u32 rootColor, CGameFont* pLeafFont, u32 leafColor, bool markRead);
