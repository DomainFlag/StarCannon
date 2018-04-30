function get_frame_rate_updater() {
    let frame_display = document.querySelector(".frame_display");
    let sec = 0;

    let now = 0;
    let total = 0;
    let counter = 0;

    return (time) => {
        let diff = time-now;

        diff *= 0.001;
        total += diff;
        if(total > 1.0) {
            sec = Math.floor(1/(total/counter));
            frame_display.textContent = sec + "fps";
            if(sec > 40) {
                frame_display.style.color = "#228B22";
            } else if(sec > 20) {
                frame_display.style.color = "#FF8C00";
            } else {
                frame_display.style.color = "#DC143C";
            }

            counter = 0;
            total = 0;
        }

        now = time;
        counter++;
    }
}