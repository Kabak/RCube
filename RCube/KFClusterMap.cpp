#include "stdafx.h"
#include "KFClusterMap.h"

KFClusterMap::KFClusterMap(float Width, float Height, float Depth, float ClusterWidth, float ClusterHeight, float ClusterDepth)
{

	int Clusters_X_Cout = (int)(Width / ClusterWidth), Clusters_Y_Cout = (int)(Height / ClusterHeight)
		, Clusters_Z_Cout = (int)(Depth / ClusterDepth);

	int c = 0, b, a;// �������� �����

	XMFLOAT3 NowPos;// ��������� ��� ���������� ����������������� ��������� �������

	NowPos.y = ClusterHeight / 2;

	//���� ��������� ������ �� �������� 
	// ������ ������� �� � ���� ����� �������� �� Z � ����� ��� Y 
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