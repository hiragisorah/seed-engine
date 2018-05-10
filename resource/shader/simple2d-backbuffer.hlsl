Texture2D tex : register(t0);

SamplerState own_sampler : register(s0);

cbuffer unique : register(b0)
{
    row_major matrix g_world : packoffset(c0);
};

struct VsIn
{
    float3 position_ : POSITION;
    float3 normal_ : NORMAL;
    float2 uv_ : TEXCOORD;
};

struct VsOut
{
    float4 sv_position_ : SV_Position;
    float2 uv_ : TEXCOORD;
};

struct PsOut
{
    float4 color_ : SV_Target0;
};

VsOut VS(VsIn input)
{
    VsOut output = (VsOut) 0;

    output.sv_position_ = mul(float4(input.position_, 1.f),  g_world);
    output.uv_ = input.uv_;

    return output;
}

PsOut PS(VsOut input)
{
    PsOut output = (PsOut) 0;

    output.color_ = tex.Sample(own_sampler, input.uv_);

    return output;
}