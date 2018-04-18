let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let sliders = [
    {
        label : "speedX",
        valueStart : -2,
        valueEnd : 2,
        value : 0,
        measurement : "ms"
    }, {
        label : "speedY",
        valueStart : -10,
        valueEnd : 10,
        value : 0,
        measurement : "ms"
    }, {
        label: "speedZ",
        valueStart: 0,
        valueEnd: 400,
        value: 0,
        measurement: "ms"
    }, {
        label: "fieldOfView",
        valueStart: 1,
        valueEnd: Math.PI,
        value: Math.PI/3*2,
        measurement: "f"
    }, {
        label : "rotationX",
        valueStart : 0,
        valueEnd : 360,
        value : 0,
        measurement : "°"
    }, {
        label : "rotationY",
        valueStart : 0,
        valueEnd : 360,
        value : 0,
        measurement : "°"
    }, {
        label : "rotationZ",
        valueStart : 0,
        valueEnd : 360,
        value : 0,
        measurement : "°"
    }
];

setSliders(sliders, drawScene, false, gl);

let vertexShaderSource = `
attribute vec4 a_position;
varying float v_depth;
varying float v_far;
varying vec4 vTextureCoord;
uniform mat4 u_matrix;
uniform float u_random;
varying float v_random;


vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P) {
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

void main() {
    float p = cnoise(vec3(a_position.xy, u_random)*10.0);
    vec4 newpos = a_position+a_position*p/20.0;

    vec4 pos = u_matrix*newpos;
    pos.y -= 1.0;
    // Clipping triangles that their vertices are in the opposite Y direction.
    if(pos.y < -1.0+0.04 && pos.y > -1.0)
        return;
    if(pos.y < -1.0)
        pos.y = pos.y+1.0;
     
    vTextureCoord = pos;
    pos.yz /= (pos.y+1.0+0.1)*5.0;
    gl_Position = vec4(pos.xyz, 1.0);
    
    v_depth = pos.z;
    v_far = pos.y;
    v_random = u_random;
}
`;

let fragmentShaderSource = `
precision mediump float;
varying float v_depth;
varying float v_far;
varying vec4 vTextureCoord;
uniform float u_depth_current;
uniform float u_depth_max;
uniform sampler2D u_water;
uniform float u_translation;
varying float v_random;

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P) {
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod289(Pi0);
  Pi1 = mod289(Pi1);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

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
    float y = -(vTextureCoord.y+u_translation);
    if(y > 1.0)
        y = y-1.0;
        
    float p = cnoise(vec3(vTextureCoord.x, y, v_random)*10.0);
    
    vec4 color = texture2D(u_water, (vec2(vTextureCoord.x/2.0+0.5, y)));
    color.b = color.b + color.b*p/5.0;
    
    vec3 hsv = rgb2hsv(color.rgb);
    
    hsv.z = 0.85 - v_depth*u_depth_max/u_depth_current/5.5;
    vec3 rgb = hsv2rgb(hsv);
    
    float ex = 0.2+1.0/exp(pow(v_far+1.0, 15.0));
    gl_FragColor = vec4(rgb.rgb, ex);
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

function Ocean() {
    this.speed = -3.0;
    this.mesh = [];
    this.honeycomb = [];
    this.translation = matrices["translation"](0, this.speed, 0);
    this.setHoneycomb();
    this.parseHoneycomb();
}

function Point3D(x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
}

Point3D.prototype.xyz  = function() {
    return [this.x, this.y, this.z];
};

Ocean.prototype.setHoneycomb = function() {
    let dimX = gl.canvas.width;
    let dimY = gl.canvas.height/2;
    this.partitionX = 15;
    this.partitionY = 8;
    for(let x = -this.partitionX*2; x < dimX+this.partitionX*2; x += this.partitionX) {
        let layer = [];
        for(let y = -this.partitionY*2; y < dimY+this.partitionY*2; y += this.partitionY) {
            let coordX = x + Math.random()*this.partitionX;
            let coordY = y + Math.random()*this.partitionY;
            let coordZ = Math.random()*200;
            layer.push(new Point3D(coordX, coordY, coordZ));
        }
        this.honeycomb.push(layer);
    }
};

Ocean.prototype.parseHoneycomb  = function() {
    for(let g = 0; g < this.honeycomb.length-1; g++) {
        for(let h = 0; h < this.honeycomb[g].length-1; h++) {
            this.mesh = this.mesh.concat(
                this.honeycomb[g][h].xyz(),
                this.honeycomb[g][h+1].xyz(),
                this.honeycomb[g+1][h+1].xyz(),

                this.honeycomb[g+1][h+1].xyz(),
                this.honeycomb[g+1][h].xyz(),
                this.honeycomb[g][h].xyz()
            );
        }
    }
};

Ocean.prototype.fetchTrianglesNumber = function() {
    return this.mesh.length/3;
};

function Noise() {
    this.noise = [];
    this.speed = 10;
    this.natural = 40;
    this.smoothness = 320;
    this.setNoise();
    this.index = 0;
}

Noise.prototype.setNoise = function() {
    let ran = Math.random()*200;
    this.noise.push(ran);
    for(let g = 1; g < this.natural*this.speed/2; g++) {
        ran = this.noise[g-1]+Math.random()/this.smoothness;
        this.noise.push(ran);
    }

    for(let g = 0; g < this.natural*this.speed/2; g++) {
        this.noise.push(this.noise[this.natural*this.speed/2-g-1]);
    }
};

Noise.prototype.getNoise = function() {
    this.index = (this.index+1)%(this.natural*this.speed);
    return this.noise[this.index];
};

let ocean = new Ocean();
let noise = new Noise();

function drawScene(gl) {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    let perspective = matrices["perspective"](sliders[3].value, gl.canvas.width/gl.canvas.height, 1, 2000);
    let projection = matrices["projection"](gl.canvas.width, gl.canvas.height, 2000);

    ocean.speed = sliders[1].value;
    if(ocean.translation[13]-ocean.speed > gl.canvas.height/2)
        ocean.translation = matrices["translation"](0, 0, 0);
    else
        ocean.translation = matrices["translation"](0, ocean.translation[13]-ocean.speed, 0);

    let matrix = multiplyMatrices(ocean.translation, projection);

    gl.uniform1f(uniformTranslation, ocean.translation[13]/gl.canvas.height*2.0-1.0);
    gl.uniformMatrix4fv(uniformMatrixLocation, false, matrix);
    gl.uniform1f(uniformDepthCurrentLocation, 200);
    gl.uniform1f(uniformDepthMaxLocation, 2000);
    gl.uniform1f(uniformRandomLocation, noise.getNoise());

    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(ocean.mesh), gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(vertexPositionLocation, 3, gl.FLOAT, false, 0, 0);
    gl.drawArrays(gl.TRIANGLES, 0, ocean.fetchTrianglesNumber());
    requestAnimationFrame(drawScene.bind(this, gl));
}

let uniformMatrixLocation;
let vertexPositionLocation;
let uniformDepthCurrentLocation;
let uniformDepthMaxLocation;
let uniformTranslation;
let uniformRandomLocation;

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
img.src = "http://localhost:8000/texture?filename=water1.jpg";
img.addEventListener("load", startWebGL.bind(this, gl));

function startWebGL(gl) {
    // resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    vertexPositionLocation = gl.getAttribLocation(program, "a_position");
    uniformMatrixLocation = gl.getUniformLocation(program, "u_matrix");
    uniformDepthCurrentLocation = gl.getUniformLocation(program, "u_depth_current");
    uniformDepthMaxLocation = gl.getUniformLocation(program, "u_depth_max");
    uniformTranslation = gl.getUniformLocation(program, "u_translation");
    uniformRandomLocation = gl.getUniformLocation(program, "u_random");

    let texture = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);

    let bufferPosition = gl.createBuffer();
    let colorPosition = gl.createBuffer();

    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
    gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);
    gl.clearColor(0, 0, 0, 0);

    gl.useProgram(program);
    gl.enableVertexAttribArray(vertexPositionLocation);

    gl.bindBuffer(gl.ARRAY_BUFFER, bufferPosition);
    drawScene(gl);
}