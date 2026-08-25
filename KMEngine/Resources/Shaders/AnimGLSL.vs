/*#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aJoints;
layout(location = 2) in vec4 aWeights;
layout(location = 3) in vec2 aTexCoord;

// For testing purposes
layout(location = 4) in mat4 aSkinMatrix;

out vec2 TexCoord;

uniform mat4 bones[64];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 skin[64];


void main()
{
    mat4 skinMatrix = aWeights.x * bones[int(aJoints.x)] + aWeights.y * bones[int(aJoints.y)] + aWeights.z * bones[int(aJoints.z)] + aWeights.w * bones[int(aJoints.w)];
    vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);
    //vec4 skinnedPos = skin * vec4(aPos, 1.0);
    gl_Position = projection * view * model * skinnedPos;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}
*/

#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aJoints;
layout(location = 2) in vec4 aWeights;
layout(location = 3) in vec2 aTexCoord;

out vec2 TexCoord;

// Debug outputs — visible in RenderDoc vertex output
out vec4 debugSkinnedPos;
out vec4 debugWeights;      // aWeights.xyzw
out vec4 debugJoints;       // aJoints.xyzw (joint indices as floats)

// bones[int(aJoints.x)] — each mat4 needs 4 vec4 outputs
out vec4 debugBone0Col0;
out vec4 debugBone0Col1;
out vec4 debugBone0Col2;
out vec4 debugBone0Col3;

out vec4 debugBone1Col0;
out vec4 debugBone1Col1;
out vec4 debugBone1Col2;
out vec4 debugBone1Col3;

out vec4 debugBone2Col0;
out vec4 debugBone2Col1;
out vec4 debugBone2Col2;
out vec4 debugBone2Col3;

out vec4 debugBone3Col0;
out vec4 debugBone3Col1;
out vec4 debugBone3Col2;
out vec4 debugBone3Col3;

uniform mat4 bones[64];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 b0 = bones[int(aJoints.x)];
    mat4 b1 = bones[int(aJoints.y)];
    mat4 b2 = bones[int(aJoints.z)];
    mat4 b3 = bones[int(aJoints.w)];

    mat4 skinMatrix = aWeights.x * b0
                    + aWeights.y * b1
                    + aWeights.z * b2
                    + aWeights.w * b3;

    vec4 skinnedPos = skinMatrix * vec4(aPos, 1.0);

    // Debug outputs
    debugSkinnedPos = skinnedPos;
    debugWeights    = aWeights;
    debugJoints     = aJoints;

    // bones[int(aJoints.x)] columns
    debugBone0Col0 = b0[0]; debugBone0Col1 = b0[1]; debugBone0Col2 = b0[2]; debugBone0Col3 = b0[3];

    // bones[int(aJoints.y)] columns
    debugBone1Col0 = b1[0]; debugBone1Col1 = b1[1]; debugBone1Col2 = b1[2]; debugBone1Col3 = b1[3];

    // bones[int(aJoints.z)] columns
    debugBone2Col0 = b2[0]; debugBone2Col1 = b2[1]; debugBone2Col2 = b2[2]; debugBone2Col3 = b2[3];

    // bones[int(aJoints.w)] columns
    debugBone3Col0 = b3[0]; debugBone3Col1 = b3[1]; debugBone3Col2 = b3[2]; debugBone3Col3 = b3[3];

    gl_Position = projection * view * model * skinnedPos;
    TexCoord = aTexCoord;
}