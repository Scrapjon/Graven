out vec4 FragColor;

uniform vec3 lightColor;
uniform float intensity;

void main() {
    FragColor = lightColor * intensity;
}