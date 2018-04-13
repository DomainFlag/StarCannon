let canvas = document.querySelector("canvas");
let width = canvas.getBoundingClientRect().width;
let height = canvas.getBoundingClientRect().height;
let ctx = canvas.getContext("2d");

let body = document.querySelector("body");

let angle = 0;

let interval;

canvas.addEventListener("mousemove", (e) => {
    /**
     * Function equation f(x)=1/x will be used to lower the rotation from outer to center of the screen
     * Where f(x)=1/x has the domain [1/2, 4] with the image [0.25, 2] and [-4, -1/2] with the image [-2, -0.25]
     * Where, the derivative decreases from 1/2 to 4 and from -4 to -1/2
     */
    let x = ((e.pageX-body.clientWidth/2)/(body.clientWidth/2));

    let r;
    if(x < 0) {
        x = (x+1)*7/2+1/2;
        r = 1/x;
        angle += r/4.0;
    } else {
        x = -(x-1)*7/2+1/2;
        r = -1/x;
        angle += r/4.0;
        if(angle < 0)
            angle += 360;
    }

    angle %= 360;

    if(interval != null)
        clearInterval(interval);
    interval = setInterval(() => {
        console.log(x);
        angle += r/4.0;
        if(angle < 0)
            angle += 360;
        angle %= 360;
        drawScene(angle);
    }, 10);
    drawScene(angle);
});


resize(ctx);

let offsetY = ctx.canvas.height/4.0;
let dim = ctx.canvas.height-offsetY*2;
let offsetX = (ctx.canvas.width-dim)/2.0;

let partition = dim/10.0;

function drawScene(angle) {
    ctx.fillStyle = "rgb(0, 0, 0);";
    ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    ctx.font = "28px Times New Roman";
    ctx.fillText("Angle: " + parseFloat(angle).toFixed(0) + "°", ctx.canvas.width/2, 40);

    ctx.fillStyle = "gray";
    for(let g = 0; g < 10; g++) {
        for(let h = 0; h < 10; h++) {
            ctx.fillRect(offsetX+g*partition, offsetY+h*partition, partition, partition);
        }
    }

    ctx.fillStyle = "crimson";
    if(angle >= 0 && angle < 90) {
        let up = angle/10;
        let right = 9-up;

        for(let h = offsetY; right >= 0; h += partition) {
            ctx.fillRect(dim+offsetX-partition, h+partition, partition, partition);
            right--;
        }

        for(let w = offsetX; up >= 0; w -= partition) {
            ctx.fillRect(dim-partition+w, offsetY, partition, partition);
            up--;
        }
    } else if(angle >= 90 && angle < 180) {
        let left = Math.floor((angle-90)/10);
        let up = Math.ceil(10-left);

        for(let h = offsetY; left > 0; h += partition) {
            ctx.fillRect(offsetX, h+partition, partition, partition);
            left--;
        }

        for(let w = 0; up > 0; w += partition) {
            ctx.fillRect(offsetX+w, offsetY, partition, partition);
            up--;
        }
    } else if(angle >= 180 && angle < 270) {
        let bottom = (angle-180)/10;
        let left = 10-bottom;

        for(let h = 0; left >= 0; h += partition) {
            ctx.fillRect(offsetX, offsetY+dim-partition-h, partition, partition);
            left--;
        }

        for(let w = 0; bottom >= 0; w += partition) {
            ctx.fillRect(offsetX+w, offsetY+dim-partition, partition, partition);
            bottom--;
        }
    } else if(angle >= 270 && angle <= 360) {
        let right = (angle-270)/10;
        let bottom = 10-right;

        for(let h = 0; right > 0; h += partition) {
            ctx.fillRect(offsetX+dim-partition, offsetY+dim-partition-h, partition, partition);
            right--;
        }

        for(let w = 0; bottom > 0; w += partition) {
            ctx.fillRect(offsetX+dim-w-partition, offsetY+dim-partition, partition, partition);
            bottom--;
        }
    }
}

function resize(ctx) {
    let realToCSSPixels = window.devicePixelRatio;

    let displayWidth  = Math.floor(ctx.canvas.clientWidth  * realToCSSPixels);
    let displayHeight = Math.floor(ctx.canvas.clientHeight * realToCSSPixels);

    if (ctx.canvas.width  !== displayWidth ||
        ctx.canvas.height !== displayHeight) {

        ctx.canvas.width  = displayWidth;
        ctx.canvas.height = displayHeight;
    }
}

