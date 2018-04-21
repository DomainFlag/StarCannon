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
            xMax = Math.min(this.x0+partX*(g+1), this.x1);
            yMax = Math.min(this.y0+partY*(h+1), this.y1);
            boxes.push(new Box(this.x0+partX*g, this.y0+partY*h, xMax, yMax));
        }
    }

    return boxes;
};

Box.prototype.checkBoundary  = function(tCols) {
    return this.x0 >= 0 && this.x1 <= tCols-1 && this.y0 >= 0 && this.y1 <= tCols-1;
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

Box.prototype.getLinesCoord = function(tCols, index) {
    if(index === 0 || index === 3)
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
    else if(index === 1 || index === 2)
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
        return [
            new Box(this.x0, Math.max(this.y0-diffY*2, 0), this.x1+diffX, this.y0),
            new Box(Math.max(this.x0-diffX*2, 0), this.y0, this.x0, this.y1+diffY)
        ];
    else if(index === 1)
        return [
            new Box(Math.max(this.x0-diffX*2, 0), Math.min(this.y0-diffY, 0), this.x0, this.y1),
            new Box(this.x0, this.y1, Math.min(this.x1+diffX, tCols-1), Math.min(this.y1+diffY*2, tCols-1))
        ];
    else if(index === 2)
        return [
            new Box(Math.max(this.x0-diffX, 0), Math.max(this.y0-diffY*2, 0), this.x1, this.y0),
            new Box(this.x1, this.y0, Math.min(this.x1+diffX*2, tCols-1), Math.min(this.y1+diffY, tCols-1))
        ];
    else {
        return [
            new Box(Math.max(this.x0-diffX, 0), this.y1, this.x1, Math.min(this.y1+diffY*2, tCols)),
            new Box(this.x1, Math.max(this.y0-diffY, 0), Math.min(this.x1+diffX*2, tCols), this.y1)
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
 */
QuadTree.prototype.fillTree = function(currentNode, depth, currentBox, index) {
    if(depth === 0 || !currentBox.checkPartition())
        return;

    let node = {
        "vertices" : [],
        "lines" : [],
        "children" : [],
        "box" : currentBox
    };

    currentNode.push(node);

    currentBox.getVerticesCoord(this.cols).forEach((coord) => {
        this.getPlaneVertices(node.vertices, coord);
    });

    currentBox.getLinesCoord(this.cols, index).forEach((coord) => {
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
    let distance = -10;
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

        if(v[0] >= -1.0 && v[0] <= 1.0 &&
            v[1] >= -1.0 && v[1] <= 1.0 &&
            v[2] >= -1.0 && v[2] <= 1.0)
            withinFrustum = true;
    }

    return {
        "withinFrustum" : withinFrustum,
        "minZ" : Math.max(Math.abs(distance), 0.01)/10
    };
};

QuadTree.prototype.readProjection = function(projection, viewCamera, currentNode = this.tree[0].children) {
    this.data.length = 0;
    for(let g = 0; g < 4; g++)
        this.readComplexity(projection, viewCamera, currentNode[g], g);

    return this.data;
};

QuadTree.prototype.testNeighbours = function(adjacentBoxes, projection, viewCamera, depth) {
    return adjacentBoxes
        .filter((neighbour) => neighbour.checkBoundary(this.cols))
        .some((neighbour) => {
            let vertices = [];
            neighbour.getVerticesCoord(this.cols).forEach((coord) => {
                for(let g = 0; g < 3; g++) {
                    vertices.push(this.mesh[coord][g]);
                }
            });

            let frustumBoundaries = this.checkFrustumBoundaries(vertices, projection, viewCamera);
            if(frustumBoundaries.withinFrustum) {

                let neighbourDepth = this.depth-Math.ceil(frustumBoundaries.minZ/this.section);

                return neighbourDepth < depth;
            }
        });
};

QuadTree.prototype.fetchLines = function(projection, viewCamera, currentNode, depth, index) {
    let box = currentNode.box;
    let adjacentBoxes = box.getAdjacentBoxes(this.cols, index);

    if(!this.testNeighbours(adjacentBoxes, projection, viewCamera, depth))
        currentNode.lines.forEach((val) => {
            this.data.push(val);
        });
};

QuadTree.prototype.readComplexity = function(projection, viewCamera, currentNode, index, currentDepth = 1) {
    let frustumBoundaries = this.checkFrustumBoundaries(currentNode.vertices, projection, viewCamera);

    if(frustumBoundaries.withinFrustum) {
        let depth = this.depth-Math.ceil(frustumBoundaries.minZ/this.section);

        if(depth <= currentDepth) {
            //Even complexity, or the others quarters inherits the parent previous complexity
            this.fetchLines(projection, viewCamera, currentNode, currentDepth, index);
        } else if(depth > currentDepth) {
            //Needs more complexity
            currentNode.children.forEach((child, index) => {
                this.readComplexity(projection, viewCamera, child, index, currentDepth+1);
            });
        }
    }
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