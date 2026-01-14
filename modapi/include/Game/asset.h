#ifndef ASSET_H
#define ASSET_H

class Asset {
    private:
        static inline Globals_Canvas* globals_canvas = 0;
    public:
        static void init(void);
        static std::string getassetfilepath(unsigned int id);
        static void setassetfilepath(unsigned int id, const std::string value);
        static std::string gettext(int id);
};
#endif