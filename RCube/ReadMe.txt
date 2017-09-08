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
// Описание моделей загружаемых в RCube

	struct RCudeObjDesc
	{
		bool IsObjDraw; // Нужно ли рисовать объект
		XMFLOAT3 Position; // Позиция объекта на экране
		XMVECTOR Quat; // Кватернион поворота объекта
		float FrustumRadius; // размер объекта для проверки во Frustum
        INT Shaders; // номер в спаске связок шейдеров
        ID3D11Buffer *VertexBuffer; // указатель на вертексный буфер объекта
        ID3D11Buffer *IndexBuffe;
		UINT FrustumType; // 0 - шар, 1 - куб, 2 - точка, 3 - ещё не решили что

		int Instancing; // Если равен 0, то этот объект не Instancing  Если > 0  значит объект Instancing и содержит Instancing-1 повторений

		Massive *Ptr = new InstanceType [Instancing] Указатель на массив переменных для каждого Instancе, если объект Instancing

	}	ThisObjDesc;

Константный слот  0 - матрицы World , View, Projection, Ortho, Camera Position для кадра
Константный слот  1 - свет LightClass
Константный слот 13 - цвет шрифта для рисования
Константный слот 12 - FXAA фильтрация

/// Qpar-report:2 