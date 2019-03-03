#version 330 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float lightFar;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);

    // map to [0;1] range by dividing by lightFar
    lightDistance = lightDistance / lightFar;

    // Write this as modified depth
    gl_FragDepth = lightDistance;
}