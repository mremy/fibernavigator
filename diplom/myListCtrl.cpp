#include "myListCtrl.h"
#include "datasetInfo.h"
#include "mainFrame.h"
#include "selectionBox.h"

BEGIN_EVENT_TABLE(MyListCtrl, wxListCtrl)
	EVT_LEFT_DOWN(MyListCtrl::OnLeftClick)
END_EVENT_TABLE()



void MyListCtrl::OnLeftClick(wxMouseEvent& event)
{
	int col;
	int x = event.GetPosition().x;
	int sizeX = 0;
	for ( col = 0 ; col < GetColumnCount() ; ++col)
	{
		sizeX += GetColumnWidth(col);
		if (x <= sizeX) break;
	}
	m_col_clicked = col + 10;
	m_col_activated = col + 10;

	event.Skip();
}

int MyListCtrl::getColClicked()
{
	int r = m_col_clicked;
	m_col_clicked = 0;
	return r;
}

int MyListCtrl::getColActivated()
{
	int r = m_col_activated;
	m_col_activated = 0;
	return r;
}


void MyListCtrl::swap(long a, long b)
{
	DatasetInfo *infoA = (DatasetInfo*) GetItemData(a);
	DatasetInfo *infoB = (DatasetInfo*) GetItemData(b);

	SetItem(a, 0, wxT(""), infoB->getShow() ? 0 : 1);
	SetItem(a, 1, infoB->getName());
	SetItem(a, 2, wxString::Format(wxT("%.2f"), infoB->getThreshold()));
	SetItemData(a, (long)infoB);

	SetItem(b, 0, wxT(""), infoA->getShow() ? 0 : 1);
	SetItem(b, 1, infoA->getName());
	SetItem(b, 2, wxString::Format(wxT("%.2f"), infoA->getThreshold()));
	SetItemData(b, (long)infoA);
}

void MyListCtrl::moveItemUp(long item)
{
	if (item == 0) return;
	swap (item - 1, item);
	SetItemState(item - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void MyListCtrl::moveItemDown(long item)
{
	if (item == GetItemCount() - 1) return;
	swap (item, item +1);
	SetItemState(item + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

DECLARE_EVENT_TYPE(wxEVT_TREE_EVENT, -1)
DEFINE_EVENT_TYPE(wxEVT_TREE_EVENT)

BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
	EVT_CHAR(MyTreeCtrl::OnChar)
	EVT_RIGHT_UP(MyTreeCtrl::OnRightClick)
	EVT_MENU(TREE_CTRL_TOGGLE_ANDNOT, MyTreeCtrl::OnToggleAndNot)
	EVT_MENU(TREE_CTRL_DELETE_BOX, MyTreeCtrl::OnDeleteBox)
END_EVENT_TABLE()

int MyTreeCtrl::getSelectedType()
{
	wxTreeItemId treeid = GetSelection();

	wxTreeItemId pId = GetItemParent(treeid);
	wxTreeItemId ppId = GetItemParent(pId);

	if (GetItemText(pId) == _T("selection boxes"))
		return MasterBox;
	else if (GetItemText(ppId) == _T("selection boxes"))
		return ChildBox;
	else if (GetItemText(pId) == _T("points"))
		return Point_;
	else return 0;
}
void MyTreeCtrl::OnChar(wxKeyEvent& event)
{
	int selected = getSelectedType();

	wxTreeItemId treeid = GetSelection();

	wxTreeItemId pId = GetItemParent(treeid);
	wxTreeItemId ppId = GetItemParent(pId);

	if ( event.GetKeyCode() == WXK_DELETE)
    {
    	if (selected == ChildBox)
		{
			((SelectionBox*) ((GetItemData(pId))))->setDirty();
		}
    	if (selected == ChildBox || selected == MasterBox || selected == Point_)
    	{
			Delete(treeid);
			wxCommandEvent event1( wxEVT_TREE_EVENT, GetId() );
			GetEventHandler()->ProcessEvent( event1 );
    	}
    }

	else if (selected == MasterBox || selected == ChildBox)
	{
		switch( event.GetKeyCode() )
		{
		case WXK_LEFT:
			 if (wxGetKeyState(WXK_CONTROL))
				 ((SelectionBox*) (GetItemData(treeid)))->resizeLeft();
			 else
				 ((SelectionBox*) (GetItemData(treeid)))->moveLeft();
			 break;
		case WXK_RIGHT:
			if (wxGetKeyState(WXK_CONTROL))
				((SelectionBox*) (GetItemData(treeid)))->resizeRight();
			else
				((SelectionBox*) (GetItemData(treeid)))->moveRight();
			break;
		case WXK_UP:
			if (wxGetKeyState(WXK_CONTROL))
				((SelectionBox*) (GetItemData(treeid)))->resizeForward();
			else
				((SelectionBox*) (GetItemData(treeid)))->moveForward();
			break;
		case WXK_DOWN:
			if (wxGetKeyState(WXK_CONTROL))
				((SelectionBox*) (GetItemData(treeid)))->resizeBack();
			else
				((SelectionBox*) (GetItemData(treeid)))->moveBack();
			break;
		case WXK_PAGEUP:
			if (wxGetKeyState(WXK_CONTROL))
				((SelectionBox*) (GetItemData(treeid)))->resizeUp();
			else
				((SelectionBox*) (GetItemData(treeid)))->moveUp();
			break;
		case WXK_PAGEDOWN:
			if (wxGetKeyState(WXK_CONTROL))
				((SelectionBox*) (GetItemData(treeid)))->resizeDown();
			else
				((SelectionBox*) (GetItemData(treeid)))->moveDown();
			break;
		case WXK_HOME:
			((SelectionBox*) (GetItemData(treeid)))->lockToCrosshair();
			break;
		default:
			event.Skip();
			return;
		}
	}
    Refresh(false);
}

void MyTreeCtrl::OnRightClick(wxMouseEvent& event)
{
	int selected = getSelectedType();

	wxMenu* menu = new wxMenu;

	if (selected == MasterBox)
	{
		SelectionBox* box =((SelectionBox*) (GetItemData( GetSelection())));
		if (box->getActive())
			menu->Append(MENU_VOI_TOGGLE_SELBOX, _T("deactivate"));
		else
			menu->Append(MENU_VOI_TOGGLE_SELBOX, _T("activate"));
		if (box->getShow())
			menu->Append(MENU_VOI_TOGGLE_SHOWBOX, _T("hide"));
		else
			menu->Append(MENU_VOI_TOGGLE_SHOWBOX, _T("show"));

		menu->AppendSeparator();
		menu->Append(TREE_CTRL_DELETE_BOX, _T("delete"));
	}

	if (selected == ChildBox)
	{
		SelectionBox* box =((SelectionBox*) (GetItemData( GetSelection())));
		menu->Append(TREE_CTRL_TOGGLE_ANDNOT, _T("toggle AND/NOT"));
		menu->AppendSeparator();
		if (box->getActive())
			menu->Append(MENU_VOI_TOGGLE_SELBOX, _T("deactivate"));
		else
			menu->Append(MENU_VOI_TOGGLE_SELBOX, _T("activate"));
		if (box->getShow())
			menu->Append(MENU_VOI_TOGGLE_SHOWBOX, _T("hide"));
		else
			menu->Append(MENU_VOI_TOGGLE_SHOWBOX, _T("show"));
		menu->AppendSeparator();
		menu->Append(TREE_CTRL_DELETE_BOX, _T("delete"));

	}
	PopupMenu(menu, event.GetPosition());
}

void MyTreeCtrl::OnToggleAndNot(wxCommandEvent& WXUNUSED(event))
{
	wxTreeItemId treeid = GetSelection();
	((SelectionBox*) (GetItemData(treeid)))->toggleNOT();
}

void MyTreeCtrl::OnDeleteBox(wxCommandEvent& WXUNUSED(event))
{
	if (getSelectedType() == ChildBox)
	{
		((SelectionBox*) ((GetItemData(GetItemParent(GetSelection())))))->setDirty();
	}
	Delete(GetSelection());
	wxCommandEvent event1( wxEVT_TREE_EVENT, GetId() );
	GetEventHandler()->ProcessEvent( event1 );
}
