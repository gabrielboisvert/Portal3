#version 330 core

in vec3 fragPos;
in vec3 norm;
in vec2 text;

uniform vec3 view;

struct DirLight {
    vec3 direction;
  
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform int dirlightCount;
#define NR_DIR_LIGHTS 4
uniform DirLight dirLights[NR_DIR_LIGHTS];


struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform int pointlightCount;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];


struct SpotLight {    
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
uniform int spotlightCount;
#define NR_SPOT_LIGHTS 4
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

uniform sampler2D aTexture;
uniform int hasTexture;
uniform vec4 objColor;
uniform float alpha;
uniform int isSelected;

out vec4 fragColor;


vec3 CalcDirLight(DirLight light)
{
	// diffuse shading
    float diff = max(dot(norm, -normalize(light.direction)), 0.0);

	// specular shading
    vec3 viewDir = normalize(view - fragPos);
    vec3 halfwayDir = normalize(-light.direction + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 528);//material.shininess);
    
	// combine results
	vec3 ambient  = light.ambient.w * vec3(light.ambient);
    if (hasTexture == 1)
        ambient * vec3(texture(aTexture, text));

    vec3 diffuse  = light.diffuse.w  * diff * vec3(light.diffuse);
    if (hasTexture == 1)
        diffuse *= vec3(texture(aTexture, text));

    vec3 specular = light.specular.w * spec * vec3(light.specular);
    if (hasTexture == 1)
        specular *= vec3(texture(aTexture, text));
    
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light)
{
    vec3 normal = normalize(norm);

    vec3 lightDir = normalize(light.position - fragPos);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 viewDir = normalize(view - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 256);//material.shininess);

    // combine results
    vec3 ambient  = light.ambient.w * vec3(light.ambient);
    if (hasTexture == 1)
        ambient * vec3(texture(aTexture, text));

    vec3 diffuse  = light.diffuse.w  * diff * vec3(light.diffuse);
    if (hasTexture == 1)
        diffuse *= vec3(texture(aTexture, text));

    vec3 specular = light.specular.w * spec * vec3(light.specular);
    if (hasTexture == 1)
        specular *= vec3(texture(aTexture, text));
    
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse 
    float diff = max(dot(norm, lightDir), 0.0);
        
    // specular shading
    vec3 viewDir = normalize(view - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 256);//material.shininess);  

    // combine results
    vec3 ambient  = light.ambient.w * vec3(light.ambient);
    if (hasTexture == 1)
        ambient * vec3(texture(aTexture, text));

    vec3 diffuse  = light.diffuse.w  * diff * vec3(light.diffuse);
    if (hasTexture == 1)
        diffuse *= vec3(texture(aTexture, text));

    vec3 specular = light.specular.w * spec * vec3(light.specular);
    if (hasTexture == 1)
        specular *= vec3(texture(aTexture, text));

    float theta = dot(lightDir, -light.direction);
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    ambient  *= attenuation;
    diffuse   *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
	vec3 dirLightsRes;
    for (int i = 0; i < dirlightCount; i++)
        dirLightsRes += CalcDirLight(dirLights[i]);

    vec3 pointLightsRes;
    for (int i = 0; i < pointlightCount; i++)
        pointLightsRes += CalcPointLight(pointLights[i]);

    vec3 spotLightsRes;
    for (int i = 0; i < spotlightCount; i++)
        spotLightsRes += CalcSpotLight(spotLights[i]);

	fragColor = vec4((dirLightsRes + pointLightsRes + spotLightsRes), alpha) * objColor;

    if (isSelected == 1)
		fragColor = fragColor * vec4(0, 0.5f, 0, 1);
}