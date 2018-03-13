let canvas = document.querySelector("canvas");
let ctx = canvas.getContext("2d");

ctx.canvas.width = canvas.getBoundingClientRect().width;
ctx.canvas.height = canvas.getBoundingClientRect().height;

let simplex = new SimplexNoise();
let i = 0;

let drawScene = () => {
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
    for(let g = 0; g < ctx.canvas.width/3; g++) {
        for(let h = 0; h < ctx.canvas.height/3; h++) {
            ctx.fillStyle = "rgba(0, 0, " + Math.floor((255*(simplex.noise3D(g/24, h/24, i)+1.0)/2.0*0.5)+127) + ", 1)";
            ctx.fillRect(g, h, 1, 1);
        }
    }
    i = (i+1)%20;
    requestAnimationFrame(drawScene);
};

if(ctx) {
    drawScene();
}