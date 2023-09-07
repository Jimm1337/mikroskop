var url = "ws://192.168.4.1:1337/";
var button;

// This is called when the page finishes loading
function init() {

    // Assign page elements to variables
    button = document.getElementById("send");

    // Connect to WebSocket server
    wsConnect(url);
}

// Call this to connect to the WebSocket server
function wsConnect(url) {

    // Connect to WebSocket server
    websocket = new WebSocket(url);

    // Assign callbacks
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    websocket.onerror = function(evt) { onError(evt) };
}

function onOpen(evt) {
    console.log("Connected");

    // Enable button
    button.disabled = false;

    doSend("READY\0");
}

function onClose(evt) {
    console.log("Disconnected");

    button.disabled = true;

    // Try to reconnect after a few seconds
    setTimeout(function() { wsConnect(url) }, 2000);
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

function onPress() {
    doSend("TEST\0");
}

// Call the init function as soon as the page loads
window.addEventListener("load", init, false);