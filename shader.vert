#version 330 

in vec3 vert;
in vec2 uv;
in vec3 vertexColor;
out vec2 varying_uv;
out vec3 varying_color;
uniform mat4 mvpMatrix;

void main()
{	
    gl_Position = mvpMatrix * vec4(vert, 1.0);
    varying_uv = uv;
    varying_color = vertexColor;
}
