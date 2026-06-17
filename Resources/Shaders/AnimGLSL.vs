#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aJoints;
layout(location = 2) in vec4 aWeights;
layout(location = 3) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 bones[64];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //mat4 skinMatrix = aWeights.x * bones[int(aJoints.x)] + aWeights.y * bones[int(aJoints.y)];
    mat4 skinMatrix = aWeights.x * bones[int(aJoints.x)] + aWeights.y * bones[int(aJoints.y)] + aWeights.z * bones[int(aJoints.z)] + aWeights.w * bones[int(aJoints.w)];
    vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);
    gl_Position = projection * view * model * skinnedPos;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
