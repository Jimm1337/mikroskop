//////////////////////////////////////////////////////////////////////////////////////////
// WS
//////////////////////////////////////////////////////////////////////////////////////////

let url = "ws://192.168.4.1:1337/";
let websocket;

//////////////////////////////////////////////////////////////////////////////////////////
// Page elements
//////////////////////////////////////////////////////////////////////////////////////////

let statusConnection;
let statusManual;
let statusUpperLeft;
let statusLowerRight;
let statusUp;
let statusDown;
let statusStep;
let btnCal;
let btnUpperLeft;
let btnLowerRight;
let btnUp;
let btnDown;
let btnStep;
let btnResetUpperLeft;
let btnResetLowerRight;
let btnResetUp;
let btnResetDown;
let btnResetStep;
let btnStart;
let btnStop;
let radioTime_1_4000;
let radioTime_1_2000;
let radioTime_1_1000;
let radioTime_1_500;
let radioTime_1_250;
let radioTime_1_125;
let radioTime_1_60;
let radioTime_1_30;
let radioTime_1_15;
let radioTime_1_8;
let radioTime_1_4;
let radioTime_1_2;
let radioTime_1;
let radioTime_2;
let radioTime_5;
let radioTime_10;

//////////////////////////////////////////////////////////////////////////////////////////
// Message types
//////////////////////////////////////////////////////////////////////////////////////////

const TYPE_START = "S";
const TYPE_STOP = "#";
const TYPE_CAL_START = "C";
const TYPE_CAL_UL = "L";
const TYPE_CAL_LR = "R";
const TYPE_CAL_UP = "U";
const TYPE_CAL_DOWN = "D";
const TYPE_CAL_STEP = "T";
const TYPE_CAL_RESET_UL = "l";
const TYPE_CAL_RESET_LR = "r";
const TYPE_CAL_RESET_U = "u";
const TYPE_CAL_RESET_D = "d";
const TYPE_CAL_RESET_STEP = "t";
const TYPE_READY = "Y"; // RECEIVE ONLY
const TYPE_NOT_READY = "N"; // RECEIVE ONLY
const TYPE_MANUAL_START_LEFT = "M"; // SEND ONLY
const TYPE_MANUAL_START_RIGHT = "m"; // SEND ONLY
const TYPE_MANUAL_START_UPPER = "X"; // SEND ONLY
const TYPE_MANUAL_START_LOWER = "x"; // SEND ONLY
const TYPE_MANUAL_START_UP = "W"; // SEND ONLY
const TYPE_MANUAL_START_DOWN = "w"; // SEND ONLY
const TYPE_MANUAL_STOP_LEFT = "O"; // SEND ONLY
const TYPE_MANUAL_STOP_RIGHT = "o"; // SEND ONLY
const TYPE_MANUAL_STOP_UPPER = "Z"; // SEND ONLY
const TYPE_MANUAL_STOP_LOWER = "z"; // SEND ONLY
const TYPE_MANUAL_STOP_UP = "Q"; // SEND ONLY
const TYPE_MANUAL_STOP_DOWN = "q"; // SEND ONLY
const TYPE_MANUAL_CONFIRM = "K"; // RECEIVE ONLY

//////////////////////////////////////////////////////////////////////////////////////////
// Message data
//////////////////////////////////////////////////////////////////////////////////////////

const DATA_TIME_1_4000 = "1";
const DATA_TIME_1_2000 = "2";
const DATA_TIME_1_1000 = "3";
const DATA_TIME_1_500 = "4";
const DATA_TIME_1_250 = "5";
const DATA_TIME_1_125 = "6";
const DATA_TIME_1_60 = "7";
const DATA_TIME_1_30 = "8";
const DATA_TIME_1_15 = "9";
const DATA_TIME_1_8 = "a";
const DATA_TIME_1_4 = "b";
const DATA_TIME_1_2 = "c";
const DATA_TIME_1 = "d";
const DATA_TIME_2 = "e";
const DATA_TIME_5 = "f";
const DATA_TIME_10 = "g";
const DATA_PROGRESS = "P"; // RECEIVE ONLY

//////////////////////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////////////////////

const COLOR_ON = "linear-gradient(to right, #54A81C, #54A81C);";
const COLOR_PROGRESS = "linear-gradient(to right, #E3D400, #E3D400);";
const COLOR_OFF = "linear-gradient(to right, #A82C1C, #A82C1C);";
const COLOR_LEFT_PROGRESS = "linear-gradient(to right, #E3D400, #A82C1C);";
const COLOR_RIGHT_PROGRESS = "linear-gradient(to right, #54A81C, #E3D400);";

//////////////////////////////////////////////////////////////////////////////////////////
// init
//////////////////////////////////////////////////////////////////////////////////////////

function init() {
    assignPageElements();

    wsConnect(url);
}

//////////////////////////////////////////////////////////////////////////////////////////
// WS Callbacks
//////////////////////////////////////////////////////////////////////////////////////////

function onOpen(evt) {
    console.log("Connected: " + evt.data);

    setStatus(statusConnection, COLOR_ON);
    setStatus(statusManual, COLOR_ON);

    radioTime_1_4.checked = true; // Default to 1/4 second

    enableManualControl();
    enableCalButton();
    enableRadioButtons();
}

function onClose(evt) { // TODO: also handle this case in ESP
    console.log("Disconnected: " + evt.data);

    setStatus(statusConnection, COLOR_OFF);
    setStatus(statusManual, COLOR_OFF);
    setStatus(statusUpperLeft, COLOR_OFF);
    setStatus(statusLowerRight, COLOR_OFF);
    setStatus(statusUp, COLOR_OFF);
    setStatus(statusDown, COLOR_OFF);
    setStatus(statusStep, COLOR_OFF);

    if (btnStart.display === "none") {
        toggleStartStopDisplay();
    }

    disableManualControl();
    disableCalButton();
    disableCalPosButtons();
    disableStartButton();
    disableRadioButtons();

    // Try to reconnect after a few seconds
    setTimeout(function () {
        wsConnect(url)
    }, 2000);
}

function onError(evt) {
    console.log("!!! ERROR: " + evt.data);
}

function onMessage(evt) {
    let msg = evt.data;
    let msgType = msg[0];
    let msgData = msg.substring(1);

    console.log("Received: " + msg);

    switch (msgType) {
        case TYPE_CAL_START: // Received on start calibration
            setStatus(statusUpperLeft, COLOR_OFF);
            setStatus(statusLowerRight, COLOR_OFF);
            setStatus(statusUp, COLOR_OFF);
            setStatus(statusDown, COLOR_OFF);
            setStatus(statusStep, COLOR_OFF);
            enableCalPosButtons();
            break;
        case TYPE_CAL_UL: // Received on progress(data_progress)/set Upper Left
            if (msgData[0] === DATA_PROGRESS) {
                setStatus(statusLowerRight, COLOR_PROGRESS);
            } else {
                setStatus(statusLowerRight, COLOR_ON);
            }
            break;
        case TYPE_CAL_LR: // Received on progress(data_progress)/set Lower Right
            if (msgData[0] === DATA_PROGRESS) {
                setStatus(statusUpperLeft, COLOR_PROGRESS);
            } else {
                setStatus(statusUpperLeft, COLOR_ON);
            }
            break;
        case TYPE_CAL_UP: // Received on progress(data_progress)/set Up
            if (msgData[0] === DATA_PROGRESS) {
                setStatus(statusStep, COLOR_PROGRESS);
            } else {
                setStatus(statusStep, COLOR_ON);
            }
            break;
        case TYPE_CAL_DOWN: // Received on progress(data_progress)/set Down
            if (msgData[0] === DATA_PROGRESS) {
                setStatus(statusStep, COLOR_PROGRESS);
            } else {
                setStatus(statusStep, COLOR_ON);
            }
            break;
        case TYPE_CAL_STEP: // Received on progress(data_progress)/set Step
            if (msgData[0] === DATA_PROGRESS) {
                if (isLeftProgress(statusStep)) {
                    setStatus(statusStep, COLOR_RIGHT_PROGRESS);
                } else {
                    setStatus(statusStep, COLOR_LEFT_PROGRESS)
                }
            } else {
                setStatus(statusStep, COLOR_ON);
            }
            break;
        case TYPE_CAL_RESET_UL: // Received on reset Upper Left
            setStatus(statusUpperLeft, COLOR_OFF);
            btnUpperLeft.disabled = false;
            break;
        case TYPE_CAL_RESET_LR: // Received on reset Lower Right
            setStatus(statusLowerRight, COLOR_OFF);
            btnLowerRight.disabled = false;
            break;
        case TYPE_CAL_RESET_U: // Received on reset Up
            setStatus(statusUp, COLOR_OFF);
            btnUp.disabled = false;
            break;
        case TYPE_CAL_RESET_D: // Received on reset Down
            setStatus(statusDown, COLOR_OFF);
            btnDown.disabled = false;
            break;
        case TYPE_CAL_RESET_STEP: // Received on reset Step
            setStatus(statusStep, COLOR_OFF);
            btnStep.disabled = false;
            break;
        case TYPE_START: // Received on start
            setStatus(statusManual, COLOR_OFF);
            disableManualControl();
            toggleStartStopDisplay();
            break;
        case TYPE_STOP: // Received on stop/end of movement
            setStatus(statusManual, COLOR_ON);
            enableManualControl();
            toggleStartStopDisplay();
            break;
        case TYPE_READY: // Received on ready
            enableStartButton();
            break;
        case TYPE_NOT_READY: // Received on not ready
            disableStartButton();
            break;
        default:
            console.log("Unknown message type: " + msgType);
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// Button callbacks
//////////////////////////////////////////////////////////////////////////////////////////

function calStart() {
    doSend(makeMessage(TYPE_CAL_START, ""));
}

function calUpperLeft() {
    doSend(makeMessage(TYPE_CAL_UL, ""));
}

function calLowerRight() {
    doSend(makeMessage(TYPE_CAL_LR, ""));
}

function calUp() {
    doSend(makeMessage(TYPE_CAL_UP, ""));
}

function calDown() {
    doSend(makeMessage(TYPE_CAL_DOWN, ""));
}

function calStep() {
    doSend(makeMessage(TYPE_CAL_STEP, ""));
}

function calResetUL() {
    doSend(makeMessage(TYPE_CAL_RESET_UL, ""));
}

function calResetLR() {
    doSend(makeMessage(TYPE_CAL_RESET_LR, ""));
}

function calResetU() {
    doSend(makeMessage(TYPE_CAL_RESET_U, ""));
}

function calResetD() {
    doSend(makeMessage(TYPE_CAL_RESET_D, ""));
}

function calResetStep() {
    doSend(makeMessage(TYPE_CAL_RESET_STEP, ""));
}

function ctrlStart() {
    // Determine which radio button is checked
    const data = //TODO: Change this yuck!
        (radioTime_1_4000.checked ? DATA_TIME_1_4000 :
            radioTime_1_2000.checked ? DATA_TIME_1_2000 :
                radioTime_1_1000.checked ? DATA_TIME_1_1000 :
                    radioTime_1_500.checked ? DATA_TIME_1_500 :
                        radioTime_1_250.checked ? DATA_TIME_1_250 :
                            radioTime_1_125.checked ? DATA_TIME_1_125 :
                                radioTime_1_60.checked ? DATA_TIME_1_60 :
                                    radioTime_1_30.checked ? DATA_TIME_1_30 :
                                        radioTime_1_15.checked ? DATA_TIME_1_15 :
                                            radioTime_1_8.checked ? DATA_TIME_1_8 :
                                                radioTime_1_4.checked ? DATA_TIME_1_4 :
                                                    radioTime_1_2.checked ? DATA_TIME_1_2 :
                                                        radioTime_1.checked ? DATA_TIME_1 :
                                                            radioTime_2.checked ? DATA_TIME_2 :
                                                                radioTime_5.checked ? DATA_TIME_5 :
                                                                    radioTime_10.checked ? DATA_TIME_10 : "");

    doSend(makeMessage(TYPE_START, data));
}

function ctrlStop() {
    doSend(makeMessage(TYPE_STOP, ""));
}

//////////////////////////////////////////////////////////////////////////////////////////
// Disabling and enabling functions
//////////////////////////////////////////////////////////////////////////////////////////

function disableRadioButtons() {
    radioTime_1_4000.disabled = true;
    radioTime_1_2000.disabled = true;
    radioTime_1_1000.disabled = true;
    radioTime_1_500.disabled = true;
    radioTime_1_250.disabled = true;
    radioTime_1_125.disabled = true;
    radioTime_1_60.disabled = true;
    radioTime_1_30.disabled = true;
    radioTime_1_15.disabled = true;
    radioTime_1_8.disabled = true;
    radioTime_1_4.disabled = true;
    radioTime_1_2.disabled = true;
    radioTime_1.disabled = true;
    radioTime_2.disabled = true;
    radioTime_5.disabled = true;
    radioTime_10.disabled = true;
}

function enableRadioButtons() {
    radioTime_1_4000.disabled = false;
    radioTime_1_2000.disabled = false;
    radioTime_1_1000.disabled = false;
    radioTime_1_500.disabled = false;
    radioTime_1_250.disabled = false;
    radioTime_1_125.disabled = false;
    radioTime_1_60.disabled = false;
    radioTime_1_30.disabled = false;
    radioTime_1_15.disabled = false;
    radioTime_1_8.disabled = false;
    radioTime_1_4.disabled = false;
    radioTime_1_2.disabled = false;
    radioTime_1.disabled = false;
    radioTime_2.disabled = false;
    radioTime_5.disabled = false;
    radioTime_10.disabled = false;
}

function disableStartButton() {
    btnStart.disabled = true;
}

function enableStartButton() {
    btnStart.disabled = false;
}

function disableCalButton() {
    btnCal.disabled = true;
}

function enableCalButton() {
    btnCal.disabled = false;
}

function disableCalPosButtons() {
    btnUpperLeft.disabled = true;
    btnLowerRight.disabled = true;
    btnUp.disabled = true;
    btnDown.disabled = true;
    btnStep.disabled = true;
    btnResetUpperLeft.disabled = true;
    btnResetLowerRight.disabled = true;
    btnResetUp.disabled = true;
    btnResetDown.disabled = true;
    btnResetStep.disabled = true;
}

function enableCalPosButtons() {
    btnUpperLeft.disabled = false;
    btnLowerRight.disabled = false;
    btnUp.disabled = false;
    btnDown.disabled = false;
    btnStep.disabled = false;
    btnResetUpperLeft.disabled = false;
    btnResetLowerRight.disabled = false;
    btnResetUp.disabled = false;
    btnResetDown.disabled = false;
    btnResetStep.disabled = false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// WS functions
//////////////////////////////////////////////////////////////////////////////////////////

function wsConnect(url) {
    websocket = new WebSocket(url);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
    websocket.onerror = onError;
}

function doSend(message) {
    console.log("Sending: " + message);
    websocket.send(message);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Helper functions
//////////////////////////////////////////////////////////////////////////////////////////

function assignPageElements() {
    statusConnection = document.getElementById("statusConnection");
    statusManual = document.getElementById("statusManual");
    statusUpperLeft = document.getElementById("statusUpperLeft");
    statusLowerRight = document.getElementById("statusLowerRight");
    statusUp = document.getElementById("statusUp");
    statusDown = document.getElementById("statusDown");
    statusStep = document.getElementById("statusStep");
    btnCal = document.getElementById("btnCal");
    btnUpperLeft = document.getElementById("btnUpperLeft");
    btnLowerRight = document.getElementById("btnLowerRight");
    btnUp = document.getElementById("btnUp");
    btnDown = document.getElementById("btnDown");
    btnStep = document.getElementById("btnStep");
    btnResetUpperLeft = document.getElementById("btnResetUpperLeft");
    btnResetLowerRight = document.getElementById("btnResetLowerRight");
    btnResetUp = document.getElementById("btnResetUp");
    btnResetDown = document.getElementById("btnResetDown");
    btnResetStep = document.getElementById("btnResetStep");
    btnStart = document.getElementById("btnStart");
    btnStop = document.getElementById("btnStop");
    radioTime_1_4000 = document.getElementById("radioTime_1_4000");
    radioTime_1_2000 = document.getElementById("radioTime_1_2000");
    radioTime_1_1000 = document.getElementById("radioTime_1_1000");
    radioTime_1_500 = document.getElementById("radioTime_1_500");
    radioTime_1_250 = document.getElementById("radioTime_1_250");
    radioTime_1_125 = document.getElementById("radioTime_1_125");
    radioTime_1_60 = document.getElementById("radioTime_1_60");
    radioTime_1_30 = document.getElementById("radioTime_1_30");
    radioTime_1_15 = document.getElementById("radioTime_1_15");
    radioTime_1_8 = document.getElementById("radioTime_1_8");
    radioTime_1_4 = document.getElementById("radioTime_1_4");
    radioTime_1_2 = document.getElementById("radioTime_1_2");
    radioTime_1 = document.getElementById("radioTime_1");
    radioTime_2 = document.getElementById("radioTime_2");
    radioTime_5 = document.getElementById("radioTime_5");
    radioTime_10 = document.getElementById("radioTime_10");
}

function isOn(status) {
    return status.style.background === COLOR_ON;
}

function isProgress(status) {
    return status.style.background === COLOR_PROGRESS;
}

function isLeftProgress(status) {
    return status.style.background === COLOR_LEFT_PROGRESS;
}

function isRightProgress(status) {
    return status.style.background === COLOR_RIGHT_PROGRESS;
}

function setStatus(status, color) {
    status.style.background = color;
}

function makeMessage(type, data) {
    return type + data + "\0";
}

function toggleStartStopDisplay() {
    if (btnStart.display === "none") {
        btnStart.display = "inline-block";
        btnStop.display = "none";
    } else {
        btnStart.display = "none";
        btnStop.display = "inline-block";
    }
}

function enableManualControl() {
    window.addEventListener("keydown", listenerKeyDown, true);
    window.addEventListener("keyup", listenerKeyUp, true);
}

function disableManualControl() {
    window.removeEventListener("keydown", listenerKeyDown, true);
    window.removeEventListener("keyup", listenerKeyUp, true);
}

//////////////////////////////////////////////////////////////////////////////////////////
// Listeners for manual control
//////////////////////////////////////////////////////////////////////////////////////////

function listenerKeyDown(event) {
    if (event.defaultPrevented) {
        return; // Do nothing if the event was already processed
    }

    switch (event.key) {
        case "ArrowUp":
            doSend(makeMessage(TYPE_MANUAL_START_UPPER, ""));
            break;
        case "ArrowDown":
            doSend(makeMessage(TYPE_MANUAL_START_LOWER, ""));
            break;
        case "ArrowLeft":
            doSend(makeMessage(TYPE_MANUAL_START_LEFT, ""));
            break;
        case "ArrowRight":
            doSend(makeMessage(TYPE_MANUAL_START_RIGHT, ""));
            break;
        case "+":
        case "Add":
            doSend(makeMessage(TYPE_MANUAL_START_UP, ""));
            break;
        case "-":
        case "Subtract":
            doSend(makeMessage(TYPE_MANUAL_START_DOWN, ""));
            break;
        case "x":
        case "X":
            doSend(makeMessage(TYPE_MANUAL_CONFIRM, ""));
            break;
        default:
            return; // Quit when this doesn't handle the key event.
    }

    // Cancel the default action to avoid it being handled twice
    event.preventDefault();
}

function listenerKeyUp(event) {
    if (event.defaultPrevented) {
        return; // Do nothing if the event was already processed
    }

    switch (event.key) {
        case "ArrowUp":
            doSend(makeMessage(TYPE_MANUAL_STOP_UPPER, ""));
            break;
        case "ArrowDown":
            doSend(makeMessage(TYPE_MANUAL_STOP_LOWER, ""));
            break;
        case "ArrowLeft":
            doSend(makeMessage(TYPE_MANUAL_STOP_LEFT, ""));
            break;
        case "ArrowRight":
            doSend(makeMessage(TYPE_MANUAL_STOP_RIGHT, ""));
            break;
        case "+":
        case "Add":
            doSend(makeMessage(TYPE_MANUAL_STOP_UP, ""));
            break;
        case "-":
        case "Subtract":
            doSend(makeMessage(TYPE_MANUAL_STOP_DOWN, ""));
            break;
        default:
            return; // Quit when this doesn't handle the key event.
    }

    // Cancel the default action to avoid it being handled twice
    event.preventDefault();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Functionality
//////////////////////////////////////////////////////////////////////////////////////////

// Call the init function as soon as the page loads

window.addEventListener("load", init, false);
