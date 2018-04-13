let setSliders = function setSliders(sliders, callback, render, gl) {
    let sliderDOM = document.querySelector(".slider");
    let sliderClone = sliderDOM.cloneNode(true);
    let slidersDOM = document.querySelector(".sliders");

    const sliderWidth = sliderDOM.querySelector(".slider_container").clientWidth;
    const sliderOffset = sliderDOM.querySelector(".slider_container").offsetLeft;

    slidersDOM.removeChild(sliderDOM);

    sliders.forEach(function(slider) {
        let slider_container = sliderClone.querySelector(".slider_container");
        let thumb = sliderClone.querySelector(".thumb");
        let sliderValue = sliderClone.querySelector(".slider_value");
        let sliderPlaceHolder = sliderClone.querySelector(".slider_placeholder");
        let sliderLabel = sliderClone.querySelector(".slider_label");

        sliderLabel.textContent = slider.label;
        sliderValue.textContent = slider.value + slider.measurement;
        thumb.style.left = (slider.value-slider.valueStart)/(slider.valueEnd - slider.valueStart)*100 + "%";
        sliderPlaceHolder.style.width = thumb.style.left;

        let mouseDown = function mouseDown(event) {
            thumb.removeEventListener("mousedown", mouseDown);
            document.addEventListener("mousemove", mouseMove);
            document.addEventListener("mouseup", mouseUp);
            event.stopPropagation();
            event.preventDefault();
        };

        let mouseMove = function mouseMove(event) {
            let pageX = event.pageX;
            let value = 0;
            if(pageX >= sliderOffset && pageX <= sliderWidth+sliderOffset) {
                value = (pageX-sliderOffset)/sliderWidth*100;
                thumb.style.left = value + "%";
            } else if(pageX < sliderOffset) {
                value = 0;
                thumb.style.left = value + "%";
            } else {
                value = 100;
                thumb.style.left = value + "%";
            }
            value = (slider.valueStart + (slider.valueEnd - slider.valueStart)*value/100).toFixed(2);
            sliderValue.textContent = value + slider.measurement;
            sliderPlaceHolder.style.width = thumb.style.left;
            slider.value = value;
            if(render) callback(gl);
            event.stopPropagation();
            event.preventDefault();
        };

        let mouseUp = function mouseUp() {
            document.removeEventListener("mousemove", mouseMove);
            thumb.addEventListener("mousedown", mouseDown);
        };

        thumb.addEventListener("mousedown", mouseDown);

        slidersDOM.appendChild(sliderClone);
        sliderClone = sliderClone.cloneNode(true);
    });
};