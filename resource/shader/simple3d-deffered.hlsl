Texture2D tex : register(t0);

SamplerState own_sampler : register(s0);

cbuffer unique : register(b0)
{
    row_major matrix g_world;
};

cbuffer unique : register(b1)
{
    row_major matrix g_view;
    row_major matrix g_proj;
};

cbuffer camera : register(b1)
{
    row_major matrix g_v : packoffset(c0);
    row_major matrix g_p : packoffset(c4);

    float4 g_eye : packoffset(c8);
    float4 g_at : packoffset(c9);

    float2 g_view_port : packoffset(c10);

    float4 g_color : packoffset(c11);
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
    float4 position_ : TEXCOORD0;
    float4 normal_ : TEXCOORD1;
    float2 uv_ : TEXCOORD2;
};

struct PsOut
{
    float4 color_ : SV_Target0;
    float4 position_ : SV_Target1;
    float4 normal_ : SV_Target2;
    float4 depth_ : SV_Target3;
};

VsOut VS(VsIn input)
{
    VsOut output = (VsOut) 0;

    matrix wvp = mul(g_world, mul(g_view, g_proj));

    output.sv_position_ = mul(float4(input.position_, 1.f), wvp);
    output.position_ = mul(float4(input.position_, 1.f), g_world);
    output.normal_.xyz = normalize(mul(input.normal_, (float3x3) g_world));
    output.uv_ = input.uv_;

    return output;
}

PsOut PS(VsOut input)
{
    PsOut output = (PsOut) 0;

    output.color_ = tex.Sample(own_sampler, input.uv_);
    output.color_.a = 1.f;
    output.position_ = input.position_;
    output.position_.a = 1.f;
    output.normal_ = input.normal_;
    output.normal_.a = 1.f;
    output.depth_.x = input.sv_position_.z / input.sv_position_.w;
    output.depth_.a = 1.f;

    return output;
}