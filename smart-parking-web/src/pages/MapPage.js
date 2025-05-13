import React, { useEffect, useState } from "react";
import {firebaseDB} from "../config/FirebaseConfig"
import {ref, get} from "firebase/database";

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

    const casePark = (slot) => {
        if (slot === 1) {
            return (
                <div style={custom_style.car_container}>
                    <img src="/assets/car.jpg" style={custom_style.car} />
                </div>
            )
        } else if (slot === 0) {
            return (
                <div style={custom_style.car_container}>
                    <div style={custom_style.car_empty_box} className="h-14 text-center bg-gradient-to-b from-yellow-200 to-red-500">
                        <label style={custom_style.car_label} className="inline-block align-baseline font-mono text-xl">empty</label>
                    </div>
                </div>
            )
        }

    }

    const checkParking = () => {
        // if (data.slot1 === 1 && data.slot2 === 1) {
        //     return full_parking_style.container
        // }

        // return full_parking_style.hidden_container

        let flag = true;
        for (let temp in data) {
            let slotValue = data[temp];
            
            if (slotValue !== 1) {
                flag = false;
                break;
            }
        }

        if (flag === true) {
            return full_parking_style.container
        }

        return full_parking_style.hidden_container
    }

    return(
        <>
            <div className="text-center">
                <h1 style={{fontFamily: "Trebuchet MS", fontWeight: "bold", fontSize: 30}}>MAP PAGE</h1>
            </div>
            {/* <pre>{JSON.stringify(data, null, 2)}</pre> */}
            <div style={{display: "flex"}}>
                <div style={custom_style.floor_parking}>
                    <div style={container_style.top_table} className="h-14 bg-gradient-to-r from-cyan-500 to-blue-500 text-center">
                        <label style={container_style.title_floor} className="text-xl">Floor 1</label>
                    </div>
                    <div style={container_style.bottom_table}>
                        <div className="bg-auto bg-size-[auto_10px] bg-center bg-no-repeat h-screen" style={custom_style.bg_image}>
                        <div style={checkParking()} className="bg-gray-100">
                            <div style={full_parking_style.center_container}>
                                <label style={full_parking_style.text_black}>Parking Full</label>
                            </div>
                        </div>
                        <div style={custom_style.flex_container}>
                            {casePark(data.slot1)}
                            {casePark(data.slot2)}
                        </div>
                    </div>
                    </div>
                </div>
                <div style={custom_style.floor_parking}>
                    <div style={container_style.top_table} className="h-14 bg-gradient-to-r from-cyan-500 to-blue-500 text-center">
                        <label style={container_style.title_floor} className="text-xl">Floor 2</label>
                    </div>
                    <div style={container_style.bottom_table}>
                        <div className="bg-auto bg-size-[auto_10px] bg-center bg-no-repeat h-screen" style={custom_style.bg_image}>
                        {/* <div style={custom_style.flex_container}>
                            {casePark(data.slot1)}
                            {casePark(data.slot2)}
                        </div> */}
                    </div>
                    </div>
                </div>
            </div>

        </>
    );

}

export default MapPage;

const container_style = {
    top_table: {
        borderTop: "3px solid black",
        borderLeft: "3px solid black",
        borderRight: "3px solid black",
        borderRadius: "30px 30px 0px 0px",
    },
    bottom_table: {
        borderBottom: "3px solid black",
        borderLeft: "3px solid black",
        borderRight: "3px solid black",
        borderRadius: "0px 0px 30px 30px",
        height: 600, 
    },
    title_floor: {
        paddingTop: 30,
        fontSize: 30,
        fontFamily: "Trebuchet MS",
        fontWeight: "bold"
    }
}

const custom_style = {
    floor_parking: {
        // border: "3px solid black",
        // borderRadius: 30,
        // boxShadow: "0 4px 20px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19)",
        margin: 10,
        padding: 10,
        width: 750
    },
    bg_image: {
        backgroundImage: `url("/assets/parking-slot.jpg")`,
        backgroundSize: "700px 500px",
        // border: "3px solid red",
        width: 700,
        height: 500,
        margin: "auto",

    },
    flex_container: {
        display: "flex",    
    },
    car_container: {
        width:"40%",
        height: 420,
        paddingLeft: "17%",
        paddingTop: "18%",
        // border: "3px solid blue",
    },
    car_empty_box: {
        height: "100%",
        borderRadius: 30,
    },
    car_label: {
        paddingTop: "80%",
        fontFamily: "Courier New",
        fontWeight: "bold"
    },
    car: {
        width: 170,
        height: "auto",
    },
}

const full_parking_style = {
    hidden_container: {
        display: "none"
    },
    container: {
        display: "flex",
        position: "absolute",
        paddingTop: 20,
        zIndex: 2,
        width: 700,
        height: 500,
        justifyContent: "center",
        opacity: 0.8
        // border: "3px solid green",
    },
    center_container: {
        paddingTop: "30%",
        opacity: 1
    },
    text_black: {
        opacity: 1,
        fontSize: 30,
        fontFamily: "Courier New",
        fontWeight: "bold",
        color: "black"
    }
}