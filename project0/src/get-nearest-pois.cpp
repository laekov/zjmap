//
// Created by zijia on 2022/4/6.
//

#include "get-nearest-pois.h"

std::vector<POI> MapCore::get_nearest_pois(Coord coord) {
    std::vector<POI> pois;
    auto mark_id = marks.find_nearest(coord);
    auto poi_ids = get_poi_ids(mark_id);
    for (auto poi_id : poi_ids)
        pois.push_back(get_poi(poi_id));
    /*
     * You need to find the nearest mark point by coordinate coord,
     * then find the related POI IDs by the result mark_id, finally.
     * Finally, you need to retrieve all the POI data and put them into the vector pois.
     * */
    return pois;
}