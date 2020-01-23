#include "MtxMath.h"

// �s�񂩂�ړ������݂̂𒊏o-----------------------------------------------------------------------
XMMATRIX GetMtxOffset(const XMMATRIX& mtx) {
    return XMMatrixTranslation(mtx.r[3].m128_f32[0], mtx.r[3].m128_f32[1], mtx.r[3].m128_f32[2]);
}

// �s�񂩂�X�P�[�������݂̂𒊏o-------------------------------------------------------------------
XMMATRIX GetMtxScaling(const XMMATRIX& mtx) {
    return XMMatrixScaling(
        XMVector3Length(XMVECTOR{ mtx.r[0].m128_f32[0],mtx.r[0].m128_f32[1],mtx.r[0].m128_f32[2] }).m128_f32[0],
        XMVector3Length(XMVECTOR{ mtx.r[1].m128_f32[0],mtx.r[1].m128_f32[1],mtx.r[1].m128_f32[2] }).m128_f32[0],
        XMVector3Length(XMVECTOR{ mtx.r[2].m128_f32[0],mtx.r[2].m128_f32[1],mtx.r[2].m128_f32[2] }).m128_f32[0]
    );
}

// �s�񂩂��]�����݂̂𒊏o-----------------------------------------------------------------------
XMMATRIX GetMtxRotation(const XMMATRIX&  mtx) {
    XMMATRIX mOffset = GetMtxOffset(mtx);
    XMMATRIX mScaling = GetMtxScaling(mtx);

    XMVECTOR det;
    XMMATRIX scalingInv = XMMatrixInverse(&det, mScaling);
    XMMATRIX offsetInv = XMMatrixInverse(&det, mOffset);
    
    if (isnan(scalingInv.r[0].m128_f32[0]))
    {
        scalingInv = XMMatrixIdentity();
    }
    if (isnan(offsetInv.r[0].m128_f32[0]))
    {
        offsetInv = XMMatrixIdentity();
    }

    return scalingInv * mtx * offsetInv;
}

// �s��̃u�����h-----------------------------------------------------------------------------------
XMMATRIX MyDirectX::Fbx::BlendMtx(const XMMATRIX mtx1, const XMMATRIX mtx2, const float weight )
{
    XMMATRIX blend = XMMatrixIdentity();

    // �X�P�[��
    blend *= GetMtxScaling(mtx1) * (1.0f - weight) + GetMtxScaling(mtx2) * weight;

    // ��]
    XMVECTOR q1;
    XMVECTOR q2;
    XMVECTOR qOut;

    q1 = XMQuaternionRotationMatrix(GetMtxRotation(mtx1));
    q2 = XMQuaternionRotationMatrix(GetMtxRotation(mtx2));

    qOut = XMQuaternionSlerp(q1, q2, weight);
    blend *= XMMatrixRotationQuaternion(qOut);

    // �g�����X�t�H�[��
    blend *= GetMtxOffset(mtx1) * (1.0f - weight) + GetMtxOffset(mtx2) * weight;


    return blend;
}

// FbxMatrix��XMMATRIX�ɕϊ�------------------------------------------------------------------------
XMMATRIX MyDirectX::Fbx::ConvFbxMtxXmtx(FbxMatrix& pSrc) {
    XMMATRIX  mmx;

    mmx =
        XMMatrixSet(
            static_cast<FLOAT>(pSrc.Get(0, 0)), static_cast<FLOAT>(pSrc.Get(0, 1)), static_cast<FLOAT>(pSrc.Get(0, 2)), static_cast<FLOAT>(pSrc.Get(0, 3)),
            static_cast<FLOAT>(pSrc.Get(1, 0)), static_cast<FLOAT>(pSrc.Get(1, 1)), static_cast<FLOAT>(pSrc.Get(1, 2)), static_cast<FLOAT>(pSrc.Get(1, 3)),
            static_cast<FLOAT>(pSrc.Get(2, 0)), static_cast<FLOAT>(pSrc.Get(2, 1)), static_cast<FLOAT>(pSrc.Get(2, 2)), static_cast<FLOAT>(pSrc.Get(2, 3)),
            static_cast<FLOAT>(pSrc.Get(3, 0)), static_cast<FLOAT>(pSrc.Get(3, 1)), static_cast<FLOAT>(pSrc.Get(3, 2)), static_cast<FLOAT>(pSrc.Get(3, 3)));


    return mmx;
}

XMMATRIX MyDirectX::Fbx::ConvFbxMtxXmtx(FbxAMatrix& pSrc) {
	XMMATRIX  mmx;

	mmx =
		XMMatrixSet(
			static_cast<FLOAT>(pSrc.Get(0, 0)), static_cast<FLOAT>(pSrc.Get(0, 1)), static_cast<FLOAT>(pSrc.Get(0, 2)), static_cast<FLOAT>(pSrc.Get(0, 3)),
			static_cast<FLOAT>(pSrc.Get(1, 0)), static_cast<FLOAT>(pSrc.Get(1, 1)), static_cast<FLOAT>(pSrc.Get(1, 2)), static_cast<FLOAT>(pSrc.Get(1, 3)),
			static_cast<FLOAT>(pSrc.Get(2, 0)), static_cast<FLOAT>(pSrc.Get(2, 1)), static_cast<FLOAT>(pSrc.Get(2, 2)), static_cast<FLOAT>(pSrc.Get(2, 3)),
			static_cast<FLOAT>(pSrc.Get(3, 0)), static_cast<FLOAT>(pSrc.Get(3, 1)), static_cast<FLOAT>(pSrc.Get(3, 2)), static_cast<FLOAT>(pSrc.Get(3, 3)));


	return mmx;
}


//--------------------------------------------------------------------------------------------------