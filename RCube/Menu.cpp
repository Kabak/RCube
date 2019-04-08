#include "stdafx.h"
#include "Menu.h"


Menu::Menu ()
{


}


Menu::~Menu ()
{
	int c = 0;

	RCUBE_DELETE ( Background );

	// Удаляем все кнопки
	int size = (int)ScrollBars.size ();
	for (int c = 0; c < size; ++c)
	{
		delete ScrollBars[c];
	}
	ScrollBars.clear ();

	// Удаляем все кнопки
	size = (int)Buttons.size ();
	for (int c = 0; c < size; ++c)
	{
		delete Buttons[c];
	}
	Buttons.clear ();

	// Удаляем все StringLists
	size = (int)StringsList.size ();
	for (int c = 0; c < size; ++c)
	{
		delete StringsList[c];
	}
	StringsList.clear ();

	// Удаляем все StringLists
	size = ( int ) ColorPickers.size ();
	for ( int c = 0; c < size; ++c )
	{
		delete ColorPickers[c];
	}
	ColorPickers.clear ();
	// --------------------------------------------------
}