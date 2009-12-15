//
// A simple shader without lighting
//

texture tex0;

// transformations
float4x4 World      : WORLD;
float4x4 View       : VIEW;
float4x4 Projection : PROJECTION;

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex  : TEXCOORD0;
};

sampler Sampler = sampler_state
{
    Texture   = (tex0);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


VS_OUTPUT VS(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL, 
    float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float4x4 WorldView = mul(World, View);

    float3 P = mul(float4(Pos, 1), (float4x3)WorldView);  // position (view space)
    Out.Pos  = mul(float4(P, 1), Projection);             // position (projected)
    Out.Tex  = Tex;                                       

    return Out;
}

float4 PS(    
    float2 Tex  : TEXCOORD0) : COLOR
{
    //return tex2D(Sampler, Tex);
    
    return float4( 1.0, 1.0, 1.0, 1.0 );
}

technique t0
{
    pass p0
    {
        // lighting
        Lighting   = FALSE;
     	CullMode   = None;

        // samplers
        Sampler[0] = (Sampler);

        // texture stages
        ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        AlphaOp[0]   = MODULATE;
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = DIFFUSE;

        ColorOp[1]   = DISABLE;
        AlphaOp[1]   = DISABLE;

        // shaders
        VertexShader = compile vs_2_0 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}

