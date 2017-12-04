#include "stdafx.h"

#include "Texture.h"


Texture::Texture ()
{
		Index = -1;
		 Name = nullptr;
		 Type = -1;
		 SRV  = nullptr;
		 DSV  = nullptr;
		 UAV  = nullptr;
		 RTV  = nullptr;
	Resource  = nullptr;
	Texture1D = nullptr;
	Texture2D = nullptr;
	Texture3D = nullptr;
}


Texture::~Texture ()
{
	RCUBE_RELEASE ( SRV );
	RCUBE_RELEASE ( DSV );
	RCUBE_RELEASE ( UAV );
	RCUBE_RELEASE ( RTV );
	// ≈сли была создана текстура, то Resource релизить нельз€ 
	if ( Texture1D == nullptr && Texture2D == nullptr && Texture3D == nullptr )
		RCUBE_RELEASE ( Resource );

	RCUBE_RELEASE ( Texture1D );
	RCUBE_RELEASE ( Texture2D );
	RCUBE_RELEASE ( Texture3D );

	RCUBE_ARR_DELETE ( Name );
}