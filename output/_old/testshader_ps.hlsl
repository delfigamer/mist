// built from a mistsh source
struct mistsh_varyings_t
{
	float4 pos: TEXCOORD0;
	float4 color: TEXCOORD1;
	float2 texcoord1: TEXCOORD2;
};
sampler2D tex;
float4 main(
	in mistsh_varyings_t mistsh_varyings )
	: COLOR0
{
	return tex2D( tex, mistsh_varyings.texcoord1 );
}
