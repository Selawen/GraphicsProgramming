#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColour;
layout(location = 2) in vec2 vUV;
layout(location = 3) in vec3 vNormal;

uniform mat4 world, view, projection;
uniform float time;
uniform vec2 mouse;

out vec3 colour;
out vec2 uv;
out vec3 normal;
out vec4 worldPixel;
out float t;
out vec2 dis;

//random noise
float random(vec2 p)
{
	vec2 K1 = vec2(
		23.14069263277926, // e^pi (Gelfond's constant)
		2.665144142690225 // 2^sqrt(2) (Gelfond-Schneider constant)
	);

	//float mouseVal = mouse.x*mouse.y;

	//return fract(cos(mod(sin(mouseVal)*10, 1e-7 + 256.0 * dot(p, K1))) * time);
	return fract(cos(mod(123456789., 1e-7 + 256.0 * dot(p, K1))) * time);
}

void main() {

	// Time varying pixel color
	vec3 col = 0.5 + 0.5 * cos(time + vUV.xyx + vec3(0, 2, 4));

	worldPixel = world * vec4(vPos, 1.0f);
	//worldPixel.y += random(vUV)*10;
	
	//dis = int(sqrt(int(vPos.x - mouse.x) ^ 2 + int(vPos.y - mouse.y) ^ 2));
	dis = (vPos.xy - mouse) ;
	worldPixel += normalize(vec3(random(col.rg)*dis.x, random(col.bg)*dis.y, random(col.gr)*2))*10;
	//worldPixel += normalize(vec3(random(col.rg*dis)*dis.x, random(col.bg)*2, random(col.gr*dis)*dis.y))*10;
	//worldPixel += normalize(vec3(random(col.rg)+dis, random(col.bg)*2, random(col.gr)+dis))*10;
	//worldPixel += vec3(random(vUV), random(vUV)*2, random(vUV))*10;

	gl_Position = projection * view * worldPixel;
	//gl_Position = projection * view * world * vec4(vPos, 1.0f);

	colour = vColour+normalize(random(vUV) * 100);
	uv = vUV;
	normal = mat3(world) * vNormal;
	//normal *= (random(vUV) *100);
	t = time;
}