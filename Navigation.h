#pragma once

#include <fstream>
#include <string>

class Navigation
{
	std::ofstream _outFile;


public:
	Navigation();
	~Navigation();

	double CalcDistanceFromTwoPoints(const double originLong, const double originLat, const double destLong, const double destLat) const;

	bool BuildNetwork(const std::string& fileNamePlaces, const std::string& fileNameLinks);
	bool ProcessCommand(const std::string& commandString);
	
};