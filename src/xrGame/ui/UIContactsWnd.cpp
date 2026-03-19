#include "StdAfx.h"
#include "UIContactsWnd.h"
#include "Pda.h"
#include "UIXmlInit.h"
#include "xrUICore/Windows/UIFrameLineWnd.h"
#include "xrUICore/Windows/UIFrameWindow.h"
#include "xrUICore/ScrollView/UIScrollView.h"
#include "Actor.h"

#define PDA_CONTACT_HEIGHT 70

#define PDA_CONTACTS_XML "pda_contacts.xml"

CUIPdaContactsWnd::CUIPdaContactsWnd(): CUIWindow(CUIPdaContactsWnd::GetDebugType())
{
}

CUIPdaContactsWnd::~CUIPdaContactsWnd()
{
}

