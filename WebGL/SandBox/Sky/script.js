let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let sliders = [
    {
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
    }
];

setSliders(sliders, drawScene, true, gl);

let vertexShaderSunSource = `
    attribute vec4 a_sunPos;
    
    uniform mat4 u_camera;
    uniform mat4 u_projection;
    
    varying vec3 v_sunPos;
    
    void main() {
        vec4 pos = u_projection*u_camera*a_sunPos;
        gl_Position = pos;
        
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
        //Workaround for simple radial effect that projects to the cube box
        float dist = (1.0-distance(vec3(v_skyLoc.x, v_skyLoc.y+u_sunPos.y, v_skyLoc.z), u_sunPos)/(2.0*sqrt(2.0)));
        
        vec3 hsv = rgb2hsv(u_color.rgb);
        
        hsv.rb *= u_pitch;
        
        //Gamma cycle correction
        hsv.r -= pow(log((u_pitch)/2.0+1.0), 1.0/2.0);
        
        //Luminosity cycle
        // hsv.b -= (1.0-u_pitch)/3.0;
        
        //Radial gradient
        hsv.b += dist;
        
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

    let deltaTime = (now-this.past)/5.0;
    this.rotationX = (this.rotationX-deltaTime)%(-2*Math.PI);

    this.past = now;
};

Sky.prototype.getNbTriangles = function() {
    return this.mesh.length/3;
};

Sky.prototype.init = function() {
    let vertexSkyShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSkySource);
    let fragmentSkyShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSkySource);
    this.skyProgram = createProgram(gl, vertexSkyShader, fragmentSkyShader);

    gl.useProgram(this.skyProgram);
    this.attribPosSkyLoc = gl.getAttribLocation(this.skyProgram, "a_skyPos");
    this.unifPitchLoc = gl.getUniformLocation(this.skyProgram, "u_pitch");
    this.unifYawLoc = gl.getUniformLocation(this.skyProgram, "u_yaw");
    this.unifColorSkyLoc = gl.getUniformLocation(this.skyProgram, "u_color");
    this.unifSunSkyLoc = gl.getUniformLocation(this.skyProgram, "u_sunPos");

    this.positionSkyBuffer = gl.createBuffer();

    this.sun.init();
};

Sky.prototype.render = function(viewMatrix, projection) {
    gl.useProgram(this.skyProgram);

    gl.enableVertexAttribArray(this.attribPosSkyLoc);

    gl.bindBuffer(gl.ARRAY_BUFFER, this.positionSkyBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this.mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(this.attribPosSkyLoc, 3, gl.FLOAT, false, 0, 0);

    let vector = multiplyVector(viewMatrix, this.sun.position);

    let pitch = Math.sin(-sliders[0].value/360*2*Math.PI)+0.3;
    let yaw = Math.cos(sliders[1].value/360*2*Math.PI);

    gl.uniform1f(this.unifPitchLoc, pitch);
    gl.uniform1f(this.unifYawLoc, yaw);
    gl.uniform3fv(this.unifColorSkyLoc, this.color);
    gl.uniform3fv(this.unifSunSkyLoc, vector);

    gl.drawArrays(gl.TRIANGLES, 0, this.getNbTriangles());

    sky.sun.render(viewMatrix, projection);
};

function Sun() {
    this.ratio = 1.0;
    this.mesh = [];
    this.clarity = 64;
    this.radiusX = 0.03;
    this.position = [0, 0, -1, 0];

    this.radiusY = this.radiusX*this.ratio;
    this.color = [254, 255, 255];

    this.init();
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

        this.mesh.push(c1, s1, -1);
        this.mesh.push(c2, s2, -1);
        this.mesh.push(0, 0, -1);

        c1 = c2;
        s1 = s2;
    }
};

Sun.prototype.getNbTriangles = function() {
    return this.mesh.length/3;
};

Sun.prototype.init = function() {
    let vertexSunShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSunSource);
    let fragmentSunShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSunSource);
    this.sunProgram = createProgram(gl, vertexSunShader, fragmentSunShader);

    gl.useProgram(this.sunProgram);

    this.attribPosSunLoc = gl.getAttribLocation(this.sunProgram, "a_sunPos");
    this.unifColorLoc = gl.getUniformLocation(this.sunProgram, "u_color");
    this.unifCameraLoc = gl.getUniformLocation(this.sunProgram, "u_camera");
    this.unifProjectionLoc = gl.getUniformLocation(this.sunProgram, "u_projection");

    this.unifSunOriginLoc = gl.getUniformLocation(this.sunProgram, "u_sunOrigin");
    this.unifRadiusLoc = gl.getUniformLocation(this.sunProgram, "u_radius");

    this.positionSunBuffer = gl.createBuffer();
};

Sun.prototype.render = function(viewMatrix, projection) {
    gl.useProgram(this.sunProgram);

    gl.enableVertexAttribArray(this.attribPosSunLoc);

    gl.bindBuffer(gl.ARRAY_BUFFER, this.positionSunBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this.mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(this.attribPosSunLoc, 3, gl.FLOAT, false, 0, 0);

    gl.uniformMatrix4fv(this.unifCameraLoc, false, viewMatrix);
    gl.uniformMatrix4fv(this.unifProjectionLoc, false, projection);

    gl.uniform3fv(this.unifColorLoc, this.color);
    gl.uniform3fv(this.unifSunOriginLoc, this.position.slice(0, 3));
    gl.uniform1f(this.unifRadiusLoc, this.radiusX);

    gl.drawArrays(gl.TRIANGLES, 0, this.getNbTriangles());
};

function drawScene(time) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    // sky.act(time);

    let objectRotX = matrices["rotationX"](sliders[0].value/360*2*Math.PI);
    let objectRotY = matrices["rotationY"](sliders[1].value/360*2*Math.PI);
    let objectRotZ = matrices["rotationZ"](sliders[2].value/360*2*Math.PI);
    let perspective = matrices["perspective"](Math.PI/3.0, gl.canvas.width/gl.canvas.height, 0.01, 10);

    let viewMatrix = multiplyMatrices(objectRotX, objectRotY, objectRotZ);

    sky.render(viewMatrix, perspective);

    // requestAnimationFrame(drawScene);
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

let sky = new Sky();
startWebGL(gl);

function startWebGL(gl) {
    resize(gl);

    sky.init();

    gl.clearColor(0, 0, 0, 0);
    gl.enable(gl.BLEND);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    requestAnimationFrame(drawScene);
}