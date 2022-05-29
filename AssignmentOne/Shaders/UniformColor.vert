#version 410 core

uniform mat4  vertexTransform;
uniform vec3  setColor;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3  vPosition;
layout(location = 1) in vec3  vColor;

out vec3 fragColor;

void main()
{
    vec4 vPos = vec4( vPosition.x, vPosition.y, vPosition.z, 1.0f );
    gl_Position = vertexTransform * vPos;
    fragColor = setColor;
}
