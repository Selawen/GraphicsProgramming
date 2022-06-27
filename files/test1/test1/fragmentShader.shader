#version 330 core

out vec4 FragColor;
in vec3 colour;
in vec2 uv;
in vec3 normal;
in vec4 worldPixel;

uniform sampler2D heightmap;
uniform sampler2D normalMap;

uniform sampler2D dirt, sand, grass, snow, rock;

uniform vec3 lightDirection;

vec3 lerp(vec3 a, vec3 b, float t) {
	return a + (b - a) * t;
}

void main() 
{
	vec4 diffuseColour = texture(heightmap, uv);

	vec3 normalColor = normalize(texture(normalMap, uv).rbg * 2 - 1);
	normalColor.b *= -1;
	normalColor.r *= -1;
	
	vec3 lightDir = normalize(lightDirection); //vec3(0, -0.5, -1)

	vec3 camPos = vec3(3, 1, -3);
	vec3 viewDir = normalize(vec3(worldPixel) - camPos);
	vec3 lightReflect = normalize(reflect(-lightDir, normal));
	
	float specular = pow( max( dot( lightReflect, normalize( viewDir ) ), 0.0), 2048);

	float light = max(dot(-lightDir, normalColor), 0.4f);
	
	vec3 dirtColor = texture(dirt, uv).rgb;
	vec3 sandColor = texture(sand, uv).rgb;
	vec3 grassColor = texture(grass, uv).rgb;
	vec3 snowColor = texture(snow, uv).rgb;
	vec3 rockColor = texture(rock, uv).rgb;

	float ds = clamp((worldPixel.y - 1) / 10, 0, 1);
	float sg = clamp((worldPixel.y - 4) / 10, 0, 1);
	float gg = clamp((worldPixel.y - 4.5) / 10, 0, 1);
	float gr = clamp((worldPixel.y - 10) / 10, 0, 1);
	float rsn = clamp((worldPixel.y - 22) / 10, 0, 1);

	vec3 diffuse = lerp(lerp(lerp(lerp(lerp(dirtColor, sandColor, ds), grassColor, sg), grassColor, gg), rockColor, gr), snowColor, rsn);

	FragColor = vec4(diffuse, 1.0) * light; // + specular;
}