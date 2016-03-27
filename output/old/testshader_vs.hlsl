// built from a mistsh source
float4x4 worldviewmatrix;
struct mistsh_varyings_t
{
float4 pos: TEXCOORD0;
float4 color: TEXCOORD1;
float2 texcoord1: TEXCOORD2;
};
struct mistsh_attributes_t
{
float4 a_position: POSITION;
float4 a_texcoord1: TEXCOORD0;
float4 a_color: COLOR0;
};
float4 main(
	in mistsh_attributes_t mistsh_attributes,
	out mistsh_varyings_t mistsh_varyings )
	: POSITION
{
mistsh_varyings.pos = mistsh_attributes.a_position;
mistsh_varyings.color = mistsh_attributes.a_color;
mistsh_varyings.texcoord1 = mistsh_attributes.a_texcoord1.xy;
mistsh_varyings.texcoord1.x = 0.5;
return mul( mistsh_attributes.a_position, worldviewmatrix );
}