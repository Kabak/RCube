#include "stdafx.h"
#include "KFMap.h"

D3D11_INPUT_ELEMENT_DESC Maplayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "ObjPOSITION",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "ObjROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
};


KFMap::KFMap() {
	IsClastersDarw = false;
	IsItNeedToUpdate = false;

}

KFMap::~KFMap() {

	delete [] g_Objcts;

}


UINT Map_numElements = ARRAYSIZE(Maplayout);

HRESULT KFMap::Init(HWND hwnd, std::wstring filename, ID3D11Device *d3d11Device, IDXGISwapChain* SwapChain
	, ID3D11DeviceContext * DevCon, ID3D10Blob * Blob, UINT Shaders, XMFLOAT3 & CameraPos, FrustumClass * ActiveFrustumSystem) {

	HRESULT hr = 0;
	g_hwnd = hwnd;
	//ID3DXEffect * 

	ThisObjDesc.instanceCount = 0;
	Subsets = 0;
	d3d11DevCon = DevCon;
	KFModel::d3d11Device = d3d11Device;
	KFModel::SwapChain = SwapChain;
	g_Frustum = ActiveFrustumSystem;
	meshSubsets = 1;

	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;
	cmdesc.FrontCounterClockwise = true;
//	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = d3d11Device->CreateRasterizerState(&cmdesc, &RSCullNone);

	// Describe the Sample State
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create the Sample State
	d3d11Device->CreateSamplerState(&sampDesc, &CubesTexSamplerState);

	//Create the Input Layout
	if (ThisObjDesc.instanceCount == 0)
		hr = d3d11Device->CreateInputLayout(Maplayout, Map_numElements, Blob->GetBufferPointer(),
			Blob->GetBufferSize(), &vertLayout);

	if (FAILED(hr))
		return E_FAIL;


	ObjectNum = 0;

	LoadKFObject(filename);



		ThisObjDesc.Positions = new PositionType;
		ThisObjDesc.PositionsDraw = new  PositionType;
		memset(ThisObjDesc.Positions, 0, sizeof(PositionType));
		memset(ThisObjDesc.PositionsDraw, 0, sizeof(PositionType));

	/*		UINT c = 0;
	while (c < m_instanceCount)
	{
	ThisObjDesc.instances[c].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	ThisObjDesc.instances[c].rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	instances[c].IsDraw = XMFLOAT3(1.0f, 0.0f, 0.0f);

	c++;
	}
	*/

	// Give the subresource structure a pointer to the instance data.

	// Create the instance buffer.

	ThisObjDesc.IsObjDraw = new bool;

	ThisObjDesc.FrustumRadius = 3.0f; // пока загоняем радиус 1.0f  - размер фигуры в нашем мире
	ThisObjDesc.FrustumType = 0; // пока загоняем модель 0 - ноль для всех типов объектов
	*ThisObjDesc.IsObjDraw = true;
	ThisObjDesc.IndexBuffe = IndexBuff;
	ThisObjDesc.VertexBuffer = VertBuff;
	ThisObjDesc.Shaders = Shaders;

	//	ThisObjDesc.LightBuffer.diffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	ThisObjDesc.LightBuffer.lightDirection = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	//	ThisObjDesc.LightBuffer.padding = 0.0f;
	//	ThisObjDesc.LightBuffer.ambientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	//	ThisObjDesc.LightBuffer.specularColor = XMFLOAT4(1.0f ,1.0f, 1.0f, 1.0f);
	//	ThisObjDesc.LightBuffer.specularPower = 32.0f;

	CamepaPosition = CameraPos;

	int c = 0, f = 0 , Size = (int)Clusters.size(), Size2 = (int)Objects.size();
	bool X_Line, Y_Line, Z_Line;
	ObjectsClusterIndexes.resize(Size2);
	while (c < Size) {
		while (f < Size2) {
			X_Line = ((Objects[f].MIN_X > Clusters[c].MAX_X) || (Objects[f].MAX_X  < Clusters[c].MIN_X) ? false : true);

			Y_Line = ((Objects[f].MIN_Y > Clusters[c].MAX_Y) || (Objects[f].MAX_Y  < Clusters[c].MIN_Y) ? false : true);

			Z_Line = ((Objects[f].MIN_Z > Clusters[c].MAX_Z) || (Objects[f].MAX_Z  < Clusters[c].MIN_Z) ? true : false);


			if (X_Line && Y_Line && Z_Line) 
				ObjectsClusterIndexes[f].push_back(c);



			++f;
		}
		f = 0;
		++c;
	}


	return hr;

}


void KFMap::Render() {

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	int DrawClustCout = 0;

	//Set Vertex and Pixel Shaders
	//	d3d11DevCon->VSSetShader(VS, 0, 0);
	//	d3d11DevCon->PSSetShader(PS, 0, 0);

	int TextureIndex = 0;

	d3d11DevCon->IASetInputLayout(vertLayout);
	int t = 0, ObjCout = 0 , l = 0 , CoutClustersISee = (int)ClusterISee.size();
	bool IsDraw;
	int Draw = 0;;

	for (int i = 0; i < ObjectNum; ++i)
	{
		IsDraw = false;
		if (!ClusterOrObj[i]) {
			int Size = (int)ObjectsClusterIndexes[ObjCout].size ();
			while ( t < Size )
			{

				l = 0;

				while (l < CoutClustersISee) {

					if (ObjectsClusterIndexes[ObjCout][t] == ClusterISee[l]) {

						IsDraw = true;
						break;

					}


					++l;
				}

				++t;

			}
			++ObjCout;
		}
		else
			if (IsClastersDarw ) {

					IsDraw = true;


			}
		
		t = 0;

		if (!IsDraw)
			continue;

		TextureIndex = (i < g_NumOfTextures) ? i : TextureIndex;

		//Set the grounds index buffer
		d3d11DevCon->IASetIndexBuffer(IndexBuff, DXGI_FORMAT_R32_UINT, 0);

		if(AllMaterials.size() > i)
		d3d11DevCon->PSSetShaderResources(0, 1, &AllMaterials[i].Texture);
		else
			d3d11DevCon->PSSetShaderResources(0, 1, &AllMaterials[AllMaterials.size() - 1].Texture);

		d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

//		d3d11DevCon->RSSetState(RSCullNone);
		int indexStart = UsingSubsetIndexStart[i];
		int indexDrawAmount = UsingSubsetIndexStart[i + 1] - UsingSubsetIndexStart[i];

		unsigned int strides;
		unsigned int offsets;

		// Set vertex buffer stride and offset.
		strides = sizeof(Vertex);

		// Set the buffer offsets.
		offsets = 0;


		    d3d11DevCon->IASetVertexBuffers(0, 1, &VertBuff, &strides, &offsets);
			d3d11DevCon->DrawIndexed(indexDrawAmount, indexStart, 0);
			++Draw;
	}

	int v = Draw;

}


void KFMap::Frame(bool IsItAnyCameraReplases) {

	std::vector<bool> IsThisObjReComute;

	RCudeObjDesc ** DescPoinder = &g_Objcts[0];
	RCudeObjDesc * Temp;
//	if (IsItAnyCameraReplases) {

		int Size = (int)Clusters.size(), CoutClustersISee ;
		int c = 0, f;
		ClusterISee.clear();
		while (c < Size) {

			if (g_Frustum->CheckCube(CenterPointObjsArray[c] , RadiusObjsArray[c] )) {
				ClusterISee.push_back(c);
			}

			++c;
		}
		
		CoutClustersISee = (int)ClusterISee.size();


			IsItNeedToUpdate = true;
			c = 0;

			while (c < CoutClustersISee) {

				Size = (int)ClusterHaveObj[ClusterISee[c]].size();
				f = 0;
				if (Size > 0){
					DescPoinder = &g_Objcts[ClusterHaveObj[ClusterISee[c]][f].ObjIndex];

				while (f < Size) {

					Temp = *DescPoinder;

					KFModel::PositionType * Temp1 = &Temp->Positions[ClusterHaveObj[ClusterISee[c]][f].InstIndex];
					//IsObjDraw = g_Frustum->CheckSphere(g_Objcts[ClusterHaveObj[ClusterISee[c]][f].ObjIndex]->Positions[ClusterHaveObj[ClusterISee[c]][f].InstIndex].position, g_Objcts[ClusterHaveObj[ClusterISee[c]][f].ObjIndex]->FrustumRadius);
					if (g_Frustum->CheckSphere( Temp1->position, Temp->FrustumRadius) || IsItNeedToRefreshBuffer[ClusterHaveObj[ClusterISee[c]][f].ObjIndex]) {

						
						++Temp->InstanceFrustumPassedAmount;
						if (Temp->InstanceFrustumPassedAmount < Temp->instanceCount) {
							Temp->PositionsDraw[Temp->InstanceFrustumPassedAmount] = *Temp1;
							IsItNeedToRefreshBuffer[ClusterHaveObj[ClusterISee[c]][f].ObjIndex] = false;
						}

					}

					++f;
					if(f < Size)
					DescPoinder += ClusterHaveObj[ClusterISee[c]][f].ObjIndex - ClusterHaveObj[ClusterISee[c]][f - 1].ObjIndex;

				}
			}
				++c;
			}		
		
//	}

	UsingSubsetIndexStart = SubSetIndexStarts[0];

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mapperResurse, mapperResurse2;
	Vertex * vertixesPtr;
	DWORD  * IndexesPtr;

	result = d3d11DevCon->Map(VertBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapperResurse);

	vertixesPtr = (Vertex*)mapperResurse.pData;


	memcpy(vertixesPtr, VertArr[0], (sizeof(Vertex) * VertexSizes[0]));

	d3d11DevCon->Unmap(VertBuff, 0);


	result = d3d11DevCon->Map(IndexBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapperResurse2);


	IndexesPtr = (DWORD*)mapperResurse2.pData;


	memcpy(IndexesPtr, Indexes[0], (sizeof(DWORD) * IndexesSizes[0]));

	d3d11DevCon->Unmap(IndexBuff, 0);

	UsingSubsetIndexStart = SubSetIndexStarts[0];

	ObjectNum = ObjectNumArr[0];

}

void KFMap::addExternObject(RCudeObjDesc * ObjCube) {
	std::vector<std::vector<int>> Temp;
	std::vector<int> Temp2;
	std::vector<XMFLOAT3> Temp3;
	static int ObjTypesSize = -1;
	++ObjTypesSize;

	ExternObjectsClusterIndex.push_back(Temp);
	NeeedFruatrumData.push_back(Temp2);
	UpdateData.push_back(false);
	g_Objcts[ObjTypesSize] = ObjCube;
	IsItNeedToRefreshBuffer.push_back(true);

	int c = 0, Size = (int)Clusters.size(), InstanceClusterSize = ObjCube->instanceCount, f = 0;
	bool X_Line, Y_Line, Z_Line;


	if (InstanceClusterSize < 1)
		InstanceClusterSize = 1;

	ExternObjectsClusterIndex[ExternObjectsClusterIndex.size() - 1].resize(InstanceClusterSize);

	while (c < Size) {
		f = 0;
		while (f < InstanceClusterSize) {
			X_Line = ((ObjCube->MyObj.MIN_X + ObjCube->Positions[f].position.x > Clusters[c].MAX_X) || (ObjCube->MyObj.MAX_X + ObjCube->Positions[f].position.x < Clusters[c].MIN_X) ? false : true);

			Y_Line = ((ObjCube->MyObj.MIN_Y + ObjCube->Positions[f].position.y > Clusters[c].MAX_Y) || (ObjCube->MyObj.MAX_Y + ObjCube->Positions[f].position.y < Clusters[c].MIN_Y) ? false : true);

			Z_Line = ((ObjCube->MyObj.MIN_Z + ObjCube->Positions[f].position.z > Clusters[c].MAX_Z) || (ObjCube->MyObj.MAX_Z + ObjCube->Positions[f].position.z < Clusters[c].MIN_Z) ? false : true);

			if (X_Line && Y_Line && Z_Line) {

				ExternObjectsClusterIndex[ExternObjectsClusterIndex.size() - 1][f].push_back(c);
				ObjectDescStruct MyStruct;
				MyStruct.ObjIndex = ObjTypesSize;
				MyStruct.InstIndex = f;

				ClusterHaveObj[c].push_back(MyStruct);
				
			}

			++f;
		}
		++c;
	}

}


void KFMap::SetAutoGenerateClustGrid(AutoGridParam GridParam) {

	Clusters.clear();
	CenterPointObjsArray.clear();
	RadiusObjsArray.clear();
	int c = 0, f = 0 , e = 0;
	ClusterWidth = (GridParam.MaxX - GridParam.MinX) / GridParam.NumXClusters;
		ClusterHeight = (GridParam.MaxY - GridParam.MinY) / GridParam.NumYClusters;
	ClusterLength = (GridParam.MaxZ - GridParam.MinZ) / GridParam.NumZClusters;

	ClusterHelpPosesData.resize(GridParam.NumYClusters);
	ClusterHaveObj.resize(GridParam.NumXClusters * GridParam.NumYClusters *GridParam.NumZClusters);

	while (c < GridParam.NumYClusters) {
		f = 0;

		ClusterHelpPosesData[c].resize(GridParam.NumXClusters);
		while (f < GridParam.NumXClusters) {
			e = 0;

			ClusterHelpPosesData[c][f].resize(GridParam.NumZClusters);
			while(e < GridParam.NumZClusters){
				ObjCube TempCube;

				TempCube.MIN_Y = GridParam.MinY + (c * ClusterHeight);
				TempCube.MAX_Y = GridParam.MinY + (c * ClusterHeight) + ClusterHeight;
				TempCube.MIN_X = GridParam.MinX + (f * ClusterWidth);
				TempCube.MAX_X = GridParam.MinX + (f * ClusterWidth) + ClusterWidth;
				TempCube.MIN_Z = GridParam.MinZ + (e * ClusterLength);
				TempCube.MAX_Z = GridParam.MinZ + (e * ClusterLength) + ClusterLength;

				Clusters.push_back(TempCube);

				CenterPointObjsArray.push_back(XMFLOAT3((TempCube.MAX_X + TempCube.MIN_X) / 2, (TempCube.MAX_Y + TempCube.MIN_Y) / 2
					, (TempCube.MAX_Z + TempCube.MIN_Z) / 2));
				RadiusObjsArray.push_back(TempCube.MAX_X - TempCube.MIN_X);
				ClusterHelpPosesData[c][f][e] = (int)Clusters.size() - 1;


				KFInt3 Temp;
				Temp.x = c;
				Temp.y = f;
				Temp.z = e;

				ClusterPos.push_back(Temp);

				++e;
			}


			++f;
		}


		++c;
	}



}

int KFMap::FindClearSlot(std::vector<ObjectDescStruct> ClusterData) {
	int c = 0 , Size = (int)ClusterData.size();

	while (c < Size) {

		if (ClusterData[c].ObjIndex == -1)
			return c;

		++c;
	}

	c = -1;

	return c;
}

void KFMap::RememberPoses(std::vector<KFModel*> & ClustersUsingObj) {
	int size = (int)ClustersUsingObj.size() , c = 0;
	g_Objcts =  new RCudeObjDesc*[size];
	while (c < size) {
	
		addExternObject(&ClustersUsingObj[c]->ThisObjDesc);

		++c;
	}

}