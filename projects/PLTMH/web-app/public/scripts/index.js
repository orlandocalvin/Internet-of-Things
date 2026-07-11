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
    apiKey: "AIzaSyBlEgmF5ViEQpIMny71RwGS7J2s2vprAbs",
    authDomain: "pltmh-project.firebaseapp.com",
    databaseURL: "https://pltmh-project-default-rtdb.asia-southeast1.firebasedatabase.app",
    projectId: "pltmh-project",
    storageBucket: "pltmh-project.firebasestorage.app",
    messagingSenderId: "381276964103",
    appId: "1:381276964103:web:ea26efd38e63a668e3b1a2"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const database = getDatabase(app);
export { auth };

// ===== Helper Functions =====
const epochToDateTime = (epochTime) => {
    if (!epochTime) return "N/A";
    const date = new Date(epochTime * 1000);
    const day = String(date.getDate()).padStart(2, "0");
    const month = String(date.getMonth() + 1).padStart(2, "0");
    const year = date.getFullYear();
    const hours = String(date.getHours()).padStart(2, "0");
    const minutes = String(date.getMinutes()).padStart(2, "0");
    const seconds = String(date.getSeconds()).padStart(2, "0");
    return `${day}/${month}/${year} ${hours}:${minutes}:${seconds}`;
};
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
const voltElement = document.getElementById("volt");
const currElement = document.getElementById("curr");
const powerElement = document.getElementById("power");
const rpmElement = document.getElementById("rpm");
const updateElement = document.getElementById("lastUpdate");

const formatInt = (value) => (Number.isFinite(value) ? Math.round(value) : "N/A");

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
        let latestData = { voltage: null, current: null, power: null, rpm: null, timestamp: null };

        onValue(query(dbRef, orderByKey()), (snapshot) => {
            const dataObj = snapshot.val();
            if (!dataObj) return;

            const lastKey = Object.keys(dataObj).sort().pop();
            const data = dataObj[lastKey] || {};

            if (typeof data.voltage !== "undefined") latestData.voltage = data.voltage;
            if (typeof data.current !== "undefined") latestData.current = data.current;
            if (typeof data.power !== "undefined") latestData.power = data.power;
            if (typeof data.rpm !== "undefined") latestData.rpm = data.rpm;
            if (typeof data.timestamp !== "undefined") latestData.timestamp = data.timestamp;

            const allReady =
                latestData.voltage !== null &&
                latestData.current !== null &&
                latestData.power !== null &&
                latestData.rpm !== null &&
                latestData.timestamp !== null;

            if (allReady) {
                voltElement.textContent = formatReading(latestData.voltage);
                currElement.textContent = formatReading(latestData.current);
                powerElement.textContent = formatReading(latestData.power);
                rpmElement.textContent = formatInt(latestData.rpm);
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
            voltElement.textContent = currElement.textContent =
                powerElement.textContent = rpmElement.textContent =
                updateElement.textContent = "N/A";
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

                // Buffer per timestamp
                const buffer = {};
                Object.keys(dataObj).forEach((key) => {
                    const data = dataObj[key];
                    if (!data.timestamp) return;

                    const ts = data.timestamp;
                    if (!buffer[ts]) {
                        buffer[ts] = { voltage: null, current: null, power: null, rpm: null, timestamp: ts };
                    }

                    if (typeof data.voltage !== "undefined") buffer[ts].voltage = data.voltage;
                    if (typeof data.current !== "undefined") buffer[ts].current = data.current;
                    if (typeof data.power !== "undefined") buffer[ts].power = data.power;
                    if (typeof data.rpm !== "undefined") buffer[ts].rpm = data.rpm;
                });

                const ordered = Object.values(buffer).sort((a, b) => b.timestamp - a.timestamp);

                ordered.forEach((row) => {
                    if (
                        row.voltage !== null &&
                        row.current !== null &&
                        row.power !== null &&
                        row.rpm !== null
                    ) {
                        const tr = document.createElement("tr");
                        tr.innerHTML = `
                            <td>${epochToDateTime(row.timestamp)}</td>
                            <td>${formatReading(row.voltage)}</td>
                            <td>${formatReading(row.current)}</td>
                            <td>${formatReading(row.power)}</td>
                            <td>${formatInt(row.rpm)}</td>
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

            const rows = [["Timestamp", "Voltage (V)", "Current (A)", "Power (W)", "RPM"]];
            Object.keys(dataObj).forEach(key => {
                const d = dataObj[key];
                rows.push([
                    d.timestamp ? epochToDateTime(d.timestamp) : "",
                    d.voltage ?? "",
                    d.current ?? "",
                    d.power ?? "",
                    Number.isFinite(d.rpm) ? Math.round(d.rpm) : ""
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