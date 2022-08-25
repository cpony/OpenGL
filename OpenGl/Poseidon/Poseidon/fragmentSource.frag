#version 450 core

out vec4 FragColor;

in float Height;

void main()
{
    //float h = (Height + 16)/64.0f;	// shift and scale the height in to a grayscale value
    float h = (Height + 32)/32.0f;
    //FragColor = vec4(h, h, h, 1.0);
    FragColor = vec4(0, 0.2, h, 1.0);
    //FragColor = vec4(0.5, 0.5, 1, 1.0);
}