cbuffer cbSettings
{

	static const float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
	};

	/*
	static const float gWeights[11] =
	{
	0.2f, 0.2f, 0.1f, 0.01f, 0.01f, 0.01f, 0.01f, 0.01f, 0.1f, 0.2f, 0.2f,
	};
	*/
};
cbuffer cbFixed
{
	static const int gBlurRadius = 5;
};

Texture2D gInput  : register(t0);
RWTexture2D<unorm float4> gOutput  : register(u0);

#define N 256
#define CacheSize (N + 2*gBlurRadius)
groupshared float4 gCache[CacheSize];


[numthreads(1, N, 1)]
void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
	//	gOutput[dispatchThreadID.xy] = gInput[dispatchThreadID.xy].w * 0.1f;

	//
	// Fill local thread storage to reduce bandwidth. To blur
	// N pixels, we will need to load N + 2*BlurRadius pixels
	// due to the blur radius.
	//
	// This thread group runs N threads. To get the extra 2*BlurRadius
	// pixels, have 2*BlurRadius threads sample an extra pixel.
	if (groupThreadID.y < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if (groupThreadID.y >= N - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = min(dispatchThreadID.y + gBlurRadius, gInput.Length.y - 1);
		gCache[groupThreadID.y + 2 * gBlurRadius] =
			gInput[int2(dispatchThreadID.x, y)];
	}
	// Clamp out of bound samples that occur at image borders.
	gCache[groupThreadID.y + gBlurRadius] =
		gInput[min(dispatchThreadID.xy, gInput.Length.xy - 1)];
	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();
	//
	// Now blur each pixel.
	//
	float4 blurColor = float4(0, 0, 0, 0);
	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;
		blurColor += gWeights[i + gBlurRadius] * gCache[k];
	}
	gOutput[dispatchThreadID.xy] = blurColor;
	//*/
}
