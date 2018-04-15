let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = gl.canvas.getBoundingClientRect().width;
gl.canvas.height = gl.canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "translationX",
        valueStart : 0,
        valueEnd : gl.canvas.width,
        valueCurrent : 0,
        measurement : "px"
    }, {
        label : "translationY",
        valueStart : 0,
        valueEnd : gl.canvas.height,
        valueCurrent : 0,
        measurement : "px"
    }, {
        label : "translationZ",
        valueStart : 0,
        valueEnd : 400,
        valueCurrent : 0,
        measurement : "px"
    }, {
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
        label : "rotationZ",
        valueStart : 0,
        valueEnd : 360,
        valueCurrent : 0,
        measurement : "°"
    }, {
        label : "scaleX",
        valueStart : -10,
        valueEnd : 10,
        valueCurrent : 1,
        measurement : "times"
    }, {
        label : "scaleY",
        valueStart : -10,
        valueEnd : 10,
        valueCurrent : 1,
        measurement : "times"
    }, {
        label : "scaleZ",
        valueStart : -10,
        valueEnd : 10,
        valueCurrent : 1,
        measurement : "times"
    }
];

setSliders(sliders, drawScene, gl);

let vertexShaderSource = `
attribute vec4 aPosition;
attribute vec2 aTextureCoord;
uniform mat4 uMatrix;
varying vec2 vTextureCoord;
void main() {
    gl_Position = vec4((uMatrix*aPosition).xyz, 1);
    vTextureCoord = aTextureCoord;
}
`;

let fragmentShaderSource = `
precision mediump float;
varying vec2 vTextureCoord;
uniform sampler2D uSampler;
void main() {
    gl_FragColor = texture2D(uSampler, vec2(1, 1)-vTextureCoord);
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

let data = {
    obj : {
        "vertices" : null,
        "texture" : null
    }, texture : null
};

(function() {
    let promiseVertices = new Promise(function(resolve, reject) {
        let req = new XMLHttpRequest();
        req.open("GET", "http://localhost:8000/obj?filename=Cube_Redish/Cube_Textured.obj", true);
        req.addEventListener("load", function(e) {
            if(req.status < 400) {
                data.obj = JSON.parse(req.responseText);
                for(let g = 0; g < data.obj.vertices.length; g++)
                    data.obj.vertices[g] = (data.obj.vertices[g]*2+1.0)*100;
                resolve();
            } else {
                reject(req.status);
            }
        });
        req.send();
    });

    let promiseTexture = new Promise(function(resolve, reject) {
        let img = document.createElement("img");
        img.crossOrigin = "anonymous";
        img.src = "http://localhost:8000/texture?filename=Cube_Redish/Cube_Textured.jpg";
        img.addEventListener("load", function(e) {
            data.img = img;
            resolve();
        });
    });

    Promise.all([promiseVertices, promiseTexture]).then(function() {
        if(gl)
            initGL(gl);
    }).catch(function(error) {
        console.log(error);
    });
})();

function Matrix(type, x, y, z) {
    this.dim = 4;
    if(type === "scaling") {
        this.matrix = [
            x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1
        ]
    } else if(type === "translation") {
        this.matrix = [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        ]
    } else if(type === "translationOForth") {
        this.matrix = [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -x, -y, -z, 1
        ]
    } else if(type === "translationOBack") {
        this.matrix = [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        ]
    } else if(type === "rotationX") {
        this.matrix = [
            1, 0, 0, 0,
            0, y, -x, 0,
            0, -x, y, 0,
            0, 0, 0, 1
        ]
    } else if(type === "rotationY") {
        this.matrix = [
            y, 0, -x, 0,
            0, 1, 0, 0,
            x, 0, y, 0,
            0, 0, 0, 1
        ]
    } else if(type === "rotationZ") {
        this.matrix = [
            y, -x, 0, 0,
            x, y, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]
    } else if(type === "projection") {
        this.matrix = [
            2/x, 0, 0, 0,
            0, -2/y, 0, 0,
            0, 0, 2/z, 0,
            -1, 1, -1, 1
        ]
    } else if(type === "idMatrix") {
        this.matrix = [
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]
    }
}

Matrix.prototype.multiply = function(m2) {
    let mat = new Matrix("idMatrix");
    for(let g = 0; g < this.dim; g++) {
        for(let h = 0; h < this.dim; h++) {
            mat.matrix[g*this.dim+h] = 0;
            for(let i = 0; i < this.dim; i++) {
                mat.matrix[g*this.dim+h] += this.matrix[g*this.dim+i]*m2.matrix[i*this.dim+h];
            }
        }
    }
    return mat;
};

function drawScene(gl) {
    gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);
    gl.clearColor(0, 0, 0, 0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let projection = new Matrix("projection", gl.canvas.width, gl.canvas.height, 3200);
    let translationForthO = new Matrix("translationOForth", 350, 350, 50);
    let translationBackO = new Matrix("translationOBack", 350, 350, 50);
    let translationM = new Matrix("translation", sliders[0].valueCurrent, sliders[1].valueCurrent, sliders[2].valueCurrent);
    let rotationMX = new Matrix("rotationX", Math.sin(sliders[3].valueCurrent/360*2*Math.PI), Math.cos(sliders[3].valueCurrent/360*2*Math.PI));
    let rotationMY = new Matrix("rotationY", Math.sin(sliders[4].valueCurrent/360*2*Math.PI), Math.cos(sliders[4].valueCurrent/360*2*Math.PI));
    let rotationMZ = new Matrix("rotationZ", Math.sin(sliders[5].valueCurrent/360*2*Math.PI), Math.cos(sliders[5].valueCurrent/360*2*Math.PI));
    let scalingM = new Matrix("scaling", sliders[6].valueCurrent, sliders[7].valueCurrent, sliders[8].valueCurrent);

    gl.uniformMatrix4fv(uniformMatrixLocation, false, translationForthO.multiply(scalingM).multiply(rotationMZ).multiply(rotationMY).multiply(rotationMX).multiply(translationM).multiply(translationBackO).multiply(projection).matrix);

    gl.drawArrays(gl.TRIANGLES, 0, data.obj.vertices.length/3);
}

let uniformMatrixLocation;
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


function initGL(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    vertexPositionLocation = gl.getAttribLocation(program, "aPosition");
    uniformMatrixLocation = gl.getUniformLocation(program, "uMatrix");
    let vertexTextureLocation = gl.getAttribLocation(program, "aTextureCoord");

    let positionBuffer = gl.createBuffer();
    let colorBuffer = gl.createBuffer();
    let textureBuffer = gl.createBuffer();
    let texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, data.img);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);

    // gl.bindBuffer(gl.ARRAY_BUFFER, colorBuffer);
    // gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.useProgram(program);
    gl.enableVertexAttribArray(vertexPositionLocation);
    gl.enableVertexAttribArray(vertexTextureLocation);

    gl.bindBuffer(gl.ARRAY_BUFFER, textureBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data.obj.texture), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(vertexTextureLocation, 2, gl.FLOAT, false, 0, 0);

    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(data.obj.vertices), gl.STATIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);

    drawScene(gl);
}