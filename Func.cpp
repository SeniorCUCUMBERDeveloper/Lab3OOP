#include "./Octree/Octree.hpp"
#include "./Storage/Storage.hpp"
#include "./Container/Container.hpp"
#include "./Terminal/Terminal.hpp"
#include "./Container/FragileContainer.hpp"
#include "./Container/RefragedContainer.hpp"
#include "./Request/Request.hpp"


void Storage::checkTemperature(IContainer* container, ContainerPosition<int> pos){
    IRefragedContainer* ref = dynamic_cast<IRefragedContainer*>(container);
    if(((*container).isType() == "Refraged" || (*container).isType() == "Fragile and Refraged Container") &&
    temperature > (*ref).getMaxTemperature())
    {
        throw std::invalid_argument("Container is too hot");
    }
}


void Storage::checkPressure(IContainer* container, ContainerPosition<int> pos){
    if(pos.LLDown.z != 0){
        std::vector<std::pair<ContainerPosition<int>,IContainer*>> con = searchUnderContainer(pos);
        if(con.empty()){
            throw std::invalid_argument("Container can t fly 1");
        }
        if(!checkSupport(pos, con)){
            throw std::invalid_argument("Support doesn t exist");
        }
        for(size_t i = 0; i < con.size(); i++){
            ContainerPosition<int> check = con[i].first;
            if(((*con[i].second).isType() == "Fragile" || (*con[i].second).isType() == "Fragile and Refraged Container") && calculatemass(con, i) + (*container).getMass() > (*(dynamic_cast<IFragileContainer*>(con[i].second))).getMaxPressure()){
                throw std::invalid_argument("Container would be too heavy");
            }
        }
    }
}