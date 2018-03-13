let http = require("http");
let fs = require("fs");
let Router = require("./router");
let router = new Router();

http.createServer(function(request, response) {
    if(!router.resolve(request, response)) {
        server(request, response);
    }
}).listen(8000);

router.add("GET", /^\/obj/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    fs.readFile("./Resources/" + query.filename, "utf-8", function(err, data) {
        response.writeHead("200", {
            "Content-Type" : "application/json",
            "Access-Control-Allow-Origin" : "*"
        });
        response.write(JSON.stringify(readObject(data)));
        response.end();
    });
});

router.add("GET", /^\/texture/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    fs.readFile("./Resources/" + query.filename, function(err, data) {
        response.writeHead("200", {
            "Content-Type" : "image/jpeg",
            "Access-Control-Allow-Origin" : "*"
        });
        response.write(data);
        response.end();
    });
});

function readObject(data) {
    let verticesFetched = data.match(/v(\s(-?\d\.\d+)){3}/g);
    let ordersFetched = data.match(/f(\s(\d\/?)+){3}/g);
    let verticesTextureFetched = data.match(/vt(\s(\d\.\d+)){2}/g);

    let vertices = [];
    verticesFetched.forEach(function(vertex) {
        vertex.replace(/(-?\d\.\d*)/g, function(e) {
            vertices.push(e);
        });
    });

    let orders = [];
    ordersFetched.forEach(function(vertex) {
        vertex.replace(/(\d+)/g, function(e) {
            orders.push(e);
        });
    });

    let verticesTexture = [];
    verticesTextureFetched.forEach(function(vertex) {
        vertex.replace(/(-?\d\.\d*)/g, function(e) {
            verticesTexture.push(Number(e));
        });
    });

    let object = [];
    for(let g = 0; g < orders.length; g += 3) {
        for(let h = 0; h < 3; h++)
            object.push(Number(vertices[(orders[g]-1)*3+h]));
    }

    let texture = [];
    for(let g = 1; g < orders.length; g += 3) {
        for(let h = 0; h < 2; h++)
            texture.push(verticesTexture[(orders[g]-1)*2+h]);
    }

    return {
        "vertices" : object,
        "texture" : texture
    };
}