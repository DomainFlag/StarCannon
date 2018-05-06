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

const vertexCycle = [0, 1, 2, 4];

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

    let partX = diffX/2.0;
    let partY = diffY/2.0;

    let boxes = [];
    boxCycle.forEach((cycle) => {
        boxes.push(new Box(this.x0+partX*cycle[1], this.y0+partY*cycle[0], this.x0+partX*(cycle[1]+1), this.y0+partY*(cycle[0]+1)));
    });

    return boxes;
};

Box.prototype.checkBoundary = function(tCols, tRows) {
    return this.x0 >= 1 && this.x1 <= tCols && this.y0 >= 1 && this.y1 <= tRows;
};

Box.prototype.withinBox = function(posX, posY) {
    return (this.x0 <= posX && this.x1 >= posX) &&
        (this.y0 <= posY && this.y1 >= posY);
};

Box.prototype.getNextOrientation = function(center, posX, posY) {
    if(posX < this.x0) {
        if(posY > this.y1)
            return new Tile(center.x-1, center.y-1);
        else if(posY < this.y1)
            return new Tile(center.x-1, center.y+1);
        else
            return new Tile(center.x-1, center.y);
    } else if(posX > this.x1) {
        if(posY > this.y1)
            return new Tile(center.x+1, center.y-1);
        else if(posY < this.y1)
            return new Tile(center.x+1, center.y+1);
        else
            return new Tile(center.x+1, center.y);
    } else {
        if(posY > this.y1)
            return new Tile(center.x, center.y-1);
        else if(posY < this.y1)
            return new Tile(center.x, center.y+1);
        else
            return new Tile(center.x, center.y);
    }
};

/**
 * Giving the box calculate vertices coordinates in the heightmap mesh to be fetched
 * latter and to be stored into leafs.
 * @param tCols
 * @returns {{}}
 */
Box.prototype.getVerticesCoord = function(tCols) {
    return [
        this.y0*tCols + this.x0,
        this.y1*tCols + this.x0,
        this.y1*tCols + this.x1,
        this.y1*tCols + this.x1,
        this.y0*tCols + this.x1,
        this.y0*tCols + this.x0
    ];
};

Box.prototype.getLinesCoord = function(tCols) {
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
};

Box.prototype.getAdjacentBoxes = function(tCols, index) {
    let diffX = this.x1-this.x0;
    let diffY = this.y1-this.y0;

    if(index === 0)
        //Top-Left
        return [
            {
                "box" : new Box(this.x0-diffX*2, this.y0, this.x0, this.y1+diffY),
                "index" : 0
            }, {
                "box" : new Box(this.x0, this.y0-diffY*2, this.x1+diffX, this.y0),
                "index" : 1
            }
        ];
    else if(index === 1)
        //Top-Right
        return [
            {
                "box" : new Box(this.x0-diffX, this.y0-diffY*2, this.x1, this.y0),
                "index" : 0
            }, {
                "box" : new Box(this.x1, this.y0, this.x1+diffX*2, this.y1+diffY),
                "index" : 1
            }
        ];
    else if(index === 2)
        //Bottom-Right
        return [
            {
                "box" : new Box(this.x1, this.y0-diffY, this.x1+diffX*2, this.y1),
                "index" : 0
            }, {
                "box" : new Box(this.x0-diffX, this.y1, this.x1, this.y1+diffY*2),
                "index" : 1
            }
        ];
    else {
        //Bottom-Left
        return [
            {
                "box" : new Box(this.x0, this.y1, this.x1+diffX, this.y1+diffY*2),
                "index" : 0
            }, {
                "box" : new Box(this.x0-diffX*2, this.y0-diffY, this.x0, this.y1),
                "index" : 1
            }
        ];
    }
};

Box.prototype.interpolateHeights = function(tCols, index, aindex) {
    if(index === 0) {
        if(aindex === 0) {
            return [
                this.y0*tCols + this.x1,
                this.y1*tCols + this.x1
            ];
        } else {
            return [
                this.y1*tCols + this.x0,
                this.y1*tCols + this.x1
            ];
        }
    } else if(index === 1) {
        if(aindex === 0) {
            return [
                this.y1*tCols + this.x0,
                this.y1*tCols + this.x1
            ];
        } else {
            return [
                this.y0*tCols + this.x0,
                this.y1*tCols + this.x0
            ];
        }
    } else if(index === 2) {
        if(aindex === 0) {
            return [
                this.y0*tCols + this.x0,
                this.y1*tCols + this.x0
            ];
        } else {
            return [
                this.y0*tCols + this.x0,
                this.y0*tCols + this.x1
            ];
        }
    } else {
    if(aindex === 0) {
        return [
            this.y0*tCols + this.x0,
            this.y0*tCols + this.x1
        ];
    } else {
        return [
            this.y0*tCols + this.x1,
            this.y1*tCols + this.x1
        ];
    }
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
function QuadTree(mesh, tile, offsetX, offsetY, size) {
    this.tree = [];

    this.tile = tile;
    this.dimen = new Box(-size+offsetX, -size+offsetY, size+offsetX, size+offsetY);

    this.mesh = mesh;
    this.cols = Math.sqrt(mesh.length);
    this.rows = this.cols;

    this.depth = Math.floor(Math.log2(this.rows))-1;
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
QuadTree.prototype.fillTree = function(currentNode, depth, currentBox) {
    if(depth === 0 || !currentBox.checkPartition())
        return;

    let node = {
        "vertices" : [],
        "lines" : [],
        "children" : [],
        "box" : currentBox
    };

    currentNode.push(node);

    currentBox.getVerticesCoord(this.rows).forEach((coord) => {
        this.getPlaneVertices(node.vertices, coord);
    });

    currentBox.getLinesCoord(this.rows).forEach((coord) => {
        this.getPlaneVertices(node.lines, coord);
    });

    currentBox.getPartitions().forEach((box, index) => {
        this.fillTree(node["children"], depth-1, box, index);
    });
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
 * @param flag
 */
QuadTree.prototype.checkFrustumBoundaries = function(vertices, projection, viewCamera, flag) {
    let distance = 10;
    let withinFrustum = false;

    vertexCycle.forEach((cycle) => {
        let g = cycle*3;

        let viewVector = multiplyVector(viewCamera, [
                vertices[g],
                vertices[g+1],
                vertices[g+2],
                1.0
            ]
        );

        distance = Math.min(distance, distanceVecs([0, 0, 0], [viewVector[0], viewVector[1], viewVector[2]]));

        if(!flag) {
            let v = multiplyVector(projection, viewVector);

            for(let h = 0; h < 3; h++)
                v[h] /= v[3];

            if((-1.0 <= v[0] && v[0] <= 1.0) ||
                (-1.0 <= v[1] && v[1] <= 1.0))
                withinFrustum = true;
        }
    });

    if(flag)
        return  Math.max(distance, 0.01)/10;
    else
        return {
            "withinFrustum" : withinFrustum,
            "minDistance" : Math.max(distance, 0.01)/10
        };
};

QuadTree.prototype.getNeighbours = function(adjacentBoxes, projection, viewCamera, depth) {
    return adjacentBoxes
        .filter((neighbour) => neighbour.box.checkBoundary(this.cols, this.rows))
        .filter((neighbour) => {
            let vertices = [];

            neighbour.box.getVerticesCoord(this.rows).forEach((coord) => {
                this.getPlaneVertices(vertices, coord);
            });

            let minDistance = this.checkFrustumBoundaries(vertices, projection, viewCamera, true);
            let neighbourDepth = this.depth-Math.ceil(minDistance/this.section);

            return neighbourDepth < depth;
        });
};

QuadTree.prototype.getInterpolatedHeights = function(vals) {
    return (this.mesh[vals[0]][1]+this.mesh[vals[1]][1])/2.0;
};

QuadTree.prototype.fetchVertices = function(data, projection, viewCamera, currentNode, depth, index) {
    let adjacentBoxes = currentNode.box.getAdjacentBoxes(this.rows, index);

    let adjacentLODsHigher = this.getNeighbours(adjacentBoxes, projection, viewCamera, depth);

    currentNode.vertices.forEach((val) => {
        data.push(val);
    });

    if(adjacentLODsHigher.length !== 0) {
        if(index === 0) {
            if(adjacentLODsHigher.length === 2) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                let lerp2 = adjacentLODsHigher[1].box.interpolateHeights(this.rows, index, 1);
                data[data.length-5*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-2*3+1] = this.getInterpolatedHeights(lerp2);
            } else if(adjacentLODsHigher[0].index === 0) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                data[data.length-5*3+1] = this.getInterpolatedHeights(lerp1);
            } else {
                let lerp2 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 1);
                data[data.length-2*3+1] = this.getInterpolatedHeights(lerp2);
            }
        } else if(index === 1) {
            if(adjacentLODsHigher.length === 2) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                let lerp2 = adjacentLODsHigher[1].box.interpolateHeights(this.rows, index, 1);
                data[data.length-6*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-1*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-4*3+1] = this.getInterpolatedHeights(lerp2);
                data[data.length-3*3+1] = this.getInterpolatedHeights(lerp2);
            } else if(adjacentLODsHigher[0].index === 0) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                data[data.length-6*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-1*3+1] = this.getInterpolatedHeights(lerp1);
            } else {
                let lerp2 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 1);
                data[data.length-4*3+1] = this.getInterpolatedHeights(lerp2);
                data[data.length-3*3+1] = this.getInterpolatedHeights(lerp2);
            }
        } else if(index === 2) {
            if(adjacentLODsHigher.length === 2) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                let lerp2 = adjacentLODsHigher[1].box.interpolateHeights(this.rows, index, 1);
                data[data.length-2*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-5*3+1] = this.getInterpolatedHeights(lerp2);
            } else if(adjacentLODsHigher[0].index === 0) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                data[data.length-2*3+1] = this.getInterpolatedHeights(lerp1);
            } else {
                let lerp2 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 1);
                data[data.length-5*3+1] = this.getInterpolatedHeights(lerp2);
            }
        } else {
            if(adjacentLODsHigher.length === 2) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                let lerp2 = adjacentLODsHigher[1].box.interpolateHeights(this.rows, index, 1);
                data[data.length-4*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-3*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-6*3+1] = this.getInterpolatedHeights(lerp2);
                data[data.length-1*3+1] = this.getInterpolatedHeights(lerp2);
            } else if(adjacentLODsHigher[0].index === 0) {
                let lerp1 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 0);
                data[data.length-4*3+1] = this.getInterpolatedHeights(lerp1);
                data[data.length-3*3+1] = this.getInterpolatedHeights(lerp1);
            } else {
                let lerp2 = adjacentLODsHigher[0].box.interpolateHeights(this.rows, index, 1);
                data[data.length-6*3+1] = this.getInterpolatedHeights(lerp2);
                data[data.length-1*3+1] = this.getInterpolatedHeights(lerp2);
            }
        }
    }
};

QuadTree.prototype.readComplexity = function(data, projection, viewCamera, currentNode, currentIndex, currentDepth = 1, index = 1) {
    let frustumBoundaries = this.checkFrustumBoundaries(currentNode.vertices, projection, viewCamera, false);

    if(frustumBoundaries.withinFrustum) {
        let div = frustumBoundaries.minDistance/this.section;
        let depth = this.depth-Math.ceil(div);

        if(depth <= currentDepth) {
            this.fetchVertices(data, projection, viewCamera, currentNode, currentDepth, index);
        } else if(depth > currentDepth) {
            currentNode.children.forEach((child, index) => {
                this.readComplexity(data, projection, viewCamera, child, child.index, currentDepth+1, index);
            });
        }
    }
};

QuadTree.prototype.readProjection = function(data, projection, viewCamera, currentNode = this.tree[0].children) {
    for(let g = 0; g < 4; g++)
        this.readComplexity(data, projection, viewCamera, currentNode[g], 1);
};

/**
 * Tile constructor for lod switching during translation
 * @param x
 * @param y
 * @constructor
 */
function Tile(x, y) {
    this.x = x;
    this.y = y;
}

Tile.prototype.equal = function(tile) {
    return this.x === tile.x &&
        this.y === tile.y;
};

Tile.prototype.isAdjacent = function(tile) {
    return Math.abs(this.x-tile.x) < 2 &&
        Math.abs(this.y-tile.y) < 2;
};

Tile.prototype.normalize = function(center) {
    return new Tile(this.x-center.x, this.y-center.y);
};

Tile.prototype.append = function(tile) {
    this.x += tile.x;
    this.y += tile.y;
};

Tile.prototype.generateMissingTiles = function(tiles) {
    let currentTiles = [];

    for(let g = -1; g <= 1; g++)
        for(let h = -1; h <= 1; h++)
            currentTiles.push(new Tile(this.x+g, this.y+h));

    return currentTiles.filter((tile) => {
        for(let g = 0; g < tiles.length; g++) {
            if(tiles[g].equal(tile))
                return false;
        }
        return true;
    });
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
    this.trees = [];
    this.dimen = 16.0;
    this.complexity = 256;
    this.center = new Tile(0, 0);
    this.gl = gl;
    this.render = render;
    this.fetchNoiseMap();
}

const treeCycle = [
    //Top-Left
    new Tile(-1, -1),
    //Center-Left
    new Tile(0, -1),
    //Bottom-Left
    new Tile(1, -1),
    //Top
    new Tile(-1, 0),
    //Center
    new Tile(0, 0),
    //Bottom
    new Tile(1, 0),
    //Top-Right
    new Tile(-1, 1),
    //Center-Right
    new Tile(0, 1),
    //Bottom-Right
    new Tile(1, 1)
];

LOD.prototype.fetchNoiseMap = function() {
    let offsetX, offsetZ;

    treeCycle.forEach((tile) => {
        offsetX = tile.x*this.dimen;
        offsetZ = tile.y*this.dimen;
        let tree = new Terrain(this.complexity, this.dimen/2.0, false, offsetX, offsetZ);
        this.trees.push(new QuadTree(tree, tile, offsetX, offsetZ, this.dimen/2.0));
    });

    this.render(this.gl);
};


LOD.prototype.fetchData = function() {
    fetch("./../QuadTree/pixels.json")
        .then((data) => data.json())
        .then((mesh) => {
            this.trees.push(new QuadTree(mesh));
            this.render(this.gl);
        });
};

LOD.prototype.plantTrees = function() {
    let offsetX, offsetZ;
    this.center.generateMissingTiles(this.trees.map((tree) => tree.tile))
        .forEach((tile) => {
            offsetX = tile.x*this.dimen;
            offsetZ = tile.y*this.dimen;
            let tree = new Terrain(this.complexity, this.dimen/2.0, false, offsetX, offsetZ);
            this.trees.push(new QuadTree(tree, tile, offsetX, offsetZ, this.dimen/2.0));
        });
};

LOD.prototype.replaceTrees = function(viewCamera) {
    let centerTree = this.trees.filter((tree) => tree.tile.equal(this.center))[0];
    if(!centerTree.dimen.withinBox(viewCamera[12], -viewCamera[14])) {
        let orientation = centerTree.dimen.getNextOrientation(this.center, viewCamera[12], viewCamera[14]);

        this.trees = this.trees.filter((tree) => tree.tile.isAdjacent(orientation));

        this.center = orientation;
        this.plantTrees();
    }
};

LOD.prototype.readProjection = function(projection, viewCamera) {
    let data = [];

    this.trees.forEach((tree) => {
        tree.readProjection(data, projection, viewCamera);
    });

    this.replaceTrees(viewCamera);

    return data;
};