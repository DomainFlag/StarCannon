let canvas = document.querySelector("canvas");
let gl = canvas.getContext("webgl");

gl.canvas.width = canvas.getBoundingClientRect().width;
gl.canvas.height = canvas.getBoundingClientRect().height;

let noise = new SimplexNoise();

function Terrain() {
    this.partition = 1/15;
    this.speed = this.partition/2;
    this.mesh = [];
    this.honeycomb = [];
    this.translation = [0, 0, 0];
    this.angleH = 0;
    this.angleV = 0;
    this.rx = 0;
    this.ry = 0;
    this.setHoneycomb();
    this.parseHoneycomb();
}

canvas.addEventListener("mousemove", (e) => {
    /**
     * Function equation f(x)=1/x will be used to lower the rotation from outer to center of the screen
     * Where f(x)=1/x has the domain [1/2, 4] with the image [0.25, 2] and [-4, -1/2] with the image [-2, -0.25]
     * Where, the derivative decreases from 1/2 to 4 and from -4 to -1/2
     */
    let x = ((e.pageX-canvas.clientWidth/2)/(canvas.clientWidth/2));
    let y = ((e.pageY-canvas.clientHeight/2)/(canvas.clientHeight/2));

    if(x < 0) {
        x = (x+1)*7/2+1/2;
        terrain.rx = 1/x;
    } else {
        x = -(x-1)*7/2+1/2;
        terrain.rx = -1/x;
    }

    if(y < 0) {
        y = (y+1)*7/2+1/2;
        terrain.ry = 1/y;
    } else {
        y = -(y-1)*7/2+1/2;
        terrain.ry = -1/y;
    }
});

Terrain.prototype.preserveMouse = function() {
    this.angleH += this.rx/360*2;
    this.angleV += -this.ry/360*2;
    if(this.angleH < 0)
        this.angleH += 2*Math.PI;
    if(this.angleV < 0)
        this.angleV += 2*Math.PI;
    this.angleH %= 2*Math.PI;
    this.angleV %= 2*Math.PI;
    this.act();
};

Terrain.prototype.act = function() {

    let quaternion = matrices["quarternion"]();
    let quaternionRot = matrices["fromEuler"](quaternion, -terrain.angleV/Math.PI*360, terrain.angleH/Math.PI*360, 0);

    let result = [0, 0, 0];
    result = transformQuat(result, [0, 0, -this.speed], quaternionRot);

    for(let g = 0; g < this.meshLength; g += 3) {
        this.mesh[g] += result[0];
        this.mesh[g+2] += result[2];
        this.mesh[g+1] = noise.noise2D(this.mesh[g], this.mesh[g+2])/2.5;
    }

    this.translation[0] += result[0];
    this.translation[1] += result[1];
    this.translation[2] += result[2];
};

function Vector(x, y, z) {
    this.x = x;
    this.y = y;
    this.z = z;
}

Vector.prototype.xyz  = function() {
    return [this.x, this.y, this.z];
};

Terrain.prototype.setHoneycomb = function() {
    for(let z =  -3*Math.sqrt(2)-this.partition; z < 3*Math.sqrt(2)+this.partition; z += this.partition) {
        let layer = [];
        for(let x = -3*Math.sqrt(2)-this.partition; x < 3*Math.sqrt(2)+this.partition; x += this.partition) {
            let coordX = x;
            let coordY = noise.noise2D(x, z)/2.5; //[-0.4, 0.4]
            let coordZ = z;
            layer.push(new Vector(coordX, coordY, coordZ));
        }
        this.honeycomb.push(layer);
    }
};

Terrain.prototype.parseHoneycomb  = function() {
    let cycle = [ [0, 0], [0, 1], [1, 1], [1, 1], [1, 0], [0, 0] ];
    for(let g = 0; g < this.honeycomb.length-1; g++) {
        for(let h = 0; h < this.honeycomb[g].length-1; h++) {
            for(let i = 0; i < cycle.length; i++) {
                let vector = this.honeycomb[g+cycle[i][0]][h+cycle[i][1]].xyz();
                this.mesh.push(vector[0]);
                this.mesh.push(vector[1]);
                this.mesh.push(vector[2]);
            }
        }
    }
    this.meshLength = this.mesh.length;
};

Terrain.prototype.fetchTrianglesNumber = function() {
    return this.mesh.length/3;
};

let terrain = new Terrain();

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
        value : 1.0,
        measurement : "dp"
    }, {
        label : "cameraZ",
        valueStart : -8,
        valueEnd :8,
        value : 0,
        measurement : "dp"
    }, {
        label : "cameraRotX",
        valueStart : 0,
        valueEnd : 2*Math.PI,
        value : 0,
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

let vertexShaderSource = `
    attribute vec4 a_position;
    varying float v_depth;
    varying float v_far;
    uniform mat4 u_model;
    uniform mat4 u_camera;
    uniform mat4 u_projection;
    void main() {
        vec4 camera = u_camera*u_model*a_position;
        vec4 result = u_projection*camera;
        gl_Position = result;
        v_depth = (a_position.y+1.0/2.5)*5.0/4.0; //->[0, 1]
        v_far = -camera.z; //->[0.0, inf]
    }
`;

let fragmentShaderSource = `
    precision mediump float;
    varying float v_depth;
    varying float v_far;
    uniform vec3 u_gradients[3];
    
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
    
    //HSV interpolation
    vec3 colorInterpolation() {
        float partition;
        if(v_depth < 0.7) {
            partition = v_depth/(7.0/10.0);
            return u_gradients[0]*(1.0-partition)+u_gradients[1]*partition;
        } else {
            partition = (v_depth-0.7)/(3.0/10.0);
            return u_gradients[1]*(1.0-partition)+u_gradients[2]*partition;
        }
    }
    
    void main() {
        vec3 color = hsv2rgb(colorInterpolation());
        vec4 depth_color = vec4(color.rgb, 1.0); //->[0, 0.2]
        // vec4 depth_color = vec4(color.rgb, 0.8+v_depth/5.0); //->[0, 0.2]
        vec4 far_color = vec4(depth_color.rgb, depth_color.a-pow(sqrt(exp(v_far)-1.0), 2.0)/100.0); //->(sqrt(e^x)-1)^2
        gl_FragColor = far_color;
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

    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(terrain.mesh), gl.STATIC_DRAW);
    gl.vertexAttribPointer(attribPositionLoc, 3, gl.FLOAT, false, 0, 0);

    let quaternion = matrices["quarternion"]();
    let quaternionRot = matrices["fromEuler"](quaternion, -terrain.angleV/Math.PI*360, terrain.angleH/Math.PI*360, 0);
    let quaternionMatrix = matrices["fromQuat"](matrices["idMatrix"](), quaternionRot);

    let cameraTranslation = matrices["translation"](terrain.translation[0]+sliders[0].value, terrain.translation[1]+sliders[1].value, Number(sliders[2].value)+terrain.translation[2]);
    let cameraRotX = matrices["rotationX"](terrain.angleV);
    let cameraRotY = matrices["rotationY"](terrain.angleH);
    let cameraRotZ = matrices["rotationZ"](sliders[5].value);
    let objectRotX = matrices["rotationX"](sliders[6].value);
    let objectRotY = matrices["rotationY"](sliders[7].value);
    let objectRotZ = matrices["rotationZ"](sliders[8].value);
    let perspective = matrices["perspective"](sliders[9].value, gl.canvas.width/gl.canvas.height, 0.0001, 50);

    let modelMatrix = multiplyMatrices(objectRotX, objectRotY, objectRotZ);
    let viewMatrix = multiplyMatrices(cameraRotX, cameraRotY, cameraRotZ, cameraTranslation);
    let cameraMatrix = inverseMatrix(viewMatrix);

    let quaternionCamera = inverseMatrix(multiplyMatrices(quaternionMatrix, cameraTranslation));
    gl.uniformMatrix4fv(uniformViewLocation, false, modelMatrix);
    gl.uniformMatrix4fv(uniformCameraLocation, false, quaternionCamera);
    gl.uniformMatrix4fv(uniformProjectionLocation, false, perspective);

    gl.drawArrays(gl.TRIANGLES, 0, terrain.fetchTrianglesNumber());

    terrain.preserveMouse();

    requestAnimationFrame(drawScene.bind(this, gl));
}

let uniformCameraLocation, uniformViewLocation, uniformProjectionLocation;
let uniformLowerColorLoc, uniformMedianColorLoc, uniformUpperColorLoc;
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

if(gl) {
    resize(gl);
    let vertexShader = createShader(gl, gl.VERTEX_SHADER, vertexShaderSource);
    let fragmentShader = createShader(gl, gl.FRAGMENT_SHADER, fragmentShaderSource);

    let program = createProgram(gl, vertexShader, fragmentShader);

    gl.useProgram(program);

    attribPositionLoc = gl.getAttribLocation(program, "a_position");
    uniformViewLocation = gl.getUniformLocation(program, "u_model");
    uniformCameraLocation = gl.getUniformLocation(program, "u_camera");
    uniformProjectionLocation = gl.getUniformLocation(program, "u_projection");

    uniformLowerColorLoc = gl.getUniformLocation(program, "u_gradients[0]");
    uniformMedianColorLoc = gl.getUniformLocation(program, "u_gradients[1]");
    uniformUpperColorLoc = gl.getUniformLocation(program, "u_gradients[2]");

    gl.uniform3f(uniformLowerColorLoc, 120/360, 76/100, 55/100);
    gl.uniform3f(uniformMedianColorLoc, 26/360, 36/100, 65/100);
    gl.uniform3f(uniformUpperColorLoc, 0/360, 2/100, 100/100);


    // gl.depthRange(-1.0, 1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LESS);
    // gl.enable(gl.CULL_FACE);
    // gl.cullFace(gl.BACK);
    gl.clearColor(0, 0, 0, 0);
    gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);

    let positionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, positionBuffer);
    gl.enableVertexAttribArray(attribPositionLoc);

    drawScene(gl);
}