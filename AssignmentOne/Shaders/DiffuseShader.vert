
#version 410 core

uniform mat4 modelTransform;
uniform mat4 vertexTransform;

layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 diffuseColor;

out VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;
    vec3 fragPos;

} vs_out;

void main()
{
    vec4 vPos = vec4( modelPosition.x, modelPosition.y, modelPosition.z, 1.0f );
    gl_Position = vertexTransform * vPos;

    vs_out.fragPos = vec3(modelTransform * vPos);
    vs_out.fragDiffuse = diffuseColor;
    vs_out.fragNormal = vertexNormal;
}
