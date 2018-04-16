let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = gl.canvas.getBoundingClientRect().width;
gl.canvas.height = gl.canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "translationX",
        valueStart : -30,
        valueEnd : 30,
        value : 0,
        measurement : "px"
    }, {
        label : "translationY",
        valueStart : -30,
        valueEnd : 30,
        value : 4.8,
        measurement : "px"
    }, {
        label : "translationZ",
        valueStart : -50,
        valueEnd : 50,
        value : -50,
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
        value : 2.32,
        measurement : "°"
    }, {
        label : "rotationZ",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }
];

setSliders(sliders, drawScene, true, gl);

let vertexShaderSource = `
attribute vec4 a_position;
attribute vec3 a_texture;

uniform mat4 u_camera;
uniform mat4 u_projection;

varying vec3 v_texture;

void main() {
    vec4 projection = u_projection*u_camera*a_position;
    gl_Position = projection;
    
    v_texture = a_texture;
}
`;

let fragmentShaderSource = `
precision mediump float;

uniform sampler2D u_texture[8];
varying vec3 v_texture;

void main() {
    int texUnit = int(v_texture.z);
    if(texUnit == 0)
        gl_FragColor = texture2D(u_texture[0], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 1)
        gl_FragColor = texture2D(u_texture[1], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 2)
        gl_FragColor = texture2D(u_texture[2], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 3)
        gl_FragColor = texture2D(u_texture[3], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 4)
        gl_FragColor = texture2D(u_texture[4], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 5)
        gl_FragColor = texture2D(u_texture[5], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 6)
        gl_FragColor = texture2D(u_texture[6], vec2(1, 1)-v_texture.xy);
    else if(texUnit == 7)
        gl_FragColor = texture2D(u_texture[7], vec2(1, 1)-v_texture.xy);
    else  gl_FragColor = texture2D(u_texture[3], v_texture.xy);
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
    if (success)
        return program;
    console.log(gl.getProgramInfoLog(program));
    gl.deleteProgram(program);
}

let objHttp = new ObjHttp(initGL, gl, "X-Fighter");

function drawScene(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let perspective = matrices["perspective"](Math.PI/3.0, gl.canvas.width/gl.canvas.height, 0.0001, 200);

    let translM = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);
    let rotX = matrices["rotationX"](sliders[3].value);
    let rotY = matrices["rotationY"](sliders[4].value);
    let rotZ = matrices["rotationZ"](sliders[5].value);

    let viewMatrix = multiplyMatrices(rotX, rotY, rotZ, translM);
    let cameraMatrix = inverseMatrix(viewMatrix);

    gl.uniformMatrix4fv(unifCameraLoc, false, viewMatrix);
    gl.uniformMatrix4fv(unifProjectionLoc, false, perspective);

    gl.drawArrays(gl.TRIANGLES, 0, objHttp.data.vertices.geometricVertices.length/3);
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

let unifCameraLoc, unifProjectionLoc;
let attribPosLoc, attribTexLoc;

function initGL(gl, data) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    attribPosLoc = gl.getAttribLocation(program, "a_position");
    attribTexLoc = gl.getAttribLocation(program, "a_texture");
    unifCameraLoc = gl.getUniformLocation(program, "u_camera");
    unifProjectionLoc = gl.getUniformLocation(program, "u_projection");

    let posBuffer = gl.createBuffer();
    let texBuffer = gl.createBuffer();

    data.textures.forEach((texture, ind) => {
        let unifTexLoc = gl.getUniformLocation(program, "u_texture[" + ind + "]");
        gl.uniform1i(unifTexLoc, ind);
        gl.activeTexture(gl.TEXTURE0+ind);

        let tex = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, tex);

        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);

        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture);
    });

    gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.enableVertexAttribArray(attribPosLoc);
    gl.enableVertexAttribArray(attribTexLoc);

    gl.bindBuffer(gl.ARRAY_BUFFER, posBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data.vertices.geometricVertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPosLoc, 3, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, texBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data.vertices.textureVertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribTexLoc, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl, data);
}