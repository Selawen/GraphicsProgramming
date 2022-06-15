#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normals;
in vec4 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_ao1;

uniform vec3 cameraPosition;
uniform vec3 lightDirection;

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void clip(float a) {
    if (a <= 0.0f) discard;
}

void main()
{
    vec3 lightDir = normalize(lightDirection);
    
    vec4 diffuse = texture(texture_diffuse1, TexCoords);
    vec4 specTex = texture(texture_specular1, TexCoords);

    float light = max(dot(-lightDir, Normals), 0.5);

    //clip(light - 0.3f);

    vec3 viewDir = normalize(cameraPosition - FragPos.rgb);
    vec3 refl = normalize(reflect(lightDir, Normals));

    float ambientOcclusion = texture(texture_ao1, TexCoords).r;
    
    float roughness = texture(texture_roughness1, TexCoords).r;
    float spec = pow(max(dot(viewDir, refl), 0.0), lerp(1, 128, roughness));
    vec3 specular = spec * specTex.rgb;
    
    vec4 result = diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0);
    result.a = 0.6f;
    //FragColor = diffuse;
    FragColor = result;//diffuse * max(light * ambientOcclusion, 0.2 * ambientOcclusion) + vec4(specular, 0);
}