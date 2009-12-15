// default.fx

shared float4x4 World;
shared float4x4 View;
shared float4x4 Projection;

texture tex0;

sampler DiffSampler = sampler_state
{
    Texture     = <tex0>;
    MinFilter   = LINEAR;
    MagFilter   = LINEAR;
    MipFilter   = LINEAR;
};

technique t0
{
    pass p0
    {
      	WorldTransform[0]   = <World>;
        ViewTransform       = <View>;
        ProjectionTransform = <Projection>;

		Sampler[0] = <DiffSampler>;

		Lighting   = False;
		CullMode   = None;
		ZEnable    = True;
	
		VertexShader    = NULL;
        PixelShader     = NULL;
    }
}
