#version 100
// built from a mistsh source
varying vec4 color1;
varying vec4 color2;
const vec4 foo = vec4(0, 1, 2, 3);
uniform vec4 color3;
void main()
{

gl_FragColor = ( ( color1 * color2 ) * color3 );
}