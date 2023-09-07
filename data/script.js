let url = "ws://192.168.4.1:1337/";
let websocket;
let sendButton;
let stopButton;
let xStart;
let xEnd;
let yStart;
let yEnd;
let zStart;
let zEnd;
let speed;

const clamp = (num, min, max) => Math.min(Math.max(num, min), max);

function processInput(input, min, max) {
    let num = parseInt(input.value);
    if (isNaN(num)) {
        num = 0;
    }
    else {
        let temp = clamp(num, min, max)
        if (temp !== num) {
            input.value = temp.toString();
            num = temp;
        }
    }

    return num;
}

// This is called when the page finishes loading
function init() {

    // Assign page elements to variables
    sendButton = document.getElementById("send");
    stopButton = document.getElementById("stop");
    xStart = document.getElementById("xs");
    xEnd = document.getElementById("xe");
    yStart = document.getElementById("ys");
    yEnd = document.getElementById("ye");
    zStart = document.getElementById("zs");
    zEnd = document.getElementById("ze");
    speed = document.getElementById("s");

    // Connect to WebSocket server
    wsConnect(url);
}

// Call this to connect to the WebSocket server
function wsConnect(url) {

    // Connect to WebSocket server
    websocket = new WebSocket(url);

    // Assign callbacks
    websocket.onopen = function (evt) {
        onOpen(evt)
    };
    websocket.onclose = function (evt) {
        onClose(evt)
    };
    websocket.onmessage = function (evt) {
        onMessage(evt)
    };
    websocket.onerror = function (evt) {
        onError(evt)
    };
}

function onOpen(evt) {
    console.log("Connected");

    // Enable buttons
    sendButton.disabled = false;
    stopButton.disabled = false;

    doSend("READY\0");
}

function onClose(evt) {
    console.log("Disconnected");

    // Disable buttons
    sendButton.disabled = true;
    stopButton.disabled = true;

    // Try to reconnect after a few seconds
    setTimeout(function () {
        wsConnect(url)
    }, 2000);
}

function onMessage(evt) {
    console.log("Received: " + evt.data);
}

function onError(evt) {
    console.log("ERROR: " + evt.data);
}

function doSend(message) {
    console.log("Sending: " + message);
    websocket.send(message);
}

function onPressSend() {
    let msg =
        "s" + processInput(speed, 0, 100) +
        "x" + processInput(xStart, 0, 300) + "-" + processInput(xEnd, 0, 300) +
        "y" + processInput(yStart, 0, 300) + "-" + processInput(yEnd, 0, 300) +
        "z" + processInput(zStart, 0, 200) + "-" + processInput(zEnd, 0, 200) +
        "\0";

    doSend(msg);
}

function onPressStop() {
    doSend("#\0");
}

// Call the init function as soon as the page loads
window.addEventListener("load", init, false);
