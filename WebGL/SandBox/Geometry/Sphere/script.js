let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = gl.canvas.getBoundingClientRect().width;
gl.canvas.height = gl.canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "translationX",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "px"
    }, {
        label : "translationY",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "px"
    }, {
        label : "translationZ",
        valueStart : -4,
        valueEnd : 4,
        value : -1.5,
        measurement : "px"
    }, {
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
        label : "rotationZ",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "fudge",
        valueStart : 0,
        valueEnd : Math.PI,
        value : Math.PI/3,
        measurement : "dg"
    }
];

setSliders(sliders, drawScene, true, gl);

let vertexShaderSource = `
attribute vec4 a_position;

uniform mat4 u_worldViewProjection;
uniform mat4 u_world;

varying float v_height;

void main() {
    gl_Position = (u_worldViewProjection*a_position);
    
    v_height = (a_position.z+1.0)/2.0;
}
`;

let fragmentShaderSource = `
precision mediump float;

varying float v_height;

void main() {
    gl_FragColor = vec4(0.7-v_height, 0.0, 0.0, 1.0);
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

function generateSphereGeometricVertices(x1, y1, length) {
    let longitudeBands = 50;
    let latitudeBands = 50;
    let radius = 0.5;

    let vertexPositionData = [];
    let normalData = [];
    let textureCoordData = [];
    for (let latNumber = 0; latNumber <= latitudeBands; latNumber++) {
        let theta = latNumber * Math.PI / latitudeBands;
        let sinTheta = Math.sin(theta);
        let cosTheta = Math.cos(theta);

        for (let longNumber = 0; longNumber <= longitudeBands; longNumber++) {
            let phi = longNumber * 2 * Math.PI / longitudeBands;
            let sinPhi = Math.sin(phi);
            let cosPhi = Math.cos(phi);

            let x = cosPhi * sinTheta;
            let y = cosTheta;
            let z = sinPhi * sinTheta;
            let u = 1 - (longNumber / longitudeBands);
            let v = 1 - (latNumber / latitudeBands);

            normalData.push(x);
            normalData.push(y);
            normalData.push(z);

            textureCoordData.push(u);
            textureCoordData.push(v);

            vertexPositionData.push(radius * x);
            vertexPositionData.push(radius * y);
            vertexPositionData.push(radius * z);
        }
    }

    let indexData = [];
    for (let latNumber = 0; latNumber < latitudeBands; latNumber++) {
        for (let longNumber = 0; longNumber < longitudeBands; longNumber++) {
            let first = (latNumber * (longitudeBands + 1)) + longNumber;
            let second = first + longitudeBands + 1;

            let cycle = [first, second, first+1, second, second+1, first+1];
            for(let g = 0; g < 6; g++) {
                for(let h = 0; h < 3; h++) {
                    indexData.push(vertexPositionData[cycle[g]*3+h]);
                }
            }
        }
    }

    return indexData;
}

let sphere = generateSphereGeometricVertices(0.1, 0.1, -0.2);

function drawScene(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let perspective = matrices["perspective"](sliders[6].value, gl.canvas.width/gl.canvas.height, 1, 2000);

    let translation = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);
    let rotX = matrices["rotationX"](sliders[3].value);
    let rotY = matrices["rotationY"](sliders[4].value);
    let rotZ = matrices["rotationZ"](sliders[5].value);

    let viewMatrix = multiplyMatrices(rotX, rotY, rotZ, translation);

    gl.uniformMatrix4fv(unifWorldViewProjecLoc, false, multiplyMatrices(viewMatrix, perspective));
    gl.uniformMatrix4fv(unifWorldLoc, false, viewMatrix);

    gl.drawArrays(gl.TRIANGLES, 0, sphere.length/3);
}

let unifWorldViewProjecLoc, unifWorldLoc;
let vertexPositionLocation;

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

    vertexPositionLocation = gl.getAttribLocation(program, "a_position");

    unifWorldViewProjecLoc = gl.getUniformLocation(program, "u_worldViewProjection");
    unifWorldLoc = gl.getUniformLocation(program, "u_world");

    gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.useProgram(program);
    gl.enableVertexAttribArray(vertexPositionLocation);

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sphere), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl);
}