========================================================================
    WIN32 APPLICATION : RCube Project Overview
========================================================================
VS Find & Replace didnt work
https://social.msdn.microsoft.com/Forums/vstudio/en-US/40c9ad4a-8cee-4f3f-b420-c7cc0463942a/visual-studio-2012-find-in-files-not-displaying-files?forum=vsx

DirectX  WHAT is ?
https://msdn.microsoft.com/en-us/library/ff604998.aspx
DIRECTX
https://msdn.microsoft.com/en-us/library/windows/desktop/hh309466(v=vs.85).aspx
Dx11 Core Interfaces
https://msdn.microsoft.com/en-us/library/ff476154(v=vs.85).aspx
Dx12 Core Interfaces
https://msdn.microsoft.com/en-us/library/dn934203(v=vs.85).aspx

�������� � �������������������� � �������� Texture2DMS
http://www.gamedev.ru/code/forum/?id=211108
http://www.gamedev.net/topic/660879-using-texture2dms-in-shader/
http://www.gamedev.net/topic/636068-msaa-issues/
http://www.gamedev.net/topic/571238-directx-10-view-dimensions-problem/

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

// Updatesubresources vs Map
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb205132(v=vs.85).aspx?ranMID=24542&ranEAID=TnL5HPStwNw&ranSiteID=TnL5HPStwNw-XB.gglp_gdtjhkUUWs5azg&tduid=(7912aabb7cab1eed0b147fdd3e7d6c86)(256380)(2459594)(TnL5HPStwNw-XB.gglp_gdtjhkUUWs5azg)()
// http://xboxforums.create.msdn.com/forums/t/47875.aspx

// Octree
// http://www.uraldev.ru/articles/6/page/1
// BVH
// https://habrahabr.ru/post/257339/
// http://ray-tracing.ru/articles184.html
// BVH ������� ��� MIT
// https://github.com/brandonpelfrey/Fast-BVH

// http://www.cs.uu.nl/docs/vakken/magr/2015-2016/slides/lecture%2003%20-%20the%20perfect%20BVH.pdf
// https://www.scss.tcd.ie/Michael.Manzke/CS7057/cs7057-1516-06-BVH-mm.pdf
// http://fileadmin.cs.lth.se/cs/Education/EDAN30/lectures/S2-bvh.pdf

// ���������������
// http://www.gamedev.ru/code/articles/?id=4244
// ����� ����� �� ���� �������
// http://www.gamedev.ru/flame/forum/?id=216202

// ���������� ������� Bi-Linear  Tri-Linear � ������
// https://msdn.microsoft.com/en-us/library/dn642451.aspx
// ����������� �������� ��  register HLSL
// // https://msdn.microsoft.com/en-us/library/windows/desktop/dn899207(v=vs.85).aspx

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

Constant Buffers slots
0 - ������� World , View, Projection, Ortho, Camera Position ��� �����
1 - ���� D3DClass Light PerFrameConstants
13 - ���� ������ ��� ���������
12 - FXAA ����������

Samplers Slots
0 - 3D Model Samspling
1 - Shadow Map Sampling
2 - Shadow Map PCF Filter Sampling
3 - Flat Object Sampling ( Text, Menu, 2DAnimation, Particles, CubMap )

/// Qpar-report:2 