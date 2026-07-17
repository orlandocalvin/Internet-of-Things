import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";
import { getDatabase, ref, onValue, set, off } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-database.js";

// Firebase configuration (Tetap sama)
const firebaseConfig = {
    apiKey: "AIzaSyBeHJC02JLGuYI4_t-br5QjkySj6e0_LU8",
    authDomain: "orca-esp32-firebase.firebaseapp.com",
    databaseURL: "https://orca-esp32-firebase-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "orca-esp32-firebase",
    storageBucket: "orca-esp32-firebase.firebasestorage.app",
    messagingSenderId: "16837685795",
    appId: "1:16837685795:web:756bd60e45ff69df3cf796"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const database = getDatabase(app);

// Export auth untuk auth.js
export { auth };

// DOM elements
const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-container"); // ID diubah
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");

// --- DOM Elements Baru (Sensor Values) ---
const valNutrientTemp = document.getElementById("val-nutrient-temp");
const valNutrientLevel = document.getElementById("val-nutrient-level");
const valAirTemp = document.getElementById("val-air-temp");
const valHumidity = document.getElementById("val-humidity");
const valLux = document.getElementById("val-lux");

// --- DOM Elements Baru (Controls) ---
const toggleAutoMode = document.getElementById("toggle-auto-mode");
const stateAutoMode = document.getElementById("state-auto-mode");
const btnPumpOn = document.getElementById("btn-pump-on");
const btnPumpOff = document.getElementById("btn-pump-off");
const stateRelay1 = document.getElementById("state-relay1");
const btnLightOn = document.getElementById("btn-light-on");
const btnLightOff = document.getElementById("btn-light-off");
const stateRelay2 = document.getElementById("state-relay2");

// Variabel untuk menyimpan path database & listener
let uid = null;
let statePath = null;
let cmdPath = null;
let stateRef = null;

// --- Helper Functions ---

function updateSensorUI(state) {
    // Cek null/undefined, beri nilai default '--'
    valNutrientTemp.textContent = state.nutrientTemp?.toFixed(1) ?? '--';
    valNutrientLevel.textContent = state.nutrientLevel?.toFixed(0) ?? '--';
    valAirTemp.textContent = state.airTemp?.toFixed(1) ?? '--';
    valHumidity.textContent = state.airHumidity?.toFixed(0) ?? '--';
    valLux.textContent = state.luxValue ?? '--';
}

function updateControlUI(state) {
    const isAuto = state.isAutoMode === true;
    const relay1On = state.relay1 === true;
    const relay2On = state.relay2 === true;

    // 1. Update Mode Toggle
    toggleAutoMode.checked = isAuto;
    stateAutoMode.textContent = isAuto ? "AUTO" : "MANUAL";

    // 2. Update State Teks Relay
    stateRelay1.textContent = relay1On ? "ON" : "OFF";
    stateRelay2.textContent = relay2On ? "ON" : "OFF";

    // 3. Logika Enable/Disable Tombol (PENTING)
    // Nonaktifkan tombol manual jika mode AUTO aktif
    btnPumpOn.disabled = isAuto;
    btnPumpOff.disabled = isAuto;
    btnLightOn.disabled = isAuto;
    btnLightOff.disabled = isAuto;
}

function setupControlListeners() {
    // Listener untuk toggle Auto/Manual
    toggleAutoMode.addEventListener('change', (e) => {
        set(ref(database, `${cmdPath}/isAutoMode`), e.target.checked);
    });

    // Listeners untuk Pompa (Relay 1)
    btnPumpOn.addEventListener('click', () => {
        set(ref(database, `${cmdPath}/relay1`), true);
    });
    btnPumpOff.addEventListener('click', () => {
        set(ref(database, `${cmdPath}/relay1`), false);
    });

    // Listeners untuk Lampu (Relay 2)
    btnLightOn.addEventListener('click', () => {
        set(ref(database, `${cmdPath}/relay2`), true);
    });
    btnLightOff.addEventListener('click', () => {
        set(ref(database, `${cmdPath}/relay2`), false);
    });
}

// --- Fungsi Utama UI Setup ---
const setupUI = (user) => {
    console.log('setupUI dipanggil untuk:', user ? user.email : 'logout');

    if (user) {
        // --- User Logged In ---
        uid = user.uid;
        statePath = `UsersData/${uid}/state`;
        cmdPath = `UsersData/${uid}/cmd`;
        stateRef = ref(database, statePath);

        // Toggle UI
        loginElement.style.display = 'none';
        contentElement.style.display = 'block';
        authBarElement.style.display = 'block';
        userDetailsElement.style.display = 'block';
        userDetailsElement.innerHTML = user.email;

        // Pasang listener tombol (hanya sekali)
        setupControlListeners();

        // --- Listener Utama (SoT) ---
        // Mulai mendengarkan /state path
        onValue(stateRef, (snap) => {
            if (snap.exists()) {
                const state = snap.val();
                // console.log("Menerima update state:", state);
                updateSensorUI(state);
                updateControlUI(state);
            } else {
                // console.log("Belum ada data di state path.");
                // Tampilkan UI "menunggu data"
                stateAutoMode.textContent = "Waiting for ESP32...";
            }
        });

    } else {
        // --- User Logged Out ---
        // Hentikan listener Firebase jika ada
        if (stateRef) {
            off(stateRef);
            console.log("Listener Firebase dihentikan.");
        }

        // Reset variabel global
        uid = null;
        statePath = null;
        cmdPath = null;
        stateRef = null;

        // Toggle UI
        loginElement.style.display = 'block';
        authBarElement.style.display = 'none';
        userDetailsElement.style.display = 'none';
        contentElement.style.display = 'none';
    }
};

export { setupUI };