#ifndef STATION_H
#define STATION_H

class Station {
    private:
        static inline Globals_status* globals_status = 0;
    public:
        static inline std::vector<SingleStation> created_stations;
        static void init(void);
        static int getid(void);
        static void setid(int value);
        static std::string getname(void);
        static void setname(const std::string value);
        static int gettechlevel(void);
        static void settechlevel(int value);
        static int getangaritemscount(void);
        static void setangaritemscount(int value);
        static int getangarshipscount(void);
        static void setangarshipscount(int value);
        static int getagentscount();
        static void setagentscount(int value);

        static bool isvoid(void);
        static void setangarshipid(int id, int value);
        static int create(const std::string& str, int techlevel, int textureid, int systemid);
        static std::string getagentname(int id);
        static int getagentfaction(int id);
        static void createagent(const std::string& str, int factiontype, int terranwoman, int hair, int eyes, int mouth, int armor, sol::table agentinfo);
};
#endif