#include "Arc.h"
#include "Node.h"


Node::~Node()
{
}

const int Node::GetReferenceNumber() const { return referenceNumber; }

const string * Node::GetName() { return &name; }

const double Node::GetLatitude() const { return latitude; }

const double Node::GetLongitude() const { return longitude; }

void Node::AddArc(Arc* const arcPtr) { m_Arcs.push_back(arcPtr); }

vector<Arc*>* Node::GetArcs() { return &m_Arcs; }



Node::Node(string* const place, const int referenceNum, const double lat, const double longitudeVal, vector<Arc*>* const arcLinks)
{
	name = *place;
	referenceNumber = referenceNum;
	latitude = lat;
	longitude = longitudeVal;
	m_Arcs = *arcLinks;

}

