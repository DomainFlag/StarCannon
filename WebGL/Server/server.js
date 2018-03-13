let Router = require("./router");
let router = new Router();
let http = require("http");
let fs = require("fs");

http.createServer(function(request, response) {
    if(!router.resolve(request, response)) {
        server(request, response);
    }
}).listen(8000);


router.add("GET", /^\/texture/, function(request, response) {
    let query = require("url").parse(request.url, true).query;
    fs.readFile("./textures/" + query.filename, function(err, data) {
        response.writeHead("200", {
            "Content-Type" : "image/jpeg",
            "Access-Control-Allow-Origin" : "*"
        });
        response.write(data);
        response.end();
    });
});
