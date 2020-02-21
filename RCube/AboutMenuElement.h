#pragma once
#include "stdafx.h"

#ifndef _ABOUTMENUELEMENT_H_
#define _ABOUTMENUELEMENT_H_

#include <DirectXMath.h>
//#include <vector>

using namespace std;

// Text string displayed when mouse is on the body of this menu element
class AboutMenuElement
{
public:
	AboutMenuElement ();
	~AboutMenuElement ();

	int		FontIndex;
	size_t	StrLength;	// Current string length 
	size_t	MaxLength;	// Max length without recreation

	int		MenuObjIndex;	// Which menu object has relation to this about object
	int		MenuObjType;	// Menu Element type

	XMFLOAT4 ObjParam;	// x - position , y - position , z - Width in pixels , w - Height in pixels

	float	FontScale;	// Calculated value for scale down font size to draw strings in Obj Param dimensions

	bool	Show;	// Draw that object ?  1 - draw ,  0 - NOT draw

	int		TextureIndex;	// Texture number on which all strings rendered

	int		SentencesGroupIndex; // see GLOBAL_TEXT_INDEXES


	void	SetText (char* Text);	// Apply new string 

	char*	GetText ();

private:

	char*	LabelText;// Text string displayed when mouse is on the body of element

};


#endif