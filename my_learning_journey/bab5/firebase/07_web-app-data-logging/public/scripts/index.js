import { initializeApp } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-app.js";
import { getAuth } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";
import {
    getDatabase,
    ref,
    query,
    orderByKey,
    remove,
    limitToLast,
    onValue,
    get
} from "https://www.gstatic.com/firebasejs/11.6.0/firebase-database.js";

// ===== Firebase Config =====
const firebaseConfig = {
    apiKey: "AIzaSyBeHJC02JLGuYI4_t-br5QjkySj6e0_LU8",
    authDomain: "orca-esp32-firebase.firebaseapp.com",
    databaseURL: "https://orca-esp32-firebase-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "orca-esp32-firebase",
    storageBucket: "orca-esp32-firebase.firebasestorage.app",
    messagingSenderId: "16837685795",
    appId: "1:16837685795:web:756bd60e45ff69df3cf796"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const database = getDatabase(app);
export { auth };

// ===== Helper Functions =====
const epochToJsDate = (epochTime) => new Date(epochTime * 1000);
const epochToDateTime = (epochTime) =>
    epochTime ? epochToJsDate(epochTime).toLocaleString() : "N/A";
const formatReading = (value) =>
    typeof value === "number" ? value.toFixed(2) : "N/A";

// ===== DOM Elements =====
const loginElement = document.querySelector("#login-form");
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector("#user-details");
const authBarElement = document.querySelector("#authentication-bar");
const deleteButtonElement = document.getElementById("delete-button");
const deleteModalElement = document.getElementById("delete-modal");
const deleteDataFormElement = document.querySelector("#delete-data-form");
const viewDataButtonElement = document.getElementById("view-data-button");
const hideDataButtonElement = document.getElementById("hide-data-button");
const tableContainerElement = document.querySelector("#table-container");
const loadDataButtonElement = document.getElementById("load-data");
const downloadCsvButton = document.getElementById("download-csv-button");
const tempElement = document.getElementById("temp");
const humElement = document.getElementById("hum");
const presElement = document.getElementById("pres");
const updateElement = document.getElementById("lastUpdate");

// ===== Setup UI on Auth State =====
window.setupUI = (user) => {
    if (user) {
        loginElement.style.display = "none";
        contentElement.style.display = "block";
        authBarElement.style.display = "block";
        userDetailsElement.style.display = "block";
        userDetailsElement.textContent = user.email;

        const dbRef = ref(database, `UsersData/${user.uid}/logs`);

        // ====== CARD SECTION ======
        let latestData = { temperature: null, humidity: null, pressure: null, timestamp: null };

        onValue(query(dbRef, orderByKey()), (snapshot) => {
            const dataObj = snapshot.val();
            if (!dataObj) return;

            const lastKey = Object.keys(dataObj).sort().pop();
            const data = dataObj[lastKey] || {};

            if (typeof data.temperature !== "undefined") latestData.temperature = data.temperature;
            if (typeof data.humidity !== "undefined") latestData.humidity = data.humidity;
            if (typeof data.pressure !== "undefined") latestData.pressure = data.pressure;
            if (typeof data.timestamp !== "undefined") latestData.timestamp = data.timestamp;

            const allReady =
                latestData.temperature !== null &&
                latestData.humidity !== null &&
                latestData.pressure !== null &&
                latestData.timestamp !== null;

            if (allReady) {
                tempElement.textContent = formatReading(latestData.temperature);
                humElement.textContent = formatReading(latestData.humidity);
                presElement.textContent = formatReading(latestData.pressure);
                updateElement.textContent = epochToDateTime(latestData.timestamp);
            }
        });

        // ====== DELETE DATA ======
        deleteButtonElement.addEventListener("click", (e) => {
            e.preventDefault();
            deleteModalElement.style.display = "block";
        });

        deleteDataFormElement.addEventListener("submit", (e) => {
            e.preventDefault();
            remove(dbRef);
            deleteModalElement.style.display = "none";
            tempElement.textContent = humElement.textContent = presElement.textContent = updateElement.textContent = "N/A";
            document.getElementById("tbody").innerHTML = "";
        });

        // ====== TABLE SECTION (batch 10 + reset) ======
        let batchSize = 10;
        let unsubscribe = null;

        const renderTable = () => {
            if (unsubscribe) unsubscribe();

            const tableQuery = query(dbRef, orderByKey(), limitToLast(batchSize));
            unsubscribe = onValue(tableQuery, (snapshot) => {
                const dataObj = snapshot.val();
                const tbody = document.getElementById("tbody");
                tbody.innerHTML = "";

                if (!dataObj) return;

                const buffer = {};
                Object.keys(dataObj).forEach((key) => {
                    const data = dataObj[key];
                    if (!data.timestamp) return;

                    const ts = data.timestamp;
                    if (!buffer[ts]) {
                        buffer[ts] = { temperature: null, humidity: null, pressure: null, timestamp: ts };
                    }

                    if (typeof data.temperature !== "undefined") buffer[ts].temperature = data.temperature;
                    if (typeof data.humidity !== "undefined") buffer[ts].humidity = data.humidity;
                    if (typeof data.pressure !== "undefined") buffer[ts].pressure = data.pressure;
                });

                const ordered = Object.values(buffer).sort((a, b) => b.timestamp - a.timestamp);

                ordered.forEach((row) => {
                    if (
                        row.temperature !== null &&
                        row.humidity !== null &&
                        row.pressure !== null
                    ) {
                        const tr = document.createElement("tr");
                        tr.innerHTML = `
              <td>${epochToDateTime(row.timestamp)}</td>
              <td>${formatReading(row.temperature)}</td>
              <td>${formatReading(row.humidity)}</td>
              <td>${formatReading(row.pressure)}</td>
            `;
                        tbody.appendChild(tr);
                    }
                });
            });
        };

        // Render awal 10 data
        renderTable();

        // ====== LOAD MORE BUTTON ======
        loadDataButtonElement.style.display = "inline-block";
        loadDataButtonElement.addEventListener("click", () => {
            batchSize += 10;
            renderTable();
        });

        // ====== DOWNLOAD CSV BUTTON ======
        downloadCsvButton.addEventListener("click", async () => {
            const snapshot = await get(dbRef);
            const dataObj = snapshot.val();
            if (!dataObj) {
                alert("Tidak ada data untuk diunduh.");
                return;
            }

            const rows = [["Timestamp", "Temperature (°C)", "Humidity (%)", "Pressure (hPa)"]];
            Object.keys(dataObj).forEach(key => {
                const d = dataObj[key];
                rows.push([
                    d.timestamp ? epochToDateTime(d.timestamp) : "",
                    d.temperature ?? "",
                    d.humidity ?? "",
                    d.pressure ?? ""
                ]);
            });

            const csvContent = "data:text/csv;charset=utf-8," + rows.map(e => e.join(",")).join("\n");
            const encodedUri = encodeURI(csvContent);
            const link = document.createElement("a");
            link.setAttribute("href", encodedUri);
            link.setAttribute("download", "sensor_log.csv");
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        });

        // ====== SHOW/HIDE TABLE ======
        viewDataButtonElement.addEventListener("click", () => {
            tableContainerElement.style.display = "block";
            viewDataButtonElement.style.display = "none";
            hideDataButtonElement.style.display = "inline-block";
            loadDataButtonElement.style.display = "inline-block";
            downloadCsvButton.style.display = "inline-block";
            renderTable();
        });

        hideDataButtonElement.addEventListener("click", () => {
            tableContainerElement.style.display = "none";
            viewDataButtonElement.style.display = "inline-block";
            hideDataButtonElement.style.display = "none";
            loadDataButtonElement.style.display = "none";
            downloadCsvButton.style.display = "none";

            batchSize = 10; // reset batch
            renderTable();
        });

    } else {
        loginElement.style.display = "block";
        authBarElement.style.display = "none";
        userDetailsElement.style.display = "none";
        contentElement.style.display = "none";
    }
};