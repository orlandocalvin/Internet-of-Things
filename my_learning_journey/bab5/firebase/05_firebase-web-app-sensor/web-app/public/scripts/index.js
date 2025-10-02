import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-database.js";

// Firebase configuration
const firebaseConfig = {
    apiKey: "AIzaSyBsTCTUfmyTyD25NKP7bglEa1Dg8Q2Jrnk",
    authDomain: "esp-project-a1ed5.firebaseapp.com",
    databaseURL: "https://esp-project-a1ed5-default-rtdb.firebaseio.com",
    projectId: "esp-project-a1ed5",
    storageBucket: "esp-project-a1ed5.firebasestorage.app",
    messagingSenderId: "861253345210",
    appId: "1:861253345210:web:200a664cc33033fc920783"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const database = getDatabase(app);

// Export auth for use in auth.js
export { auth };

// UI elements
const loginElement = document.getElementById("login-form");
const contentElement = document.getElementById("content-sign-in");
const authBarElement = document.getElementById("authentication-bar");
const userDetailsElement = document.getElementById("user-details");
const tempElement = document.getElementById("temp");
const humElement = document.getElementById("hum");
const presElement = document.getElementById("pres");

// Start reading sensor data when user logs in
export function startSensorListeners(uid) {
    const tempRef = ref(database, `UsersData/${uid}/temperature`);
    const humRef = ref(database, `UsersData/${uid}/humidity`);
    const presRef = ref(database, `UsersData/${uid}/pressure`);

    // Update readings when data changes
    onValue(tempRef, (snap) => {
        const val = snap.val();
        tempElement.textContent = val !== null ? val.toFixed(2) : "N/A";
    });

    onValue(humRef, (snap) => {
        const val = snap.val();
        humElement.textContent = val !== null ? val.toFixed(2) : "N/A";
    });

    onValue(presRef, (snap) => {
        const val = snap.val();
        presElement.textContent = val !== null ? val.toFixed(2) : "N/A";
    });
}

// UI toggle function
export function setupUI(user) {
    if (user) {
        loginElement.classList.add("hidden");
        contentElement.classList.remove("hidden");
        authBarElement.classList.remove("hidden");
        userDetailsElement.textContent = user.email;

        // Start listening to database changes
        startSensorListeners(user.uid);
    } else {
        loginElement.classList.remove("hidden");
        contentElement.classList.add("hidden");
        authBarElement.classList.add("hidden");
        userDetailsElement.textContent = "";
    }
}