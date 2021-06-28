#version 450 core
layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(vert, 1.0f);
	TexCoord    = texcoord;
	// this is needed if the model is sacled by non-uniform scale
	// the model matrix does not preserve the normal's normal.
	// hence we need the normal matrix which is inverse tranpose of model matrix

	Normal      = mat3(transpose(inverse(model))) * normal;
	FragPos     = vec3(model * vec4(vert, 1.0));
}