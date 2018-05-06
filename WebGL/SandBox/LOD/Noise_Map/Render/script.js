let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

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

let sliders = [
    {
        label : "cameraX",
        valueStart : -1.5,
        valueEnd : 1.5,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraY",
        valueStart : -3.5,
        valueEnd : 3.5,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraZ",
        valueStart : -8,
        valueEnd :8,
        value : 6.0,
        measurement : "dp"
    }, {
        label : "cameraRotX",
        valueStart : 0,
        valueEnd : 360,
        value : 270,
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

let terrain = new Terrain(512, 4.0, true);

let vertexShaderSource = `
    attribute vec4 a_position;
    
    varying float v_height;
    
    uniform mat4 u_camera;
    uniform mat4 u_projection;
    
    void main() {
        vec4 view = u_projection*u_camera*a_position;
        gl_Position = view;
        
        v_height = a_position.y;
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    varying float v_height;
    
    uniform float u_amplitude;
    
    void main() {
        if(v_height <= 0.0)
             gl_FragColor = vec4(0.12, 0.24, 0.9, 1.0);
        else if(v_height < 0.1)
            gl_FragColor = vec4(0.76, 0.69, 0.5, 1.0);
        else if(v_height < 0.2)
            gl_FragColor = vec4(0.42, 0.95, 0.1, 1.0);
        else if(v_height < (u_amplitude-1.1))
            gl_FragColor = vec4(0.22, 0.65, 0.05, 1.0);
        else
            gl_FragColor = vec4(0.8, 0.8, 0.8, 0.2);
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
    let quaternionRot = matrices["fromEuler"]([], sliders[3].value, sliders[4].value, sliders[5].value);
    let quaternionMatrix = matrices["fromQuat"](matrices["idMatrix"](), quaternionRot);

    let cameraTranslation = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);
    let perspective = matrices["perspective"](Math.PI/3, gl.canvas.width/gl.canvas.height, 0.001, 15);

    let viewMatrix = multiplyMatrices(cameraTranslation, quaternionMatrix);
    let cameraMatrix = inverseMatrix(viewMatrix);

    gl.uniform1f(unifAmplitudeLoc, terrain.amplitude);
    gl.uniformMatrix4fv(unifCameraLoc, false, cameraMatrix);
    gl.uniformMatrix4fv(unifProjectionLoc, false, perspective);

    gl.drawArrays(gl.TRIANGLES, 0, terrain.fetchTrianglesNumber());

    requestAnimationFrame(drawScene.bind(this, gl));
}

let unifCameraLoc, unifProjectionLoc;
let unifAmplitudeLoc;
let attribPositionLoc;

let img = document.createElement("img");
img.crossOrigin = "null";
img.src = "http://localhost:8000/texture?filename=Texture_Gradient.png";
img.addEventListener("load", startWebGL.bind(this, gl));

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    attribPositionLoc = gl.getAttribLocation(program, "a_position");
    unifCameraLoc = gl.getUniformLocation(program, "u_camera");
    unifProjectionLoc = gl.getUniformLocation(program, "u_projection");
    unifAmplitudeLoc = gl.getUniformLocation(program, "u_amplitude");

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.enableVertexAttribArray(attribPositionLoc);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(terrain.mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl);
}