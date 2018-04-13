let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

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
        value : 0.0,
        measurement : "dp"
    }, {
        label : "cameraZ",
        valueStart : -8,
        valueEnd :8,
        value : 0.5,
        measurement : "dp"
    }, {
        label : "cameraRotX",
        valueStart : 0,
        valueEnd : 360,
        value : 0,
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
    },{
        label : "objectRotX",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "objectRotY",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "objectRotZ",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label: "fieldOfView",
        valueStart : 0,
        valueEnd : Math.PI,
        value : 1,
        measurement: "°"
    }
];

setSliders(sliders, drawScene, false, gl);

let vertexShaderSunSource = `
    attribute vec4 a_sunPos;
    
    uniform mat4 u_model;
    uniform mat4 u_camera;
    uniform mat4 u_projection;
    
    varying vec3 v_sunPos;
    
    void main() {
        vec4 camera = u_camera*u_model*a_sunPos;
        vec4 result = u_projection*camera;
        gl_Position = result;
        
        v_sunPos = a_sunPos.xyz;
    }
`;

let fragmentShaderSunSource = `
    precision mediump float;
    
    uniform vec3 u_color;
    uniform float u_radius;
    uniform vec3 u_sunOrigin;
    
    varying vec3 v_sunPos;
    
    void main() {
        float distFromOrigin = distance(u_sunOrigin, v_sunPos);
        float intensity = mix(0.0, u_radius, distFromOrigin)/4.0; //->[0, 0.25]
    
        gl_FragColor = vec4(u_color, 1.0-intensity);
    }
`;

let vertexShaderSkySource = `
    attribute vec4 a_skyPos;
    
    varying vec3 v_skyLoc;
    
    void main() {
        gl_Position = a_skyPos;
        
        v_skyLoc = a_skyPos.xyz;
    }
`;

let fragmentShaderSkySource = `
    precision mediump float;
    
    uniform vec3 u_color;
    uniform vec3 u_sunPos;
    
    uniform float u_pitch;
    uniform float u_yaw;
    
    varying vec3 v_skyLoc;
    
    vec3 rgb2hsv(vec3 c) {
        vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
        vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
        vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
        float d = q.x - min(q.w, q.y);
        float e = 1.0e-10;
        return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
    }

    vec3 hsv2rgb(vec3 c) {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    
    void main() {
        float dist = 1.0-distance(v_skyLoc, u_sunPos)/2.0*sqrt(2.0);
        
        vec3 hsv = rgb2hsv(u_color.rgb);
        hsv.rb *= u_pitch;
        
        hsv.r -= pow(log((u_pitch)/2.0+1.0), 1.0/4.0);
        hsv.b -= (1.0-u_pitch)/3.0;
        hsv.b += dist/1.5;
        
        vec3 color = hsv2rgb(hsv);
        
        gl_FragColor = vec4(color.rgb, 0.8);
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

function Sky() {
    this.sun = new Sun();
    this.mesh = null;
    this.color = [225, 152, 190];
    this.past = 0;
    this.rotationX = 0;

    this.createMesh();
    this.parseColor();
}

Sky.prototype.createMesh = function() {
    this.mesh = [
        -1, -1, -1,
        -1, 1, -1,
        1, 1, -1,
        1, 1, -1,
        1, -1, -1,
        -1, -1, -1
    ];
};

Sky.prototype.parseColor = function() {
    for(let g = 0; g < this.color.length; g++)
        this.color[g] /= 255;
};

Sky.prototype.act = function(now) {
    now *= 0.001;

    let deltaTime = (now-this.past)/30.0;
    this.rotationX = (this.rotationX-deltaTime)%(-2*Math.PI);

    this.past = now;
};

Sky.prototype.getNbTriangles = function() {
    return this.mesh.length/3;
};

function Sun() {
    // this.ratio = gl.canvas.width/gl.canvas.height;
    this.ratio = 1.0;
    this.mesh = [];
    this.clarity = 64;
    this.radiusX = 0.03;
    this.position = [0, 0, -1, 0];

    this.radiusY = this.radiusX*this.ratio;
    this.color = [254, 255, 255];

    this.parseColor();
    this.createMesh();
}

Sun.prototype.parseColor = function() {
    for(let g = 0; g < this.color.length; g++)
        this.color[g] /= 255;
};

Sun.prototype.createMesh = function() {
    let rotation = 0;
    let partition = 2*Math.PI/this.clarity;

    let c1 = this.radiusX, s1 = 0;
    let c2, s2;
    for(let t = 0; t <= this.clarity; t++) {
        rotation += partition;

        c2 = Math.cos(rotation)*this.radiusX;
        s2 = Math.sin(rotation)*this.radiusY;

        this.mesh.push(c1, s1, -0.95);
        this.mesh.push(c2, s2, -0.95);
        this.mesh.push(0, 0, -0.95);

        c1 = c2;
        s1 = s2;
    }
};

Sun.prototype.getNbTriangles = function() {
    return this.mesh.length/3;
};

function drawScene(time) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    sky.act(time);

    let quaternion = matrices["quarternion"]();
    let quaternionRot = matrices["fromEuler"](quaternion, sky.rotationX/Math.PI*360, sliders[4].value, sliders[5].value);
    let quaternionMatrix = matrices["fromQuat"](matrices["idMatrix"](), quaternionRot);

    let cameraTranslation = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);
    let objectRotX = matrices["rotationX"](sliders[6].value);
    let objectRotY = matrices["rotationY"](sliders[7].value);
    let objectRotZ = matrices["rotationZ"](sliders[8].value);
    let perspective = matrices["perspective"](sliders[9].value, gl.canvas.width/gl.canvas.height, 1, 200);

    let modelMatrix = multiplyMatrices(objectRotX, objectRotY, objectRotZ);

    let quaternionCamera = inverseMatrix(multiplyMatrices(quaternionMatrix, cameraTranslation));

    //Sky Program
    gl.useProgram(skyProgram);

    let vector = multiplyVector(quaternionCamera, sky.sun.position);
    let sunCenterDistance = normalize(vector);

    let sunCenterDistanceX = cross(sunCenterDistance, [-1, 0, 0]);
    let pitch = dot(sunCenterDistanceX, [0, 0.5, -1]);

    let sunCenterDistanceY = cross(sunCenterDistance, [0, -1, 0]);
    let yaw = dot(sunCenterDistanceY, [0, 0, -1]);

    gl.uniform1f(unifPitchLoc, pitch);
    gl.uniform1f(unifYawLoc, yaw);
    gl.uniform3fv(unifColorSkyLoc, sky.color);
    vector[1] *= (-1);
    gl.uniform3fv(unifSunSkyLoc, vector);

    gl.bindBuffer(gl.ARRAY_BUFFER, positionSkyBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sky.mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPosSkyLoc, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(attribPosSkyLoc);

    gl.drawArrays(gl.TRIANGLES, 0, sky.getNbTriangles());

    //Sun Program
    gl.useProgram(sunProgram);

    gl.uniformMatrix4fv(unifViewLoc, false, modelMatrix);
    gl.uniformMatrix4fv(unifCameraLoc, false, quaternionCamera);
    gl.uniformMatrix4fv(unifProjectionLoc, false, perspective);

    gl.uniform3fv(unifColorLoc, sky.sun.color);
    gl.uniform3fv(unifSunOriginLoc, sky.sun.position.slice(0, 3));
    gl.uniform1f(unifRadiusLoc, sky.sun.radiusX);

    gl.bindBuffer(gl.ARRAY_BUFFER, positionSunBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(sky.sun.mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPosSunLoc, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(attribPosSunLoc);

    gl.drawArrays(gl.TRIANGLES, 0, sky.sun.getNbTriangles());

    requestAnimationFrame(drawScene);
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

let unifCameraLoc, unifViewLoc, unifProjectionLoc;
let sunProgram, skyProgram;
let attribPosSunLoc, attribPosSkyLoc;
let unifColorLoc, unifSunSkyLoc;
let unifSunOriginLoc, unifRadiusLoc;

let unifPitchLoc, unifYawLoc, unifColorSkyLoc;

let positionSkyBuffer, positionSunBuffer;

let sky = new Sky();
startWebGL(gl);

function startWebGL(gl) {
    resize(gl);
    let vertexSunShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSunSource);
    let fragmentSunShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSunSource);

    sunProgram = createProgram(gl, vertexSunShader, fragmentSunShader);

    let vertexSkyShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSkySource);
    let fragmentSkyShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSkySource);

    skyProgram = createProgram(gl, vertexSkyShader, fragmentSkyShader);

    gl.useProgram(sunProgram);

    gl.clearColor(0, 0, 0, 0);
    gl.enable(gl.BLEND);
    // gl.enable(gl.DEPTH_TEST);
    // gl.depthFunc(gl.LESS);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    attribPosSunLoc = gl.getAttribLocation(sunProgram, "a_sunPos");
    unifColorLoc = gl.getUniformLocation(sunProgram, "u_color");
    unifViewLoc = gl.getUniformLocation(sunProgram, "u_model");
    unifCameraLoc = gl.getUniformLocation(sunProgram, "u_camera");
    unifProjectionLoc = gl.getUniformLocation(sunProgram, "u_projection");
    
    unifSunOriginLoc = gl.getUniformLocation(sunProgram, "u_sunOrigin");
    unifRadiusLoc = gl.getUniformLocation(sunProgram, "u_radius");

    gl.useProgram(skyProgram);
    attribPosSkyLoc = gl.getAttribLocation(skyProgram, "a_skyPos");
    unifPitchLoc = gl.getUniformLocation(skyProgram, "u_pitch");
    unifYawLoc = gl.getUniformLocation(skyProgram, "u_yaw");
    unifColorSkyLoc = gl.getUniformLocation(skyProgram, "u_color");
    unifSunSkyLoc = gl.getUniformLocation(skyProgram, "u_sunPos");

    positionSunBuffer = gl.createBuffer();
    positionSkyBuffer = gl.createBuffer();

    requestAnimationFrame(drawScene);
}