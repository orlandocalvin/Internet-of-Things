var button = [];
var state = [];
var jumlahItem = document.getElementsByClassName('button').length;

// query semua tombol
for(let i=1;i<=jumlahItem;i++) {
    var item = document.getElementById('button'+i);
    button.push(item);
    state.push(false);
}

// tambah event pada setiap tombol
for(let i=0;i<jumlahItem;i++) {
    button[i].addEventListener("click", () => {
        fetch('/toggle' + (i+1));
        state[i] = !state[i];
        state[i] ? on(i) : off(i);
    });
}

// buat fungsi on & off
function on(i) {
    button[i].innerHTML = 'ON';
    button[i].classList.replace('button-off','button-on');
} function off(i) {
    button[i].innerHTML = 'OFF';
    button[i].classList.replace('button-on','button-off');
}