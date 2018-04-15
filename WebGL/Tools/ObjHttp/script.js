/**
 * Created by Cchiv on 14/04/2018.
 *
 * The below program is a helper tool for Obj fetching,
 * by giving desired folder name that contains the
 * obj/mtl/jpeg images, fetched geometric/normal/texture
 * vertices and textures needed for obj rendering.
 */
function ObjHttp(callback, gl, folderName) {
    this.gl = gl;
    this.callback = callback;
    this.folderName = folderName;

    this.data = {
        "textures" : [],
        "vertices" : null
    };

    this.fetchUrls()
        .then((urls) => {
            this.fetchMtl(urls.mtlUrl)
                .then((materials) => {
                    Promise.all(
                        this.fetchTextures(materials)
                            .concat(this.fetchData(urls.objUrl, materials))
                    ).then((data) => {
                        this.data["textures"] = data.slice(0, data.length-1);
                        this.data["vertices"] = JSON.parse(data[data.length-1]);

                        this.callback(this.gl, this.data);
                    });
                });
        })
}

ObjHttp.prototype.fetchUrls = function() {
    return new Promise(function(resolve, reject) {
        let req = new XMLHttpRequest();
        req.open("GET", "http://localhost:8000/obj_folder?foldername=" + this.folderName, true);
        req.addEventListener("load", function(e) {
            if(req.status < 400) {
                resolve(JSON.parse(req.responseText));
            } else {
                reject(req.status);
            }
        });
        req.send();
    }.bind(this));
};

ObjHttp.prototype.fetchMtl = function(url) {
    return new Promise((resolve, reject) => {
        let req = new XMLHttpRequest();
        req.open("GET", "http://localhost:8000/mtl?filename=" + this.folderName + "/" + url, true);
        req.addEventListener("load", (e) => {
            if(req.status < 400) {
                resolve(JSON.parse(req.responseText));
            } else {
                reject(req.status);
            }
        });
        req.send();
    });
};

ObjHttp.prototype.fetchTextures = function(materials) {
    let promises = [];

    Object.keys(materials)
        .filter((obj) => materials[obj].unit !== -1)
        .forEach((material) => {
            let img = document.createElement("img");
            img.crossOrigin = "null";
            img.src = "http://localhost:8000/texture?filename=" + this.folderName + "/" + materials[material].texture;
            promises.push(new Promise((resolve, reject) => {
                img.addEventListener("load", resolve.bind(this, img));
                img.addEventListener("error", reject);
            }));
        });

    return promises;
};

ObjHttp.prototype.fetchData = function(url, materials) {
    return new Promise(function(resolve, reject) {
        let req = new XMLHttpRequest();
        req.open("POST", "http://localhost:8000/data?objName=" + this.folderName + "/" + url, true);
        req.addEventListener("load", function(e) {
            if(req.status < 400) {
                resolve(req.responseText);
            } else {
                reject(req.status);
            }
        });
        req.send(JSON.stringify(materials));
    }.bind(this));
};

// objHttp = new ObjHttp(console.log, null, "X-Fighter");