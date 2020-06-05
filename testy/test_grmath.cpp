
#include <cstdio>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

using namespace DirectX;

using point3 = XMFLOAT3;   // 3D point
using rtcolor = XMFLOAT3;


void main()
{
    struct Ray {
        XMFLOAT3 pos;
        XMFLOAT3 dir;
    }
    
    struct ConstantBuffer {
        XMMATRIX transform;
    };

    float angle = 30;
    const ConstantBuffer cb = {
        {
            XMMatrixRotationZ(angle) *
            XMMatrixScaling(3.0/4.0, 1.0, 1.0)
        }
    };



    printf("hello\n");
    XMVECTOR  vFive = XMVectorReplicate( 5.f );
}