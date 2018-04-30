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
        valueStart : -10,
        valueEnd : 10,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraY",
        valueStart : 10,
        valueEnd : 0,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraZ",
        valueStart : -10,
        valueEnd : 10,
        value : 0,
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

setSliders(sliders, drawScene, true, gl);

let vertexShaderSource = `
    attribute vec4 a_position;
    
    uniform mat4 u_projection;
    uniform mat4 u_camera;
    
    varying float v_depth;
    varying vec2 v_tex;
    varying float v_far;
    
    void main() {
        vec4 projectionView = u_projection*u_camera*a_position;
        
        gl_Position = projectionView;
        
        v_depth = a_position.y+0.5;
        v_tex = vec2(((a_position.x+a_position.z)/4.0+0.5)*5.0, 1.0-v_depth);
        v_far = 10.0-log(distance(gl_Position.xyz, vec3(0, 0, 1.0)));
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    uniform sampler2D u_texture;
    
    varying vec2 v_tex;
    varying float v_depth;
    varying float v_far;
    
    void main() {
        if(v_depth >= 0.05) {
            gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
            gl_FragColor = texture2D(u_texture, v_tex);

            gl_FragColor.rgb *= v_depth;

            gl_FragColor.w = v_far;
        } else {
            gl_FragColor = vec4(0.147, 0.372, 0.592, 1.0);
        }
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
    sliders[2].value = this.value;
};

Animation.prototype.addEventListener = function() {
    document.addEventListener("keydown", (e) => {
        if(e.key === 's' || e.which === 83) {
            requestAnimationFrame(this.callback);
        }
    });
};

function drawScene(currentTime) {
    animation.act(currentTime);

    let cameraTranslation = matrices["translation"](sliders[1].value, sliders[2].value, sliders[3].value);
    let cameraRotX = matrices["rotationX"](sliders[4].value);
    let cameraRotY = matrices["rotationY"](sliders[5].value);
    let cameraRotZ = matrices["rotationZ"](sliders[6].value);

    let viewMatrix = multiplyMatrices(cameraRotX, cameraRotY, cameraRotZ, cameraTranslation);
    let cameraMatrix = inverseMatrix(viewMatrix);

    let perspective = matrices["perspective"](Math.PI/3, gl.canvas.width/gl.canvas.height, 0.01, 25);

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

let lod;
let animation;

let img = document.createElement("img");
img.crossOrigin = "null";
img.src = "http://localhost:8000/texture?filename=terrain_gradient.png";
img.addEventListener("load", () => {
    lod = new LOD(startWebGL, gl);
    animation = new Animation(drawScene, sliders[3].valueEnd, sliders[3].valueStart, 1/30);
});


function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    uniformProjectionLocation = gl.getUniformLocation(program, "u_projection");
    attribPositionLoc = gl.getAttribLocation(program, "a_position");
    uniformCameraLocation = gl.getUniformLocation(program, "u_camera");

    let texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.enableVertexAttribArray(attribPositionLoc);

    // drawScene();
    requestAnimationFrame(drawScene);
}