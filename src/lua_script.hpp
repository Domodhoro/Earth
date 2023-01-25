#ifndef LUA_SCRIPT_HPP
#define LUA_SCRIPT_HPP

namespace lua_script {

struct lua_script {
    lua_script(const char* script_path) : m_L {luaL_newstate()} {
        luaL_openlibs(m_L);

        if (luaL_dofile(m_L, script_path) != LUA_OK) my_exception {__FILE__, __LINE__, "falha ao ler script lua"};

        lua_getglobal(m_L, "window");

        WINDOW_WIDTH  = static_cast<int>(get_number("width"));
        WINDOW_HEIGHT = static_cast<int>(get_number("height"));
        WINDOW_TITLE  = get_string("title");

        lua_getglobal(m_L, "camera");

        CAMERA_SPEED       = get_number("speed");
        CAMERA_FOV         = get_number("FOV");
        CAMERA_SENSITIVITY = get_number("sensitivity");
    }

    ~lua_script() {
        lua_close(m_L);
    }

    float get_number(const char *key) const {
        lua_pushstring(m_L, key);
        lua_gettable(m_L, -2);

        auto result {lua_tonumber(m_L, -1)};

        lua_pop(m_L, 1);

        return result;
    }

    const char *get_string(const char *key) const {
        lua_pushstring(m_L, key);
        lua_gettable(m_L, -2);

        auto result {lua_tostring(m_L, -1)};

        lua_pop(m_L, 1);

        return result;
    }

private:
    lua_State *m_L {nullptr};
};

}

#endif
