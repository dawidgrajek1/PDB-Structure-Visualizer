#include "Atom.h"
#include <string>
#include <map>

int Atom::getSerial()
{
	return serial;
}

std::string Atom::getName()
{
	return name;
}

char Atom::getAltLoc()
{
	return altLoc;
}

std::string Atom::getResName()
{
	return resName;
}

char Atom::getChainID()
{
	return chainID;
}

int Atom::getResSeq()
{
	return resSeq;
}

char Atom::getICode()
{
	return iCode;
}

float Atom::getX()
{
	return x;
}

float Atom::getY()
{
	return y;
}

float Atom::getZ()
{
	return z;
}

float Atom::getOccupancy()
{
	return occupancy;
}

float Atom::getTempFactor()
{
	return tempFactor;
}

std::string Atom::getElement()
{
	return element;
}

std::string Atom::getCharge()
{
	return charge;
}

float Atom::getVdwRadius()
{
	return vdwRadius;
}

void Atom::setSerial(int serial)
{
	this->serial = serial;
}

void Atom::setName(std::string name)
{
	this->name = name;
}

void Atom::setAltLoc(char altLoc)
{
	this->altLoc = altLoc;
}

void Atom::setResName(std::string resName)
{
	this->resName = resName;
}

void Atom::setChainID(char chainID)
{
	this->chainID = chainID;
}

void Atom::setResSeq(int resSeq)
{
	this->resSeq = resSeq;
}

void Atom::setICode(char iCode)
{
	this->iCode = iCode;
}

void Atom::setX(float x)
{
	this->x = x;
}

void Atom::setY(float y)
{
	this->y = y;
}

void Atom::setZ(float z)
{
	this->z = z;
}

void Atom::setOccupancy(float occupancy)
{
	this->occupancy = occupancy;
}

void Atom::setTempFactor(float tempFactor)
{
	this->tempFactor = tempFactor;
}

void Atom::setElement(std::string element)
{
	this->element = element;
}

void Atom::setCharge(std::string charge)
{
	this->charge = charge;
}

void Atom::setVdwRadius()
{
	std::map<std::string, float> vdwRadii;
	vdwRadii["H"] = 1.20f;
	vdwRadii["N"] = 1.55f;
	vdwRadii["C"] = 1.70f;
	vdwRadii["O"] = 1.52f;
	vdwRadii["S"] = 1.80f;
	vdwRadii["P"] = 1.80f;
	vdwRadii["F"] = 1.47f;

	if (vdwRadii.find(element) == vdwRadii.end())
	{
		this->vdwRadius = 0.0;
	}
	else
	{
		this->vdwRadius = vdwRadii[element];
	}
}

Atom::Atom()
{
	this->serial = 0;
	this->altLoc = ' ';
	this->chainID = ' ';
	this->resSeq = 0;
	this->iCode = ' ';
	this->x = 0.0;
	this->y = 0.0;
	this->z = 0.0;
	this->occupancy = 0.0;
	this->tempFactor = 0.0;
	this->element = "";
	this->charge = "";
	this->vdwRadius = 0.0;
}

Atom::Atom(	int serial, std::string name, char altLoc, std::string resName, char chainID, int resSeq, char iCode,
			float x, float y, float z, float occupancy, float tempFactor, std::string element, std::string charge)
{
	this->serial = serial;
	this->name = name;
	this->altLoc = altLoc;
	this->resName = resName;
	this->chainID = chainID;
	this->resSeq = resSeq;
	this->iCode = iCode;
	this->x = x;
	this->y = y;
	this->z = z;
	this->occupancy = occupancy;
	this->tempFactor = tempFactor;
	this->element = element;
	this->charge = charge;
	this->setVdwRadius();
}

Atom::~Atom()
{
}
