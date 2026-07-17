let lastState = ""

function sendCommand(state) {
    if (lastState !== state) {
        fetch("/command?State=" + state)
            .then((response) => response.text())
            .then((data) => console.log("Response:", data))
            .catch((error) => console.error("Error:", error))

        lastState = state
    }
}

document.addEventListener("keydown", function (event) {
    if (event.key === "ArrowUp") sendCommand("F")
    if (event.key === "ArrowDown") sendCommand("B")
    if (event.key === "ArrowRight") sendCommand("R")
    if (event.key === "ArrowLeft") sendCommand("L")
    if (event.key === " ") sendCommand("V")
})

document.addEventListener("keyup", function () {
    sendCommand("S")
})

function setupTouchControl(buttonId, state) {
    let button = document.getElementById(buttonId)
    button.addEventListener("touchstart", function () {
        sendCommand(state)
    })
    button.addEventListener("touchend", function () {
        sendCommand("S")
    })
}

setupTouchControl("forwardBtn", "F")
setupTouchControl("backwardBtn", "B")
setupTouchControl("leftBtn", "L")
setupTouchControl("rightBtn", "R")
setupTouchControl("hornBtn", "V")
setupTouchControl("lightOnBtn", "W")
setupTouchControl("lightOffBtn", "w")

document.getElementById("stopBtn").addEventListener("click", function () {
    sendCommand("S")
})