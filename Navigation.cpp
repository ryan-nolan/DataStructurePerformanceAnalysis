#include "Navigation.h"
#include "Node.h"
#include "Arc.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

// Converts latitude/longitude into eastings/northings
extern void LLtoUTM(const double Lat, const double Long, double &UTMNorthing, double &UTMEasting);

vector<Node*> m_Places;

Navigation::Navigation() : _outFile("Output.txt")
{
}

Navigation::~Navigation()
{
}

bool Navigation::ProcessCommand(const string& commandString)
{
	istringstream inString(commandString);
	string command;
	string mode;
	int startNodeID, endNodeID, place1, place2, place3, place4;
	inString >> command;
	if (command == "MaxDist" || command == "MaxLink" || command == "FindDist" || command == "FindNeighbour")
	{
		inString >> startNodeID;
		inString >> endNodeID;
	}
	else if (command == "Check")
	{
		inString >> mode;
		inString >> place1;
		inString >> place2;
		inString >> place3;
		inString >> place4;
	}
	else if (command == "FindRoute" || "FindShortestRoute")
	{
		inString >> mode;
		inString >> startNodeID;
		inString >> endNodeID;
	}


	if (command == "MaxDist")
	{
		double maxDist = 0;
		string firstPlace, SecondPlace;
		for (int i = 0; i < m_Places.size(); i++)
		{
			for (int j = 0; j < m_Places.size(); j++)
			{
				const double dist = CalcDistanceFromTwoPoints(m_Places[i]->GetLongitude(), m_Places[i]->GetLatitude(),
					m_Places[j]->GetLongitude(), m_Places[j]->GetLatitude());
				if (dist>maxDist)
				{
					maxDist = dist;
					firstPlace = *m_Places[i]->GetName();
					SecondPlace = *m_Places[j]->GetName();
				}
			}
		}

		_outFile << commandString << endl << firstPlace << "," << SecondPlace << "," << setprecision(6) << maxDist / 1000 << endl << endl;
		return true;
	}
	if (command == "MaxLink")
	{
		float maxArcDist = 0;
		int originRef, destRef;
		for (int i = 0; i < m_Places.size(); i++)
		{
			vector<Arc*>& currentNodeArcs = *m_Places[i]->GetArcs();
			for (int j = 0; j < currentNodeArcs.size(); j++)
			{
				Node* const destNode = currentNodeArcs[j]->GetDest();
				const double dist = CalcDistanceFromTwoPoints(m_Places[i]->GetLongitude(), m_Places[i]->GetLatitude(),
					destNode->GetLongitude(), destNode->GetLatitude());
				if (dist > maxArcDist)
				{
					maxArcDist = dist;
					originRef = m_Places[i]->GetReferenceNumber();
					destRef = destNode->GetReferenceNumber();
				}
			}
		}
		
		_outFile << commandString << endl << originRef << "," << destRef << "," << setprecision(7) << maxArcDist / 1000 << endl << endl;
		return true;
	}
	if (command == "FindDist")
	{
		Node* startNode = nullptr;
		Node* endNode = nullptr;
		for (int i = 0; i < m_Places.size(); i++)
		{
			if (m_Places[i]->GetReferenceNumber() == startNodeID)
			{
				startNode = m_Places[i];
			}
			else if (m_Places[i]->GetReferenceNumber() == endNodeID)
			{

				endNode = m_Places[i];
			}
			if (startNode != nullptr && endNode != nullptr)
			{
				i = m_Places.size();
			}
		}
		const double dist = CalcDistanceFromTwoPoints(startNode->GetLongitude(), startNode->GetLatitude(),
			endNode->GetLongitude(), endNode->GetLatitude());
		_outFile << commandString << endl;
		_outFile << *startNode->GetName() << "," << *endNode->GetName() << ",";
		_outFile << setprecision(5) << dist / 1000 << endl << endl;

		return true;
	}
	if (command == "FindNeighbour")
	{
		_outFile << commandString << endl;
		for (int i = 0; i < m_Places.size(); i++)
		{

			if (m_Places[i]->GetReferenceNumber() == startNodeID)
			{
				vector<Arc*>& currentNodeArcs = *m_Places[i]->GetArcs();
				for (int j = 0; j < currentNodeArcs.size(); j++)
				{
					Node* const dest = currentNodeArcs[j]->GetDest();
					_outFile << dest->GetReferenceNumber() << endl;
				}
				_outFile << endl;
				return true;
			}
		}
	}
	if (command == "Check")
	{

		_outFile << commandString << endl;
		static vector<int> places;
		places.push_back(place1); places.push_back(place2); places.push_back(place3); places.push_back(place4);
		bool fail = true;
		for (int i = 0; i < m_Places.size(); i++)
		{

			if (m_Places[i]->GetReferenceNumber() == place1)
			{
				Node* startNode = m_Places[i];
				for (int j = 1; j < places.size(); j++)//Loop through all places
				{
					vector<Arc*>* const dest = startNode->GetArcs();
					for (int k = 0; k < (*dest).size(); k++)//Loop through all links of node 
					{
						if ((*dest)[k]->GetDest()->GetReferenceNumber() == places[j] && *(*dest)[k]->GetMode() == mode)
						{

							startNode = (*dest)[k]->GetDest();
							fail = false;
							_outFile << places[j-1] << " " << places[j] << ",PASS" << endl;
							
						}
					}
					if (fail)
					{
						_outFile << startNode->GetReferenceNumber() << "," << places[j] << ",FAIL" << endl << endl;
						return true;
					}
					fail = true;
				}
				_outFile << endl;
				return true;
			}
		}
	}
	if (command == "FindRoute")
	{
		Node* startNode = nullptr;
		Node* endNode = nullptr;
		vector<Node*> currentPath;
		for (int i = 0; i < m_Places.size(); i++)
		{
			if (m_Places[i]->GetReferenceNumber() == endNodeID)
			{
				endNode = m_Places[i];
			}
			if (m_Places[i]->GetReferenceNumber() == startNodeID)
			{
				startNode = m_Places[i];
			}
			if (startNode != nullptr && endNode != nullptr)
			{
				i = m_Places.size();
			}
		}
		currentPath.push_back(endNode);
		vector<Arc*>* dest = endNode->GetArcs();
		Node* currentNode = endNode;
		int currentNodeTempI = 0;
		while (startNode != currentNode)
		{
			if (currentPath.size() == 0)
			{
				_outFile << commandString << endl << "FAIL" << endl << endl;
				return true;
			}
			dest = currentNode->GetArcs();
			for (int i = 0; i < (*dest).size(); i++)
			{
				if (mode == *(*dest)[i]->GetMode())
				{
					currentNode = (*dest)[i]->GetDest();
					bool fails = false;
					for (int j = 0; j < currentPath.size(); j++)
					{
						if (currentNode->GetReferenceNumber() == currentPath[0]->GetReferenceNumber())
						{

						}
						else if (currentNode->GetReferenceNumber() == currentPath[j]->GetReferenceNumber() )
						{
							fails = true;
							j = currentPath.size();
						}
					}
					if (!fails)
					{
						currentPath.push_back(currentNode);
						currentNodeTempI = i;
						i = (*dest).size();
					}
				}
				else if (i == (*dest).size()-1)
				{
					currentPath.erase(currentPath.end() - 1);
					if (currentPath.size() != 0)
					{
						currentNode = currentPath[currentPath.size() - 1];
						i = currentNodeTempI + 1;
					}
				}
			}
		}
		_outFile << commandString << endl;
		for (int i = 0; i < currentPath.size(); i++)
		{
			_outFile << currentPath[i]->GetReferenceNumber() << endl;
		}
		_outFile << endl;
		return true;

	}
	if (command == "FindShortestRoute")
	{
		Node* startNode = nullptr;
		Node* endNode = nullptr;
		vector<Node*> currentPath;
		for (int i = 0; i < m_Places.size(); i++)
		{
			if (m_Places[i]->GetReferenceNumber() == endNodeID)
			{
				endNode = m_Places[i];
			}
			if (m_Places[i]->GetReferenceNumber() == startNodeID)
			{
				startNode = m_Places[i];
			}
			if (startNode != nullptr && endNode != nullptr)
			{
				i = m_Places.size();
			}
		}
		currentPath.push_back(endNode);
		vector<Arc*>* dest = endNode->GetArcs();
		Node* currentNode = endNode;
		int currentNodeTempI = 0;
		while (startNode != currentNode)
		{
			if (currentPath.size() == 0)
			{
				_outFile << commandString << endl << "FAIL" << endl << endl;
				return true;
			}
			dest = currentNode->GetArcs();
			for (int i = 0; i < (*dest).size(); i++)
			{
				if (mode == *(*dest)[i]->GetMode())
				{
					currentNode = (*dest)[i]->GetDest();
					bool fails = false;
					for (int j = 0; j < currentPath.size(); j++)
					{
						if (currentNode->GetReferenceNumber() == currentPath[0]->GetReferenceNumber())
						{

						}
						else if (currentNode->GetReferenceNumber() == currentPath[j]->GetReferenceNumber())
						{
							fails = true;
							j = currentPath.size();
						}
					}
					if (!fails)
					{
						currentPath.push_back(currentNode);
						currentNodeTempI = i;
						i = (*dest).size();
					}
				}
				else if (i == (*dest).size() - 1)
				{
					currentPath.erase(currentPath.end() - 1);
					if (currentPath.size() != 0)
					{
						currentNode = currentPath[currentPath.size() - 1];
						i = currentNodeTempI + 1;
					}
				}
			}
		}
		_outFile << commandString << endl;
		for (int i = 0; i < currentPath.size(); i++)
		{
			_outFile << currentPath[i]->GetReferenceNumber() << endl;
		}
		_outFile << endl;
		return true;
	}
}


double Navigation::CalcDistanceFromTwoPoints(const double originLong, const double originLat, const double destLong, const double destLat) const
{
	double originNorthings, originEastings, destNorthings, destEastings;

	LLtoUTM(originLat, originLong, originNorthings, originEastings);
	LLtoUTM(destLat, destLong, destNorthings, destEastings);
	double dist;
	dist = sqrt((pow((originNorthings - destNorthings), 2) + pow((originEastings - destEastings), 2)));
	return dist;
}

bool Navigation::BuildNetwork(const string &fileNamePlaces, const string &fileNameLinks)
{
	fstream finPlaces(fileNamePlaces);
	fstream finLinks(fileNameLinks);
	if (finPlaces.fail() || finLinks.fail()) return false;

	while (!finPlaces.eof())
	{
		char line[255];
		finPlaces.getline(line, 255, ',');
		string place = string(line);
		finPlaces.getline(line, 255, ',');
		const int refrerenceNumber = stoi(line);
		finPlaces.getline(line, 255, ',');
		const double latitude = atof(line);
		finPlaces.getline(line, 255, '\n');
		const double longitude = atof(line);
		vector<Arc*> arcLinks;
		vector<Arc*>* arcLinksPtr = &arcLinks;
		Node* const newNode = new Node(&place, refrerenceNumber, latitude, longitude, arcLinksPtr);
		m_Places.push_back(newNode);
	}
	for (int i = 0; i < m_Places.size(); i++)
	{
		Node* const currNode = m_Places[i];
		while (!finLinks.eof())
		{
			char linksLine[255];
			finLinks.getline(linksLine, 255, '\n');
#pragma region split
			stringstream ss(linksLine);
			vector<string> splitStringVector;
			while (ss.good())
			{
				string substring;
				getline(ss, substring, ',');
				splitStringVector.push_back(substring);
			}
#pragma endregion

			if (stoi(splitStringVector[0]) == currNode->GetReferenceNumber())
			{
				for (int j = 0; j < m_Places.size(); j++)
				{
					if (m_Places[j]->GetReferenceNumber() == stoi(splitStringVector[1]))
					{
						string mode = string(splitStringVector[2]);
						Arc* const pointToDest = new Arc(m_Places[j], mode);
						Arc* const DestToPoint = new Arc(m_Places[i], mode);
						m_Places[i]->AddArc(pointToDest);
						m_Places[j]->AddArc(DestToPoint);
					}

				}
				
			}
			
		}
		finLinks.clear();
		finLinks.seekg(0, ios::beg);
	}

	return true;
}
