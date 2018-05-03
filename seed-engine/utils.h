#pragma once

#include <DirectXMath.h>

namespace Seed
{
	DirectX::XMFLOAT2 Mul(const DirectX::XMFLOAT2 & x, const DirectX::XMFLOAT2 & y) { return{ x.x * y.x, x.y * y.y }; }
	DirectX::XMFLOAT3 Mul(const DirectX::XMFLOAT3 & x, const DirectX::XMFLOAT3 & y) { return{ x.x * y.x, x.y * y.y, x.z * y.z }; }
	DirectX::XMFLOAT4 Mul(const DirectX::XMFLOAT4 & x, const DirectX::XMFLOAT4 & y) { return{ x.x * y.x, x.y * y.y, x.z * y.z, x.w * y.w }; }
}