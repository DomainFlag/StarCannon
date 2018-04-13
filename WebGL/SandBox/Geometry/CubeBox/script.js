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
    }, {
        label: "translationCamera",
        valueStart : -2,
        valueEnd : 2,
        valueCurrent : 0,
        measurement: "t"
    }
];

setSliders(sliders, drawScene, true, gl);

let vertexShaderSource = `
attribute vec4 a_position;
attribute vec4 a_color;
attribute vec3 a_texture;
varying vec3 v_texture;
varying vec4 v_color;
uniform mat4 u_matrix;

void main() {
    vec4 pos = u_matrix*a_position;
    pos.xyz /= 2.0*sqrt(2.0);
    gl_Position = vec4(a_position.xy, a_position.z/sqrt(2.0), 1.0);
    v_texture = pos.xyz;
}
`;


let fragmentShaderSource = `
precision mediump float;
varying vec3 v_texture;
varying vec4 v_color;
uniform samplerCube u_skybox;

void main() {
    gl_FragColor = textureCube(u_skybox, v_texture);
} 
`;

function createCube(x1, y1, length) {
    return [
        //Blue
        x1+length, y1, length/2,
        x1+length, y1+length, length/2,
        x1+length, y1+length, -length/2,
        x1+length, y1+length, -length/2,
        x1+length, y1, -length/2,
        x1+length, y1, length/2,

        //Green
        x1, y1, length/2,
        x1, y1, -length/2,
        x1, y1+length, -length/2,
        x1, y1+length, -length/2,
        x1, y1+length, length/2,
        x1, y1, length/2,

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
        x1, y1, length/2,

        //Red
        x1, y1, -length/2,
        x1+length, y1+length, -length/2,
        x1, y1+length, -length/2,
        x1+length, y1, -length/2,
        x1+length, y1+length, -length/2,
        x1, y1, -length/2
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
    gl.clear(gl.COLOR_BUFFER_BIT);

    let rotationXCamera = matrices["rotationX"](Math.sin(sliders[0].valueCurrent/360*2*Math.PI), Math.cos(sliders[0].valueCurrent/360*2*Math.PI));
    let rotationYCamera = matrices["rotationY"](Math.sin(sliders[1].valueCurrent/360*2*Math.PI), Math.cos(sliders[1].valueCurrent/360*2*Math.PI));
    let rotationZCamera = matrices["rotationZ"](Math.sin(sliders[2].valueCurrent/360*2*Math.PI), Math.cos(sliders[2].valueCurrent/360*2*Math.PI));
    let perspectiveMatrix = matrices["perspective"](1.0472, gl.canvas.clientWidth / gl.canvas.clientHeight, 1, 2000);

    let viewCamera = multiplyMatrices(rotationXCamera, rotationYCamera, rotationZCamera);
    let viewMatrix = inverseMatrix(viewCamera);

    gl.uniformMatrix4fv(matrixCameraLocation, false, multiplyMatrices(viewMatrix));

    gl.drawArrays(gl.TRIANGLES, 0, 36);
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
    img.src = "http://localhost:8000/texture?filename=FullMoon" + path + "2048.png";
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
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MIN_FILTER, gl.LINEAR);

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
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(createCube(-Math.sqrt(2), -Math.sqrt(2), Math.sqrt(2)*2)), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl);
}