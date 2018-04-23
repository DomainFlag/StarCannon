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

let vertexShaderSource = `
    attribute vec4 a_position;
    
    uniform mat4 u_model;
    uniform mat4 u_camera;
    uniform float u_tail;
    uniform float u_scalarTailNormalizer;
    
    varying float v_far;
    varying float v_dispersion;
    
    void main() {
        gl_PointSize = 1.5;
        
        vec4 pos = u_camera*u_model*vec4(a_position.xyz, 1.0);
        gl_Position = pos;
        
        float len = length(vec3(0, a_position.xy))/a_position.w;
        
        v_far = 1.0/exp(-2.0*(a_position.z+u_tail/2.0))/u_scalarTailNormalizer;
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

function Blaster() {
    this.nbShots = 0;
    this.shots = [];

    this.addEventListener();
    this.renderShots();
}

Blaster.prototype.renderShots = function() {
    this.shots.forEach((shot) => {
        shot.renderShot();
    })
};

Blaster.prototype.addEventListener = function() {
    document.addEventListener("click", function() {
        this.fireShot([-0.35, -0.5, 0]);
        this.fireShot([0.35, -0.5, 0]);
    }.bind(this));
};

Blaster.prototype.fireShot = function(translation) {
    this.shots.push(
        new Shot(translation)
    );
    this.nbShots++;
};

Blaster.prototype.act = function() {
    this.shots = this.shots.filter((shot) => {
       shot.act();
       return distanceVecs(shot.translation, [0, 0, 0]) < 30;
    });
};

function Shot(translation) {
    this.direction = [0, 0, -1/2];
    this.translation = translation;
    this.rotation = [0, 0, 0];
    this.tail = 0.5;
    this.color = [114, 236, 254];
    this.errorY = 0.005;
    this.amplitude = 0.6;

    this.nbParticles = 800;
    this.range = 0.8;

    this.particles = [];

    this.initializeShot();
}

Shot.prototype.initializeShot = function() {
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    this.program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(this.program);

    this.attribPosLoc = gl.getAttribLocation(this.program, "a_position");
    this.unifModelViewLoc = gl.getUniformLocation(this.program, "u_model");
    this.unifCameraLoc = gl.getUniformLocation(this.program, "u_camera");
    this.unifColorLoc = gl.getUniformLocation(this.program, "u_color");
    this.unifTailLoc = gl.getUniformLocation(this.program, "u_tail");
    this.unifScalarTailNormalizerLoc = gl.getUniformLocation(this.program, "u_scalarTailNormalizer");

    this.posBuffer = gl.createBuffer();

    gl.enableVertexAttribArray(this.attribPosLoc);

    for(let g = 0; g < this.nbParticles; g++) {
        this.initializeParticle().forEach((el) => {
            this.particles.push(el);
        });
    }

    gl.bindBuffer(gl.ARRAY_BUFFER, this.posBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(this.particles), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(this.attribPosLoc, 4, gl.FLOAT, false, 0, 0);

    this.normalizeColor();
};

Shot.prototype.normalizeColor = function() {
    this.color = this.color.map((col) => col/255.0);
};

Shot.prototype.initializeParticle = function() {
    let z = Math.random()*this.tail-this.tail/2.0; //->[-this.tail, this.tail]

    return this.projectParticleProperties(z);
};

Shot.prototype.projectParticleProperties = function(z) {
    let max = this.getMaxBell(z);
    let y = Math.random()*(max+this.errorY*Math.random())-((max-this.errorY)/2.0);

    let rotZ = Math.random()*2*Math.PI;
    let rotation = matrices["rotationZ"](rotZ);

    let particle = multiplyVector(rotation, [0, y, z, 0]);
    particle.push(max);

    return particle;
};

Shot.prototype.bell = function(x) {
    //Bell because the derivative of function is bell shape if Math.abs is used
    return Math.sqrt(Math.log(x))/Math.pow(x, 6)*this.amplitude; //->D = [1, +inf]
};

Shot.prototype.getMaxBell = function(x) {
    let bellXNormalized = (x+this.tail/2.0); //->[0, this.tail]
    let bellXRange = bellXNormalized*this.range+1.0; //->[1.0, this.tail*this.range]

    return this.bell(bellXRange);
};

Shot.prototype.renderShot = function() {
    gl.useProgram(this.program);

    let translation = matrices["translation"](this.translation[0], this.translation[1], this.translation[2]);
    let rotX = matrices["rotationX"](this.rotation[0]);
    let rotY = matrices["rotationY"](this.rotation[1]);
    let rotZ = matrices["rotationZ"](this.rotation[2]);

    let modelView = multiplyMatrices(rotX, rotY, rotZ, translation);
    let perspective = matrices["perspective"](Math.PI/3, gl.canvas.width/gl.canvas.height, 0.001, 30);

    gl.uniformMatrix4fv(this.unifModelViewLoc, false, modelView);
    gl.uniformMatrix4fv(this.unifCameraLoc, false, perspective);
    gl.uniform3fv(this.unifColorLoc, this.color);
    gl.uniform1f(this.unifTailLoc, this.tail);
    gl.uniform1f(this.unifScalarTailNormalizerLoc, 1/Math.exp(-2.0*this.tail));

    gl.drawArrays(gl.POINTS, 0, this.particles.length/4);
};

Shot.prototype.act = function() {
    this.translation = addValues(this.translation, this.direction);
};

let blaster = new Blaster();

function drawShots(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.COLOR_BUFFER_BIT);

    blaster.renderShots();
    blaster.act();

    requestAnimationFrame(drawShots.bind(this, gl));
}

if(gl) {
    resize(gl);

    gl.clearColor(38/255, 67/255, 72/255, 1);
    gl.enable(gl.BLEND);
    gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    drawShots(gl);
}