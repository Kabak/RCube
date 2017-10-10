#include "stdafx.h"
#include "KFModel.h"
#include <math.h>


KFModel::KFModel(){

	m_posesBuffer = nullptr;
	vertLayout = nullptr;
	ThisObjDesc.Positions = nullptr;
	ThisObjDesc.PositionsDraw = nullptr;

}

bool KFModel::LoadKFObject(std::wstring FileName){

	HRESULT hr;
	int c = 0 , h = 0;
	D3D11_BUFFER_DESC bd;
	D3D11_SUBRESOURCE_DATA InitData;
	RCube_VecFloat34 TempFloat3;
	RCube_VecFloat34 TempFloat3_2;

	std::basic_ifstream<unsigned char> file (FileName , std::ios::in | std::ios::binary);
	if (!file){
	   MessageBox( NULL,L"�� ������� ������� ���� ��� ������.", L"Error", MB_OK );
	   return false ;
	}

	file.read((unsigned char*)&ThisObjDesc.MeshesCount, sizeof(int));

	ThisObjDesc.Meshes = new MeshData[ThisObjDesc.MeshesCount]; // ������ ������ ��� ����
	MeshData * tempMesh = ThisObjDesc.Meshes;

	
	int TextureSize = NULL; // ������ ������� ������� � ���� ������
	unsigned char * TextuteFile = nullptr; // ��� ���� �������� ������� � ����� �� ����� ����
	int g = 0;

	// ���� ���������� �����
	while (c < ThisObjDesc.MeshesCount ) {

		file.read((unsigned char*)&tempMesh->VertexBufferSize, sizeof(int)); // ����� ������ ���������� ����������� ������
		ThisObjDesc.ObjectVertexesCout += tempMesh->VertexBufferSize; // ������ ����� ������ ���� ����� ��������� ���� ��������
		tempMesh->VertArr = new Vertex_Model3D[tempMesh->VertexBufferSize]; // ������� ������ ��� ��������� ����� ���

		file.read((unsigned char*)&tempMesh->VertArr[0] , sizeof( Vertex_Model3D ) * tempMesh->VertexBufferSize);//������ ����� �����


		file.read((unsigned char*)&TextureSize , sizeof(int));

		if ( TextureSize )
		{
			// ������ � �������� ��������
			TextuteFile = new unsigned char[TextureSize];

			file.read( TextuteFile, TextureSize * sizeof( unsigned char ) );

			hr = DirectX::CreateWICTextureFromMemory( d3d11Device, d3d11DevCon, &TextuteFile[0], ( size_t ) TextureSize, &tempMesh->TextureResurce
													  , &tempMesh->Texture, NULL );
			if ( hr == E_FAIL )
			{
				MessageBox( g_hwnd, L"���� ��������� � KAF ���������.ResourceManager", L"Error", MB_OK );
			}

			delete[] TextuteFile;
			// ������ � �������� ��������
		}
		 // �������� ���������� ������*****
		hr = S_OK;
		// Set up the description of the static vertex buffer.
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof( Vertex_Model3D ) * tempMesh->VertexBufferSize;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;


		// Give the subresource structure a pointer to the vertex data.
		InitData.pSysMem = tempMesh->VertArr;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = d3d11Device->CreateBuffer(&bd, &InitData, &tempMesh->VertBuff);

		// �������� ���������� ������*****



		// ����� �������� ����� (��� ����������� ����)

		file.read((unsigned char*)&TempFloat3, sizeof(XMFLOAT3)); // ����� �����������
		file.read((unsigned char*)&TempFloat3_2, sizeof(XMFLOAT3)); // ����� ������������
		
		tempMesh->MeshBox.CreateBoundStruct(TempFloat3 , TempFloat3_2);

		// ����� �������� ����� (��� ����������� ����)

		++tempMesh;
		++c;
	}

	file.read((unsigned char*)&TempFloat3, sizeof(XMFLOAT3)); // ����� �����������
	file.read((unsigned char*)&TempFloat3_2, sizeof(XMFLOAT3)); // ����� ������������

	ThisObjDesc.ObjectBox.CreateBoundStruct(TempFloat3 , TempFloat3_2);

	file.close();

	return true ;

}

HRESULT KFModel::Init(HWND hwnd, std::wstring filename, ID3D11Device *d3d11Device
	, ID3D11DeviceContext * DevCon, UINT Shaders, int m_instanceCount, D3DClass * ActiveLightClass){

	HRESULT hr = 0;
	g_ActiveLightClass = ActiveLightClass;
	g_hwnd = hwnd;
	//ID3DXEffect * 

	ThisObjDesc.InstanceCount = m_instanceCount;
	d3d11DevCon = DevCon ;
	KFModel::d3d11Device = d3d11Device ;
	
	LoadKFObject(filename) ;

		D3D11_BUFFER_DESC posesBufferDesc;
		D3D11_SUBRESOURCE_DATA posesData;
		int c = 0;

		if (m_instanceCount > 0) {
			ThisObjDesc.Positions = new PositionType[m_instanceCount];
			ThisObjDesc.PositionsDraw = new  PositionType[m_instanceCount];
            memset(ThisObjDesc.Positions, 0, (sizeof(PositionType) * m_instanceCount));
			memset(ThisObjDesc.PositionsDraw, 0, (sizeof(PositionType) * m_instanceCount));


		}
		else {
			ThisObjDesc.Positions = new PositionType;
			ThisObjDesc.PositionsDraw = new  PositionType;
			memset(ThisObjDesc.Positions, 0, sizeof(PositionType));
			memset(ThisObjDesc.PositionsDraw, 0, sizeof(PositionType));
		}



		posesBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		if(m_instanceCount > 1)
		    posesBufferDesc.ByteWidth = sizeof(PositionType) * m_instanceCount;
		else
			posesBufferDesc.ByteWidth = sizeof(PositionType);

		posesBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		posesBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		posesBufferDesc.MiscFlags = 0;
		posesBufferDesc.StructureByteStride = 0;

		
		posesData.pSysMem = ThisObjDesc.Positions;
		posesData.SysMemPitch = 0;
		posesData.SysMemSlicePitch = 0;

		
		HRESULT result = d3d11Device->CreateBuffer(&posesBufferDesc, &posesData, &m_posesBuffer);
		if (FAILED(result))
		{
			return result;
		}

	ThisObjDesc.FrustumType = 0; // ���� �������� ������ 0 - ���� ��� ���� ����� ��������
	ThisObjDesc.Shaders = Shaders;
	ThisObjDesc.InstanceFrustumPassedAmount = 0;


	return hr;

}

HRESULT KFModel::Frame(int frames ){

	/*HRESULT result ;
	D3D11_MAPPED_SUBRESOURCE mapperResurse , mapperResurse2 ;
	Vertex * vertixesPtr ;
	DWORD  * IndexesPtr ;
	int c = 0;

	result = d3d11DevCon->Map(VertBuff , 0 , D3D11_MAP_WRITE_DISCARD , 0 , &mapperResurse ) ;
	if (FAILED(result)){
	//	MessageBox(working_hwnd, L"2DRenderingClass. ������ � ��������� ����������� ������", L"Error", MB_OK);
		return E_FAIL ;
	}

	vertixesPtr = (Vertex*)mapperResurse.pData ;


	memcpy(vertixesPtr , VertArr[frames] , (sizeof(Vertex) * VertexSizes[frames]));

	d3d11DevCon->Unmap(VertBuff , 0);


	result = d3d11DevCon->Map(IndexBuff , 0 , D3D11_MAP_WRITE_DISCARD , 0 , &mapperResurse2 ) ;
	if (FAILED(result)){
	//	MessageBox(working_hwnd, L"2DRenderingClass. ������ � ��������� ����������� ������", L"Error", MB_OK);
		return E_FAIL ;
	}

	IndexesPtr = (DWORD*)mapperResurse2.pData ;


	memcpy(IndexesPtr , Indexes[frames] , (sizeof(DWORD) * IndexesSizes[frames]));

	d3d11DevCon->Unmap(IndexBuff , 0);

	UsingSubsetIndexStart = SubSetIndexStarts[frames] ;

	ObjectNum = ObjectNumArr[frames] ;*/
	return S_OK ;

}

KFModel::~KFModel(){
	 

	RCUBE_RELEASE(m_posesBuffer);
	RCUBE_RELEASE(vertLayout);



}




bool KFModel::UpdateInstancingPos() {

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	PositionType* IndexPtr;

	// Lock the vertex buffer.
	result = d3d11DevCon->Map(m_posesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	IndexPtr = (PositionType*)mappedResource.pData;

	if (ThisObjDesc.InstanceFrustumPassedAmount != -1 ) {

		Instance = ThisObjDesc.InstanceFrustumPassedAmount ;
		ThisObjDesc.InstanceFrustumPassedAmount = (-1);

	}

	int help = sizeof(PositionType);
	help = sizeof(XMFLOAT4);

	if(ThisObjDesc.InstanceCount > 1)
	    memcpy(IndexPtr, (void*)ThisObjDesc.PositionsDraw, (sizeof(PositionType) * Instance));
	else
		memcpy(IndexPtr, (void*)ThisObjDesc.PositionsDraw, sizeof(PositionType));

	d3d11DevCon->Unmap(m_posesBuffer, 0);

	return S_OK;
}


void KFModel::LightRender() {
	int c = 0;

	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];
	MeshData * tempMesh = ThisObjDesc.Meshes;
	UpdateInstancingPos();

	if (Instance != 0) {

		d3d11DevCon->IASetInputLayout(vertLayout);

		// Set vertex buffer stride and offset.
		strides[0] = sizeof( Vertex_Model3D );
		strides[1] = sizeof(PositionType);

		// Set the buffer offsets.
		offsets[0] = 0;
		offsets[1] = 0;

		bufferPointers[1] = m_posesBuffer;



		while (c < ThisObjDesc.MeshesCount ) {

			// Set the array of pointers to the vertex and instance buffers.
			bufferPointers[0] = tempMesh->VertBuff;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			d3d11DevCon->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);


			g_ActiveLightClass->RenderClustered(tempMesh->Texture, tempMesh->VertexBufferSize, Instance);

			++tempMesh;
			++c;
		}

	}
}


void KFModel::Render() {

	int c = 0;

	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];
	MeshData * tempMesh = ThisObjDesc.Meshes;
	UpdateInstancingPos();

	if (Instance != 0) {

		d3d11DevCon->IASetInputLayout(vertLayout);

		// Set vertex buffer stride and offset.
		strides[0] = sizeof( Vertex_Model3D );
		strides[1] = sizeof(PositionType);

		// Set the buffer offsets.
		offsets[0] = 0;
		offsets[1] = 0;

		bufferPointers[1] = m_posesBuffer;



		while (c < ThisObjDesc.MeshesCount ) {

			// Set the array of pointers to the vertex and instance buffers.
			bufferPointers[0] = tempMesh->VertBuff;

			// Set the vertex buffer to active in the input assembler so it can be rendered.
			d3d11DevCon->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);


			if (Instance > 1)
				d3d11DevCon->DrawInstanced(tempMesh->VertexBufferSize, Instance, 0, 0);
			else
				d3d11DevCon->Draw( tempMesh->VertexBufferSize, 0);

			++tempMesh;
			++c;
		}

	}



}



void KFModel::UpdateInstancingDrawData() {

	int c = 0 , CounterInstanceNeedDraw = 0;

	PositionType *Dest = &ThisObjDesc.PositionsDraw[0];
	PositionType *Source = &ThisObjDesc.Positions[0];

	//ThisObjDesc.IsObjDraw[1] = false;
	while (c < ThisObjDesc.InstanceCount) {

		if (ThisObjDesc.IsObjDraw) {

			memcpy(&Dest->position, &Dest->rotation, sizeof(XMVECTOR) * 2);

			++Dest;
			++CounterInstanceNeedDraw;

		}


		++Source;
		++c;
	}

	ThisObjDesc.InstanceFrustumPassedAmount = CounterInstanceNeedDraw;

}


void BoundingBox::CreateBoundStruct(RCube_VecFloat34 minValues, RCube_VecFloat34 maxValues) {

	MinValues.Fl4 = minValues.Fl4;
	MaxValues.Fl4 = maxValues.Fl4;


	CentralPoint.Vec = (MaxValues.Vec + MinValues.Vec) / 2;


	SphereRadius = sqrt(pow(MaxValues.Fl3.x - MinValues.Fl3.x, 2) + pow(MaxValues.Fl3.y - MinValues.Fl3.y, 2) + pow(MaxValues.Fl3.z - MinValues.Fl3.z, 2));
}


// BREATBUS version of import
/*

bool BTSAssets :: load3DScene( const std :: string fileName )
{
if( fileName.empty() )
return false;

Assimp :: Importer importer;
const aiScene* scene = importer.ReadFile( fileName, aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_SortByPType );
if( !scene )
return false;

// ��������� ���������
for( int i = 0; i < scene -> mNumMaterials; i++ )
{
// �������� ��������
aiMaterial*    assimpMaterial  =  scene -> mMaterials[i];
BTSLoadTexture*  texture      =  nullptr;
aiColor3D    ambient      =  aiColor3D( 0.0f, 0.0f, 0.0f );
aiColor3D    diffuse      =  aiColor3D( 0.0f, 0.0f, 0.0f );
aiColor4D    specular    =  aiColor4D( 0.0f, 0.0f, 0.0f, 0.0f );
float      shininess    =  1.0f;
std :: string  nameMaterial  =  "";

aiString name;
if( assimpMaterial -> Get( AI_MATKEY_NAME, name ) == AI_SUCCESS )
nameMaterial = name.data;

// ���� �������� ���� � ������
if( _materialManager.getMaterial( nameMaterial ) )
continue;

aiString path;
if( assimpMaterial -> GetTexture( aiTextureType_DIFFUSE, 0, &path ) == AI_SUCCESS )
{
const std :: string    pathToTexture  = path.data;
const std::size_t    found      = pathToTexture.find_last_of("/\\");
const std :: string    key        = pathToTexture.substr( found + 1 );

texture = _textureManager.getTexture<BTSLoadTexture>( key );
if( !texture )
{
texture = _textureManager.createTexture<BTSLoadTexture>( key );
texture -> loadTexture( pathToTexture );
}
}

assimpMaterial -> Get( AI_MATKEY_COLOR_AMBIENT, ambient );
assimpMaterial -> Get( AI_MATKEY_COLOR_DIFFUSE, diffuse );
assimpMaterial -> Get( AI_MATKEY_COLOR_SPECULAR, specular );
assimpMaterial -> Get( AI_MATKEY_SHININESS, shininess );

// ��������� ��������
auto material      =  _materialManager.createMaterial<BTSMaterial>( nameMaterial );
material -> ambient    =  XMFLOAT4( ambient.r, ambient.g, ambient.b, 1.0f );
material -> diffuse    =  XMFLOAT4( diffuse.r, diffuse.g, diffuse.b, 1.0f );
material -> specular  =  XMFLOAT4( specular.r, specular.g, specular.b, shininess );
material -> texture    =  texture;
}

// ��������� �����
for( int i = 0; i < scene -> mNumMeshes; i++ )
{
// �������� �����
aiMesh*          assimpMesh    =  scene -> mMeshes[i];
unsigned int      numVertices    =  assimpMesh -> mNumVertices;
unsigned int      numIndices    =  assimpMesh -> mNumFaces * 3;
unsigned int      numFaces    =  assimpMesh -> mNumFaces;
XMMATRIX        worldMatrix    =  XMMatrixTranspose( (float*)&scene -> mRootNode -> mChildren[i] -> mTransformation );
XMFLOAT3        position    =  XMFLOAT3( worldMatrix._41, worldMatrix._42, worldMatrix._43 );
std :: string      nameMesh    =  scene -> mRootNode -> mChildren[i] -> mName.data;
std :: vector<Vertex>  vertices;
std :: vector<WORD>    indices;

// ���� ����� ���� � ������
if( _meshManager.getMesh( nameMesh ) )
continue;

// �������� ������� �����
for( int j = 0; j < assimpMesh -> mNumVertices; j++ )
{
Vertex vertex;

if( assimpMesh -> HasPositions() )
{
vertex.pos.x  = assimpMesh -> mVertices[j].x;
vertex.pos.y  = assimpMesh -> mVertices[j].y;
vertex.pos.z  = assimpMesh -> mVertices[j].z;
}

if( assimpMesh -> HasNormals() )
{
vertex.normal.x = assimpMesh -> mNormals[j].x;
vertex.normal.y = assimpMesh -> mNormals[j].y;
vertex.normal.z = assimpMesh -> mNormals[j].z;
}

if( assimpMesh -> HasTextureCoords( 0 ) )
{
vertex.tex.x  = assimpMesh -> mTextureCoords[0][j].x;
vertex.tex.y  = assimpMesh -> mTextureCoords[0][j].y;
}

vertices.push_back( vertex );
}

// �������� ������� �����
for( int j = 0; j < assimpMesh -> mNumFaces; j++ )
{
aiFace face = assimpMesh -> mFaces[j];
for( int k = 0; k < 3; k++ )
indices.push_back( face.mIndices[k] );
}

//  ��������� �����
auto mesh          =  _meshManager.createMesh<BTSMesh>( nameMesh );
mesh -> vertices      =  std :: move( vertices );
mesh -> indices        =  std :: move( indices );
mesh -> numFaces      =  numFaces;
mesh -> numVertices      =  numVertices;
mesh -> numIndices      =  numIndices;
mesh -> transformMatrix    =  worldMatrix;
}

return true;
}
*/