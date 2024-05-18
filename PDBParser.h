#pragma once
#include <string>
#include <vector>
#include "Atom.h"


class PDBParser
{
	public:
	PDBParser();
	~PDBParser();

	std::vector<Atom> parsePDBFile(std::string pdbFile);
};

