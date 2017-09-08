========================================================================
    WIN32 APPLICATION : RCube Project Overview
========================================================================
Lessons NEW
http://ogldev.atspace.co.uk/index.html
Compile shaders 
Games for Windows and the DirectX SDK
http://blogs.msdn.com/b/chuckw/archive/2012/05/07/hlsl-fxc-and-d3dcompile.aspx
https://msdn.microsoft.com/en-us/library/windows/desktop/bb509633%28v=vs.85%29.aspx
Integrate shaders into EXE
http://stackoverflow.com/questions/21658937/can-i-put-hlsl-into-a-lib-somehow
Fog Shader
http://ploobs.com.br/?p=1868
http://www.gamedev.net/topic/602626-hlsl-fog-shader/
Fog formulas
https://msdn.microsoft.com/en-us/library/bb173401%28v=vs.85%29.aspx
HLSL C instructions set
https://msdn.microsoft.com/en-us/library/ff471376%28v=VS.85%29.aspx
HLSL 5_0
https://msdn.microsoft.com/en-us/library/ff471358%28v=vs.85%29.aspx

// RCube models
// �������� ������� ����������� � RCube

	struct RCudeObjDesc
	{
		bool IsObjDraw; // ����� �� �������� ������
		XMFLOAT3 Position; // ������� ������� �� ������
		XMVECTOR Quat; // ���������� �������� �������
		float FrustumRadius; // ������ ������� ��� �������� �� Frustum
        INT Shaders; // ����� � ������ ������ ��������
        ID3D11Buffer *VertexBuffer; // ��������� �� ���������� ����� �������
        ID3D11Buffer *IndexBuffe;
		UINT FrustumType; // 0 - ���, 1 - ���, 2 - �����, 3 - ��� �� ������ ���

		int Instancing; // ���� ����� 0, �� ���� ������ �� Instancing  ���� > 0  ������ ������ Instancing � �������� Instancing-1 ����������

		Massive *Ptr = new InstanceType [Instancing] ��������� �� ������ ���������� ��� ������� Instanc�, ���� ������ Instancing

	}	ThisObjDesc;

����������� ����  0 - ������� World , View, Projection, Ortho, Camera Position ��� �����
����������� ����  1 - ���� LightClass
����������� ���� 13 - ���� ������ ��� ���������
����������� ���� 12 - FXAA ����������

/// Qpar-report:2 