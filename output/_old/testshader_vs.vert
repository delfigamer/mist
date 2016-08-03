#version 100
// built from a mistsh source
varying vec4 color1;
varying vec4 color2;
const vec4 foo = vec4(0, 1, 2, 3);
uniform mat4 worldview;
attribute vec4 position;
attribute vec4 color;
void main()
{



color1 = position;
color2 = color;
gl_Position = mul( position, worldview );
}