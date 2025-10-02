import { auth, setupUI } from "./index.js";
import {
    signInWithEmailAndPassword,
    signOut,
    onAuthStateChanged
} from "https://www.gstatic.com/firebasejs/11.6.0/firebase-auth.js";

document.addEventListener("DOMContentLoaded", () => {
    // Watch auth state changes
    onAuthStateChanged(auth, (user) => {
        console.log(user ? `User logged in: ${user.email}` : "User logged out");
        setupUI(user);
    });

    // Login form submission
    const loginForm = document.getElementById("login-form");
    loginForm.addEventListener("submit", async (e) => {
        e.preventDefault();
        const email = loginForm["input-email"].value;
        const password = loginForm["input-password"].value;

        try {
            await signInWithEmailAndPassword(auth, email, password);
            loginForm.reset();
            console.log("Login successful:", email);
        } catch (error) {
            const errorMsg = document.getElementById("error-message");
            errorMsg.textContent = error.message;
            console.error("Login error:", error.message);
        }
    });

    // Logout link click
    const logoutLink = document.getElementById("logout-link");
    logoutLink.addEventListener("click", async (e) => {
        e.preventDefault();
        try {
            await signOut(auth);
            console.log("User signed out");
        } catch (error) {
            console.error("Logout error:", error.message);
        }
    });
});