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
    this.distance = 1.0;
    this.underground = {
        "geometricVertices" : [],
        "textureVertices" : [],
        "normalVertices" : []
    };
    this.surface = {
        "geometricVertices" : [],
        "textureVertices" : [],
        "normalVertices" : []
    };
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
    let distance, y;
    let coordY;
    for(let z = -this.distance; z < this.distance; z += this.partition) {
        let layer = [];
        for(let x = -this.distance; x < this.distance; x += this.partition) {
            distance = distanceVecs([x, 0, z], [0, 0, 0]);
            y = noise.noise2D(x, z)/2.5+this.depth/2.0; //->[0, 0.8]
            coordY = Math.min(y, this.depth*(1.0-distance/this.distance))+this.depth*distance/this.distance;
            layer.push(new Vector(x, coordY-this.depth, z));
        }
        this.honeycomb.push(layer);
    }
};

Lake.prototype.parseHoneycomb  = function() {
    let cycle = [ [0, 0], [0, 1], [1, 0], [1, 1] ];
    let vector;
    for(let g = 0; g < this.honeycomb.length-1; g++) {
        for(let h = 0; h < this.honeycomb[g].length-1; h++) {
            for(let i = 0; i < cycle.length; i++) {
                vector = this.honeycomb[g+cycle[i][0]][h+cycle[i][1]].xyz();
                this.underground.geometricVertices.push(vector[0], vector[1], vector[2]);
                this.surface.geometricVertices.push(vector[0], 0, vector[2]);
            }
        }
    }
};

Lake.prototype.fetchTrianglesNumber = function() {
    return this.underground.geometricVertices.length/3;
};

let lake = new Lake();

let vertexShaderSource = `
    attribute vec4 a_position;
    
    uniform mat4 u_camera;
    uniform mat4 u_projection;
    
    uniform float u_depth;
    uniform float u_invert;
    
    varying float v_far;
    varying vec2 v_pos;
    
    void main() {
        vec4 view = u_projection*u_camera*a_position;
        view.y *= u_invert;
        gl_Position = view;
        
        v_far = (a_position.y+u_depth)/u_depth;
        v_pos = (a_position.xz+1.0)/2.0;
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    uniform sampler2D u_texture;
    
    varying float v_far;
    varying vec2 v_pos;
    
    void main() {
        gl_FragColor = texture2D(u_texture, v_pos);
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

function setTexture(img, width, height) {
    let texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);

    if(img)
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
    else
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, width, height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);

    return texture;
}

function drawScene(time) {
    gl.viewport(0, 0, img.width, img.height);
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    update_frame(time);

    let quaternionRot = matrices["fromEuler"]([], -sliders[3].value, -sliders[4].value, -sliders[5].value);
    let quaternionMatrix = matrices["fromQuat"](matrices["idMatrix"](), quaternionRot);
    let cameraTranslation = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);

    let quaternionCamera = inverseMatrix(multiplyMatrices(cameraTranslation, quaternionMatrix));
    gl.uniformMatrix4fv(unifCameraLoc, false, quaternionCamera);

    /* Drawing to framebuffer the lake's underground */
    gl.bindBuffer(gl.ARRAY_BUFFER, posUndergroundBuffer);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);
    gl.bindTexture(gl.TEXTURE_2D, groundTexture);

    gl.uniform1f(unifInvertLoc, -1.0);

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, lake.fetchTrianglesNumber());

    /* Drawing the texture attached to framebuffer to the main framebuffer */
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.bindBuffer(gl.ARRAY_BUFFER, posSurfaceBuffer);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);
    gl.bindTexture(gl.TEXTURE_2D, fboTexture);

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, lake.fetchTrianglesNumber());

    requestAnimationFrame(drawScene);
}

let img = document.createElement("img");
img.crossOrigin = "null";
img.src = "http://localhost:8000/texture?filename=lake_underground.jpg";
img.addEventListener("load", startWebGL.bind(this, gl));

let unifCameraLoc, unifProjectionLoc;
let unifDepthLoc, unifInvertLoc;
let attribPositionLoc;
let posUndergroundBuffer, posSurfaceBuffer;

let fboTexture, groundTexture;

let fbo;

let update_frame = get_frame_rate_updater();

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    attribPositionLoc = gl.getAttribLocation(program, "a_position");
    unifCameraLoc = gl.getUniformLocation(program, "u_camera");
    unifProjectionLoc = gl.getUniformLocation(program, "u_projection");

    unifDepthLoc = gl.getUniformLocation(program, "u_depth");
    unifInvertLoc = gl.getUniformLocation(program, "u_invert");

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);
    gl.depthMask(true);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    fboTexture = setTexture(null, img.width, img.height);
    groundTexture = setTexture(img);

    fbo = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, fbo);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, fboTexture, 0);

    gl.enableVertexAttribArray(attribPositionLoc);

    posUndergroundBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, posUndergroundBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(lake.underground.geometricVertices), gl.STATIC_DRAW);

    posSurfaceBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, posSurfaceBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(lake.surface.geometricVertices), gl.STATIC_DRAW);

    gl.uniform1f(unifDepthLoc, lake.depth);

    let perspective = matrices["perspective"](Math.PI/3.0, gl.canvas.width/gl.canvas.height, 0.0001, 50);
    gl.uniformMatrix4fv(unifProjectionLoc, false, perspective);

    requestAnimationFrame(drawScene);
}