let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let vertexShaderSource = `
attribute vec4 a_position;
attribute vec4 a_color;
varying vec4 v_color;

void main() {
    vec4 pos = a_position;
    gl_Position = vec4(a_position.xyz, 1.0);
    v_color = a_color;
}
`;


let fragmentShaderSource = `
precision mediump float;
varying vec4 v_color;

void main() {
    gl_FragColor = v_color;
} 
`;

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
    gl.drawArrays(gl.TRIANGLES, 0, 36);
}

let vertexPositionLocation, vertexColorLocation, uniformCameraLocation;
let bufferPosition, colorBuffer;

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

startWebGL(gl);

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    vertexPositionLocation = gl.getAttribLocation(program, "a_position");
    vertexColorLocation = gl.getAttribLocation(program, "a_color");
    uniformCameraLocation = gl.getAttribLocation(program, "u_camera");

    bufferPosition = gl.createBuffer();
    colorBuffer = gl.createBuffer();

    gl.useProgram(program);

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.enable(gl.CULL_FACE);
    gl.disable(gl.DEPTH_TEST);
    gl.depthMask(false);

    gl.enableVertexAttribArray(vertexPositionLocation);

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferPosition);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(createCube(-1.0, -1.0, 2.0)), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);

    gl.enableVertexAttribArray(vertexColorLocation);

    gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexColorLocation, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl);
}