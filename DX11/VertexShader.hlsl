struct VS_INPUT
{
    float4 pos : POSITION;
    float4 color: COLOR;
    float2 inTexCoord : TEXCOORD;
};

cbuffer CBuf
{
    matrix transform;
};

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
    float2 outTexCoord : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(float4(input.pos), transform);
    output.color = input.color;
    output.outTexCoord = input.inTexCoord;
    return output;
}