#include "Arc.h"
#include "Node.h""

Arc::~Arc()
{
}

Arc::Arc(Node* const dest, string const& modeOfT)
{
	m_destination = dest;
	mode = modeOfT;
}

Node * Arc::GetDest() const
{
	return m_destination;
}

string * Arc::GetMode()
{
	return &mode;
}