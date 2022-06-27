#version 330 core

out vec4 FragColor;
in vec3 colour;
in vec2 uv;
in vec3 normal;
in vec4 worldPixel;

uniform sampler2D image;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

void main() {

	vec4 diffuseColour = texture(image, uv);
	
	vec3 normalColor = diffuseColour.xyz;

	vec3 lightDir = normalize(lightDirection)*3;
	vec3 viewDir = normalize(vec3(worldPixel) - cameraPosition);
	vec3 lightReflect = normalize(reflect(-lightDir, normal));

	float specular = pow(max(dot(lightReflect, normalize(viewDir)), 0.0), 2048);
	float light = max(dot(-lightDir, normalColor), 0.4f);

	FragColor = diffuseColour * light + specular;
}