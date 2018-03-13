let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;


let sliders = [
    {
        label : "rotationOX",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement : "°"
    }, {
        label : "rotationOY",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement : "°"
    }, {
        label: "rotationOZ",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement : "°"
    }, {
        label: "fieldOfView",
        valueStart: -1,
        valueEnd: 1,
        valueCurrent: 0,
        measurement: "f"
    }, {
        label : "rotationX",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 40,
        measurement : "°"
    }, {
        label : "rotationY",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 90,
        measurement : "°"
    }, {
        label : "rotationZ",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 250,
        measurement : "°"
    }
];

setSliders(sliders, drawThruster, false, gl);

let speedScalar = 0.5;

let vertexThrusterShaderSource = `
attribute vec4 a_position;
varying float v_color;
uniform mat4 u_matrix;
uniform mat4 u_matrixO;
void main() {
    gl_PointSize = 3.5;
    vec4 pos = u_matrix*u_matrixO*a_position;
    gl_Position = vec4(pos.xyz, (pos.z+1.0));
    v_color = pos.z+1.0;
}
`;

let fragmentThrusterShaderSource = `
precision mediump float;
varying float v_color;
void main() {
    gl_FragColor = vec4(1, 0.2, 0, v_color);
}
`;

let vertexObjectShaderSource = `
attribute vec4 a_position;
uniform mat4 u_matrix;
uniform mat4 u_matrixO;
attribute vec4 a_color;
varying vec4 v_color;
void main() {
    vec4 pos = u_matrix*u_matrixO*a_position;
    gl_Position = vec4(pos.xyz, (pos.z+1.0));
    v_color = a_color;
}
`;

let fragmentObjectShaderSource = `
precision mediump float;
varying vec4 v_color;
void main() {
    gl_FragColor = v_color;
}
`;

function resize(gl) {
    let realToCSSPixels = window.devicePixelRatio;

    let displayWidth  = Math.floor(gl.canvas.clientWidth  * realToCSSPixels);
    let displayHeight = Math.floor(gl.canvas.clientHeight * realToCSSPixels);

    // Check if the canvas is not the same size.
    if (gl.canvas.width  !== displayWidth ||
        gl.canvas.height !== displayHeight) {

        // Make the canvas the same size
        gl.canvas.width  = displayWidth;
        gl.canvas.height = displayHeight;
    }
}

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
    if (success)
        return program;
    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

function Thruster() {
    this.tail = -0.7;

    this.nbParticles = 15000;
    this.bellfat = 15.0;

    this.particles = [];
    let x, y, max, z;
    for(let g = 0; g < this.nbParticles; g++) {
        x = Math.random()*this.tail;
        max = this.bell(x);
        y = Math.random()*max;

        let rotation = Math.random()*2*Math.PI;
        let rotationM = matrices["rotationX"](Math.sin(rotation), Math.cos(rotation));
        this.particles = this.particles.concat(multiplyVector(rotationM, [x, y, 0, 0]));
    }
}

Thruster.prototype.bell = function(x) {
    return ((-Math.exp(x*x)+1)/(Math.exp(1)-1)+1)/this.bellfat;
};

Thruster.prototype.act = function() {
    for(let g = 0; g < this.nbParticles*3; g += 3) {
        if(this.particles[g] > -(this.tail+1.0)) {
            this.particles[g] = this.tail+speedScalar*Math.random()*3;
            let max = this.bell(this.particles[g]);
            this.particles[g+1] = Math.random()*max;

            let rotation = Math.random()*2*Math.PI;
            let rotationM = matrices["rotationX"](Math.sin(rotation), Math.cos(rotation));
            let vector = multiplyVector(rotationM, [this.particles[g], this.particles[g+1], 0, 0]);
            this.particles[g] = vector[0];
            this.particles[g+1] = vector[1];
            this.particles[g+2] = vector[2];
        } else {
            let proportionY = this.particles[g+1]/this.bell(this.particles[g]);
            let proportionZ = this.particles[g+2]/this.bell(this.particles[g]);
            this.particles[g] += speedScalar*(Math.random()/2+0.5);
            this.particles[g+1] = proportionY*this.bell(this.particles[g]);
            this.particles[g+2] = proportionZ*this.bell(this.particles[g]);
        }
    }
};

let scene = new Thruster();

function createCube(x1, y1, length) {
    return [
        //Red
        x1, y1, -length/2,
        x1+length, y1+length, -length/2,
        x1, y1+length, -length/2,
        x1+length, y1, -length/2,
        x1+length, y1+length, -length/2,
        x1, y1, -length/2,

        //Green
        x1, y1, length/2,
        x1, y1, -length/2,
        x1, y1+length, -length/2,
        x1, y1+length, -length/2,
        x1, y1+length, length/2,
        x1, y1, length/2,

        //Blue
        x1+length, y1, length/2,
        x1+length, y1+length, length/2,
        x1+length, y1+length, -length/2,
        x1+length, y1+length, -length/2,
        x1+length, y1, -length/2,
        x1+length, y1, length/2,

        //Yellow
        x1, y1+length, length/2,
        x1+length, y1+length, -length/2,
        x1+length, y1+length, length/2,
        x1+length, y1+length, -length/2,
        x1, y1+length, length/2,
        x1, y1+length, -length/2,

        //Light Blue
        x1, y1, length/2,
        x1+length, y1, length/2,
        x1+length, y1, -length/2,
        x1+length, y1, -length/2,
        x1, y1, -length/2,
        x1, y1, length/2,

        //Rose
        x1, y1, length/2,
        x1, y1+length, length/2,
        x1+length, y1+length, length/2,
        x1+length, y1+length, length/2,
        x1+length, y1, length/2,
        x1, y1, length/2
    ];
}

let matrixThrusterPositionLocation, matrixObjectPositionLocation, matrixThrusterOLocation, matrixObjectOLocation;
let thrusterProgram, cubeProgram, textureProgram;
let bufferThrusterPosition, bufferObjectPosition, bufferObjectColor, bufferTexturePosition;
let attributeThrusterPositionLocation, attributeObjectPositionLocation, attributeObjectColorLocation, attributeTextureLocation;
let frameTexture;
let fb;

let colors = [];

[
    [1, 0, 0],
    [0, 1, 0],
    [0, 0, 1],
    [1, 1, 0],
    [0, 1, 1],
    [1, 0, 1]
].forEach(function(color) {
    for(let g = 0; g < 6; g++)
        colors = colors.concat(color);
});

function drawThruster(gl) {

    let rotationCameraX = matrices["rotationX"](Math.sin(sliders[4].valueCurrent/360*2*Math.PI), Math.cos(sliders[4].valueCurrent/360*2*Math.PI));
    let rotationCameraY = matrices["rotationY"](Math.sin(sliders[5].valueCurrent/360*2*Math.PI), Math.cos(sliders[5].valueCurrent/360*2*Math.PI));
    let rotationCameraZ = matrices["rotationZ"](Math.sin(sliders[6].valueCurrent/360*2*Math.PI), Math.cos(sliders[6].valueCurrent/360*2*Math.PI));

    let translationCamera = matrices["translation"](0, 0, sliders[3].valueCurrent);
    let viewCamera = multiplyMatrices(rotationCameraX, rotationCameraY, rotationCameraZ, translationCamera);
    let viewMatrix = inverseMatrix(viewCamera);

    gl.useProgram(thrusterProgram);
    gl.enableVertexAttribArray(attributeThrusterPositionLocation);

    gl.uniformMatrix4fv(matrixThrusterPositionLocation, false, viewMatrix);

    let matOForth = matrices["translationOForth"](-0.7, 0, 0);
    let rotationObjectX = matrices["rotationX"](Math.sin(sliders[0].valueCurrent/360*2*Math.PI), Math.cos(sliders[0].valueCurrent/360*2*Math.PI));
    let rotationObjectY = matrices["rotationY"](Math.sin(sliders[1].valueCurrent/360*2*Math.PI), Math.cos(sliders[1].valueCurrent/360*2*Math.PI));
    let rotationObjectZ = matrices["rotationZ"](Math.sin(sliders[2].valueCurrent/360*2*Math.PI), Math.cos(sliders[2].valueCurrent/360*2*Math.PI));
    let matOBack = matrices["translationOBack"](-0.7, 0, 0);

    gl.uniformMatrix4fv(matrixThrusterOLocation, false, multiplyMatrices(matOForth, rotationObjectX, rotationObjectY, rotationObjectZ, matOBack));

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferThrusterPosition);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(scene.particles), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(attributeThrusterPositionLocation, 3, gl.FLOAT, false, 0, 0);
    gl.drawArrays(gl.POINTS, 0, scene.nbParticles);
    scene.act();

    gl.useProgram(cubeProgram);
    gl.enableVertexAttribArray(attributeObjectColorLocation);
    gl.enableVertexAttribArray(attributeObjectPositionLocation);

    gl.uniformMatrix4fv(matrixObjectPositionLocation, false, viewMatrix);

    matOForth = matrices["translationOForth"](-0.7, 0, 0);
    rotationObjectX = matrices["rotationX"](Math.sin(sliders[0].valueCurrent/360*2*Math.PI), Math.cos(sliders[0].valueCurrent/360*2*Math.PI));
    rotationObjectY = matrices["rotationY"](Math.sin(sliders[1].valueCurrent/360*2*Math.PI), Math.cos(sliders[1].valueCurrent/360*2*Math.PI));
    rotationObjectZ = matrices["rotationZ"](Math.sin(sliders[2].valueCurrent/360*2*Math.PI), Math.cos(sliders[2].valueCurrent/360*2*Math.PI));
    matOBack = matrices["translationOBack"](-0.7, 0, 0);

    gl.uniformMatrix4fv(matrixObjectOLocation, false, multiplyMatrices(matOForth, rotationObjectX, rotationObjectY, rotationObjectZ, matOBack));

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferObjectPosition);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(createCube(-1, -0.15, 0.3)), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(attributeObjectPositionLocation, 3, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferObjectColor);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(attributeObjectColorLocation, 3, gl.FLOAT, false, 0, 0);

    gl.drawArrays(gl.TRIANGLES, 0, 36);

    requestAnimationFrame(drawThruster.bind(this, gl));
}

document.addEventListener("keypress", function(e) {
    if(e.which === 119) {
        if(speedScalar < 1.0)
            speedScalar += 0.04;
    } else if(e.which === 115) {
        if(speedScalar > 0.05)
            speedScalar -= 0.04;
    }
});

if(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexThrusterShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentThrusterShaderSource);

    thrusterProgram = createProgram(gl, vertexShader, fragmentShader);

    vertexCubeShader = createShader(gl, gl.VERTEX_SHADER, vertexObjectShaderSource);
    fragmentCubeShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentObjectShaderSource);

    cubeProgram = createProgram(gl, vertexCubeShader, fragmentCubeShader);

    attributeThrusterPositionLocation = gl.getAttribLocation(thrusterProgram, "a_position");
    matrixThrusterPositionLocation = gl.getUniformLocation(thrusterProgram, "u_matrix");
    matrixThrusterOLocation = gl.getUniformLocation(thrusterProgram, "u_matrixO");

    attributeObjectPositionLocation = gl.getAttribLocation(cubeProgram, "a_position");
    matrixObjectPositionLocation = gl.getUniformLocation(cubeProgram, "u_matrix");
    attributeObjectColorLocation = gl.getAttribLocation(cubeProgram, "a_color");
    matrixObjectOLocation = gl.getUniformLocation(cubeProgram, "u_matrixO");


    bufferThrusterPosition = gl.createBuffer();
    bufferObjectPosition = gl.createBuffer();
    bufferObjectColor = gl.createBuffer();

    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.CULL_FACE);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    drawThruster(gl);
}