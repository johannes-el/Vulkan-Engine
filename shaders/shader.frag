#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
	vec3 normal = normalize(vec3(0.0, 0.0, 1.0));
	vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 ambient = 0.2 * fragColor;
	vec3 diffuse = diff * fragColor;

	vec3 color = ambient + diffuse;

	outColor = texture(texSampler, fragTexCoord);
}
