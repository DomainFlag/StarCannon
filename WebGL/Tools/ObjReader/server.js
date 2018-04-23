let http = require("http");
let fs = require("fs");
let Router = require("./router");
let router = new Router();

http.createServer(function(request, response) {
    if(request.method === "OPTIONS") {
        response.writeHead(200, {
            "Access-Control-Allow-Origin" : "*",
            'Access-Control-Allow-Methods' : 'GET, POST, OPTIONS, PUT, PATCH, DELETE'
        });
        response.end();
    } else
        if(!router.resolve(request, response)) {
            server(request, response);
        }
}).listen(8000);

function JSONresponse(response, data) {
    response.writeHead(200, {
        "Content-Type" : "application/json",
        "Access-Control-Allow-Origin" : "*"
    });
    response.write(JSON.stringify(data));
    response.end();
}

function JPEGresponse(response, data) {
    response.writeHead(200, {
        "Content-Type" : "image/jpeg",
        "Access-Control-Allow-Origin" : "*"
    });
    response.write(data);
    response.end();
}

router.add("GET", /^\/obj_folder/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    let data = {
        "objUrl" : null,
        "mtlUrl" : null,
        "texUrls" : []
    };

    fs.readdir("./resources/" + query.foldername, (err, files) => {
        files.forEach((fileName) => {
            if(/.*\.jpg$/.test(fileName))
                data["texUrls"].push(fileName);
            else if(/.*\.obj$/.test(fileName))
                data["objUrl"] = fileName;
            else if(/.*\.mtl$/.test(fileName))
                data["mtlUrl"] = fileName;
        });

        JSONresponse(response, data);
    });
});

router.add("GET", /^\/texture/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    fs.readFile("./resources/" + query.filename, function(err, data) {
        JPEGresponse(response, data);
    });
});


router.add("GET", /^\/mtl/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    fs.readFile("./resources/" + query.filename, "utf-8", function(err, data) {
        JSONresponse(response, readMtl(data));
    });
});

router.add("POST", /^\/data/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    let materials = "";
    request.on("data", (chunk) => {
        materials += chunk;
    });
    request.on("end", () => {
        fs.readFile("./resources/" + query.objName, "utf-8", function(err, data) {
            let fetchedData = readObject(data, JSON.parse(materials));
            JSONresponse(response, fetchedData);
        });
    });
});

function readMtl(data) {
    let materials = {};

    let nb = 0;
    data.split("newmtl").forEach((ln) => {
        let dt1 = ln.match(/\s(\w+:)?(\w+)/);
        let dt2 = ln.match(/map_Kd\s(.*\.jpg)/);

        if(dt1 != null && dt2 != null)
            materials[dt1[2]] = {
                "texture" : dt2[1],
                "unit" : nb++
            };
        else if(dt1 != null)
            materials[dt1[2]] = {
                "texture" : null,
                "unit" : -1
            }
    });

    return materials;
}

function readObject(data, materials) {
    let obj = {
        "geometricVertices" : [],
        "textureVertices" : [],
        "normalVertices" : []
    };

    let indexGeometricVertices = data.match(/v(\s(-?\d+\.\d+)){3}/g).map((occ) => occ.split(" ").slice(1, 4).map((nb) => Number(nb)));
    let indexTextureVertices = data.match(/vt(\s(-?\d+\.\d+)){2}/g).map((occ) => occ.split(" ").slice(1, 3).map((nb) => Number(nb)));
    let indexNormalVertices = data.match(/vn(\s(-?\d+\.\d+)){3}/g).map((occ) => occ.split(" ").slice(1, 4).map((nb) => Number(nb)));

    let chunks = data.split("usemtl");
    let orders;

    for(let g = 1; g < chunks.length; g++) {
        let chunk = chunks[g];

        let match = chunk.match(/\s(\w+:)?(\w+)/);
        let material = match[2];

        let tex = materials[material].unit;

        orders = chunk.match(/f(\s(\d\/?)+){3}/g);

        if(orders != null)
            orders.forEach((order) => {
                order.replace(/(\d+)\/(\d+)\/(\d+)/g, function(gr, e1, e2, e3) {
                    indexGeometricVertices[e1-1].forEach((val) => {
                        obj["geometricVertices"].push(val);
                    });

                    indexTextureVertices[e2-1].forEach((val) => {
                        obj["textureVertices"].push(val);
                    });
                    obj["textureVertices"].push(tex);

                    indexNormalVertices[e3-1].forEach((val) => {
                        obj["normalVertices"].push(val);
                    });

                });
            });
    }

    return obj;
}