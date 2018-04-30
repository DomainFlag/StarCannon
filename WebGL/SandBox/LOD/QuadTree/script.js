/**
 * Created by Cchiv on 13/04/2018.
 *
 * The below program is a helper tool for LOD algorithm,
 * used to fetch heightmap data, parse it and store it into
 * QuadTree data structure used for easy access to its leafs
 * that represents different complexities level for the targeted
 * terrain within the the visible area not clipped by the frustum.
 */

const boxCycle = [
    [0, 0],
    [0, 1],
    [1, 1],
    [1, 0]
];

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
    boxCycle.forEach((cycle) => {
        xMax = Math.min(this.x0+partX*(cycle[1]+1), this.x1);
        yMax = Math.min(this.y0+partY*(cycle[0]+1), this.y1);
        boxes.push(new Box(this.x0+partX*cycle[1], this.y0+partY*cycle[0], xMax, yMax));
    });

    return boxes;
};

Box.prototype.checkBoundary  = function(tCols) {
    return this.x0 >= 0 && this.x1 <= tCols-1 && this.y0 >= 0 && this.y1 <= tCols-1;
};

/**
 * Giving the box calculate vertices coordinates in the heightmap mesh to be fetched
 * latter and to be stored into leafs.
 * @param tCols
 * @param index
 * @returns {*[]}
 */
Box.prototype.getVerticesCoord = function(tCols, index) {
    if(index === 0)
    //Top-Left
        return [
            this.y0*tCols + this.x0,
            this.y1*tCols + this.x0,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y0*tCols + this.x1,
            this.y0*tCols + this.x0
        ];
    else if(index === 1)
    //Top-Right
        return [
            this.y0*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x0,
            this.y1*tCols + this.x0,
            this.y0*tCols + this.x1,
            this.y0*tCols + this.x0
        ];
    else if(index === 2)
    //Bottom-Right
        return [
            this.y0*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y0*tCols + this.x0,
            this.y0*tCols + this.x0,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x0
        ];
    else if(index === 3)
    //Bottom-Left
        return [
            this.y0*tCols + this.x0,
            this.y1*tCols + this.x0,
            this.y0*tCols + this.x1,
            this.y0*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x0
        ];
};

Box.prototype.getLinesCoord = function(tCols, index) {
    if(index === 0 || index === 2)
    //Top-Left && Bottom-Right
        return [
            this.y0*tCols + this.x0,
            this.y1*tCols + this.x0,
            this.y1*tCols + this.x0,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y0*tCols + this.x0,
            this.y0*tCols + this.x0,
            this.y0*tCols + this.x1,
            this.y0*tCols + this.x1,
            this.y1*tCols + this.x1
        ];
    else if(index === 1 || index === 3)
    //Top-Right && Bottom-Left
        return [
            this.y0*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x1,
            this.y1*tCols + this.x0,
            this.y1*tCols + this.x0,
            this.y0*tCols + this.x1,
            this.y0*tCols + this.x1,
            this.y0*tCols + this.x0,
            this.y0*tCols + this.x0,
            this.y1*tCols + this.x0
        ];
};

Box.prototype.getAdjacentBoxes = function(tCols, index) {
    let diffX = this.x1-this.x0;
    let diffY = this.y1-this.y0;

    if(index === 0)
        //Top-Left
        return [
            new Box(Math.max(this.x0-diffX*2, 0), this.y0, this.x0, Math.min(this.y1+diffY, tCols-1)),
            new Box(this.x0, Math.max(this.y0-diffY*2, 0), Math.min(this.x1+diffX, tCols-1), this.y0)
        ];
    else if(index === 1)
        //Top-Right
        return [
            new Box(Math.max(this.x0-diffX, 0), Math.max(this.y0-diffY*2, 0), this.x1, this.y0),
            new Box(this.x1, this.y0, Math.min(this.x1+diffX*2, tCols-1), Math.min(this.y1+diffY, tCols-1))
        ];
    else if(index === 2)
        //Bottom-Right
        return [
            new Box(this.x1, Math.max(this.y0-diffY, 0), Math.min(this.x1+diffX*2, tCols-1), this.y1),
            new Box(Math.max(this.x0-diffX, 0), this.y1, this.x1, Math.min(this.y1+diffY*2, tCols-1))
        ];
    else if(index === 3) {
        //Bottom-Left
        return [
            new Box(this.x0, this.y1, Math.min(this.x1+diffX, tCols-1), Math.min(this.y1+diffY*2, tCols-1)),
            new Box(Math.max(this.x0-diffX*2, 0), Math.max(this.y0-diffY, 0), this.x0, this.y1)
        ];
    }
};

QuadTree.prototype.getPlaneVertices = function(node, coord) {
    node.push(this.mesh[coord][0]);
    node.push(this.mesh[coord][1]);
    node.push(this.mesh[coord][2]);
};

QuadTree.prototype.getSphereVertices = function(node, coord) {
    let x = this.mesh[coord][0];
    let y = this.mesh[coord][1];
    let z = this.mesh[coord][2];

    let r = Math.abs(y+0.5)/2.0+1.0;

    let sinTheta = Math.sin((x+1.0)/2.0*2*Math.PI);
    let cosTheta = Math.cos((x+1.0)/2.0*2*Math.PI);


    let sinPhi = Math.sin((z+1.0)/2.0*2*Math.PI);
    let cosPhi = Math.cos((z+1.0)/2.0*2*Math.PI);

    node["vertices"].push(
        cosPhi * sinTheta * r,
        cosTheta * r,
        sinPhi * sinTheta * r
    );
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
    this.stack = [];

    this.mesh = mesh;
    this.cols = Math.sqrt(this.mesh.length);
    this.rows = this.cols;

    this.depth =  Math.floor(Math.log2(this.cols))-1;
    this.section = 1.0/this.depth;

    this.fillTree(this.tree, this.depth, new Box(0, 0, this.rows-1, this.cols-1), 0);
}

/**
 * We stop at the corresponding depth or the currentBox that doesn't have enough data to construct a square.
 * Get the vertices to the corresponding box and put them into the node.
 * Generate 4 approximately equal size boxes and pass them recursively.
 * Pass mutable object node["children"] and change it at the next recursive iteration.
 * @param currentNode
 * @param depth
 * @param currentBox
 * @param currentIndex
 */
QuadTree.prototype.fillTree = function(currentNode, depth, currentBox, currentIndex) {
    if(depth === 0 || !currentBox.checkPartition())
        return;

    let node = {
        "vertices" : [],
        "lines" : [],
        "children" : [],
        "box" : currentBox
    };

    currentNode.push(node);

    currentBox.getVerticesCoord(this.cols, currentIndex).forEach((coord) => {
        this.getPlaneVertices(node.vertices, coord);
    });

    currentBox.getLinesCoord(this.cols, currentIndex).forEach((coord) => {
        this.getPlaneVertices(node.lines, coord);
    });

    currentBox.getPartitions().forEach((box, index) => {
        this.fillTree(node["children"], depth-1, box, index);
    });
};

/**
 * Initialize the currentNode recursively with all 4 root's children.
 * Clear the data var to be filled with new data.
 * Returns the data to the WebGL program.
 * @param depth
 * @param currentNode
 * @returns {Array}
 */
QuadTree.prototype.readDepth = function(depth, currentNode = this.tree[0].children) {
    this.data.length = 0;
    for(let g = 0; g < 4; g++)
        this.readLevel(depth-1, currentNode[g]);

    return this.data;
};

/**
 * If the depth is not the one we are targeting then dive even more.
 * If yes, then we fetch the data from leafs and append them to the data var container.
 * @param depth
 * @param currentNode
 */
QuadTree.prototype.readLevel = function(depth, currentNode) {
    if(depth === 0 || currentNode.children.length !== 4) {
        currentNode.vertices.forEach((val) => {
            this.data.push(val);
        });
    } else {
        for(let g = 0; g < 4; g++)
            this.readLevel(depth-1, currentNode.children[g]);
    }
};

/**
 * Check if a given vector coord is given is within the X-axis [-1, 1],
 * Y-axis [-1, 1], Z-axis (0, -1] range given that perspective matrix is
 * used for coordinates projection.
 * Fetch the minimal distance between the mesh and the camera position to
 * compute it's complexity zone to be given to.
 * @param vertices
 * @param projection
 * @param viewCamera
 * @returns {{withinFrustum: boolean, distanceRange: number}}
 */
QuadTree.prototype.checkFrustumBoundaries = function(vertices, projection, viewCamera) {
    let distance = -15;
    let withinFrustum = false;

    for(let g = 0; g < vertices.length; g += 3) {

        let viewVector = multiplyVector(viewCamera, [
                vertices[g],
                vertices[g+1],
                vertices[g+2],
                1.0
            ]
        );

        let v = multiplyVector(projection, viewVector);

        for(let h = 0; h < 3; h++)
            v[h] /= v[3];

        distance = Math.max(distance, -distanceVecs([0, 0, 0], [viewVector[0], viewVector[1], viewVector[2]]));

        if(-1.0 <= v[0] && v[0] <= 1.0 &&
            -1.0 <= v[1] && v[1] <= 1.0)
            withinFrustum = true;
    }

    return {
        "withinFrustum" : withinFrustum,
        "minDistance" : Math.max(Math.abs(distance), 0.01)/15
    };
};

QuadTree.prototype.readProjection = function(projection, viewCamera, currentNode = this.tree[0].children) {
    this.data.length = 0;

    for(let g = 0; g < 4; g++)
        this.readComplexity(projection, viewCamera, currentNode[g], 1);

    return this.data;
};

QuadTree.prototype.getNeighbours = function(adjacentBoxes, projection, viewCamera, depth) {
    let adjacentLODsHigher = adjacentBoxes.map((adjacentBox, index) => ({
        "adjacentBox" : adjacentBox,
        "index" : index
    }));

    return adjacentLODsHigher
        .filter((neighbour) => neighbour.adjacentBox.checkBoundary(this.cols))
        .filter((neighbour) => {
            let vertices = [];
            //Index 0 because we don't care about the triangle's vertices order
            neighbour.adjacentBox.getVerticesCoord(this.cols, 0).forEach((coord) => {
                for(let g = 0; g < 3; g++) {
                    vertices.push(this.mesh[coord][g]);
                }
            });

            let frustumBoundaries = this.checkFrustumBoundaries(vertices, projection, viewCamera);
            if(frustumBoundaries.withinFrustum) {
                let neighbourDepth = this.depth-Math.ceil(frustumBoundaries.minDistance/this.section);

                return neighbourDepth < depth;
            }
        });
};

QuadTree.prototype.fetchLines = function(projection, viewCamera, currentNode, depth, index) {
    let box = currentNode.box;
    let adjacentBoxes = box.getAdjacentBoxes(this.cols, index);

    let adjacentLODsHigher = this.getNeighbours(adjacentBoxes, projection, viewCamera, depth);

    // if(adjacentLODsHigher.length === 0)
        currentNode.vertices.forEach((val) => {
            this.data.push(val);
        });
};

QuadTree.prototype.readComplexity = function(projection, viewCamera, currentNode, currentIndex, currentDepth = 1) {
    let childrenToBeRendered = [];
    let minDepth = this.depth;

    currentNode.children.forEach((child, index) => {
        let frustumBoundaries = this.checkFrustumBoundaries(child.vertices, projection, viewCamera);

        if(frustumBoundaries.withinFrustum) {
            //Section is the 1.0/this.depth
            let depth = this.depth-Math.ceil(frustumBoundaries.minDistance/this.section);

            minDepth = Math.min(minDepth, depth);

            childrenToBeRendered.push({
                "child" : child,
                "index" : index,
                "depth" : depth
            });
        }
    });

    childrenToBeRendered.forEach((child => {
        if(minDepth <= currentDepth) {
            //Even complexity, or the others quarters inherits the parent complexity
            this.fetchLines(projection, viewCamera, child.child, currentDepth, child.index);
        } else if(minDepth > currentDepth) {
            //Needs more complexity because it's too near to camera origins
            this.readComplexity(projection, viewCamera, child.child, child.index, currentDepth+1);
        }
    }));
};

/**
 * LOD constructor which will be built latter to support features
 * like mapping mesh to a sphere, use of web workers to fetch mesh data
 * in background from multiple files for a bigger world to be able to render...
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
    this.primitive = gl.LINES;
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