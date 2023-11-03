struct VS_INPUT
{
    float4 pos : POSITION;
    float4 color: COLOR;
};

cbuffer CBuf
{
    matrix transform;
};

struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.pos = mul(float4(input.pos), transform);
    output.color = input.color;
    return output;
}