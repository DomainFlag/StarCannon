let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

function resize(gl) {
    let realToCSSPixels = window.devicePixelRatio;

    let displayWidth  = Math.floor(gl.canvas.clientWidth  * realToCSSPixels);
    let displayHeight = Math.floor(gl.canvas.clientHeight * realToCSSPixels);

    // Check if the canvas is not the same size.
    if (gl.canvas.width  !== displayWidth ||
        gl.canvas.height !== displayHeight) {

        // Make the canvas the same size
        gl.canvas.width  = displayWidth;
        gl.canvas.height = displayHeight;
    }
}

let sliders = [
    {
        label : "translX",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "dp"
    }, {
        label : "translY",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "dp"
    }, {
        label: "translZ",
        valueStart : -1,
        valueEnd : 1,
        value : 0,
        measurement : "dp"
    }, {
        label : "rotX",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "rotY",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label : "rotZ",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
        measurement : "°"
    }, {
        label: "fieldOfView",
        valueStart: -1,
        valueEnd: 2,
        value: 1,
        measurement: "f"
    }
];

setSliders(sliders, drawThruster, false, gl);

let vertexShaderSource = `
    attribute vec4 a_position;
    
    uniform mat4 u_model;
    uniform float u_tail;
    uniform float u_scalarTailNormalizer;
    
    varying float v_far;
    varying float v_dispersion;
    
    void main() {
        gl_PointSize = 1.5;
        
        vec4 pos = u_model*vec4(a_position.xyz, 1.0);
        gl_Position = vec4(pos.xy, 0.0, 1.0);
        
        float len = length(vec3(0, a_position.yz))/a_position.w;
        
        v_far = 1.0/exp(-2.0*(a_position.x+u_tail/2.0))/u_scalarTailNormalizer;
        v_dispersion = len/5.0;
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    
    uniform vec3 u_color;
    
    varying float v_far;
    varying float v_dispersion;
    
    //RGB to HSV convert
    vec3 rgb2hsv(vec3 c) {
        vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
        vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
        vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    
        float d = q.x - min(q.w, q.y);
        float e = 1.0e-10;
        return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
    }
     
    //HSV to RGB convert
    vec3 hsv2rgb(vec3 c) {
        vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
        vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
        return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
    }
    
    void main() {
        vec3 hsv = rgb2hsv(u_color);
        hsv.r += v_dispersion;
        hsv.b -= v_far;
        hsv.b -= v_dispersion;
        vec3 color = hsv2rgb(hsv);
        
        gl_FragColor = vec4(color, 1.0-v_far);
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

function Thruster() {
    this.maxSpeed = 0.05;
    this.minSpeed = 0.01;
    this.stepSpeed = 0.005;
    this.speed = this.minSpeed;

    this.tail = 0.5;
    this.color = [114, 236, 254];
    this.errorX = 0.01;
    this.errorY = 0.005;
    this.amplitude = 0.6;

    this.nbParticles = 60000;
    this.range = 0.8;

    this.particles = [];

    this.initializeThruster();
}

Thruster.prototype.initializeThruster = function() {
    for(let g = 0; g < this.nbParticles; g++) {
        this.initializeParticle().forEach((el) => {
            this.particles.push(el);
        });
    }

    this.normalizeColor();
    this.addEventListener();
};

Thruster.prototype.lerp = function() {
    return this.minSpeed+(this.speed-this.minSpeed)/(this.maxSpeed-this.minSpeed)*(this.tail-0);
};

Thruster.prototype.addEventListener = function() {
    document.addEventListener("keypress", function(e) {
        if(e.which === 119) {
            if(thruster.speed < this.maxSpeed-this.stepSpeed)
                thruster.speed += this.stepSpeed;
        } else if(e.which === 115) {
            if(thruster.speed > this.minSpeed+this.stepSpeed)
                thruster.speed -= this.stepSpeed;
        } else return;

        thruster.errorX = this.lerp();
    }.bind(this));
};

Thruster.prototype.normalizeColor = function() {
    this.color = this.color.map((col) => col/255.0);
};

Thruster.prototype.projectParticleProperties = function(x) {
    let max = this.getMaxBell(x);
    let y = Math.random()*(max+this.errorY*Math.random())-((max-this.errorY)/2.0);

    let rotX = Math.random()*2*Math.PI;
    let rotation = matrices["rotationX"](rotX);

    let particle = multiplyVector(rotation, [x, y, 0, 0]);
    particle.push(max);

    return particle;
};

Thruster.prototype.initializeParticle = function() {
    let x = Math.random()*this.tail-this.tail/2.0; //->[-this.tail, this.tail]

    return this.projectParticleProperties(x);
};

Thruster.prototype.recycle = function() {
    let x = -this.tail/2.0+Math.random()*this.errorX; //->[-this.tail+err, -this.tail+err]

    return this.projectParticleProperties(x);
};

Thruster.prototype.bell = function(x) {
    //Bell because the derivative of function is bell shape if Math.abs is used
    return Math.sqrt(Math.log(x))/Math.pow(x, 6)*this.amplitude; //->D = [1, +inf]
};

Thruster.prototype.getMaxBell = function(x) {
    let bellXNormalized = (x+this.tail/2.0); //->[0, this.tail]
    let bellXRange = bellXNormalized*this.range+1.0; //->[1.0, this.tail*this.range]

    return this.bell(bellXRange);
};

Thruster.prototype.act = function() {
    for(let g = 0; g < this.particles.length; g += 4) {
        if(this.particles[g] > this.tail/2.0 ||
            Math.abs(this.particles[g+1]) > this.particles[g+3] ||
            Math.abs(this.particles[g+2]) > this.particles[g+3])

            this.recycle().forEach((el, ind) => {
                this.particles[g+ind] = el;
            });
        else {
            let proportionY = this.particles[g+1]/this.particles[g+3];
            let proportionZ = this.particles[g+2]/this.particles[g+3];

            this.particles[g] += thruster.speed*Math.random();

            let max = this.getMaxBell(this.particles[g]);
            this.particles[g+1] = proportionY*max;
            this.particles[g+2] = proportionZ*max;
            this.particles[g+3] = max;
        }
    }
};

let thruster = new Thruster();

function drawThruster(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

    let translation = matrices["translation"](sliders[0].value, sliders[1].value, sliders[2].value);
    let rotX = matrices["rotationX"](sliders[3].value);
    let rotY = matrices["rotationY"](sliders[4].value);
    let rotZ = matrices["rotationZ"](sliders[5].value);

    let modelView = multiplyMatrices(rotX, rotY, rotZ, translation);

    gl.uniformMatrix4fv(unifModelViewLoc, false, modelView);
    gl.uniform3fv(unifColorLoc, thruster.color);
    gl.uniform1f(unifTailLoc, thruster.tail);
    gl.uniform1f(unifScalarTailNormalizerLoc, 1/Math.exp(-2.0*thruster.tail));

    gl.bindBuffer(gl.ARRAY_BUFFER, posBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(thruster.particles), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(attribPosLoc, 4, gl.FLOAT, false, 0, 0);

    gl.drawArrays(gl.POINTS, 0, thruster.nbParticles);

    requestAnimationFrame(drawThruster.bind(this, gl));

    thruster.act();
}

let thrusterProgram;
let attribPosLoc;
let unifModelViewLoc, unifColorLoc, unifTailLoc, unifScalarTailNormalizerLoc;
let posBuffer;

if(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    thrusterProgram = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(thrusterProgram);

    attribPosLoc = gl.getAttribLocation(thrusterProgram, "a_position");
    unifModelViewLoc = gl.getUniformLocation(thrusterProgram, "u_model");
    unifColorLoc = gl.getUniformLocation(thrusterProgram, "u_color");
    unifTailLoc = gl.getUniformLocation(thrusterProgram, "u_tail");
    unifScalarTailNormalizerLoc = gl.getUniformLocation(thrusterProgram, "u_scalarTailNormalizer");

    posBuffer = gl.createBuffer();

    gl.clearColor(38/255, 67/255, 72/255, 1);
    // gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    gl.enableVertexAttribArray(attribPosLoc);

    drawThruster(gl);
}