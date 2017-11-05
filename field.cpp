#include "field.h"
#define height(x, z) coordinates[3*WIDTH*x+3*z+1]

	Field::Field(int width, int depth) : WIDTH(width), DEPTH(depth), fieldShader("fieldShader.vert", "fieldShader.frag") {}
	double Field::centerx() { return WIDTH / 2; }
	double Field::centerz() { return DEPTH / 2; }
	void Field::create() {
		fieldShader.link();
		fieldShader.use();
		indices = std::vector<unsigned int>((WIDTH*DEPTH) + (WIDTH - 1)*(DEPTH - 2));
		setIndices();
		coordinates = new float[WIDTH*DEPTH * 3];
		for (int i = 0; i < WIDTH; i++) {
			for (int j = 0; j < DEPTH; j++) {
				coordinates[i * WIDTH * 3 + j * 3] = i;
				coordinates[i * WIDTH * 3 + j * 3 + 1] = 0.0f;
				coordinates[i * WIDTH * 3 + j * 3 + 2] = j;
			}
		}

		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);
		int bindingPoint = 3;
		glGenBuffers(1, &uboID);
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		Wave v = { {0.0f, 0.0f},  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0.0f, 0.0f, {0.0f, 0.0f} };
		glBufferData(GL_UNIFORM_BUFFER, 1024*sizeof(Wave), NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboID);
		fieldShader.bindUniformBlock(uboID, "data", bindingPoint);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		wavesData = (Wave*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, 
			WIDTH*DEPTH * 3 * sizeof(float),
			&coordinates[0], GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		delete[] coordinates;

		glGenBuffers(1, &eboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indices.size() * sizeof(unsigned int),
			indices.data(), GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);

		coordinates = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

		glEnableVertexAttribArray(0); //layout (location = 0) in vec3
		glBindVertexArray(0);
		default = Wave{ { 0.0f, 0.0f }, 0.0f, 15.0f, 4.0f, 0.3f, 0.1f, 1, 4.0f, 0.0f, { 0.0f, 0.0f } };
	}
	void Field::update_and_draw(GLenum mode) {

		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
		getShader().setInt("arrayLength", wavesDataLength);
		glUnmapBuffer(GL_UNIFORM_BUFFER);

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindVertexArray(vaoID);

		glPolygonMode(GL_FRONT_AND_BACK, mode);
		glEnable(GL_POLYGON_OFFSET_FILL);

		glDrawElements(
			GL_TRIANGLE_STRIP,
			indices.size(),
			GL_UNSIGNED_INT,
			(void*)0
		);
		coordinates = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
		wavesData = (Wave*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

		glBindVertexArray(0);
	}
	Shader& Field::getShader() {
		return fieldShader;
	}
	/*
	*No me funciona del todo correctamente, pero funciona para NxN
	**https://stackoverflow.com/questions/5915753/generate-a-plane-with-triangle-strips
	*/
	void Field::setIndices() {

		for (int i = 0; i < DEPTH; i++) {
			indices[i] = i;
		}
		int i = 0;
		for (int row = 0; row<DEPTH - 1; row++) {
			if ((row & 1) == 0) { // even rows
				for (int col = 0; col<WIDTH; col++) {
					indices[i++] = col + row * WIDTH;
					indices[i++] = col + (row + 1) * WIDTH;
				}
			}
			else { // odd rows
				for (int col = WIDTH - 1; col>0; col--) {
					indices[i++] = col + (row + 1) * WIDTH;
					indices[i++] = col - 1 + +row * WIDTH;
				}
			}
		}
	}
	void Field::setHeight(int width, int depth, float value) {
		if (width < WIDTH && width >= 0 && depth >= 0 && depth < DEPTH) {
			coordinates[3 * WIDTH * width + 3 * depth + 1] = value;
		}
	}
	float Field::getHeight(int width, int depth) {
		if(width < WIDTH && width >= 0 && depth >= 0 && depth < DEPTH)
			return coordinates[3 * WIDTH * width + 3 * depth + 1];
		//estoy loco
		return -std::numeric_limits<float>::max();
	}
	void Field::addWave(Wave wave) {
		//happens when iterator tops
		if (wavesDataLength > wavesLimit) {
			wavesDataIterator = 0;
			wavesLimit = wavesDataLength;
		}
		wavesData[wavesDataIterator++] = wave;
		wavesDataLength = std::max(wavesDataLength, wavesDataIterator);
	}
	void Field::setLowerWavesLimit() {
		wavesLimit = wavesDataLength;
	}
	void Field::setUpperWavesLimit() {
		wavesLimit = std::numeric_limits<int>::max();
	}
	int Field::getCapacity() {
		return wavesDataLength;
	}
	void Field::readDataFile() { 
		save_data.clear();
		std::vector<float> filler;
		std::vector<float> default_filler;
		std::string str; //current line
		std::string buf; //buffer for a number
		int line = 0;
		std::ifstream file;
		file.open("data.data");
		if(!file.is_open())
			file.open("data.txt");
		if (!file.is_open())
			std::cerr << "\nNo hi ha ningu arxiu data.txt o data.data" << std::endl;
		while (std::getline(file, str))
		{
			line++;
			if (str.at(0) == '#')
				continue;
			if (str.at(0) == '!') {

				std::stringstream ss(str);
				ss >> buf;
				while (ss >> buf) {
					default_filler.push_back(atof(buf.c_str()));
				}
				if (default_filler.size() != 9) {
					std::cerr << "ENG corruption at line " << line << std::endl;
					std::cerr << "CAT corrupció a la linea " << line << std::endl;
					return;
				}
				if (!default_filler[5]) //if packet length is zero
				{
					//this is to avoid division by zero in shader
					default_filler[5] = 1.0f;
					default_filler[4] = 0;
				}
				default = { { 0.0f, 0.0f }, 0.0f, default_filler[0], default_filler[1], default_filler[2], default_filler[3], (int)default_filler[4], default_filler[5], default_filler[6], { default_filler[7], default_filler[8] } };
				continue;
			}
			std::stringstream ss(str);
			while (ss >> buf) {
				filler.push_back(atof(buf.c_str()));
			}
			if (filler.size() != 12) {
				std::cerr << "ENG corruption at line " << line << std::endl;
				std::cerr << "CAT corrupció a la linea " << line << std::endl;
				return;
			}
			Wave v = { { filler[0], filler[1] }, filler[2], filler[3], filler[4], filler[5], filler[6], filler[7], filler[8], filler[9], { filler[10], filler[11] } };
			filler.clear();
			save_data.push_back(v);
		}
	}
	void Field::loadDataSave(float t) {
		clear();
		for (Wave v : save_data) {
			v.start_t += t;
			if (v.packet_l == 0) {
				v.packet_l = 1;
				v.is_packet = 0;
			}
			addWave(v);
		}
	}
	void Field::setDefaultWave(float x, float y, float t) {
		Wave w = { { x, y }, t, default.w, default.l, default.A, default.sdecay, default.is_packet, default.packet_l, default.ph, { default.velocity[0], default.velocity[1] } };
		addWave(w);
	}
	void Field::clear() {
		wavesDataLength = 0;
		wavesDataIterator = 0;
	}