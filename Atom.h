#pragma once

#include <string>
class Atom
{
private:
	int serial;
	std::string name;
	char altLoc;
	std::string resName;
	char chainID;
	int resSeq;
	char iCode;
	float x;
	float y;
	float z;
	float occupancy;
	float tempFactor;
	std::string element;
	std::string charge;
	float vdwRadius;
	void setVdwRadius();

public:
	Atom();
	~Atom();
	Atom(int serial,
		std::string name,
		char altLoc,
		std::string resName,
		char chainID,
		int resSeq,
		char iCode,
		float x,
		float y,
		float z,
		float occupancy,
		float tempFactor,
		std::string element,
		std::string charge
	);
	int getSerial();
	std::string getName();
	char getAltLoc();
	std::string getResName();
	char getChainID();
	int getResSeq();
	char getICode();
	float getX();
	float getY();
	float getZ();
	float getOccupancy();
	float getTempFactor();
	std::string getElement();
	std::string getCharge();
	float getVdwRadius();
	void setSerial(int serial);
	void setName(std::string name);
	void setAltLoc(char altLoc);
	void setResName(std::string resName);
	void setChainID(char chainID);
	void setResSeq(int resSeq);
	void setICode(char iCode);
	void setX(float x);
	void setY(float y);
	void setZ(float z);
	void setOccupancy(float occupancy);
	void setTempFactor(float tempFactor);
	void setElement(std::string element);
	void setCharge(std::string charge);
};

