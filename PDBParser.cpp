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

			}
			else if (line.substr(0, 3) == "TER")
			{
				break;
			}
		}


		file.close();
	}
	catch (const char* msg)
	{
		std::cerr << msg << std::endl;
	}
}
