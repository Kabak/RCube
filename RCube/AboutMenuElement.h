#pragma once
#include "stdafx.h"

// Text string displayed when mouse is on the body of this menu element
class AboutMenuElement
{
public:
	AboutMenuElement ();
	~AboutMenuElement ();

	int		FontIndex;
	size_t	CurrentLength;
	size_t	MaxLength;

	int		MenuObjIndex;
	int		MenuObjType;

	bool	Show;

	void	SetText (char* Text);
    char*	GetText ();

private:

	char*	LabelText;// Text string displayed when mouse is on the body of element

};