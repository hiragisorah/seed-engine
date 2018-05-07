cbuffer unique
{
    row_major matrix g_world;
    row_major matrix g_view;
    row_major matrix g_proj;
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

    output.color_ = 1;
    output.position_ = input.position_;
    output.normal_ = input.normal_;
    output.depth_ = input.sv_position_.z / input.sv_position_.w;

    return output;
}