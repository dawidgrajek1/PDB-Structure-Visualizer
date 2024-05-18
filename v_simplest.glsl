#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec4 lp;
uniform vec4 color;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 normal;

out vec4 colorF;
out vec4 n;
out vec4 v;
out vec4 l;

void main(void) {
    gl_Position=P*V*M*vertex;

    // znormalizowany wektor do swiatla w przestrzeni oka
    l = normalize(V * lp - V * M * vertex);

    // znormalizowany wektor normalny wierzcholka w przestrzeni oka
    n = normalize(V * M * normal);
    
    // wektor jednostkowy do obserwatora
    v = normalize(-V * vertex);

    colorF = color;
}
