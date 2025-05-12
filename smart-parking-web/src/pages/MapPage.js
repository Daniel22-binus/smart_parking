import React, { useEffect, useState } from "react";
import {firebaseApp, firebaseDB} from "../config/FirebaseConfig"
import {ref, onValue, get} from "firebase/database";

const MapPage = () => {

    const [data, setData] = useState({});

    useEffect(() => {
        const collectionRef = ref(firebaseDB, "smart-parking/slot-parking");

        const intervalId = setInterval(async () => {
            try {
                const snapshot = await get(collectionRef);
                if (snapshot.exists()) {
                    setData(snapshot.val());
                } else {
                    setData({});
                }
            } catch (error) {
                console.error('Error fetching data:', error);
            }
        }, 1000);

        return () => {
            clearInterval(intervalId);
        };
    }, []);

    return(
        <>
            <h1>Map Page</h1>
            {data.slot1}
            {data["value"]}
            <pre>{JSON.stringify(data, null, 2)}</pre>
        </>
    );

}

export default MapPage;