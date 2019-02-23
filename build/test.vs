#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vRad;

out vec3 color;

uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * view * vec4(vPos, 1.0f);

	color = vRad;
}
