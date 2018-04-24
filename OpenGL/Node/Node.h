#ifndef NODE
#define NODE

class Node {
public:
	Node ** children;
	vector<float> vertices;
	vector<float> lines;
	Box box;

	Node();

	~Node();

	Node(Box box);
};

#endif