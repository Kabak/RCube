#include "stdafx.h"
#include "AboutMenuElement.h"


AboutMenuElement::AboutMenuElement ()
{ 
	LabelText		= nullptr; 
	FontIndex		= 0; 
	MaxLength		= 0;
	Show			= true;
	TextureIndex	= -1;
};


AboutMenuElement::~AboutMenuElement () 
{ 
	RCUBE_ARR_DELETE ( LabelText );
};


void AboutMenuElement::SetText (char* Text)
{
	StrLength = strlen (Text);

	if ( StrLength >= MaxLength )
	{
		MaxLength = StrLength + 1;

		delete [] LabelText;

		LabelText = new char[ MaxLength ];
	}

	strcpy_s ( LabelText, StrLength, Text);
}


char* AboutMenuElement::GetText ()
{
	return LabelText;
}
