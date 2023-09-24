#version 330

in vec2 fragTexCoord;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 texel = texture(texture0, fragTexCoord);
    float gray = dot(texel.rgb, vec3(0.299, 0.587, 0.114)); // B/W using NTCS weights
    finalColor = vec4(vec3(gray), 0.5);
}
