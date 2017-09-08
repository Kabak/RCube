#include "stdafx.h"
///////////////////////////////////////////////////////////////////////////////
// Filename: modellistclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "modellistclass.h"


ModelListClass::ModelListClass()
{
//	m_ModelInfoList = 0;
	// Create a list array of the model information.
//	m_ModelInfoList = new ModelInfoType[25];
}


ModelListClass::ModelListClass(const ModelListClass& other)
{
}


ModelListClass::~ModelListClass()
{
		// Release the model information list.
//		delete [] m_ModelInfoList;
//		m_ModelInfoList = 0;
}


bool ModelListClass::Initialize(HWND hwnd, int numModels)
{
	int i;
	float red, green, blue;


	// Store the number of models.
	m_modelCount = numModels;

	// Create a list array of the model information.
//	m_ModelInfoList = new ModelInfoType[m_modelCount];
	if ( !m_ModelInfoList )
	{
		MessageBox(hwnd, L"Could not allocate memory for m_ModelInfoList.", L"Error", MB_OK);
		return false;
	}
	// Seed the random generator with the current time.
	srand((unsigned int)time(NULL));

	// Go through all the models and randomly generate the model color and position.
	for(i=0; i<m_modelCount; i++)
	{
		// Generate a random color for the model.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		m_ModelInfoList[i].color = XMVectorSet(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		m_ModelInfoList[i].position.x = (((float)rand()-(float)rand())/RAND_MAX) * 40.0f;
		m_ModelInfoList[i].position.y = (((float)rand()-(float)rand())/RAND_MAX) * 20.0f;
		m_ModelInfoList[i].position.z = ((((float)rand()-(float)rand())/RAND_MAX) * 40.0f) + 5.0f;
	}

	return true;
}


void ModelListClass::Shutdown()
{
	// Release the model information list.
/*	if(m_ModelInfoList)
	{
		delete [] m_ModelInfoList;
		m_ModelInfoList = 0;
	}
	*/
	return;
}


int ModelListClass::GetModelCount()
{
	return m_modelCount;
}


void ModelListClass::GetData(int index, float& positionX, float& positionY, float& positionZ, XMVECTOR& color)
{
	positionX = m_ModelInfoList[index].position.x;
	positionY = m_ModelInfoList[index].position.y;
	positionZ = m_ModelInfoList[index].position.z;

	color = m_ModelInfoList[index].color;

	return;
}