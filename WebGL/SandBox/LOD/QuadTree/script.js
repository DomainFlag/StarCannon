const cycle = [
    [0, 0],
    [0, 1],
    [1, 0],
    [1, 0],
    [1, 1],
    [0, 1]
];

function QuadTree() {
    this.depth = 8;
    this.mesh = null;

    this.tree = [];
}

QuadTree.prototype.initiate = function() {
    this.fillTree(this.tree, this.mesh, this.depth);
};

QuadTree.prototype.fillTree = function(currentNode, mesh, depth) {
    if(depth === 0)
        return;


    let partition = Math.sqrt(mesh.length/3);
    let quart = Math.floor(partition/2);

    for(let g = 0; g < 2; g++) {
        for(let h = 0; h < 2; h++) {
            let data = [];

            let minC = (g+1)*quart;
            let minR = (h+1)*quart;

            for(let j = g*quart; j < minC; j++) {
                for(let k = h*quart; k < minR; k++) {
                    for(let l = 0; l < 3; l++) {
                        data.push(mesh[(j*partition+k)*3+l]);
                    }
                }
            }

            let vertices = [];
            cycle.forEach((set) => {
                let offset = ( (g+set[0])*quart*partition+(h+set[1])*quart )*3;
                for(let i = 0; i < 3; i++)
                    vertices.push(
                        mesh[offset+i]
                    );
            });

            let node = {
                "vertices" : vertices,
                "children" : []
            };

            currentNode.push(node);

            this.fillTree(node["children"], data, depth-1);
        }
    }
};

QuadTree.prototype.checkIfInFrustum = function(vector, perspective) {
    let result = multiplyVector(perspective, vector);

    for (let g = 0; g < 3; g++)
        result[g] *= result[3];

    return (result[0] >= -1 && result[0] <= 1 &&
        result[1] >= -1 && result[1] <= 1 &&
        result[2] < 0);
};

QuadTree.prototype.initReadLevel = function(gl, depth, currentNode = this.tree) {
    this.data = [];
    for(let g = 0; g < 4; g++)
        this.readLevel(gl, depth-1, currentNode[g]);
    return this.data;
};

QuadTree.prototype.readLevel = function(gl, depth, currentNode) {
    if(depth === 0) {
        currentNode.vertices.forEach((val) => {
            this.data.push(val);
        });
    } else {
        for(let g = 0; g < 4; g++)
            this.readLevel(gl, depth-1, currentNode.children[g]);
    }
};

function LOD(fn, gl) {
    this.tree = new QuadTree();
    this.gl = gl;
    this.fn = fn;
    this.fetchData();
}

LOD.prototype.fetchData = function() {
    fetch("./../QuadTree/pixels.json").then((data) => data.json())
    .then((data) => {
        this.tree.mesh = data;

        this.tree.initiate();
        this.fn(this.gl);
    });
};
