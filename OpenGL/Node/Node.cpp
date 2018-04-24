Node::Node() {
	this->children = new Node *[4];
	for(int g = 0; g < 4; g++)
		this->children[g] = NULL;
};

Node::Node(Box box) {
	this->children = new Node *[4];
	for(int g = 0; g < 4; g++)
		this->children[g] = NULL;
	this->box = box;
}

Node::~Node() {
	for(int g = 0; g < 4; g++)
		if(this->children[g] != NULL)
			delete this->children[g];

	delete [] this->children;
};