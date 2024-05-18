#pragma once
#include <glm/glm.hpp>
class Bond
{
private:
	glm::vec4 start;
	glm::vec4 end;
	glm::vec4 direction;
	glm::vec4 center;
	float length;
			
public:
	glm::vec4 getStart();
	glm::vec4 getEnd();
	float getLength();
	glm::vec4 getDirection();
	glm::vec4 getCenter();
	void setStart(glm::vec4 start);
	void setEnd(glm::vec4 end);
	Bond();
	Bond(glm::vec4 start, glm::vec4 end);
	~Bond();
};

