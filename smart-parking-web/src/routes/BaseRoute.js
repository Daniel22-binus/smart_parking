import { BrowserRouter, Routes, Route } from "react-router-dom";
import MapPage from "../pages/MapPage";
import NotFoundPage from "../exceptions/NotFoundPage";

const BaseRoute = () => {
    return (
        <BrowserRouter>
          <Routes>
            <Route path="/" element={<MapPage />} />
            {/* <Route path="/auth" element={<Navbar />}>
              
            </Route> */}
            <Route path="/*" element={<NotFoundPage />}/>
          </Routes>
        </BrowserRouter>
      );
}
export default BaseRoute;