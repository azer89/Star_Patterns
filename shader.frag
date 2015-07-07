#version 330

in vec2 varying_uv;
in vec3 varying_color;

uniform float use_color;
uniform sampler2D base_texture;

out vec4 finalColor;

void main()
{
    finalColor = texture2D(base_texture, varying_uv);
    //finalColor.a = 0.5;
    if(use_color > 0.5)
    {
     	finalColor = vec4(varying_color, 1.0);
    }
}
