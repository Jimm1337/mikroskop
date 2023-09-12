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
const TYPE_MANUAL_START_LEFT = "M";
const TYPE_MANUAL_START_RIGHT = "m";
const TYPE_MANUAL_START_UPPER = "X";
const TYPE_MANUAL_START_LOWER = "x";
const TYPE_MANUAL_START_UP = "W";
const TYPE_MANUAL_START_DOWN = "w";
const TYPE_MANUAL_STOP_LEFT = "O";
const TYPE_MANUAL_STOP_RIGHT = "o";
const TYPE_MANUAL_STOP_UPPER = "Z";
const TYPE_MANUAL_STOP_LOWER = "z";
const TYPE_MANUAL_STOP_UP = "Q";
const TYPE_MANUAL_STOP_DOWN = "q";
const TYPE_MANUAL_CONFIRM = "K";
const TYPE_INFO = "i"; // RECEIVE ONLY
const TYPE_TIME = "t";
const TYPE_LOST_CONTROL = "L"; // RECEIVE ONLY

//////////////////////////////////////////////////////////////////////////////////////////
// Message data
//////////////////////////////////////////////////////////////////////////////////////////

const DATA_TIME_1_4000 = "0";
const DATA_TIME_1_2000 = "1";
const DATA_TIME_1_1000 = "2";
const DATA_TIME_1_500 = "3";
const DATA_TIME_1_250 = "4";
const DATA_TIME_1_125 = "5";
const DATA_TIME_1_60 = "6";
const DATA_TIME_1_30 = "7";
const DATA_TIME_1_15 = "8";
const DATA_TIME_1_8 = "9";
const DATA_TIME_1_4 = "a";
const DATA_TIME_1_2 = "b";
const DATA_TIME_1 = "c";
const DATA_TIME_2 = "d";
const DATA_TIME_5 = "e";
const DATA_TIME_10 = "f";

//////////////////////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////////////////////

const COLOR_ON = "linear-gradient(to right, #54A81C, #54A81C);";
const COLOR_PROGRESS = "linear-gradient(to right, #E3D400, #E3D400);";
const COLOR_OFF = "linear-gradient(to right, #A82C1C, #A82C1C);";
const COLOR_LEFT_PROGRESS = "linear-gradient(to right, #E3D400, #A82C1C);";
const COLOR_RIGHT_PROGRESS = "linear-gradient(to right, #54A81C, #E3D400);";

//////////////////////////////////////////////////////////////////////////////////////////
// Global variables
//////////////////////////////////////////////////////////////////////////////////////////

let hasManualControl = false;

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

    doSend(makeMessage(TYPE_INFO, ""));
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

    hasManualControl = false;

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
            setStatus(statusManual, COLOR_ON);
            enableManualControl();
            hasManualControl = true;
            enableCalPosButtons();
            break;
        case TYPE_CAL_UL: // Received on progress
            if (isProgress(statusLowerRight)) {
                setStatus(statusLowerRight, COLOR_OFF);
                btnLowerRight.disabled = false;
            }
            if (isProgress(statusUp)) {
                setStatus(statusUp, COLOR_OFF);
                btnUp.disabled = false;
            }
            if (isProgress(statusDown)) {
                setStatus(statusDown, COLOR_OFF);
                btnDown.disabled = false;
            }
            if (isLeftProgress(statusStep) || isRightProgress(statusStep)) {
                setStatus(statusStep, COLOR_OFF);
                btnStep.disabled = false;
            }
            setStatus(statusUpperLeft, COLOR_PROGRESS);
            btnUpperLeft.disabled = true;
            break;
        case TYPE_CAL_LR: // Received on progress
            if (isProgress(statusUpperLeft)) {
                setStatus(statusUpperLeft, COLOR_OFF);
                btnUpperLeft.disabled = false;
            }
            if (isProgress(statusUp)) {
                setStatus(statusUp, COLOR_OFF);
                btnUp.disabled = false;
            }
            if (isProgress(statusDown)) {
                setStatus(statusDown, COLOR_OFF);
                btnDown.disabled = false;
            }
            if (isLeftProgress(statusStep) || isRightProgress(statusStep)) {
                setStatus(statusStep, COLOR_OFF);
                btnStep.disabled = false;
            }
            setStatus(statusLowerRight, COLOR_PROGRESS);
            btnLowerRight.disabled = true;
            break;
        case TYPE_CAL_UP: // Received on progress
            if (isProgress(statusUpperLeft)) {
                setStatus(statusUpperLeft, COLOR_OFF);
                btnUpperLeft.disabled = false;
            }
            if (isProgress(statusLowerRight)) {
                setStatus(statusLowerRight, COLOR_OFF);
                btnLowerRight.disabled = false;
            }
            if (isProgress(statusDown)) {
                setStatus(statusDown, COLOR_OFF);
                btnDown.disabled = false;
            }
            if (isLeftProgress(statusStep) || isRightProgress(statusStep)) {
                setStatus(statusStep, COLOR_OFF);
                btnStep.disabled = false;
            }
            setStatus(statusUp, COLOR_PROGRESS);
            btnUp.disabled = true;
            break;
        case TYPE_CAL_DOWN: // Received on progress
            if (isProgress(statusUpperLeft)) {
                setStatus(statusUpperLeft, COLOR_OFF);
                btnUpperLeft.disabled = false;
            }
            if (isProgress(statusLowerRight)) {
                setStatus(statusLowerRight, COLOR_OFF);
                btnLowerRight.disabled = false;
            }
            if (isProgress(statusUp)) {
                setStatus(statusUp, COLOR_OFF);
                btnUp.disabled = false;
            }
            if (isLeftProgress(statusStep) || isRightProgress(statusStep)) {
                setStatus(statusStep, COLOR_OFF);
                btnStep.disabled = false;
            }
            setStatus(statusDown, COLOR_PROGRESS);
            btnDown.disabled = true;
            break;
        case TYPE_CAL_STEP: // Received on progress
            if (isProgress(statusUpperLeft)) {
                setStatus(statusUpperLeft, COLOR_OFF);
                btnUpperLeft.disabled = false;
            }
            if (isProgress(statusLowerRight)) {
                setStatus(statusLowerRight, COLOR_OFF);
                btnLowerRight.disabled = false;
            }
            if (isProgress(statusUp)) {
                setStatus(statusUp, COLOR_OFF);
                btnUp.disabled = false;
            }
            if (isProgress(statusDown)) {
                setStatus(statusDown, COLOR_OFF);
                btnDown.disabled = false;
            }
            setStatus(statusStep, COLOR_LEFT_PROGRESS);
            btnStep.disabled = true;
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
            disableCalButton();
            disableRadioButtons();
            disableRadioButtons();
            break;
        case TYPE_STOP: // Received on stop/end of movement
            toggleStartStopDisplay();
            if (hasManualControl) {
                setStatus(statusManual, COLOR_ON);
                enableManualControl();
                enableCalPosButtons();
            }
            enableCalButton();
            enableRadioButtons();
            break;
        case TYPE_READY: // Received on ready
            enableStartButton();
            break;
        case TYPE_NOT_READY: // Received on not ready
            disableStartButton();
            break;
        case TYPE_INFO: // Received on info (on connect)
            processInfo(msgData);
            break;
        case TYPE_TIME: // Received on time change
            setRadio(msgData);
            break;
        case TYPE_MANUAL_START_LEFT: // Received on manual start left
            console.log("Manual start left");
            break;
        case TYPE_MANUAL_START_RIGHT: // Received on manual start right
            console.log("Manual start right");
            break;
        case TYPE_MANUAL_START_UPPER: // Received on manual start upper
            console.log("Manual start upper");
            break;
        case TYPE_MANUAL_START_LOWER: // Received on manual start lower
            console.log("Manual start lower");
            break;
        case TYPE_MANUAL_START_UP: // Received on manual start up
            console.log("Manual start up");
            break;
        case TYPE_MANUAL_START_DOWN: // Received on manual start down
            console.log("Manual start down");
            break;
        case TYPE_MANUAL_STOP_LEFT: // Received on manual stop left
            console.log("Manual stop left");
            break;
        case TYPE_MANUAL_STOP_RIGHT: // Received on manual stop right
            console.log("Manual stop right");
            break;
        case TYPE_MANUAL_STOP_UPPER: // Received on manual stop upper
            console.log("Manual stop upper");
            break;
        case TYPE_MANUAL_STOP_LOWER: // Received on manual stop lower
            console.log("Manual stop lower");
            break;
        case TYPE_MANUAL_STOP_UP: // Received on manual stop up
            console.log("Manual stop up");
            break;
        case TYPE_MANUAL_STOP_DOWN: // Received on manual stop down
            console.log("Manual stop down");
            break;
        case TYPE_MANUAL_CONFIRM: // Received on manual confirm
            if (isProgress(statusUpperLeft)) {
                setStatus(statusUpperLeft, COLOR_ON);
                btnUpperLeft.disabled = false;
            } else if (isProgress(statusLowerRight)) {
                setStatus(statusLowerRight, COLOR_ON);
                btnLowerRight.disabled = false;
            } else if (isProgress(statusUp)) {
                setStatus(statusUp, COLOR_ON);
                btnUp.disabled = false;
            } else if (isProgress(statusDown)) {
                setStatus(statusDown, COLOR_ON);
                btnDown.disabled = false;
            } else if (isLeftProgress(statusStep)) {
                setStatus(statusStep, COLOR_RIGHT_PROGRESS);
            } else if (isRightProgress(statusStep)) {
                setStatus(statusStep, COLOR_ON);
                btnStep.disabled = false;
            }
            break;
        case TYPE_LOST_CONTROL: // Received on lost manual control
            setStatus(statusManual, COLOR_OFF);
            disableManualControl();
            disableCalPosButtons();
            hasManualControl = false;
            break;
        default:
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
    doSend(makeMessage(TYPE_START, ""));
}

function ctrlStop() {
    doSend(makeMessage(TYPE_STOP, ""));
}

//////////////////////////////////////////////////////////////////////////////////////////
// Radio callbacks
//////////////////////////////////////////////////////////////////////////////////////////

function changeRT1_4000() {
    onRadioChange(radioTime_1_4000, DATA_TIME_1_4000);
}

function changeRT1_2000() {
    onRadioChange(radioTime_1_2000, DATA_TIME_1_2000);
}

function changeRT1_1000() {
    onRadioChange(radioTime_1_1000, DATA_TIME_1_1000);
}

function changeRT1_500() {
    onRadioChange(radioTime_1_500, DATA_TIME_1_500);
}

function changeRT1_250() {
    onRadioChange(radioTime_1_250, DATA_TIME_1_250);
}

function changeRT1_125() {
    onRadioChange(radioTime_1_125, DATA_TIME_1_125);
}

function changeRT1_60() {
    onRadioChange(radioTime_1_60, DATA_TIME_1_60);
}

function changeRT1_30() {
    onRadioChange(radioTime_1_30, DATA_TIME_1_30);
}

function changeRT1_15() {
    onRadioChange(radioTime_1_15, DATA_TIME_1_15);
}

function changeRT1_8() {
    onRadioChange(radioTime_1_8, DATA_TIME_1_8);
}

function changeRT1_4() {
    onRadioChange(radioTime_1_4, DATA_TIME_1_4);
}

function changeRT1_2() {
    onRadioChange(radioTime_1_2, DATA_TIME_1_2);
}

function changeRT1() {
    onRadioChange(radioTime_1, DATA_TIME_1);
}

function changeRT2() {
    onRadioChange(radioTime_2, DATA_TIME_2);
}

function changeRT5() {
    onRadioChange(radioTime_5, DATA_TIME_5);
}

function changeRT10() {
    onRadioChange(radioTime_10, DATA_TIME_10);
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

function processInfo(data) {
    // idx 0: time radio (0-9 || a-f || ' ') - ' ' = not set
    // idx 1: running (0,1)
    // idx 2: calibrating ul (0,1, D) - D = done
    // idx 3: calibrating lr (0,1, D) - D = done
    // idx 4: calibrating up (0,1, D) - D = done
    // idx 5: calibrating down (0,1, D) - D = done
    // idx 6: calibrating step (0,1,2, D) - D = done

    setRadio(data[0]);
    if (data[1] === "1") {
        toggleStartStopDisplay();
        disableCalButton();
        disableCalPosButtons();
        disableRadioButtons();
        setStatus(statusManual, COLOR_OFF);
        disableManualControl();
        hasManualControl = false;
    }
    infoSetButtons(data[2], data[3], data[4], data[5], data[6]);
}

function setRadio(radioChar) {
    switch (radioChar) {
        case DATA_TIME_1_4000:
            radioTime_1_4000.checked = true;
            break;
        case DATA_TIME_1_2000:
            radioTime_1_2000.checked = true;
            break;
        case DATA_TIME_1_1000:
            radioTime_1_1000.checked = true;
            break;
        case DATA_TIME_1_500:
            radioTime_1_500.checked = true;
            break;
        case DATA_TIME_1_250:
            radioTime_1_250.checked = true;
            break;
        case DATA_TIME_1_125:
            radioTime_1_125.checked = true;
            break;
        case DATA_TIME_1_60:
            radioTime_1_60.checked = true;
            break;
        case DATA_TIME_1_30:
            radioTime_1_30.checked = true;
            break;
        case DATA_TIME_1_15:
            radioTime_1_15.checked = true;
            break;
        case DATA_TIME_1_8:
            radioTime_1_8.checked = true;
            break;
        case DATA_TIME_1_4:
            radioTime_1_4.checked = true;
            break;
        case DATA_TIME_1_2:
            radioTime_1_2.checked = true;
            break;
        case DATA_TIME_1:
            radioTime_1.checked = true;
            break;
        case DATA_TIME_2:
            radioTime_2.checked = true;
            break;
        case DATA_TIME_5:
            radioTime_5.checked = true;
            break;
        case DATA_TIME_10:
            radioTime_10.checked = true;
            break;
        case " ":
            break;
        default:
            console.log("Unknown radio: " + radio);
    }
}

function infoSetButtons(ul, lr, up, down, step) {
    switch (ul) {
        case "0":
            setStatus(statusUpperLeft, COLOR_OFF);
            btnUpperLeft.disabled = false;
            break;
        case "1":
            setStatus(statusUpperLeft, COLOR_PROGRESS);
            btnUpperLeft.disabled = true;
            break;
        case "D":
            setStatus(statusUpperLeft, COLOR_ON);
            btnUpperLeft.disabled = false;
            break;
        default:
            console.log("Unknown upper left: " + ul);
    }

    switch (lr) {
        case "0":
            setStatus(statusLowerRight, COLOR_OFF);
            btnLowerRight.disabled = false;
            break;
        case "1":
            setStatus(statusLowerRight, COLOR_PROGRESS);
            btnLowerRight.disabled = true;
            break;
        case "D":
            setStatus(statusLowerRight, COLOR_ON);
            btnLowerRight.disabled = false;
            break;
        default:
            console.log("Unknown lower right: " + ul);
    }

    switch (up) {
        case "0":
            setStatus(statusUp, COLOR_OFF);
            btnUp.disabled = false;
            break;
        case "1":
            setStatus(statusUp, COLOR_PROGRESS);
            btnUp.disabled = true;
            break;
        case "D":
            setStatus(statusUp, COLOR_ON);
            btnUp.disabled = false;
            break;
        default:
            console.log("Unknown up: " + ul);
    }

    switch (down) {
        case "0":
            setStatus(statusDown, COLOR_OFF);
            btnDown.disabled = false;
            break;
        case "1":
            setStatus(statusDown, COLOR_PROGRESS);
            btnDown.disabled = true;
            break;
        case "D":
            setStatus(statusDown, COLOR_ON);
            btnDown.disabled = false;
            break;
        default:
            console.log("Unknown down: " + ul);
    }

    switch (step) {
        case "0":
            setStatus(statusUpperLeft, COLOR_OFF);
            btnUpperLeft.disabled = false;
            break;
        case "1":
            setStatus(statusUpperLeft, COLOR_LEFT_PROGRESS);
            btnUpperLeft.disabled = true;
            break;
        case "2":
            setStatus(statusUpperLeft, COLOR_RIGHT_PROGRESS);
            btnUpperLeft.disabled = true;
            break;
        case "D":
            setStatus(statusUpperLeft, COLOR_ON);
            btnUpperLeft.disabled = false;
            break;
        default:
            console.log("Unknown step: " + ul);
    }
}

function onRadioChange(radio, data) {
    if (radio.checked) {
        doSend(makeMessage(TYPE_TIME, data));
    }
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
