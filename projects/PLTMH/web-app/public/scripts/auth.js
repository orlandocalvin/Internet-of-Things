import { auth } from "./index.js";
import { signInWithEmailAndPassword, signOut, onAuthStateChanged } from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";

document.addEventListener("DOMContentLoaded", () => {
    onAuthStateChanged(auth, (user) => {
        user ? setupUI(user) : setupUI(null);
    });

    const loginForm = document.querySelector("#login-form");
    loginForm.addEventListener("submit", async (e) => {
        e.preventDefault();
        const email = loginForm["input-email"].value;
        const password = loginForm["input-password"].value;
        try {
            await signInWithEmailAndPassword(auth, email, password);
            loginForm.reset();
        } catch (error) {
            document.getElementById("error-message").textContent = error.message;
        }
    });

    const logoutLink = document.querySelector("#logout-link");
    logoutLink.addEventListener("click", async (e) => {
        e.preventDefault();
        await signOut(auth);
    });
});