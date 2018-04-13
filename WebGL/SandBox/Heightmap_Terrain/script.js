let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let vertexShaderSource = `
    attribute vec4 a_position;
 
    varying vec2 v_texturePos;
    
    void main() {
        gl_Position = vec4(a_position.xy, 0, 1.0);
        
        v_texturePos = (a_position.xy+1.0)/2.0;
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    uniform sampler2D u_heightmap;
    
    varying vec2 v_texturePos;
    
    void main() {
        gl_FragColor = texture2D(u_heightmap, v_texturePos);
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

let mesh = [
    -1, -1, 0,
    -1, 1, 0,
    1, 1, 0,

    1, 1, 0,
    1, -1, 0,
    -1, -1, 0
];

function drawScene(gl) {
    gl.clearColor(0, 0, 0, 0);

    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);

    gl.drawArrays(gl.TRIANGLES, 0, 6);

    if(gl.checkFramebufferStatus(gl.FRAMEBUFFER) === gl.FRAMEBUFFER_COMPLETE) {
        let min = Math.min(gl.canvas.width, gl.canvas.height);
        let pixels = new Uint8Array(min*min*4);
        gl.readPixels(0, 0, min, min, gl.RGBA, gl.UNSIGNED_BYTE, pixels);

        let data = pixels.filter((val, ind) => (ind % 4) === 0);

        let info = [];

        let offset = 2.0/min;
        for(let g = 0; g < min; g++) {
            for (let h = 0; h < min; h++) {
                info.push(offset * g - 1.0, data[g * min + h] / 255 - 0.5, offset * h - 1.0);
            }
        }

        console.log(JSON.stringify(info));
    }

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    gl.bindTexture(gl.TEXTURE_2D, frameTexture);

    gl.drawArrays(gl.TRIANGLES, 0, 6);
}

let attribPositionLoc;
let frameBuffer;
let texture, frameTexture;

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
img.src = "http://localhost:8000/heightmap?filename=heightmap.jpg";
img.addEventListener("load", startWebGL.bind(this, gl));

function startWebGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    frameBuffer = gl.createFramebuffer();

    frameTexture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, frameTexture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.canvas.width, gl.canvas.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, null);

    texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);

    gl.bindFramebuffer(gl.FRAMEBUFFER, frameBuffer);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, frameTexture, 0);

    attribPositionLoc = gl.getAttribLocation(program, "a_position");

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.enableVertexAttribArray(attribPositionLoc);

    drawScene(gl);
}