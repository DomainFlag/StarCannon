let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "rotationX",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "rotationY",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label: "rotationZ",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement: "°"
    }
];

setSliders(sliders, drawScene, true, gl);

let vertexShaderSource = `
attribute vec4 a_position;
attribute vec3 a_texture;

uniform mat4 u_matrix;

varying vec3 v_texture;

void main() {
    vec4 pos = u_matrix*a_position;
    
    gl_Position = vec4(a_position.xy, 0, 1.0);
    
    v_texture = pos.xyz;
}
`;


let fragmentShaderSource = `
precision mediump float;

uniform samplerCube u_skybox;

varying vec3 v_texture;

void main() {
    gl_FragColor = textureCube(u_skybox, v_texture);
} 
`;

function createFacade() {
    return [
        -1, -1, -1,
        1, 1, -1,
        -1, 1, -1,
        1, 1, -1,
        -1, -1, -1,
        1, -1, -1
    ];
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
    if(success)
        return program;
    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

function drawScene(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let quaternion = matrices["quarternion"]();
    let quaternionRot = matrices["fromEuler"](quaternion,
        sliders[0].value/Math.PI*360,
        sliders[1].value/Math.PI*360,
        sliders[2].value/Math.PI*360);
    let quaternionMatrix = matrices["fromQuat"](matrices["idMatrix"](), quaternionRot);

    let viewMatrix = inverseMatrix(quaternionMatrix);

    gl.uniformMatrix4fv(matrixCameraLocation, false, viewMatrix);

    gl.drawArrays(gl.TRIANGLES, 0, 6);
}

let vertexPositionLocation, matrixCameraLocation;
let bufferPosition;

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

let imgs = [];
let promises = [];

[
    "Left",
    "Right",
    "Up",
    "Down",
    "Front",
    "Back"
].forEach((path) => {
    let img = document.createElement("img");
    img.crossOrigin = "null";
    img.src = "http://localhost:8000/texture?filename=./SunSet/SunSet" + path + "2048.png";
    let res;
    promises.push(new Promise((resolve, reject) => {
        res = resolve;
    }));
    img.addEventListener("load", res);
    imgs.push(img);
});

Promise.all(promises).then(() => {
    startWebGL(gl);
});

function isPowerOf2(value) {
    return (value & (value - 1)) === 0;
}

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    vertexPositionLocation = gl.getAttribLocation(program, "a_position");
    matrixCameraLocation = gl.getUniformLocation(program, "u_matrix");

    let texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_CUBE_MAP, texture);

    for(let g = 0; g < 6; g++) {
        gl.texImage2D(gl.TEXTURE_CUBE_MAP_POSITIVE_X+g, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, imgs[g]);
    }

    if(isPowerOf2(imgs[0].width) && isPowerOf2(imgs[0].height)) {
        gl.generateMipmap(gl.TEXTURE_CUBE_MAP);
    } else {
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    }

    bufferPosition = gl.createBuffer();

    gl.useProgram(program);

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.enable(gl.CULL_FACE);
    gl.disable(gl.DEPTH_TEST);
    gl.depthMask(false);

    gl.enableVertexAttribArray(vertexPositionLocation);

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferPosition);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(createFacade()), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl);
}