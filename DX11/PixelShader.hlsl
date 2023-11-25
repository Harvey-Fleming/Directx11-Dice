struct VS_OUTPUT
{
    float4 pos: SV_POSITION;
    float4 color: COLOR;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(VS_OUTPUT input) : SV_TARGET
{
    // return interpolated color
    float3 pixelColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    return float4(pixelColor, 1.0f);
}