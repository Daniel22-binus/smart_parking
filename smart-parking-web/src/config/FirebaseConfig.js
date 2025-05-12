import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getAuth, signInWithEmailAndPassword  } from "firebase/auth";
import { getDatabase, connectDatabaseEmulator } from "firebase/database";

const firebaseConfig = {
  apiKey: "AIzaSyB-7wTDQa5SimmdxtpV2D9m8WOiaEQ8L14",
  authDomain: "smart-parking-esp32-1c799.firebaseapp.com",
  databaseURL: "https://smart-parking-esp32-1c799-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "smart-parking-esp32-1c799",
  storageBucket: "smart-parking-esp32-1c799.firebasestorage.app",
  messagingSenderId: "804636298635",
  appId: "1:804636298635:web:d2ec6de24f8c01b690f38e"
};

const firebaseApp = initializeApp(firebaseConfig);
const firebaseAuth = getAuth(firebaseApp);
const firebaseDB = getDatabase(firebaseApp);
const analytics = getAnalytics(firebaseApp);

signInWithEmailAndPassword(firebaseAuth, "daniel025@binus.ac.id", "Test123")
.then(() => {
    console.log("Signed in successfully");
})
.catch((error) => {
    console.error("Auth error:", error.message);
});

export {
  firebaseApp,
  firebaseAuth,
  firebaseDB,
  analytics
};