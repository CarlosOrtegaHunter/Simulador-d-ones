#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include "fieldShader.h"

struct Wave {
	float source[2];
	float start_t;
	float w;
	float l;
	float A;
	float sdecay;
	int is_packet;
	float packet_l;
	float ph;
	float velocity[2];
};

struct Waves_data {
	Wave waves[];
};

class Field {
private:
	GLuint vboID, eboID, vaoID, ssboID, uboID;
	int WIDTH, DEPTH;
	int wavesLimit = std::numeric_limits<int>::max();
	float* coordinates;
	Shader fieldShader;
	Wave* wavesData; //this is an array
	Wave default;
	int wavesDataLength = 0, wavesDataIterator = 0, idealWavesDataLength;
	std::vector<unsigned int> indices;
	std::vector<Wave> save_data;
	void setIndices();
public:
	Field(int width, int depth);
	double centerx();
	double centerz();
	void create();
	void update_and_draw(GLenum mode = GL_FILL);
	Shader& getShader();
	void Field::setHeight(int width, int depth, float value);
	float Field::getHeight(int width, int depth);
	void Field::addWave(Wave wave);
	void Field::setLowerWavesLimit(); //limits capacity
	void Field::setUpperWavesLimit();
	int Field::getCapacity();
	void Field::readDataFile();
	void Field::loadDataSave(float t);
	void Field::addDefaultWave(float x, float y, float t);
	void Field::clear();
};