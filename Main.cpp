#include <iostream>
#include <cmath>
#include <chrono>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include "camera.h"
#include "field.h"

#define WIDTH 400
#define DEPTH 400

float global_time; //in seconds!
float t_acc; //time accumulator for pauses
int window_width, window_height;
float scale;
std::chrono::steady_clock::time_point oldt, time_zero, pause_time;
std::chrono::high_resolution_clock main_clock;
int loopIter = 0; //loop iterator modulus of 100
bool pause, showsExecutionData;
GLenum displayMode;

Field field(WIDTH, DEPTH);
Camera camera(-10, 10, -10, WIDTH/2, 0, DEPTH/2);
int oldx = 0, oldy = 0; //mouse coords

void displayHelp() {
	std::cout << std::endl;
	std::cout << "------------------------ HOW TO USE ------------------------" << std::endl;
	std::cout << "SIMULADOR I -Carlos Ortega H. 2017- Podeu trobar el codi a https://github.com/CarlosOrtegaHunter/Simulador-d-ones" << std::endl;
	std::cout << "Presioni H per veure aquest dialeg d'ajuda. " << std::endl;
	std::cout << "Presioni W, S, A, D, +, - per moure. " << std::endl;
	std::cout << "Presioni P per pausar. " << std::endl;
	std::cout << "Presioni C per reiniciar la camera. " << std::endl;
	std::cout << "Presioni M per alternar el mode de visualització (triangles o FILL). " << std::endl;
	std::cout << "Presioni E per monitorear el temps d'execució. " << std::endl;
	std::cout << "Presioni ESPAI per situar una ona sobre el camp en la direcció de mira. " << std::endl;
	std::cout << "Presioni R per llegir un arxiu \"data.data\" o \"data.txt\" (el primer toma precedencia) i pujar la nova informacio." << std::endl;
	std::cout << "L'especificacio del arxiu data es:" << std::endl;
	std::cout << "Comencar amb # per comentaris" << std::endl;
	std::cout << "Una ona per linea de la seguent forma, amb els nombres separats per espais:" << std::endl;
	std::cout << "posicio_x, posicio_y, temps de comenc, frequencia angular, longitud d'ona, "
		<< "Amplitut, decadencia exponencial respecte a l'espai, es un paquet? (1 o 0), "
		<< "longitud del paquet, fase inicial, velocitat_x, velocitat_y " << std::endl;
	std::cout << "El temps d'aquestes ones es relatiu al punt en el temps en que son carregades." << std::endl;
	std::cout << "La ultima linea que comenci amb ! i un espai configura les ones creades per el boto ESPAI. "
		<< "Utilitza el mateix formate per posar els nombres pero sense posicio_x, posicio_y ni temps\n" << std::endl;
}

void reshape(int width, int height) {
	window_width = width;
	window_height = height;
	glViewport(0, 0, width, height);
}

void specialKey(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT: camera.moveRight(scale); break;
	case GLUT_KEY_LEFT: camera.moveRight(-scale); break;
	case GLUT_KEY_UP: camera.moveUp(scale); break;
	case GLUT_KEY_DOWN: camera.moveUp(-scale); break;
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c': //reset camera / center
	case 'C':
		camera = Camera(WIDTH / 2, 1.0f, DEPTH / 2);
		break;
	case 'w': 
	case 'W':
		camera.moveStraight(scale);
		break;
	case 's':
	case 'S':
		camera.moveStraight(-scale);
		break;
	case 'a': 
	case 'A':
		camera.moveRight(-scale);
		break;
	case 'd':
	case 'D':
		camera.moveRight(scale);
		break;
	case 'm':
	case 'M':
		if (displayMode == GL_FILL)
			displayMode = GL_LINE;
		else 
			displayMode = GL_FILL;
		break;
	case 'p':
	case 'P':
		if (pause) { //when un-pausing
			t_acc += (main_clock.now() - pause_time).count();
		}
		else
			std::cout << "(PAUSA - PAUSE)" << std::endl;
			pause = !pause;
			pause_time = main_clock.now();
		break;
	case '+':
		camera.moveForward(10.0f*scale);
		break;
	case '-':
		camera.moveForward(-10.0f*scale);
		break;
	case 'e':
	case 'E':
		showsExecutionData = !showsExecutionData;
		std::cout << std::endl;
		break;
	case '?':
	case 'h':
	case 'H':
		showsExecutionData = false;
		displayHelp();
		break;
	case 'r':
	case 'R':
		field.readDataFile();
		field.loadDataSave(global_time);
		break;
	case ' ':
		glm::vec3 dir = camera.getDirection();
		float dist = Ray::getDistanceToGround(dir.x, dir.y, dir.z, camera.getPosition().y);
		glm::vec3 ground = camera.getPosition() + dir*dist;
		field.addDefaultWave(ground.x / scale, ground.z / scale, global_time);
		/*
		field.setHeight(ground.x / scale, ground.z / scale, field.getHeight(ground.x / scale, ground.z / scale) - 0.055f);
		field.setHeight(ground.x / scale + 1, ground.z / scale, field.getHeight(ground.x / scale + 1, ground.z / scale) - 0.04f);
		field.setHeight(ground.x / scale, ground.z / scale + 1, field.getHeight(ground.x / scale, ground.z / scale + 1) - 0.04f);
		field.setHeight(ground.x / scale - 1, ground.z / scale, field.getHeight(ground.x / scale - 1, ground.z / scale) - 0.04f);
		field.setHeight(ground.x / scale, ground.z / scale - 1, field.getHeight(ground.x / scale, ground.z / scale - 1) - 0.04f);
		field.setHeight(ground.x / scale + 1, ground.z / scale + 1, field.getHeight(ground.x / scale + 1, ground.z / scale + 1) - 0.02f);
		field.setHeight(ground.x / scale - 1, ground.z / scale - 1, field.getHeight(ground.x / scale - 1, ground.z / scale - 1) - 0.02f);
		field.setHeight(ground.x / scale - 1, ground.z / scale + 1, field.getHeight(ground.x / scale - 1, ground.z / scale + 1) - 0.02f);
		field.setHeight(ground.x / scale + 1, ground.z / scale - 1, field.getHeight(ground.x / scale + 1, ground.z / scale - 1) - 0.02f);
		*/
		break;
	}

	glutPostRedisplay();
}

void init() {
	scale = 1.0f;
	glEnable(GL_DEPTH_TEST);
	field.create();
	oldt = main_clock.now();
	global_time = 0;
	time_zero = main_clock.now();
	displayMode = GL_FILL;
	displayHelp();
}

void update_and_display() {
	if(loopIter < 100)
		loopIter++;
	if (!pause) {
		global_time = ((main_clock.now() - time_zero).count() - t_acc) / 1000000000.0f;
	}
	glm::mat4 view, projection, MVP;
	if (window_height == 0)
		window_height = 1;
	projection = glm::perspective(45.0f, float(window_width/ window_height), 0.1f, 800.0f);
	view = glm::lookAt(camera.getPosition(), camera.getTarget(), glm::vec3(0.0, 1.0, 0.0));
	MVP = projection * view * glm::scale(scale, scale, scale);

	field.getShader().setMat4("MVP", MVP);
	field.getShader().setFloat("global_t", global_time);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	field.update_and_draw(displayMode);

	glFlush();
	glutSwapBuffers();
}

static void idle() {
	if (loopIter == 100) {
		//100 iterations, 100 renders, 100 frames, Dt/100 == nanoseconds/ticks
		//1 second = 0.000000001 ns
		float Dt = (main_clock.now() - oldt).count(); //nanoseconds
		float fps = 100000000000 / Dt;
		if(showsExecutionData && !pause)
			std::cout << Dt * 0.000000001 << " seconds, fps is " << fps << " @ capacity " << field.getCapacity() << std::endl;
		if (fps > 60) {
			field.setUpperWavesLimit();
		}
		//LOCK (it's usually 5fps worse than expected)
		else if (fps < 55) {
			field.setLowerWavesLimit();
		}
		loopIter = 0;
		oldt = main_clock.now();
		glutPostRedisplay();
	}
	else {
		glutPostRedisplay();
	}
}

void mouseMotion(int x, int y) {
	float dx, dy;
	dx = oldx - x;
	dy = oldy - y;
	dx /= -8; // - para que rote en sentido contrario al movimiento
	dy /= -8; 
	oldx = x;
	oldy = y;
	camera.rotate(dx, 0.0f, 1.0f);
	camera.rotate(dy, 1.0f, 0.0f);
}

void passiveMouseMotion(int x, int y) {
	oldx = x;
	oldy = y;
}

int main(int argc, char*argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	GLint w = glutCreateWindow("I trust you will compile");
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error: " << glewGetString(err) << std::endl;
	}
	glutIdleFunc(idle);
	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(passiveMouseMotion);
	glutDisplayFunc(update_and_display);
	glutSpecialFunc(specialKey);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutReshapeWindow(400, 400);
	init();
	glutMainLoop();
}