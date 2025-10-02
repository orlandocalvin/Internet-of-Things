// Import Firebase SDK modules
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-database.js";

// Firebase project config
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
const database = getDatabase(app);

// Paths in Realtime Database
const dataPaths = {
    float: "test/float",
    int: "test/int",
    string: "test/string"
};

// Attach listener function to a database path
function listenToData(path, elementId, label) {
    const dataRef = ref(database, path);
    onValue(dataRef, (snapshot) => {
        const value = snapshot.val();
        console.log(`${label} reading: ${value}`);
        document.getElementById(elementId).textContent = value;
    });
}

// Listen for changes on all three paths
listenToData(dataPaths.float, "reading-float", "Float");
listenToData(dataPaths.int, "reading-int", "Int");
listenToData(dataPaths.string, "reading-string", "String");