#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace Ray {
	static float getDistance(float x, float y, float z, float a, float b, float c) {
		return sqrt(pow(x - a, 2) + pow(y - b, 2) + pow(z - c, 2));
	}
	static float getDistanceToGround(float xdir, float ydir, float zdir, float height, float ground = 0) {
		glm::vec3 dir(xdir, ydir, zdir);
		dir = glm::normalize(dir);
		return sqrt(pow(height - ground, 2)*(1 + pow(dir.x / dir.y, 2) + pow(dir.z / dir.y, 2)));
	}
};

class Camera {
	float dmove = 0.4f;
	glm::vec3 position;
	glm::vec3 dir; //guaranteed to be always normalized
	inline glm::vec3 getViewXDir() {
		return glm::normalize(glm::cross(dir, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
public:
	Camera(float x, float y, float z, float xdir = 0.0f, float ydir = 0.0f, float zdir = 1.0f) : 
		position(glm::vec3(x, y, z)),
		dir(glm::normalize(glm::vec3(xdir, ydir, zdir))) {}
	glm::vec3 getTarget() { 
		if (position.y >= 0)
			return glm::vec3(position + Ray::getDistanceToGround(dir.x, dir.y, dir.z, position.y)*dir);
		return glm::vec3(position + Ray::getDistanceToGround(dir.x, dir.y, dir.z, position.y, position.y - 1)*dir);
	} 
	glm::vec3 getPosition() { return position; }
	void moveRight(float scale) {
		glm::vec3 viewXDir = dmove * getViewXDir();
		position += viewXDir * scale;
	}
	glm::vec3 getDirection() { return dir; }
	void moveUp(float scale) { 
		position.y += dmove * scale;
	}
	void moveForward(float scale) {
		position += dir * scale;
	}
	void moveStraight(float scale = 1.0f) {
		position.x += scale * dir.x;
		position.z += scale * dir.z;
	}
	void rotate(float radians, float x_axisdir, float y_axisdir) { //solament una direcció
		glm::vec3 rotXdir = getViewXDir() * x_axisdir; //normalized
		glm::vec3 rotYdir = glm::vec3(0.0, 1.0, 0.0) * y_axisdir;
		dir = glm::rotate(dir, radians, glm::normalize(rotXdir+rotYdir));
	}
};
