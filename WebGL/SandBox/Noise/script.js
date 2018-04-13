let Noise = require("./../../Tools/Noise/SimplexNoise");
let noise = new Noise();
let data = [];
for(let g = 0; g < 10; g += 0.01) {
    data.push(noise.noise2D(g*2, 0));
}

console.log(JSON.stringify(data));