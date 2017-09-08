#include "stdafx.h"
#include "KFClusterMap.h"

KFClusterMap::KFClusterMap(float Width, float Height, float Depth, float ClusterWidth, float ClusterHeight, float ClusterDepth)
{

	int Clusters_X_Cout = (int)(Width / ClusterWidth), Clusters_Y_Cout = (int)(Height / ClusterHeight)
		, Clusters_Z_Cout = (int)(Depth / ClusterDepth);

	int c = 0, b, a;// счетчики цикла

	XMFLOAT3 NowPos;// пермрнная для грамотного последовательного заполнени массива

	NowPos.y = ClusterHeight / 2;

	//цикл заполняет модель по сторокам 
	// тоесть сначала по Х слой потом перходим на Z и после них Y 
	while (c < Clusters_Y_Cout) {

		b = 0;
		NowPos.z = ClusterDepth / 2;

		while (b < Clusters_Z_Cout) {

			a = 0;
			NowPos.x = ClusterWidth / 2;

			while (a < Clusters_X_Cout) {

				ClustersPoses.push_back(NowPos);

				NowPos.x += ClusterWidth;
				a++;
			}
			b++;
			NowPos.z += ClusterDepth;
		}
		c++;
		NowPos.y += ClusterHeight;
	}


}

KFClusterMap::~KFClusterMap()
{

	ClustersPoses.clear();

}