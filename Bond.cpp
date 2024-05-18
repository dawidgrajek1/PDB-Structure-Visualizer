#include "Bond.h"

Bond::Bond()
{
    this->start = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->end = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->length = 0.0f;
	this->direction = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	this->center = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

Bond::Bond(glm::vec4 start, glm::vec4 end)
{
	this->start = start;
	this->end = end;
	this->length = glm::distance(start, end);
	this->direction = glm::normalize(end - start);
	this->center = (start + end) / 2.0f;
}

Bond::~Bond()
{
}

glm::vec4 Bond::getStart()
{
	return this->start;
}

glm::vec4 Bond::getEnd()
{
	return this->end;
}

float Bond::getLength()
{
	return this->length;
}

glm::vec4 Bond::getDirection()
{
	return this->direction;
}

glm::vec4 Bond::getCenter()
{
	return this->center;
}

void Bond::setStart(glm::vec4 start)
{
	this->start = start;
}

void Bond::setEnd(glm::vec4 end)
{
	this->end = end;
}


