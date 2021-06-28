#version 450 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; // world space coordinate of light pos
uniform vec3 viewPos; // eye location of camera in world space
uniform vec3 lightColor;
uniform vec3 objectColor;


uniform float ambientStrength = 0.15;
uniform float specularStrength = 0.5;
uniform int shinenss = 32;

uniform float constant = 1.0;
uniform float linear   = 0.09;
uniform float quadratic = 0.032;

void main() {
    // ambient
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - FragPos);
    float d = sqrt(dot((lightPos - FragPos), (lightPos - FragPos)));
    float attenuation = 1.0 / (constant + linear * d + 
    		                    quadratic * (d * d));    
    // diffuse
    float cosine = max(dot(Normal, lightDir), 0.0);
    vec3  diffuse = cosine * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, Normal);  
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), shinenss);
    vec3 specular = specularStrength * spec * lightColor;  

    vec4 objColor = texture(texture1, TexCoord);
	FragColor = vec4(attenuation * (shinenss + 8)/ (8 * radians(180)) * 
                      (ambient + diffuse + specular), 1.0) * objColor;
}