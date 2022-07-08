#version 410 core
in VS_OUT
{
    vec3 fragDiffuse;
    vec3 fragNormal;
    vec3 fragPos;
} fs_in;


uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;


out vec4 fragColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(fs_in.fragNormal);
    vec3 lightDir = normalize(lightPos - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * fs_in.fragDiffuse;
    fragColor = vec4(result, 1.0);
}
