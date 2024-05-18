#pragma once
#include <string>
#include <vector>


class PDBParser
{
	public:
	PDBParser();
	~PDBParser();


	std::vector<Atom> parsePDBFile(std::string pdbFile);
};

