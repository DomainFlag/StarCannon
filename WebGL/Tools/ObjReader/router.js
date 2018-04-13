let Router = module.exports = function() {
    this.routes = [];
};

Router.prototype.add = function(method, url, handler) {
    this.routes.push(
        {
            "method" : method,
            "url" : url,
            "handler" : handler
        }
    )
};

Router.prototype.resolve = function(request, response) {
    let path = require("url").parse(request.url).pathname;

    return this.routes.some(function(route) {
        let match = path.match(route.url);

        if(!match || route.method !== request.method) {
            return false;
        }

        route.handler.apply(null, [request, response, path]);
        return true;
    })
};