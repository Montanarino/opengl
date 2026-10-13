#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    // Automatically compute the true world-space face normal for ANY triangle!
    vec3 edge1 = dFdx(FragPos);
    vec3 edge2 = dFdy(FragPos);
    vec3 norm = normalize(cross(edge1, edge2));

    vec3 lightDir = normalize(lightPos - FragPos);  
    // abs(dot(...)) ensures correct lighting even if a cube face is wound clockwise
    float diff = abs(dot(norm, lightDir));
    vec3 diffuse = diff * lightColor;

    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;

    vec4 fragment = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1.0);

    vec3 result = fragment.rgb * (ambient + diffuse);
    FragColor = vec4(result, fragment.a);
}