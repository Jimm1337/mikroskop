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
let radioMove_snap;
let radioMove_smooth;
let radioCapture_photo;
let radioCapture_film;

//////////////////////////////////////////////////////////////////////////////////////////
// Message types
//////////////////////////////////////////////////////////////////////////////////////////

const TYPE_START = 'S';
const TYPE_STOP = '#';
const TYPE_CAL_START = 'C';
const TYPE_CAL_UL = 'L';
const TYPE_CAL_LR = 'R';
const TYPE_CAL_UP = 'U';
const TYPE_CAL_DOWN = 'D';
const TYPE_CAL_STEP = 'T';
const TYPE_CAL_RESET_UL = 'l';
const TYPE_CAL_RESET_LR = 'r';
const TYPE_CAL_RESET_U = 'u';
const TYPE_CAL_RESET_D = 'd';
const TYPE_CAL_RESET_STEP = 't';
const TYPE_READY = 'Y'; // RECEIVE ONLY
const TYPE_NOT_READY = 'N'; // RECEIVE ONLY
const TYPE_MANUAL_START_LEFT = 'M';
const TYPE_MANUAL_START_RIGHT = 'm';
const TYPE_MANUAL_START_UPPER = 'X';
const TYPE_MANUAL_START_LOWER = 'x';
const TYPE_MANUAL_START_UP = 'W';
const TYPE_MANUAL_START_DOWN = 'w';
const TYPE_MANUAL_STOP_LEFT = 'O';
const TYPE_MANUAL_STOP_RIGHT = 'o';
const TYPE_MANUAL_STOP_UPPER = 'Z';
const TYPE_MANUAL_STOP_LOWER = 'z';
const TYPE_MANUAL_STOP_UP = 'Q';
const TYPE_MANUAL_STOP_DOWN = 'q';
const TYPE_MANUAL_CONFIRM = 'K';
const TYPE_INFO = 'i'; // RECEIVE ONLY
const TYPE_TIME = 'y';
const TYPE_LOST_CONTROL = ']'; // RECEIVE ONLY
const TYPE_MOVEMENT = '('
const TYPE_CAPTURE = ')'

//////////////////////////////////////////////////////////////////////////////////////////
// Message data
//////////////////////////////////////////////////////////////////////////////////////////

const DATA_TIME_1_4000 = '0';
const DATA_TIME_1_2000 = '1';
const DATA_TIME_1_1000 = '2';
const DATA_TIME_1_500 = '3';
const DATA_TIME_1_250 = '4';
const DATA_TIME_1_125 = '5';
const DATA_TIME_1_60 = '6';
const DATA_TIME_1_30 = '7';
const DATA_TIME_1_15 = '8';
const DATA_TIME_1_8 = '9';
const DATA_TIME_1_4 = 'a';
const DATA_TIME_1_2 = 'b';
const DATA_TIME_1 = 'c';
const DATA_TIME_2 = 'd';
const DATA_TIME_5 = 'e';
const DATA_TIME_10 = 'f';

const DATA_MOVEMENT_SNAP = '0';
const DATA_MOVEMENT_SMOOTH = '1';

const DATA_CAPTURE_FILM = '0';
const DATA_CAPTURE_PHOTO = '1';

//////////////////////////////////////////////////////////////////////////////////////////
// Constants
//////////////////////////////////////////////////////////////////////////////////////////

const COLOR_ON = '#54a81c';
const COLOR_PROGRESS = '#e3d400';
const COLOR_OFF = '#a82c1c';
const COLOR_LEFT_PROGRESS = '#e39000';
const COLOR_RIGHT_PROGRESS = '#b2e300';

//////////////////////////////////////////////////////////////////////////////////////////
// Global variables
//////////////////////////////////////////////////////////////////////////////////////////

let hasManualControl = false;
let manualLeftLock = false;
let manualRightLock = false;
let manualUpperLock = false;
let manualLowerLock = false;
let manualUpLock = false;
let manualDownLock = false;
let manualConfLock = false;

let calibratingUL = false;
let calibratingLR = false;
let calibratingUp = false;
let calibratingDown = false;
let calibratingStepStart = false;
let calibratingStepEnd = false;

let started = false;

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

    setStatusColor(statusConnection, COLOR_ON);

    doSend(makeMessage(TYPE_INFO, ""));
}

function onClose(evt) {
    console.log("Disconnected: " + evt.data);

    setStatusColor(statusConnection, COLOR_OFF);
    setStatusColor(statusManual, COLOR_OFF);
    setStatusColor(statusUpperLeft, COLOR_OFF);
    setStatusColor(statusLowerRight, COLOR_OFF);
    setStatusColor(statusUp, COLOR_OFF);
    setStatusColor(statusDown, COLOR_OFF);
    setStatusColor(statusStep, COLOR_OFF);

    if (started) {
        toggleStartStopDisplay();
    }

    disableManualControl();
    disableCalButton();
    disableCalPosButtons();
    disableStartButton();
    disableRadioButtons();

    hasManualControl = false;
    calibratingUL = false;
    calibratingLR = false;
    calibratingUp = false;
    calibratingDown = false;
    calibratingStepStart = false;
    calibratingStepEnd = false;

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
            setStatusColor(statusUpperLeft, COLOR_OFF);
            calibratingUL = false;
            setStatusColor(statusLowerRight, COLOR_OFF);
            calibratingLR = false;
            setStatusColor(statusUp, COLOR_OFF);
            calibratingUp = false;
            setStatusColor(statusDown, COLOR_OFF);
            calibratingDown = false;
            setStatusColor(statusStep, COLOR_OFF);
            calibratingStepStart = false;
            setStatusColor(statusManual, COLOR_ON);
            enableManualControl();
            hasManualControl = true;
            enableCalPosButtons();
            break;
        case TYPE_CAL_UL: // Received on progress
            btnUpperLeft.disabled = false;
            if (calibratingLR) {
                setStatusColor(statusLowerRight, COLOR_OFF);
                calibratingLR = false;
            }
            btnLowerRight.disabled = false;
            if (calibratingUp) {
                setStatusColor(statusUp, COLOR_OFF);
                calibratingUp = false;
            }
            btnUp.disabled = false;
            if (calibratingDown) {
                setStatusColor(statusDown, COLOR_OFF);
                calibratingDown = false;
            }
            btnDown.disabled = false;
            if (calibratingStepStart || calibratingStepEnd) {
                setStatusColor(statusStep, COLOR_OFF);
                calibratingStepStart = false;
                calibratingStepEnd = false;
            }
            btnStep.disabled = false;
            setStatusColor(statusUpperLeft, COLOR_PROGRESS);
            calibratingUL = true;
            btnUpperLeft.disabled = true;
            break;
        case TYPE_CAL_LR: // Received on progress
            if (calibratingUL) {
                setStatusColor(statusUpperLeft, COLOR_OFF);
                calibratingUL = false;
            }
            btnUpperLeft.disabled = false;
            if (calibratingUp) {
                setStatusColor(statusUp, COLOR_OFF);
                calibratingUp = false;
            }
            btnUp.disabled = false;
            if (calibratingDown) {
                setStatusColor(statusDown, COLOR_OFF);
                calibratingDown = false;
            }
            btnDown.disabled = false;
            if (calibratingStepStart || calibratingStepEnd) {
                setStatusColor(statusStep, COLOR_OFF);
                calibratingStepStart = false;
                calibratingStepEnd = false;
            }
            btnStep.disabled = false;
            setStatusColor(statusLowerRight, COLOR_PROGRESS);
            calibratingLR = true;
            btnLowerRight.disabled = true;
            break;
        case TYPE_CAL_UP: // Received on progress
            if (calibratingUL) {
                setStatusColor(statusUpperLeft, COLOR_OFF);
                calibratingUL = false;
            }
            btnUpperLeft.disabled = false;
            if (calibratingLR) {
                setStatusColor(statusLowerRight, COLOR_OFF);
                calibratingLR = false;
            }
            btnLowerRight.disabled = false;
            if (calibratingDown) {
                setStatusColor(statusDown, COLOR_OFF);
                calibratingDown = false;
            }
            btnDown.disabled = false;
            if (calibratingStepStart || calibratingStepEnd) {
                setStatusColor(statusStep, COLOR_OFF);
                calibratingStepStart = false;
                calibratingStepEnd = false;
            }
            btnStep.disabled = false;
            setStatusColor(statusUp, COLOR_PROGRESS);
            calibratingUp = true;
            btnUp.disabled = true;
            break;
        case TYPE_CAL_DOWN: // Received on progress
            if (calibratingUL) {
                setStatusColor(statusUpperLeft, COLOR_OFF);
                calibratingUL = false;
            }
            btnUpperLeft.disabled = false;
            if (calibratingLR) {
                setStatusColor(statusLowerRight, COLOR_OFF);
                calibratingLR = false;
            }
            btnLowerRight.disabled = false;
            if (calibratingUp) {
                setStatusColor(statusUp, COLOR_OFF);
                calibratingUp = false;
            }
            btnUp.disabled = false;
            if (calibratingStepStart || calibratingStepEnd) {
                setStatusColor(statusStep, COLOR_OFF);
                calibratingStepStart = false;
                calibratingStepEnd = false;
            }
            btnStep.disabled = false;
            setStatusColor(statusDown, COLOR_PROGRESS);
            calibratingDown = true;
            btnDown.disabled = true;
            break;
        case TYPE_CAL_STEP: // Received on progress
            if (calibratingUL) {
                setStatusColor(statusUpperLeft, COLOR_OFF);
                calibratingUL = false;
            }
            btnUpperLeft.disabled = false;
            if (calibratingLR) {
                setStatusColor(statusLowerRight, COLOR_OFF);
                calibratingLR = false;
            }
            btnLowerRight.disabled = false;
            if (calibratingUp) {
                setStatusColor(statusUp, COLOR_OFF);
                calibratingUp = false;
            }
            btnUp.disabled = false;
            if (calibratingDown) {
                setStatusColor(statusDown, COLOR_OFF);
                calibratingDown = false;
            }
            btnDown.disabled = false;
            setStatusColor(statusStep, COLOR_LEFT_PROGRESS);
            calibratingStepEnd = false;
            calibratingStepStart = true;
            btnStep.disabled = true;
            break;
        case TYPE_CAL_RESET_UL: // Received on reset Upper Left
            setStatusColor(statusUpperLeft, COLOR_OFF);
            calibratingUL = false;
            btnUpperLeft.disabled = false;
            break;
        case TYPE_CAL_RESET_LR: // Received on reset Lower Right
            setStatusColor(statusLowerRight, COLOR_OFF);
            calibratingLR = false;
            btnLowerRight.disabled = false;
            break;
        case TYPE_CAL_RESET_U: // Received on reset Up
            setStatusColor(statusUp, COLOR_OFF);
            calibratingUp = false;
            btnUp.disabled = false;
            break;
        case TYPE_CAL_RESET_D: // Received on reset Down
            setStatusColor(statusDown, COLOR_OFF);
            calibratingDown = false;
            btnDown.disabled = false;
            break;
        case TYPE_CAL_RESET_STEP: // Received on reset Step
            setStatusColor(statusStep, COLOR_OFF);
            calibratingStepStart = false;
            calibratingStepEnd = false;
            btnStep.disabled = false;
            break;
        case TYPE_START: // Received on start
            started = true;
            setStatusColor(statusManual, COLOR_OFF);
            disableManualControl();
            toggleStartStopDisplay();
            disableCalButton();
            disableRadioButtons();
            disableRadioButtons();
            break;
        case TYPE_STOP: // Received on stop/end of movement
            started = false;
            toggleStartStopDisplay();
            if (hasManualControl) {
                setStatusColor(statusManual, COLOR_ON);
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
            setShutterRadio(msgData);
            break;
        case TYPE_MOVEMENT:
            setMovementRadio(msgData);
            break;
        case TYPE_CAPTURE:
            setCaptureRadio(msgData);
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
            if (calibratingUL) {
                setStatusColor(statusUpperLeft, COLOR_ON);
                calibratingUL = false;
                btnUpperLeft.disabled = false;
            } else if (calibratingLR) {
                setStatusColor(statusLowerRight, COLOR_ON);
                calibratingLR = false;
                btnLowerRight.disabled = false;
            } else if (calibratingUp) {
                setStatusColor(statusUp, COLOR_ON);
                calibratingUp = false;
                btnUp.disabled = false;
            } else if (calibratingDown) {
                setStatusColor(statusDown, COLOR_ON);
                calibratingDown = false;
                btnDown.disabled = false;
            } else if (calibratingStepStart) {
                setStatusColor(statusStep, COLOR_RIGHT_PROGRESS);
                calibratingStepStart = false;
                calibratingStepEnd = true;
            } else if (calibratingStepEnd) {
                setStatusColor(statusStep, COLOR_ON);
                btnStep.disabled = false;
                calibratingStepEnd = false;
            }
            break;
        case TYPE_LOST_CONTROL: // Received on lost manual control
            setStatusColor(statusManual, COLOR_OFF);
            disableManualControl();
            disableCalPosButtons();
            hasManualControl = false;
            calibratingUL = false;
            calibratingLR = false;
            calibratingUp = false;
            calibratingDown = false;
            calibratingStepStart = false;
            calibratingStepEnd = false;
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
    onRadioShutterChange(radioTime_1_4000, DATA_TIME_1_4000);
}

function changeRT1_2000() {
    onRadioShutterChange(radioTime_1_2000, DATA_TIME_1_2000);
}

function changeRT1_1000() {
    onRadioShutterChange(radioTime_1_1000, DATA_TIME_1_1000);
}

function changeRT1_500() {
    onRadioShutterChange(radioTime_1_500, DATA_TIME_1_500);
}

function changeRT1_250() {
    onRadioShutterChange(radioTime_1_250, DATA_TIME_1_250);
}

function changeRT1_125() {
    onRadioShutterChange(radioTime_1_125, DATA_TIME_1_125);
}

function changeRT1_60() {
    onRadioShutterChange(radioTime_1_60, DATA_TIME_1_60);
}

function changeRT1_30() {
    onRadioShutterChange(radioTime_1_30, DATA_TIME_1_30);
}

function changeRT1_15() {
    onRadioShutterChange(radioTime_1_15, DATA_TIME_1_15);
}

function changeRT1_8() {
    onRadioShutterChange(radioTime_1_8, DATA_TIME_1_8);
}

function changeRT1_4() {
    onRadioShutterChange(radioTime_1_4, DATA_TIME_1_4);
}

function changeRT1_2() {
    onRadioShutterChange(radioTime_1_2, DATA_TIME_1_2);
}

function changeRT1() {
    onRadioShutterChange(radioTime_1, DATA_TIME_1);
}

function changeRT2() {
    onRadioShutterChange(radioTime_2, DATA_TIME_2);
}

function changeRT5() {
    onRadioShutterChange(radioTime_5, DATA_TIME_5);
}

function changeRT10() {
    onRadioShutterChange(radioTime_10, DATA_TIME_10);
}

function changeRMVSnap() {
    onRadioMovementChange(radioMove_snap, DATA_MOVEMENT_SNAP);
}

function changeRMVSmooth() {
    onRadioMovementChange(radioMove_smooth, DATA_MOVEMENT_SMOOTH);
}

function changeRCPhoto() {
    onRadioCaptureChange(radioCapture_photo, DATA_CAPTURE_PHOTO);
}

function changeRCFilm() {
    onRadioCaptureChange(radioCapture_film, DATA_CAPTURE_FILM);
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

    radioMove_smooth.disabled = true;
    radioMove_snap.disabled = true;

    radioCapture_film.disabled = true;
    radioCapture_photo.disabled = true;
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

    radioMove_smooth.disabled = false;
    radioMove_snap.disabled = false;

    radioCapture_film.disabled = false;
    radioCapture_photo.disabled = false;
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
    radioCapture_film = document.getElementById("radioCapture_film");
    radioCapture_photo = document.getElementById("radioCapture_photo");
    radioMove_snap = document.getElementById("radioMove_snap");
    radioMove_smooth = document.getElementById("radioMove_smooth");
}

function setStatusColor(statusObj, colorObj) {
    statusObj.style.backgroundColor = colorObj;
}

function makeMessage(type, data) {
    return type + data + "\0";
}

function toggleStartStopDisplay() {
    if (started) {
        btnStart.style.display = "none";
        btnStop.style.display = "inline-block";
    } else {
        btnStart.style.display = "inline-block";
        btnStop.style.display = "none";
    }
}

function enableManualControl() {
    window.addEventListener("keydown", listenerKeyDown, false);
    window.addEventListener("keyup", listenerKeyUp, false);
}

function disableManualControl() {
    window.removeEventListener("keydown", listenerKeyDown, false);
    window.removeEventListener("keyup", listenerKeyUp, false);
}

function processInfo(data) {
    // idx 0: time radio (0-9 || a-f || ' ') - ' ' = not set
    // idx 1: running (0,1)
    // idx 2: calibrating ul (0,1, D) - D = done
    // idx 3: calibrating lr (0,1, D) - D = done
    // idx 4: calibrating up (0,1, D) - D = done
    // idx 5: calibrating down (0,1, D) - D = done
    // idx 6: calibrating step (0,1,2, D) - D = done
    // idx 7: move radio (0, 1)
    // idx 8: capture radio (0, 1)

    setShutterRadio(data[0]);
    setMovementRadio(data[7]);
    setCaptureRadio(data[8]);
    enableRadioButtons();
    enableCalButton();
    if (data[1] === "1") {
        started = true;
        toggleStartStopDisplay();
        disableCalButton();
        disableRadioButtons();
        setStatusColor(statusManual, COLOR_OFF);
        disableManualControl();
        hasManualControl = false;
    }
    if (data.substring(2) === "00000") {
        disableCalPosButtons();
    } else {
        infoSetButtons(data[2], data[3], data[4], data[5], data[6]);
    }
}

function setShutterRadio(radioChar) {
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
            console.log("Unknown radio: " + radioChar);
    }
}

function setMovementRadio(radioChar) {
    switch (radioChar) {
        case DATA_MOVEMENT_SNAP:
            radioMove_snap.checked = true;
            break;
        case DATA_MOVEMENT_SMOOTH:
            radioMove_smooth.checked = true;
            break;
        default:
            console.log("Unknown radio: " + radioChar);
    }
}

function setCaptureRadio(radioChar) {
    switch (radioChar) {
        case DATA_CAPTURE_PHOTO:
            radioCapture_photo.checked = true;
            break;
        case DATA_CAPTURE_FILM:
            radioCapture_film.checked = true;
            break;
        default:
            console.log("Unknown radio: " + radioChar);
    }
}

function infoSetButtons(ul, lr, up, down, step) {
    switch (ul) {
        case "0":
            setStatusColor(statusUpperLeft, COLOR_OFF);
            break;
        case "1":
            setStatusColor(statusUpperLeft, COLOR_PROGRESS);
            break;
        case "D":
            setStatusColor(statusUpperLeft, COLOR_ON);
            break;
        default:
            console.log("Unknown upper left: " + ul);
    }

    switch (lr) {
        case "0":
            setStatusColor(statusLowerRight, COLOR_OFF);
            break;
        case "1":
            setStatusColor(statusLowerRight, COLOR_PROGRESS);
            break;
        case "D":
            setStatusColor(statusLowerRight, COLOR_ON);
            break;
        default:
            console.log("Unknown lower right: " + ul);
    }

    switch (up) {
        case "0":
            setStatusColor(statusUp, COLOR_OFF);
            break;
        case "1":
            setStatusColor(statusUp, COLOR_PROGRESS);
            break;
        case "D":
            setStatusColor(statusUp, COLOR_ON);
            break;
        default:
            console.log("Unknown up: " + ul);
    }

    switch (down) {
        case "0":
            setStatusColor(statusDown, COLOR_OFF);
            break;
        case "1":
            setStatusColor(statusDown, COLOR_PROGRESS);
            break;
        case "D":
            setStatusColor(statusDown, COLOR_ON);
            break;
        default:
            console.log("Unknown down: " + ul);
    }

    switch (step) {
        case "0":
            setStatusColor(statusStep, COLOR_OFF);
            break;
        case "1":
            setStatusColor(statusStep, COLOR_LEFT_PROGRESS);
            break;
        case "2":
            setStatusColor(statusStep, COLOR_RIGHT_PROGRESS);
            break;
        case "D":
            setStatusColor(statusStep, COLOR_ON);
            break;
        default:
            console.log("Unknown step: " + ul);
    }
}

function onRadioShutterChange(radio, data) {
    if (radio.checked) {
        doSend(makeMessage(TYPE_TIME, data));
    }
}

function onRadioMovementChange(radio, data) {
    if (radio.checked) {
        doSend(makeMessage(TYPE_MOVEMENT, data));
    }
}

function onRadioCaptureChange(radio, data) {
    if (radio.checked) {
        doSend(makeMessage(TYPE_CAPTURE, data));
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
            if (!manualUpperLock) {
                doSend(makeMessage(TYPE_MANUAL_START_UPPER, ""));
            }
            manualUpperLock = true;
            break;
        case "ArrowDown":
            if (!manualLowerLock) {
                doSend(makeMessage(TYPE_MANUAL_START_LOWER, ""));
            }
            manualLowerLock = true;
            break;
        case "ArrowLeft":
            if (!manualLeftLock) {
                doSend(makeMessage(TYPE_MANUAL_START_LEFT, ""));
            }
            manualLeftLock = true;
            break;
        case "ArrowRight":
            if (!manualRightLock) {
                doSend(makeMessage(TYPE_MANUAL_START_RIGHT, ""));
            }
            manualRightLock = true;
            break;
        case "+":
        case "Add":
            if (!manualUpLock) {
                doSend(makeMessage(TYPE_MANUAL_START_UP, ""));
            }
            manualUpLock = true;
            break;
        case "-":
        case "Subtract":
            if (!manualDownLock) {
                doSend(makeMessage(TYPE_MANUAL_START_DOWN, ""));
            }
            manualDownLock = true;
            break;
        case "x":
        case "X":
            if (!manualConfLock) {
                doSend(makeMessage(TYPE_MANUAL_CONFIRM, ""));
            }
            manualConfLock = true;
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
            manualUpperLock = false;
            break;
        case "ArrowDown":
            doSend(makeMessage(TYPE_MANUAL_STOP_LOWER, ""));
            manualLowerLock = false;
            break;
        case "ArrowLeft":
            doSend(makeMessage(TYPE_MANUAL_STOP_LEFT, ""));
            manualLeftLock = false;
            break;
        case "ArrowRight":
            doSend(makeMessage(TYPE_MANUAL_STOP_RIGHT, ""));
            manualRightLock = false;
            break;
        case "+":
        case "Add":
            doSend(makeMessage(TYPE_MANUAL_STOP_UP, ""));
            manualUpLock = false;
            break;
        case "-":
        case "Subtract":
            doSend(makeMessage(TYPE_MANUAL_STOP_DOWN, ""));
            manualDownLock = false;
            break;
        case "x":
        case "X":
            manualConfLock = false;
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
