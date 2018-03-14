let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "rotationX",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement : "°"
    }, {
        label : "rotationY",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement : "°"
    }, {
        label: "rotationZ",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement: "°"
    }
];

setSliders(sliders, drawScene, true, gl);

let vertexShaderSource = `
attribute vec4 a_position;
attribute vec2 a_texture;
varying vec2 v_texture;
uniform mat4 u_matrix;

void main() {
    vec4 pos = a_position*u_matrix;
    gl_Position = vec4(pos.xy, 0, 1.0);
    v_texture = a_texture;
}
`;

let fragmentShaderSource = `
precision mediump float;
varying vec2 v_texture;
uniform sampler2D u_skybox;

void main() {
    gl_FragColor = texture2D(u_skybox, v_texture*vec2(1.0, -1.0)+vec2(0.0, 1.0));
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

function drawScene(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferPosition);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        //270
        -1, 0, 1, -1, 1, -1, -1, 1, 1,
        -1, 1, -1, -1, 0, 1, -1, 0, -1,

        //0
        -1, 0, -1, 1, 1, -1, -1, 1, -1,
        1, 1, -1, -1, 0, -1, 1, 0, -1,

        //90
        1, 0, -1, 1, 1, 1, 1, 1, -1,
        1, 1, 1, 1, 0, -1, 1, 0, 1,

        //180
        -1, 0, 1, -1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 1, -1, 0, 1,

        //UP
        -1, 1, -1, 1, 1, 1, -1, 1, 1,
        1, 1, 1, -1, 1, -1, 1, 1, -1
        ]), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);

    let rotationXCamera = matrices["rotationX"](Math.sin(sliders[0].valueCurrent/360*2*Math.PI), Math.cos(sliders[0].valueCurrent/360*2*Math.PI));
    let rotationYCamera = matrices["rotationY"](Math.sin(sliders[1].valueCurrent/360*2*Math.PI), Math.cos(sliders[1].valueCurrent/360*2*Math.PI));
    let rotationZCamera = matrices["rotationZ"](Math.sin(sliders[2].valueCurrent/360*2*Math.PI), Math.cos(sliders[2].valueCurrent/360*2*Math.PI));
    let viewCamera = multiplyMatrices(rotationXCamera, rotationYCamera, rotationZCamera);
    let viewMatrix = inverseMatrix(viewCamera);
    gl.uniformMatrix4fv(matrixCameraLocation, false, viewMatrix);

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferTexture);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
        0, 0.5, 0.25, 0.5+0.5/3, 0, 0.5+0.5/3,
        0.25, 0.5+0.5/3, 0, 0.5, 0.25, 0.5,

        0.25, 0.5, 0.5, 0.5+0.5/3, 0.25, 0.5+0.5/3,
        0.5, 0.5+0.5/3, 0.25, 0.5, 0.5, 0.5,

        0.5, 0.5, 0.75, 0.5+0.5/3, 0.5, 0.5+0.5/3,
        0.75, 0.5+0.5/3, 0.5, 0.5, 0.75, 0.5,

        1, 0.5, 1, 0.5+0.5/3, 0.75, 0.5+0.5/3,
        0.75, 0.5+0.5/3, 0.75, 0.5, 1, 0.5,

        0.25, 0.5+0.5/3, 0.5, 1, 0.25, 1,
        0.5, 1, 0.25, 0.5+0.5/3, 0.5, 0.5+0.5/3
    ]), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexTextureLocation, 2, gl.FLOAT, false, 0, 0);

    gl.drawArrays(gl.TRIANGLES, 0, 30);
    // requestAnimationFrame(drawScene.bind(this, gl));
}

let vertexPositionLocation, vertexTextureLocation, matrixCameraLocation;
let bufferPosition, bufferTexture;

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

let img = document.createElement("img");
img.crossOrigin = "null";
img.src = "http://localhost:8000/texture?filename=skybox.png";
img.addEventListener("load", startWebGL.bind(this, gl));

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    vertexPositionLocation = gl.getAttribLocation(program, "a_position");
    vertexTextureLocation = gl.getAttribLocation(program, "a_texture");
    matrixCameraLocation = gl.getUniformLocation(program, "u_matrix");

    let texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);

    bufferPosition = gl.createBuffer();
    bufferTexture = gl.createBuffer();

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.CULL_FACE);

    gl.useProgram(program);
    gl.enableVertexAttribArray(vertexPositionLocation);
    gl.enableVertexAttribArray(vertexTextureLocation);

    drawScene(gl);
}

function drawScene() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.uniformMatrix4fv(matrixCameraLocation, false, viewMatrix);
    gl.drawArrays(gl.TRIANGLES, 0, whateverNumber);

    requestAnimationFrame(drawScene.bind(this, someOtherFunc));
}