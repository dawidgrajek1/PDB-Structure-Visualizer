#include "PDBParser.h"
#include "Atom.h"
#include <iostream>
#include <fstream>

PDBParser::PDBParser()
{
}

PDBParser::~PDBParser()
{
}

std::vector<Atom> PDBParser::parsePDBFile(std::string pdbFile)
{
	try {
		std::vector<Atom> atoms;
		std::string line;
		std::ifstream file(pdbFile);
		if (!file.is_open())
		{
			throw "Could not open file.";
		}

		while (std::getline(file, line))
		{
			if (line.substr(0, 4) == "ATOM")
			{
				Atom tmpAtom;
				tmpAtom.setSerial(std::stoi(line.substr(6, 5)));
				tmpAtom.setName(line.substr(12, 4));
				tmpAtom.setResName(line.substr(17, 3));
				tmpAtom.setAltLoc(line[16]);
				tmpAtom.setX(std::stof(line.substr(30, 8)));
				tmpAtom.setY(std::stof(line.substr(38, 8)));
				tmpAtom.setZ(std::stof(line.substr(46, 8)));
				tmpAtom.setChainID(line[21]);
				tmpAtom.setResSeq(std::stoi(line.substr(22, 4)));
				tmpAtom.setICode(line[26]);
				tmpAtom.setOccupancy(std::stof(line.substr(54, 6)));
				tmpAtom.setTempFactor(std::stof(line.substr(60, 6)));
				tmpAtom.setElement(line.substr(76, 2));
				tmpAtom.setCharge(line.substr(78, 2));
				atoms.push_back(tmpAtom);
			}
			else if (line.substr(0, 3) == "TER")
			{
				break;
			}
		}

		file.close();
		return atoms;
	}
	catch (const char* msg)
	{
		std::cerr << msg << std::endl;
	}

	return std::vector<Atom>();
}
