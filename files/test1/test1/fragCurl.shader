#version 330 core

out vec4 FragColor;
in vec3 colour;
in vec2 uv;
in vec3 normal;
in vec4 worldPixel;
in float t;

uniform sampler2D image;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

//random noise
float random(vec2 p)
{
	vec2 K1 = vec2(
		23.14069263277926, // e^pi (Gelfond's constant)
		2.665144142690225 // 2^sqrt(2) (Gelfond-Schneider constant)
	);
	return fract(cos(mod(123456789., 1e-7 + 256.0 * dot(p, K1))) * t);
}

void main() {

	vec4 diffuseColour = texture(image, uv); //vec4(colour, 1.0);//
	//vec4 diffuseColour = texture(image, vec2(uv.x, 0.5 + 0.5 * cos(uv.xyx + random(uv))));
	
	vec3 normalColor = diffuseColour.xyz;

	vec3 lightDir = normalize(lightDirection)*3;
	vec3 viewDir = normalize(vec3(worldPixel) - cameraPosition);
	vec3 lightReflect = normalize(reflect(-lightDir, normal));

	float specular = pow(max(dot(lightReflect, normalize(viewDir)), 0.0), 2048);

	float light = max(dot(-lightDir, normalColor), 0.4f);
	FragColor = diffuseColour * light + specular;
}