/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "sphere.h"
#include "cube.h"
#include "PDBParser.h"
#include <iostream>
#include <map>
#include "Bond.h"

float speed_x = 0;
float speed_y = 0;
float speed_zoom = 0;
float zoom = -40;
float aspectRatio = 1;
float fov = 75.0f;
float opacity = 1.0f;

// move the model
float move_speed_x = 0;
float move_speed_y = 0;
float move_speed_z = 0;
float offset_x = 0;
float offset_y = 0;
float offset_z = 0;

// options
bool drawForcesRadius = false;
bool drawHydrogens = false;
bool drawBonds = true;

//fps counter
double total_time = 0.0f;
int nbFrames = 0;

ShaderProgram* sp;
Models::Sphere* sphere;
Models::Cube* cube;
Models::Cube* stick;
PDBParser* parser;
std::vector<Atom> atoms;
std::vector<Bond> bonds;
glm::vec4 moleculeCenter;


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT) speed_x = -PI / 2;
		if (key == GLFW_KEY_RIGHT) speed_x = PI / 2;
		if (key == GLFW_KEY_UP) speed_y = PI / 2;
		if (key == GLFW_KEY_DOWN) speed_y = -PI / 2;
		if (key == GLFW_KEY_PAGE_UP) speed_zoom = 10;
		if (key == GLFW_KEY_PAGE_DOWN) speed_zoom = -10;
		if (key == GLFW_KEY_W) move_speed_y = 40;
		if (key == GLFW_KEY_S) move_speed_y = -40;
		if (key == GLFW_KEY_A) move_speed_x = 40;
		if (key == GLFW_KEY_D) move_speed_x = -40;
		if (key == GLFW_KEY_Q) move_speed_z = 40;
		if (key == GLFW_KEY_E) move_speed_z = -40;
		if (key == GLFW_KEY_F1) drawForcesRadius = !drawForcesRadius;
		if (key == GLFW_KEY_F2) drawHydrogens = !drawHydrogens;
		if (key == GLFW_KEY_F3) drawBonds = !drawBonds;
		if (key == GLFW_KEY_P) if (opacity >= 1.0f) opacity = 0.1f; else opacity += 0.1f;
		if (key == GLFW_KEY_O) if (opacity <= 0.1f) opacity = 1.0f; else opacity -= 0.1f;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT) speed_x = 0;
		if (key == GLFW_KEY_RIGHT) speed_x = 0;
		if (key == GLFW_KEY_UP) speed_y = 0;
		if (key == GLFW_KEY_DOWN) speed_y = 0;
		if (key == GLFW_KEY_PAGE_UP) speed_zoom = 0;
		if (key == GLFW_KEY_PAGE_DOWN) speed_zoom = 0;
		if (key == GLFW_KEY_W) move_speed_y = 0;
		if (key == GLFW_KEY_S) move_speed_y = 0;
		if (key == GLFW_KEY_A) move_speed_x = 0;
		if (key == GLFW_KEY_D) move_speed_x = 0;
		if (key == GLFW_KEY_Q) move_speed_z = 0;
		if (key == GLFW_KEY_E) move_speed_z = 0;
	}
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

std::vector<Bond> calculateBonds(std::vector<Atom> atoms, bool includeHydrogens) {
	std::vector<Bond> bonds;
	for (int i = 0; i < atoms.size(); i++) {
		if (!includeHydrogens && atoms[i].getElement() == " H") {
			continue;
		}
		for (int j = i + 1; j < atoms.size(); j++) {
			if (!includeHydrogens && atoms[j].getElement() == " H") {
				continue;
			}
			glm::vec4 atom1 = glm::vec4(atoms[i].getX(), atoms[i].getY(), atoms[i].getZ(), 1.0f);
			glm::vec4 atom2 = glm::vec4(atoms[j].getX(), atoms[j].getY(), atoms[j].getZ(), 1.0f);
			float distance = glm::distance(atom1, atom2);
			if (2 * distance < atoms[i].getVdwRadius() + atoms[j].getVdwRadius()) {
				//std::cout << "Bond: " << atoms[i].getSerial() << " " << atoms[j].getSerial() << std::endl;
				bonds.push_back(Bond(glm::vec4(atoms[i].getX(), atoms[i].getY(), atoms[i].getZ(), 1.0f), glm::vec4(atoms[j].getX(), atoms[j].getY(), atoms[j].getZ(), 1.0f)));
			}
		}
	}
	return bonds;
}

glm::vec4 centerOfMass(std::vector<Atom> atoms) {
	glm::vec4 center = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < atoms.size(); i++) {
		center = glm::vec4(center.x + atoms[i].getX(), center.y + atoms[i].getY(), center.z + atoms[i].getZ(), center.w + 1.0f);
	}

	center /= atoms.size();
	return center;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);

	sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
	sphere = new Models::Sphere(0.4f, 15, 15);
	stick = new Models::Cube();
	cube = new Models::Cube();
	parser = new PDBParser();
	atoms = parser->parsePDBFile(".\\examples\\example1.pdb");
	std::cout << "Atoms loaded: " << atoms.size() << std::endl;
	bonds = calculateBonds(atoms, false);
	std::cout << "Bonds calculated: " << bonds.size() << std::endl;
	moleculeCenter = centerOfMass(atoms);
	std::cout << "Center of mass: " << moleculeCenter.x << " " << moleculeCenter.y << " " << moleculeCenter.z << std::endl;

	/*for (int i = 0; i < atoms.size(); i++) {
		std::cout << atoms[i].getSerial() << " " << atoms[i].getName() << " " << atoms[i].getResName() << " " << " " << atoms[i].getX() << " " << atoms[i].getY() << " " << atoms[i].getZ() << " _" << atoms[i].getElement() << "_" << std::endl;
	}*/

}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	delete sp;
	delete sphere;
	delete stick;
	delete parser;
}

glm::vec4 atomToColor(Atom atom) {
	std::map<std::string, glm::vec4> atomColors = {
		{" H", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)},
		{" C", glm::vec4(0.4f, 0.4f, 0.4f, 1.0f)},
		{" N", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)},
		{" O", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)},
		{" F", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)},
		{" P", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)},
		{" S", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)}
	};

	if (atomColors.find(atom.getElement()) == atomColors.end()) {
		return glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	return atomColors[atom.getElement()];
}	

void drawAtom(Atom atom, float angle_x, float angle_y, bool drawForcesRadius, bool drawHydrogens) {
	if (!drawHydrogens && atom.getElement() == " H") {
		return;
	}

	glm::mat4 M = glm::mat4(1.0f);
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::translate(M, glm::vec3(atom.getX(), atom.getY(), atom.getZ()));
	M = glm::translate(M, glm::vec3(offset_x, offset_y, offset_z));

	glm::vec4 color = atomToColor(atom);

	//std::cout << atom.getElement() << " " << color.x << " " << color.y << " " << color.z << " " << color.w << std::endl;
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(sp->u("color"), color.r, color.g, color.b, color.a);

	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, sphere->vertices); //Wskaż tablicę z danymi dla atrybutu vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, sphere->vertexNormals); //Wskaż tablicę z danymi dla atrybutu vertex

	if (!drawForcesRadius)
	{
		//Przeslij parametry programu cieniującego do karty graficznej
		glDrawArrays(GL_TRIANGLES, 0, sphere->vertexCount); //Narysuj obiekt
	}

	if (drawForcesRadius) {
		M = glm::scale(M, glm::vec3(atom.getVdwRadius() * 2.5f, atom.getVdwRadius() * 2.5f, atom.getVdwRadius() * 2.5f)); // mnozenie 2.5f bo model sfery ma 0.4f
		glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
		glUniform4f(sp->u("color"), color.r, color.g, color.b, opacity);
		glDrawArrays(GL_TRIANGLES, 0, sphere->vertexCount); //Narysuj obiekt
	}
}

void drawBond(Bond bond, float angle_x, float angle_y) {
	glm::mat4 M = glm::mat4(1.0f);
	// draw a stick between two points
	M = glm::rotate(M, angle_y, glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::rotate(M, angle_x, glm::vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz modelu
	M = glm::translate(M, glm::vec3(bond.getCenter().xyz));
	M = glm::translate(M, glm::vec3(offset_x, offset_y, offset_z));
	M = glm::rotate(M, atan2(bond.getDirection().y, bond.getDirection().x), glm::vec3(0.0f, 0.0f, 1.0f));
	M = glm::rotate(M, acos(bond.getDirection().z), glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::scale(M, glm::vec3(0.01f, 0.01f, bond.getLength()*0.5f));
	
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(sp->u("color"), color.x, color.y, color.z, color.w);

	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, stick->vertices); //Wskaż tablicę z danymi dla atrybutu vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, stick->normals); //Wskaż tablicę z danymi dla atrybutu vertex

	glDrawArrays(GL_TRIANGLES, 0, stick->vertexCount); //Narysuj obiekt
}

void drawBackground() {
	glm::mat4 M = glm::mat4(1.0f);

	M = glm::translate(M, glm::vec3(0.0f, 0.0f, 40.0f));
	M = glm::scale(M, glm::vec3(100.0f, 100.0f, 0.1f));

	glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));
	glUniform4f(sp->u("color"), color.x, color.y, color.z, color.w);

	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, cube->vertices); //Wskaż tablicę z danymi dla atrybutu vertex
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, cube->normals); //Wskaż tablicę z danymi dla atrybutu vertex

	glDrawArrays(GL_TRIANGLES, 0, cube->vertexCount); //Narysuj obiekt
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości

	glm::mat4 V = glm::lookAt(
		glm::vec3(0, 0, zoom), // Punkt obserwatora
		glm::vec3(0.0f, 0.0f, 0.0f), // Punkt na który patrzymy
		glm::vec3(0.0f, 1.0f, 0.0f)); // Kierunek "góry" kamery
	glm::mat4 P = glm::perspective(fov * PI / 180.0f, aspectRatio, 0.01f, 100.0f); //Wylicz macierz rzutowania

	sp->use();//Aktywacja programu cieniującego
	
	// set light position to camera position
	glm::vec4 lp = glm::vec4(0, 0, zoom, 1.0f);

	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));

	// przekazanie pozycji źródła światła do shadera
	glUniform4fv(sp->u("lp"), 1, glm::value_ptr(lp));

	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal

	// rysowanie tla
	drawBackground();

	// Rysowanie atomów
	for (int i = 0; i < atoms.size(); i++) {
		drawAtom(atoms[i], angle_x, angle_y, drawForcesRadius, drawHydrogens);
	}

	// Rysowanie wiązań
	if (drawBonds) {
		for (int i = 0; i < bonds.size(); i++)
		{
			//std::cout << bonds[i].getStart().x << " " << bonds[i].getLength() << std::endl;
			drawBond(bonds[i], angle_x, angle_y);
		}
	}

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}



	window = glfwCreateWindow(800, 800, "PDB Structure Visualizer", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //Aktualny kąt obrotu obiektu
	float angle_y = 0; //Aktualny kąt obrotu obiektu
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		// Measure speed
		total_time += glfwGetTime();
		nbFrames++;
		// fps
		if (total_time >= 1.0) {
			std::cout << "FPS: " << nbFrames << std::endl;
			total_time = 0.0;
			nbFrames = 0;
		}

		angle_x += speed_x * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Zwiększ/zmniejsz kąt obrotu na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		zoom += speed_zoom * glfwGetTime();
		offset_x += move_speed_x * glfwGetTime();
		offset_y += move_speed_y * glfwGetTime();
		offset_z += move_speed_z * glfwGetTime();


		glfwSetTime(0); //Zeruj timer
		drawScene(window, angle_x, angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
