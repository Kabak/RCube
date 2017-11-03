#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"

TextClass::TextClass()
{
	FontsNumber	= 0;
   FontsCreated = 0;

		 m_Font.reserve(10);

	mouseString = new char[STRMAXLENGTH];
	 tempString = new char[STRMAXLENGTH];
	  fpsString = new char[STRMAXLENGTH];
	  cpuString = new char[STRMAXLENGTH];

	  Triangel1 = new char[STRMAXLENGTH];
	  Triangel2 = new char[STRMAXLENGTH];
		   Gran = new char[STRMAXLENGTH];
		  Cube1 = new char[STRMAXLENGTH];
		  Cube2 = new char[STRMAXLENGTH];
		   Side = new char[STRMAXLENGTH];
	  
	  Sentences.reserve(200);
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
	delete [] mouseString;
	delete [] tempString;
	delete [] fpsString;
	delete [] cpuString;

	delete [] Triangel1;
	delete [] Triangel2;
	delete [] Cube1;
	delete [] Cube2;
	delete [] Gran;
	delete [] Side;
}


bool TextClass::Initialize(D3DGlobalContext* D3DGC, vector < FontOnTextureData* > FontList)
{
	bool result;

	Local_D3DGC = D3DGC;

	m_screenWidth = D3DGC->ScreenWidth;
	m_screenHeight = D3DGC->ScreenHeight;

	// ѕолучаем количество созданных типов шрифтов и генерим фонтовые текстуры с координатами символов в них
	FontsNumber = (UINT) FontList.size();
	FontClass* m_Font1;
	for(UINT i = 0; i < FontsNumber; ++i )
	{
		m_Font1 = new FontClass;

		result = m_Font1->Initialize(D3DGC , FontList[i] );
		if(!result)
		{
			MessageBox( D3DGC->hwnd, L"Could not initialize the font object in TextClass.", L"Error", MB_OK);
			return false;
		}
		m_Font.emplace_back (m_Font1);
		++FontsCreated;
	}

// Create and initialize the two strings that will be used for this tutorial. One string says Hello in white at 100, 100 and the other says Goodbye in yellow at 100, 200. The UpdateSentence 
// function can be called to change the contents, location, and color of the strings at any time.

	SENTENCE_INIT_DATA* Data = new SENTENCE_INIT_DATA;
	ZeroMemory(Data, sizeof(SENTENCE_INIT_DATA));
	Data->MaxLength = 27;
	Data->FontType = 1;
	Data->PosX = 100;
	Data->PosY = 100;
	Data->Colour = { 0.0f, 0.0f, 1.0f, 1.0f };
	Data->Render = true;
	Data->ShowType = SHOW;
	Data->Level = 0;

	int Number = AddSentence(Data, "FPS");

	Data->MaxLength = 35;
	Data->PosY = 200;
	Data->Colour = { 1.0f, 0.0f, 1.0f, 1.0f };
	Data->FontType = 5;
	Data->Render = true;
	Number = AddSentence(Data, "CPU");

	delete Data;

	return true;
}

// The Shutdown function will release the two sentences, the font object, and the font shader object.

void TextClass::Shutdown()
{
	DeleteAllSentences();

	// Release the font object.
	for (UINT i = 0; i < FontsCreated; ++i)
	{
		RCUBE_SHUTDOWN( m_Font[i] );
		RCUBE_DELETE ( m_Font[i] );
	}
	m_Font.clear();

	return;
}

// Render will draw the two sentences to the screen.

bool TextClass::Render( int Level )
{
	bool result = true;

	SetFps(fpstimers.FpsRate);
	SetCpu(fpstimers.CpuVal);

	Local_D3DGC->DX_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (UINT i = 0, j = (UINT)Sentences.size(); i < j ; ++i)
	{
		if ( Sentences[i]->Render == true && Sentences[i]->Level == Level )
		{
			switch ( Sentences[i]->ShowType )
			{
				case 0:
					break;

				case 1:
					break;

				case 2:
					ShowGlowing(i);
					break;

				case 3:
					ShowScrolling(i);
					break;

				default:;
			}
			result = RenderSentence( Sentences[i] );
			if (!result)
			{
				return false;
			}
		}
	}


	return true;
}

// The InitializeSentence function creates a SentenceType with an empty vertex buffer which will be used to store and render sentences. The maxLength input parameter determines how large the 
// vertex buffer will be. All sentences have a vertex and index buffer associated with them which is initialized first in this function.

bool TextClass::InitializeSentence( SentenceType* sentence, int& maxLength )
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Initialize the sentence buffers to null.
	sentence->vertexBuffer = nullptr;
	sentence->indexBuffer = nullptr;

	// Set the maximum length of the sentence.
	sentence->MaxLength = maxLength;

	// Set the number of vertices in the vertex array.
	sentence->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	sentence->indexCount = sentence->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[sentence->indexCount];
	if(!indices)
	{
		return false;
	}


	// Initialize the index array.
	for(int i=0; i<sentence->indexCount; ++i)
	{
		indices[i] = i;
	}


// During the creation of the vertex buffer description for the sentence we set the Usage type to dynamic as we may want to change the contents of the sentence at any time.

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * sentence->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = Local_D3DGC->DX_device->CreateBuffer(&vertexBufferDesc, &vertexData, &sentence->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

// The index buffer is setup as a normal static buffer since the contents will never need to change.

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * sentence->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = Local_D3DGC->DX_device->CreateBuffer(&indexBufferDesc, &indexData, &sentence->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

// UpdateSentence changes the contents of the vertex buffer for the input sentence. It uses the Map and Unmap functions along with memcpy to update the contents of the vertex buffer.
bool TextClass::UpdateSentence(int SentenceNumber, char* text, int positionX, int positionY, float RenderFontSize )
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	SentenceType *Source = Sentences[SentenceNumber];

	// Check for possible buffer overflow.
	if (numLetters > Source->MaxLength)
	{
		text[Source->MaxLength - 1] = NULL;
	}
	// копируем новый текст предложени€ в объект предложени€ ( скролинг бе этого не работает )
	strcpy_s( Source->Text, Source->MaxLength, text);

	// Create the vertex array.
	vertices = new VertexType[Source->vertexCount];

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * Source->vertexCount));

	// ќбновл€ем позицию предложени€ 
	Source->PosX = positionX;
	Source->PosY = positionY;

	// Calculate the starting location to draw the sentence on the screen at.
	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	// Build the vertex array using the FontClass and the sentence information.
	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font[Source->FontType]->BuildVertexArray((void*)vertices, text, drawX, drawY, RenderFontSize);

	// Copy the vertex array information into the sentence vertex buffer.
	Local_D3DGC->DX_deviceContext->UpdateSubresource( Source->vertexBuffer, NULL, NULL, (void*)vertices, NULL, NULL);
	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = nullptr;

	return true;
}


//  RenderSentence function puts the sentence vertex and index buffer on the input assembler and then calls the FontShaderClass object to draw the sentence that was given as input to this function. 
// Notice that we use the m_baseViewMatrix instead of the current view matrix. This allows us to draw text to the same location on the screen each frame regardless of where the current view may be. Likewise we use the orthoMatrix instead of the regular projection matrix since this should be drawn using 2D coordinates.
bool TextClass::RenderSentence( SentenceType* sentence )
{
	unsigned int stride, offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	Local_D3DGC->DX_deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	ID3D11ShaderResourceView* texture = m_Font[sentence->FontType]->GetTexture ();
	Local_D3DGC->DX_deviceContext->PSSetShaderResources ( 0, 1, &texture );

	Local_D3DGC->DX_deviceContext->DrawIndexed ( sentence->indexCount, 0, 0 );

	return true;
}

// We now have a new function in the TextClass which converts the mouse X and Y position into two strings and then updates the two sentences so that the position of the mouse can be rendered 
// to the screen.

bool TextClass::SetMousePosition(int& mouseX, int& mouseY)
{
	bool result;


	// Convert the mouseX integer to string format.
	_itoa_s(mouseX, tempString, STRMAXLENGTH, 10);

	// Setup the mouseX string.
	strcpy_s(mouseString, STRMAXLENGTH, "Mouse X: ");
	strcat_s(mouseString, STRMAXLENGTH, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(0, mouseString, 20, 20 );
	if(!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	_itoa_s(mouseY, tempString , STRMAXLENGTH, 10);

	// Setup the mouseY string.
	strcpy_s(mouseString, STRMAXLENGTH, "Mouse Y: ");
	strcat_s(mouseString, STRMAXLENGTH, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(0, mouseString, 20, 40 );
	if(!result)
	{
		return false;
	}

	return true;
}

// The SetFps function takes the fps integer value given to it and then converts it to a string. Once the fps count is in a string format it gets concatenated to another string so it has a 
// prefix indicating that it is the fps speed. After that it is stored in the sentence structure for rendering. The SetFps function also sets the color of the fps string to green if above 60 fps, 
// yellow if below 60 fps, and red if below 30 fps.

bool TextClass::SetFps(int fps)
{
	//	char tempString[32];
	//	char fpsString[32];
	//	float red, green, blue;
	bool result;

	// Truncate the fps to below 10,000.
	if (fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, STRMAXLENGTH, 10);

	// Setup the fps string.
	strcpy_s(fpsString, STRMAXLENGTH, "English Font FG Fps : ");// !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTQVWXYZ[\]^_`abcdefghijklmnopqrstqvwxyz
	strcat_s(fpsString, STRMAXLENGTH, tempString);

	// If fps is 60 or above set the fps color to green.
	if (fps >= 60)
	{
		ChangeSentenceColour(0, 0.0f, 1.0f, 0.0f, 1.0f);
	}

	// If fps is below 60 set the fps color to yellow.
	if (fps < 60)
	{
		ChangeSentenceColour(0, 1.0f, 1.0f, 0.0f, 1.0f);
	}

	// If fps is below 30 set the fps color to red.
	if (fps < 30)
	{
		ChangeSentenceColour(0, 1.0f, 0.0f, 0.0f, 1.0f);
	}

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(0, fpsString, 20, 20);
	if (!result)
	{
		return false;
	}

	return true;
}

// The SetCpu function is similar to the SetFps function. It takes the cpu value and converts it to a string which is then stored in the sentence structure and rendered.
bool TextClass::SetCpu(int cpu)
{
//	char tempString[16];
//	char cpuString[16];
	bool result;


	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, STRMAXLENGTH, 10);

	// Setup the cpu string.
	strcpy_s(cpuString,  STRMAXLENGTH, "»—ѕќЋ№«ќ¬јЌ»≈ ѕ–ќ÷≈——ќ–ј: "); //јЅ¬√ƒ≈∆«»… ЋћЌќѕ–—“”‘’÷„ЎўЏџ№Ёёяабвгдежзийклмнопрстуфхцчшщъыьэю€®Є
	strcat_s(cpuString,  STRMAXLENGTH, tempString);
	strcat_s(cpuString,  STRMAXLENGTH, "%");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence( 1, cpuString, 20, 50 );
	if(!result)
	{
		return false;
	}

	return true;
}


int TextClass::AddSentence(SENTENCE_INIT_DATA* data, char* String)
{
	bool result;
// ≈сли текст больше или равен максимальной длинне строки позволенной в классе STRMAXLENGTH или 
// строка инициализации больше указанной желаемой длинны строки, или максимальна€ длинна строки
// меньше 1 , то возвращаем -1 вместо номера строки в массиве - строка не создаЄтс€
	int Size = (int)strlen(String);
	if ( data->MaxLength > STRMAXLENGTH || Size > STRMAXLENGTH || data->MaxLength < 0 )
	{
		return -1;		
	}
	else if ( data->MaxLength == 0 || Size + 2 > data->MaxLength ) // Size + 2 - дл€ мигающего курсора + \0 
	{
		data->MaxLength = Size + 2;
	}

	SentenceType* Sentence1 = new SentenceType;
	ZeroMemory(Sentence1, sizeof(SentenceType));

	int Num = (int)Sentences.size();
	Sentences.emplace_back ( Sentence1 );

	SentenceType* Source = Sentences[Num];

	result = InitializeSentence( Source, data->MaxLength );
	if ( !result )
		return -1;

	Source->Colour = XMLoadFloat4(&data->Colour);
	Source->FontType = data->FontType;
	Source->Render = data->Render;
	Source->MaxLength = data->MaxLength;
	Source->PosX = data->PosX;
	Source->PosY = data->PosY;
	Source->ShowType = data->ShowType;
	Source->HideType = data->HideType;
	Source->Level = data->Level;
	Source->Height = m_Font[data->FontType]->GetFontHeight();

	if ( data->ScrollSpeed == 0 )	Source->ScrollSpeed = 30.0f;
	
	strcpy_s( Source->Text, Source->MaxLength, String);

	result = UpdateSentence(Num, Source->Text, data->PosX, data->PosY );
	if (!result)
		return -1;

	return (int)(Sentences.size()-1);
}


void TextClass::DeleteAllSentences()
{
	UINT i = 0;
	UINT j = ( UINT )Sentences.size();

	if (j > 0)
	do
	{
		Sentences[i]->vertexBuffer->Release();
		Sentences[i]->indexBuffer->Release();
		delete Sentences[i];
		++i;
	} while (i < j);

	Sentences.clear();
}


int TextClass::DeleteSentence( int SentenceNumber )
{
	int i = (int)Sentences.size();
	if ( SentenceNumber < i )
	{
		Sentences[SentenceNumber]->vertexBuffer->Release();
		Sentences[SentenceNumber]->indexBuffer->Release();
		delete Sentences[SentenceNumber];
		Sentences.erase(Sentences.begin() + SentenceNumber );
		return 0;
	}
	else
		return -1;

}


void TextClass::ChangeSentenceColour(int SentenceNumber, float Red, float Green, float Blue, float Alpha)
{
	Sentences[SentenceNumber]->Colour = XMVectorSet ( Red, Green, Blue, Alpha );
}


void TextClass::ChangeRender( int SentenceNumber, bool Switch )
{
	Sentences[SentenceNumber]->Render = Switch;
}


void TextClass::ShowGlowing(int i)
{
	RCube_VecFloat34 TextV34;

	SentenceType* Source = Sentences[i];
	if ( Source->ShowGoOn == false)
	{
		TextV34.Vec = Source->Colour;
		Source->ColourTemp = TextV34.Fl4;
//		XMStoreFloat4(&Source->ColourTemp, Source->Colour);
		Source->TempValShow = 2.0f;
		Source->ShowGoOn = true;
	}
	else
	{
		if (fpstimers.ComputerSpeed > 0)
			Source->TempValShow -= fpstimers.FrameTime * 100;
		if ( Source->TempValShow < 0){
			Source->ShowGoOn = false;
			TextV34.Fl4 = Source->ColourTemp;
			Source->Colour = TextV34.Vec;
		}

		if ( Source->TempValShow < 1.3f && Source->ShowGoOn == true )
		{
			TextV34.Vec = Source->Colour;
			Source->ColourAction = TextV34.Fl4;
			if ( Source->ColourAction.x > 0.0f)
				Source->ColourAction.x -= fpstimers.FrameTime * 100;
			else
				Source->ColourAction.x = 0.0f;
			if ( Source->ColourAction.y > 0.0f)
				Source->ColourAction.y -= fpstimers.FrameTime * 100;
			else
				Source->ColourAction.y = 0.0f;
			if ( Source->ColourAction.z > 0.0f)
				Source->ColourAction.z -= fpstimers.FrameTime * 100;
			else
				Source->ColourAction.z = 0.0f;
			if ( Source->ColourAction.w > 0.0f)
				Source->ColourAction.w -= fpstimers.FrameTime * 100;
			else
				Source->ColourAction.w = 0.0f;

			TextV34.Fl4 = Source->ColourAction;
			Source->Colour = TextV34.Vec;
		}
		else
		{
			TextV34.Fl4 = Source->ColourTemp;
			Source->Colour = TextV34.Vec;
		}

	}
}


void TextClass::ShowScrolling(int i)
{
	SentenceType* Source = Sentences[i];

	if ( Source->ScrollGoOn == false )
	{
//		Sentences[i]->PosX = (m_screenWidth - ( Sentences[i]->MaxLength *
//			(int)m_Font[Sentences[i]->FontType]->FontSize / 2)) / 2 ;
		Source->PosY = m_screenHeight - (UINT)m_Font[Source->FontType]->FontSize;

		Source->TempValScroll = 0.0f;

		Source->ScrollGoOn = true;
	}
	else
	{
		Source->TempValScroll += fpstimers.FrameTime * 100;

		if ( Source->TempValScroll > (1.0f / Source->ScrollSpeed) )
		{
			--Source->PosY ;
			Source->TempValScroll = 0.0f;
		}
		if ( Source->PosY == 0 )
			Source->ScrollGoOn = false;

//		UpdateSentence(i, Sentences[i]->Text, Sentences[i]->PosX, Sentences[i]->PosY);
	}
}


void TextClass::DrawOneSentence( int SentenceIndex )
{
	RenderSentence( Sentences[SentenceIndex] );
}


int TextClass::GetSentenceMaxLength( int SentenceNumber )
{
	int i = (int)Sentences.size();
	if ( SentenceNumber < i )
	{
		return ( Sentences[SentenceNumber]->MaxLength );
	}
	else
		return -1;	
}


int TextClass::GetFontHeightInPixels( int FontNumber )
{
	int i = ( int ) m_Font.size();
	if ( FontNumber < i )
	{
		return ( m_Font[FontNumber]->FontHeightInPixel );
	}
	else
		return -1;
}


UINT TextClass::GetSentenceHeight( int SentenceNumber )
{
	return Sentences[SentenceNumber]->Height;
}


UINT TextClass::GetSentenceLength(int SentenceNumber)
{
	return (UINT)strlen(Sentences[SentenceNumber]->Text);
}

char * TextClass::GetSentenceText(int SentenceNumber)
{
	return Sentences[SentenceNumber]->Text;
}


int TextClass::GetPosX( int SentenceNumber )
{
	return Sentences[SentenceNumber]->PosX;
}


int TextClass::GetPosY( int SentenceNumber )
{
	return Sentences[SentenceNumber]->PosY;
}