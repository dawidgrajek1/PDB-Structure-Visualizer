#version 330

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in vec4 n;
in vec4 v;
in vec4 l;
in vec4 colorF;

vec4 nf;
vec4 vf;
vec4 lf;

void main(void) {
    nf = normalize(n);
    vf = normalize(v);
    lf = normalize(l);

	// iloczyn skalarny wektorow normalnego i do swiatla
    float nl = clamp(dot(lf, nf), 0, 1);

    // wektor jednostkowy odbitego promienia swiatla
    vec4 r = reflect(-l, nf);

    // iloczyn skalarny wektorow odbitego i do obserwatora
    float rv = clamp(dot(r, vf), 0, 1);
    rv = pow(rv, 25);

    // kolor swiatla
    vec4 La = vec4(0, 0, 0, 1);
    vec4 Ld = vec4(1, 1, 1, 1);
    vec4 Ls = vec4(1, 1, 1, 1);
    vec4 Ks = vec4(1, 1, 1, 1);

    // kolor piksela
    // L = colorF * La + colorF * Ld * nl;

    // kolor piksela z odbiciem
    vec4 L = colorF * La + colorF * Ld * nl + Ks * Ls * rv;

	pixelColor=L;
}
