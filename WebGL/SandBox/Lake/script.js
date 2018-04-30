let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "cameraX",
        valueStart : -5,
        valueEnd : 5,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraY",
        valueStart : -5,
        valueEnd : 5,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraZ",
        valueStart : -5,
        valueEnd : 5,
        value : 3.15,
        measurement : "dp"
    }, {
        label : "cameraRotX",
        valueStart : 0,
        valueEnd : 360,
        value : 32.4,
        measurement : "°"
    }, {
        label : "cameraRotY",
        valueStart : 0,
        valueEnd : 360,
        value : 0,
        measurement : "°"
    }, {
        label : "cameraRotZ",
        valueStart : 0,
        valueEnd : 360,
        value : 0,
        measurement : "°"
    }
];

setSliders(sliders, drawScene, false, gl);

let noise = new SimplexNoise();

function Lake() {
    this.partition = 1/20;
    this.depth = 0.8;
    this.geometricVertices = [];
    this.textureVertices = [];
    this.normalVertices = [];
    this.honeycomb = [];
    this.setHoneycomb();
    this.parseHoneycomb();
}

function Vector(x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
}

Vector.prototype.xyz = function() {
    return [this.x, this.y, this.z];
};

Lake.prototype.lerp = function(val, min, max) {
    return min[1]+(val-min[0])/(max[0]-min[0])*(max[1]-min[1]);
};

Lake.prototype.setHoneycomb = function() {
    let distance;
    let y, max = Math.sqrt(2);
    for(let z = -Math.sqrt(2)-this.partition; z < Math.sqrt(2)+this.partition; z += this.partition) {
        let layer = [];
        for(let x = -Math.sqrt(2)-this.partition; x < Math.sqrt(2)+this.partition; x += this.partition) {
            distance = distanceVecs([x, 0, z], [0, 0, 0]);
            y = noise.noise2D(x, z)/2.5+this.depth/2.0; //->[0, 0.8]
            let coordX = x;
            let coordY = Math.min(y, this.depth*(1.0-distance/max))+this.depth*distance/max;
            let coordZ = z;
            layer.push(new Vector(coordX, coordY, coordZ));
        }
        this.honeycomb.push(layer);
    }
};

Lake.prototype.parseHoneycomb  = function() {
    let cycle = [ [0, 0], [0, 1], [1, 0], [1, 1] ];
    for(let g = 0; g < this.honeycomb.length-1; g++) {
        for(let h = 0; h < this.honeycomb[g].length-1; h++) {
            for(let i = 0; i < cycle.length; i++) {
                let vector = this.honeycomb[g+cycle[i][0]][h+cycle[i][1]].xyz();
                this.geometricVertices.push(vector[0]);
                this.geometricVertices.push(vector[1]);
                this.geometricVertices.push(vector[2]);
            }
        }
    }
};

Lake.prototype.fetchTrianglesNumber = function() {
    return this.geometricVertices.length/3;
};

let lake = new Lake();

let vertexShaderSource = `
    attribute vec4 a_position;
    
    uniform mat4 u_camera;
    uniform mat4 u_projection;
    
    uniform float u_depth;
    
    varying float v_far;
    
    void main() {
        vec4 view = u_projection*u_camera*a_position;
        gl_Position = view;
        v_far = a_position.y/u_depth;
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    varying float v_far;
    
    void main() {
        if(v_far == 1.0)
            gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
        else 
            gl_FragColor = vec4(0.2, 0.2, 0.8, 1.0);
        gl_FragColor *= v_far;
    }
`;

function createShader(gl, type, source) {
    let shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    let success = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
    if(success)
        return shader;
    console.log(gl.getShaderInfoLog(shader));
    gl.deleteShader(shader);
}

function createProgram(gl, vertexShader, fragmentShader) {
    let program = gl.createProgram();
    gl.attachShader(program, vertexShader);
    gl.attachShader(program, fragmentShader);
    gl.linkProgram(program);
    let success = gl.getProgramParameter(program, gl.LINK_STATUS);
    if(success)
        return program;
    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

function drawScene(time) {
    update_frame(time);

    let quaternionRot = matrices["fromEuler"]([], -sliders[3].value, -sliders[4].value, -sliders[5].value);
    let quaternionMatrix = matrices["fromQuat"](matrices["idMatrix"](), quaternionRot);

    let cameraTranslation = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);
    let perspective = matrices["perspective"](Math.PI/3.0, gl.canvas.width/gl.canvas.height, 0.0001, 50);

    let quaternionCamera = inverseMatrix(multiplyMatrices(cameraTranslation, quaternionMatrix));
    gl.uniformMatrix4fv(unifCameraLoc, false, quaternionCamera);
    gl.uniformMatrix4fv(unifProjectionLoc, false, perspective);

    gl.uniform1f(unifDepthLoc, lake.depth);

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, lake.fetchTrianglesNumber());

    requestAnimationFrame(drawScene);
}

let unifCameraLoc, unifProjectionLoc;
let unifDepthLoc;
let attribPositionLoc;

let update_frame = get_frame_rate_updater();

function resize(gl) {
    let realToCSSPixels = window.devicePixelRatio;

    let displayWidth  = Math.floor(gl.canvas.clientWidth  * realToCSSPixels);
    let displayHeight = Math.floor(gl.canvas.clientHeight * realToCSSPixels);

    if (gl.canvas.width  !== displayWidth ||
        gl.canvas.height !== displayHeight) {

        gl.canvas.width  = displayWidth;
        gl.canvas.height = displayHeight;
    }
}

if(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    attribPositionLoc = gl.getAttribLocation(program, "a_position");
    unifCameraLoc = gl.getUniformLocation(program, "u_camera");
    unifProjectionLoc = gl.getUniformLocation(program, "u_projection");

    unifDepthLoc = gl.getUniformLocation(program, "u_depth");

    gl.enable(gl.DEPTH_TEST);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.enableVertexAttribArray(attribPositionLoc);

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(lake.geometricVertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);

    requestAnimationFrame(drawScene);
}