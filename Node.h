#pragma once

#include <string>
#include <vector>
using namespace std;

class Arc;

class Node
{
public:
	~Node();
	const int GetReferenceNumber() const;
	const string* GetName();
	const double GetLatitude() const ;
	const double GetLongitude() const;
	void AddArc(Arc* const arcPtr);

	vector<Arc*>* GetArcs();

	Node(string * const place, const int referenceNum, const double lat, const double longitudeVal, vector<Arc*>* const arcLinks);

private:
	vector<Arc*> m_Arcs;
	string name;
	int referenceNumber;
	double latitude;
	double longitude;
};

