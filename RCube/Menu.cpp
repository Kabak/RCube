#include "stdafx.h"
#include "Menu.h"


Menu::Menu ()
{


}


Menu::~Menu ()
{
	int c = 0;

	RCUBE_DELETE ( Background );

	// ������� ��� ������
	int size = (int)ScrollBars.size ();
	for (int c = 0; c < size; ++c)
	{
		delete ScrollBars[c];
	}
	ScrollBars.clear ();

	// ������� ��� ������
	size = (int)Buttons.size ();
	for (int c = 0; c < size; ++c)
	{
		delete Buttons[c];
	}
	Buttons.clear ();

	// ������� ��� StringLists
	size = (int)StringsList.size ();
	for (int c = 0; c < size; ++c)
	{
		delete StringsList[c];
	}
	StringsList.clear ();

	// ������� ��� StringLists
	size = ( int ) ColorPickers.size ();
	for ( int c = 0; c < size; ++c )
	{
		delete ColorPickers[c];
	}
	ColorPickers.clear ();
	// --------------------------------------------------
}