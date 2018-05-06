function Terrain(size, distance, render, offsetX, offsetZ) {
    this.noise = new SimplexNoise(Math.random());
    this.amplitude = 3.5;
    this.distance = distance;
    this.size = size+1;
    this.partition = 2*this.distance/(this.size-1);
    this.frequency = 1/(this.distance/5.0*2.0);
    this.mesh = [];
    this.honeycomb = [];
    this.setHoneycomb(offsetX, offsetZ);
    if(render)
        this.parseHoneycomb();
    else
        return this.honeycomb;
}

Terrain.prototype.radialClipping = function(x, y, z) {
    let dist = distanceVecs([x, 0, z], [0, 0, 0]);
    let prop = dist/this.distance;
    let val = Math.min(this.amplitude-this.amplitude*prop, y);

    if(prop >= 0.5)
        return 0;
    else return Math.max(val, 0);
};

Terrain.prototype.noiseGenerate = function(x, z, octaves, persistence) {
    let total = 0;
    let frequency = this.frequency;
    let amplitude = this.amplitude;
    let maxValue = 0;  //Used for normalizing result

    for(let i = 0; i < octaves; i++) {
        total += (this.noise.noise2D(x * frequency, z * frequency) * amplitude);

        amplitude *= persistence;
        frequency *= 2.0;

        maxValue++;
    }

    return this.radialClipping(x, total/maxValue, z);
};

Terrain.prototype.setHoneycomb = function(offsetX = 0, offsetZ = 0) {
    for(let z = -this.distance; z <= this.distance; z += this.partition) {
        for(let x = -this.distance; x <= this.distance; x += this.partition) {
            let coordY = this.noiseGenerate(x, z, 4, 0.5);
            this.honeycomb.push([x+offsetX, coordY, z+offsetZ]);
        }
    }
};

Terrain.prototype.parseHoneycomb  = function() {
    let cycle = [ [0, 0], [0, 1], [1, 1], [1, 1], [1, 0], [0, 0] ];
    for(let g = 0; g < this.size-1; g++) {
        for(let h = 0; h < this.size-1; h++) {
            for(let i = 0; i < cycle.length; i++) {
                let vector = this.honeycomb[(g+cycle[i][0])*(this.size-1)+ h+cycle[i][1]];
                this.mesh.push(vector[0]);
                this.mesh.push(vector[1]);
                this.mesh.push(vector[2]);
            }
        }
    }
};

Terrain.prototype.fetchTrianglesNumber = function() {
    return this.mesh.length/3;
};