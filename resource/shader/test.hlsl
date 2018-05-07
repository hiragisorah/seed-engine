cbuffer unique
{
    row_major matrix g_world;
};

struct VsIn
{
    float3 position_ : POSITION;
    float3 normal_ : NORMAL;
    float2 uv_ : TEXCOORD;
};

struct VsOut
{
    float4 position_ : SV_Position;
};

struct PsOut
{
    float4 color_ : SV_Target;
};

VsOut VS(VsIn input)
{
    VsOut output = (VsOut) 0;

    output.position_ = mul(float4(input.position_, 1.f), g_world);

    return output;
}

PsOut PS(VsOut input)
{
    PsOut output = (PsOut) 0;

    output.color_ = 1;

    return output;
}