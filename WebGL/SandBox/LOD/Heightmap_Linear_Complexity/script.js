let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "Complexity Level",
        valueStart : 0,
        valueEnd : 12,
        value : 7,
        measurement : "dg"
    }, {
        label : "cameraX",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraY",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraZ",
        valueStart : -5,
        valueEnd : 5,
        value : 4.4,
        measurement : "dp"
    }, {
        label : "cameraRotX",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : Math.PI/2,
        measurement : "°"
    }, {
        label : "cameraRotY",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "cameraRotZ",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }
];

setSliders(sliders, drawScene, false, gl);

let vertexShaderSource = `
    attribute vec4 a_position;
    
    uniform mat4 u_projection;
    uniform mat4 u_camera;
    
    varying float v_depth;
    void main() {
        vec4 projectionView = u_projection*u_camera*a_position;
        gl_Position = projectionView;
        
        v_depth = a_position.y+0.5;
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    varying float v_depth;
    
    void main() {
        gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
        
        gl_FragColor.rgb *= v_depth;
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

function Animation(callback, from, to, by) {
    this.callback = callback;
    this.from = from;
    this.to = to;
    this.by = by;
    this.value = this.from;
    this.partition = (this.from-this.to)*this.by;
    this.current = 0;
    this.addEventListener();
}

Animation.prototype.act = function(currentTime) {
    let delTime = (currentTime/1000-this.current);

    this.value -= delTime*this.partition;

    this.current = currentTime/1000;
    this.update();
};

Animation.prototype.update = function() {
    sliders[3].value = this.value;
};

Animation.prototype.addEventListener = function() {
    document.addEventListener("keydown", (e) => {
        if(e.key === 's' || e.which === 83) {
            requestAnimationFrame(this.callback);
        }
    });
};

function drawScene(time) {
    animation.act(time);

    let cameraTranslation = matrices["translation"](sliders[1].value, sliders[2].value, animation.value);
    let cameraRotX = matrices["rotationX"](sliders[4].value);
    let cameraRotY = matrices["rotationY"](sliders[5].value);
    let cameraRotZ = matrices["rotationZ"](sliders[6].value);

    let viewMatrix = multiplyMatrices(cameraTranslation, cameraRotX, cameraRotY, cameraRotZ);
    let cameraMatrix = inverseMatrix(viewMatrix);

    let perspective = matrices["perspective"](1.047, gl.canvas.width/gl.canvas.height, 0.01, 5);

    let data = lod.tree.readProjection(perspective, cameraMatrix);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);

    gl.uniformMatrix4fv(uniformProjectionLocation, false, perspective);
    gl.uniformMatrix4fv(uniformCameraLocation, false, cameraMatrix);

    gl.drawArrays(gl.TRIANGLES, 0, data.length/3);

    requestAnimationFrame(drawScene);
}

let uniformProjectionLocation, uniformCameraLocation;
let attribPositionLoc;

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

let lod = new LOD(startWebGL, gl);
let animation = new Animation(drawScene, sliders[3].valueEnd, sliders[3].valueStart, 1/65);

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    uniformProjectionLocation = gl.getUniformLocation(program, "u_projection");
    attribPositionLoc = gl.getAttribLocation(program, "a_position");
    uniformCameraLocation = gl.getUniformLocation(program, "u_camera");

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.enableVertexAttribArray(attribPositionLoc);
}