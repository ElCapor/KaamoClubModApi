#include <Game/item.h>

int Item::create(const std::string& name, const std::string& description) {
    if (EventManager::isearlyinit_finished) {
        std::cout << "[-] Failed to call item:Create()" << std::endl;
        return -1;
    }
    int len = MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, nullptr, 0);
    std::wstring namews(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, namews.data(), len);
    len = MultiByteToWideChar(CP_UTF8, 0, description.c_str(), -1, nullptr, 0);
    std::wstring descriptionws(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, description.c_str(), -1, descriptionws.data(), len);

    ModdedItems m;
    m.id = (int)created_items.size() + 196;
    m.name = namews;
    m.description = descriptionws;
    memset(&m.item, 0, sizeof(SingleItem));
    m.item.m_nID = m.id;
    m.item.m_nType = 3;
    m.item.m_nSubType = 14;
    m.item.m_nTechLevel = 8;
    m.item.m_nLowestPriceSystemId = 10;
    m.item.m_nHighestPriceSystemId = 4;
    m.item.m_nPrice = 77937;
    m.item.m_nMinPrice = 72500;
    m.item.m_nMaxPrice = 83375;
    AEArray<ItemInfo>* iteminfoarr = (AEArray<ItemInfo>*)AbyssEngine::memory_allocate(sizeof(AEArray<ItemInfo>));
    iteminfoarr->size = 30;
    iteminfoarr->size2 = 30;
    iteminfoarr->data = (ItemInfo*)AbyssEngine::memory_allocate(sizeof(ItemInfo));
    memset(iteminfoarr->data, 0, sizeof(ItemInfo));
    ItemInfo* info = &iteminfoarr->data[0];
    info->field_0 = 0;
    info->m_nID = (int)created_items.size() + 196;
    info->m_bIsThermo = 1;
    //info->m_nType = 3;
    info->m_nType = 0;
    info->field_10 = 2;
    //info->m_nSubType = 14;
    info->m_nSubType = 0;
    info->field_18 = 3;
    info->m_nTechLevel = 8;
    info->m_nHighestPriceSystemId = 4;
    info->m_nLowestPriceSystemId = 10;
    info->field_28 = 5;
    //info->field_2C = 4;
    info->field_2C = 7;
    info->field_30 = 6;
    //info->field_34 = 0;
    info->field_34 = 90;
    info->field_38 = 7;
    //info->field_3C = 72500;
    info->field_3C = 2000;
    info->field_40 = 8;
    info->m_nPrice = 83375;
    //info->field_48 = 25;
    info->field_48 = 9;
    //info->field_4C = 300; // effect
    info->field_4C = 1113;
    //info->m_nPropertyOne = 26;
    info->m_nPropertyOne = 11;
    //info->m_nPropertyOneValue = 16000;
    info->m_nPropertyOneValue = 400;
    //info->m_nPropertyTwo = 27;
    info->m_nPropertyTwo = 12;
    //info->m_nPropertyTwoValue = 910000;
    info->m_nPropertyTwoValue = 2000;
    //info->m_nPropertyThree = 44;
    info->m_nPropertyThree = 13;
    //info->m_nPropertyThreeValue = 0;
    info->m_nPropertyThreeValue = 20;
    //info->m_nPropertyFour = 45;
    info->m_nPropertyFour = 44;
    //info->m_nPropertyFourValue = 107;
    info->m_nPropertyFourValue = 0;
    //info->field_70 = 0;
    info->field_70 = 45;
    //info->field_74 = 0;
    info->field_74 = 52;
    m.item.m_pItemInfo = iteminfoarr;
    created_items.push_back(m);
    return m.id;
}

void Item::refreshitemsprices() {
    Globals_status** globals_status_ptr = (Globals_status**)0x0060AD6C;
    Globals_status* globals_status = *globals_status_ptr;
    int old_size = 196;
    int newsize = old_size + (int)created_items.size();

    if(created_items.size() == 0)
        return;

    AEArray<unsigned int>* oldlow = globals_status->m_pItemLowestPrices;
    AEArray<unsigned int>* newlow = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newlow->size = newsize;
    newlow->size2 = newsize;
    newlow->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * newsize);
    memcpy(newlow->data, oldlow->data, sizeof(unsigned int) * old_size);
    for (int i = 0; i < (int)created_items.size(); i++)
        newlow->data[old_size + i] = created_items[i].item.m_nMinPrice;
    globals_status->m_pItemLowestPrices = newlow;
    AbyssEngine::memory_free(oldlow->data);
    AbyssEngine::memory_free(oldlow);

    AEArray<unsigned int>* oldhigh = globals_status->m_pItemHighestPrices;
    AEArray<unsigned int>* newhigh = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newhigh->size = newsize;
    newhigh->size2 = newsize;
    newhigh->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * newsize);
    memcpy(newhigh->data, oldhigh->data, sizeof(unsigned int) * old_size);
    for (int i = 0; i < (int)created_items.size(); i++)
        newhigh->data[old_size + i] = created_items[i].item.m_nMaxPrice;
    globals_status->m_pItemHighestPrices = newhigh;
    AbyssEngine::memory_free(oldhigh->data);
    AbyssEngine::memory_free(oldhigh);

    AEArray<unsigned int>* oldlowsys = globals_status->m_pItemLowestPricesSystem;
    AEArray<unsigned int>* newlowsys = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newlowsys->size = newsize;
    newlowsys->size2 = newsize;
    newlowsys->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * newsize);
    memcpy(newlowsys->data, oldlowsys->data, sizeof(unsigned int) * old_size);
    for (int i = 0; i < (int)created_items.size(); i++)
        newlowsys->data[old_size + i] = created_items[i].item.m_nLowestPriceSystemId;
    globals_status->m_pItemLowestPricesSystem = newlowsys;
    AbyssEngine::memory_free(oldlowsys->data);
    AbyssEngine::memory_free(oldlowsys);

    AEArray<unsigned int>* oldhighsys = globals_status->m_pItemHighestPricesSystem;
    AEArray<unsigned int>* newhighsys = (AEArray<unsigned int>*)AbyssEngine::memory_allocate(sizeof(AEArray<unsigned int>));
    newhighsys->size = newsize;
    newhighsys->size2 = newsize;
    newhighsys->data = (unsigned int*)AbyssEngine::memory_allocate(sizeof(unsigned int) * newsize);
    memcpy(newhighsys->data, oldhighsys->data, sizeof(unsigned int) * old_size);
    for (int i = 0; i < (int)created_items.size(); i++)
        newhighsys->data[old_size + i] = created_items[i].item.m_nHighestPriceSystemId;
    globals_status->m_pItemHighestPricesSystem = newhighsys;
    AbyssEngine::memory_free(oldhighsys->data);
    AbyssEngine::memory_free(oldhighsys);
}