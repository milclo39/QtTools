#ifndef json_util_h
#define json_util_h


#include "raptor_service/picojson.h"


inline std::string string_from(picojson::object obj, const std::string name,
                                    const std::string defaultValue)
{
    if(obj[name].is<std::string>())
    {
        return obj[name].get<std::string>();
    }
    else
    {
#ifdef DEBUG
        std::cout << "No " << name << " in json. (or type is not <std::string>.)" << std::endl;
#endif
        return defaultValue;
    }
}

inline std::string string_from(picojson::object obj, const std::string name)
{
    return string_from(obj, name, std::string());
}

inline picojson::object object_from(picojson::object obj, const std::string name)
{
    if(obj[name].is<picojson::object>())
    {
        return obj[name].get<picojson::object>();
    }
    else
    {
#ifdef DEBUG
        std::cout << "No " << name << " in json. (or type is not <picojson::object>.)" << std::endl;
#endif
        return picojson::object();
    }
}

inline picojson::array array_from(picojson::object obj, const std::string name)
{
    if(obj[name].is<picojson::array>())
    {
        return obj[name].get<picojson::array>();
    }
    else
    {
#ifdef DEBUG
        std::cout << "No " << name << " in json. (or type is not <picojson::array>.)" << std::endl;
#endif
        return picojson::array();
    }
}

inline std::vector<std::string> vector_string_from(picojson::object obj, const std::string name)
{
    if(obj[name].is<picojson::array>())
    {
        std::vector<std::string> result;
        
        const picojson::array& a = obj[name].get<picojson::array>();
        for (picojson::array::const_iterator ite = a.begin(); ite != a.end(); ++ite)
        {
            result.push_back(ite->get<std::string>());
        }
        return result;
    }
    else
    {
#ifdef DEBUG
        std::cout << "No " << name << " in json. (or type is not <picojson::array>.)" << std::endl;
#endif
        return std::vector<std::string>();
    }
}

inline double number_from(picojson::object obj, const std::string name,
                                const double defaultValue = -1)
{
    if(obj[name].is<double>())
    {
        return obj[name].get<double>();
    }
    else
    {
#ifdef DEBUG
        std::cout << "No " << name << " in json. (or type is not <double>.)" << std::endl;
#endif
        return defaultValue;
    }
}

inline bool bool_from(picojson::object obj, const std::string name, const bool defaultValue)
{
    if(obj[name].is<bool>())
    {
        return obj[name].get<bool>();
    }
    else
    {
#ifdef DEBUG
        std::cout << "No " << name << " in json. (or type is not <bool>.)" << std::endl;
#endif
        return defaultValue;
    }
}

#endif

