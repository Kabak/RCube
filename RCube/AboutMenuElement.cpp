#include "stdafx.h"
#include "AboutMenuElement.h"


AboutMenuElement::AboutMenuElement ()
{ 
	LabelText		= nullptr; 
	FontIndex		= 0; 
	CurrentLength	= 0;
	MaxLength		= 0;
	Show			= true;
};


AboutMenuElement::~AboutMenuElement () 
{ 
	RCUBE_ARR_DELETE ( LabelText );
};


void AboutMenuElement::SetText (char* Text)
{
	size_t StrLength = strlen (Text);

	if ( StrLength > MaxLength )
	{
		MaxLength = CurrentLength = StrLength;

		RCUBE_ARR_DELETE ( LabelText );

		LabelText = new char[StrLength];

		strcpy_s ( LabelText, StrLength, Text);
	}
	else
	{
		strcpy_s ( LabelText, StrLength, Text);
		CurrentLength = StrLength;
	}
}


char* AboutMenuElement::GetText ()
{
	return LabelText;
}