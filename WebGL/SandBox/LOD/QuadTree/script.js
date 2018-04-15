/**
 * Created by Cchiv on 13/04/2018.
 *
 * The below program is a helper tool for LOD algorithm,
 * used to fetch heightmap data, parse it and store it into
 * QuadTree data structure used for easy access to its leafs
 * that represents different complexities level for the targeted
 * terrain within the the visible area not clipped by the frustum.
 */

/**
 * Containing terrain 2D Box that spans from [x0, x1] to [y0, y1], inclusive
 * where x and y corresponds to cols and rows respectively.
 * @param x0 Minimal dim value on X-axis
 * @param y0 Minimal dim value on Y-axis
 * @param x1 Maximum dim value on X-axis
 * @param y1 Maximum dim value on Y-axis
 * @constructor
 */
function Box(x0, y0, x1, y1) {
    this.x0 = x0;
    this.y0 = y0;
    this.x1 = x1;
    this.y1 = y1;
}

Box.prototype.checkPartition = function() {
    return (this.x1-this.x0 >= 1 && this.y1-this.y0 >= 1);
};

/**
 * Project coordinates to the origin and returns 4 terrain squares that all 4 have
 * approximate same dimensions with their coordinates projected back with both left
 * squares having more or equal dimensions then the two right due do division by 2
 * being uneven in most of cases.
 * @returns {Array}
 */
Box.prototype.getPartitions = function() {
    let diffX = this.x1-this.x0;
    let diffY = this.y1-this.y0;

    let partX = Math.ceil(diffX/2.0);
    let partY = Math.ceil(diffY/2.0);

    let xMax, yMax;
    let boxes = [];
    for(let g = 0; g < 2; g++) {
        for(let h = 0; h < 2; h++) {
            xMax = Math.min(this.x0+partX*(h+1), this.x1);
            yMax = Math.min(this.y0+partY*(g+1), this.y1);
            boxes.push(new Box(this.x0+partX*h, this.y0+partY*g, xMax, yMax));
        }
    }

    return boxes;
};

/**
 * Giving the box calculate vertices coordinates in the heightmap mesh to be fetched
 * latter and to be stored into leafs.
 * @param tCols
 * @returns {*[]}
 */
Box.prototype.getVerticesCoord = function(tCols) {
    return [
        this.y0*tCols + this.x0,
        this.y1*tCols + this.x0,
        this.y1*tCols + this.x1,
        this.y1*tCols + this.x1,
        this.y0*tCols + this.x1,
        this.y0*tCols + this.x0,
    ];
};

/**
 * QuadTree constructor, each leaf has higher level complexity up to 4x times.
 * Depth is the max depth where box's checkPartition is true for deepest Breadth-first search for all leafs.
 * Mesh is stored too with cols and rows to fetch triangles and manage box boundaries.
 * Data var used for fetching triangles to be used for rendering.
 * @constructor
 */
function QuadTree(mesh) {
    this.tree = [];
    this.data = [];

    this.mesh = mesh;
    this.cols = Math.sqrt(this.mesh.length);
    this.rows = this.cols;

    this.depth =  Math.floor(Math.log2(this.cols))+1;

    this.fillTree(this.tree, this.depth, new Box(0, 0, this.rows-1, this.cols-1));
}

/**
 * We stop at the corresponding depth or the currentBox that doesn't have enough data to construct a square.
 * Get the vertices to the corresponding box and put them into the node.
 * Generate 4 approximately equal size boxes and pass them recursively.
 * Pass mutable object node["children"] and change it at the next recursive iteration.
 * @param currentNode
 * @param depth
 * @param currentBox
 */
QuadTree.prototype.fillTree = function(currentNode, depth, currentBox) {
    if(depth === 0 || !currentBox.checkPartition())
        return;

    let node = {
        "vertices" : [],
        "children" : []
    };

    currentNode.push(node);

    currentBox.getVerticesCoord(this.cols).forEach((coord) => {
        for(let i = 0; i < 3; i++)
            node["vertices"].push(this.mesh[coord][i]);
    });

    currentBox.getPartitions().forEach((box) => {
        this.fillTree(node["children"], depth-1, box);
    });
};

/**
 * Check if a given vector coord is given is within the X-axis [-1, 1],
 * Y-axis [-1, 1], Z-axis (0, -1] range given that perspective matrix is
 * used for coordinates projection.
 * @param vector
 * @param perspective
 * @returns {boolean}
 */
QuadTree.prototype.checkIfInFrustum = function(vector, perspective) {
    let result = multiplyVector(perspective, vector);

    for (let g = 0; g < 3; g++)
        result[g] *= result[3];

    return (result[0] >= -1 && result[0] <= 1 &&
        result[1] >= -1 && result[1] <= 1 &&
        result[2] < 0);
};

/**
 * Initialize the currentNode recursively with all 4 root's children.
 * Clear the data var to be filled with new data.
 * Returns the data to the WebGL program.
 * @param gl
 * @param depth
 * @param currentNode
 * @returns {Array}
 */
QuadTree.prototype.readDepth = function(gl, depth, currentNode = this.tree[0].children) {
    this.data.length = 0;
    for(let g = 0; g < 4; g++)
        this.readLevel(gl, depth-1, currentNode[g]);

    return this.data;
};

/**
 * If the depth is not the one we are targeting then dive even more.
 * If yes, then we fetch the data from leafs and append them to the data var container.
 * @param gl
 * @param depth
 * @param currentNode
 */
QuadTree.prototype.readLevel = function(gl, depth, currentNode) {
    if(depth === 0 || currentNode.children.length !== 4) {
        currentNode.vertices.forEach((val) => {
            this.data.push(val);
        });
    } else {
        for(let g = 0; g < 4; g++)
            this.readLevel(gl, depth-1, currentNode.children[g]);
    }
};

/**
 * LOD constructor which will be built latter to support features
 * like mapping mesh to a sphere, use of webworkers to fetch mesh data
 * from multiple files for a bigger world to be able to render...
 *
 * Callback render is used for calling the WebGL rendering fn when data is
 * fetched properly.
 * @param render
 * @param gl
 * @constructor
 */
function LOD(render, gl) {
    this.tree = null;
    this.gl = gl;
    this.render = render;
    this.fetchData();
}

/**
 * Fetch data from json file, and initialize the QuadTree constructor with
 * the data fetched.
 */
LOD.prototype.fetchData = function() {
    fetch("./../QuadTree/pixels.json").then((data) => data.json())
    .then((mesh) => {
        this.tree = new QuadTree(mesh);
        this.render(this.gl);
    });
};