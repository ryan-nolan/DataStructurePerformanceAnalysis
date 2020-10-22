#pragma once

#include <string>
using namespace std;

class Node;

class Arc
{
public:
	Arc(const Arc &old_obj);
	Arc& operator = (const Arc &a);
	~Arc();
	Arc(Node * const dest, string const & modeOfT);
	Node * GetDest() const;
	string * GetMode();

private:
	Node* m_destination;
	string mode;
};

