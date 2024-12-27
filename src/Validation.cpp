#include "../inc/core_server.hpp"
#include <cctype>

bool isStringDigits(const std::string& str)
{
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!isdigit(*it))
            return false;
    }
    return true;
}

bool IsValidPort(std::string port, int& _tport)
{
    std::cout << formatServerMessage("DEBUG", "Validating port: " + port) << std::endl;
    
    if (!isStringDigits(port))
    {
        std::cout << formatServerMessage("ERROR", "Port contains non-digits") << std::endl;
        return false;
    }

    char* end;
    long val = std::strtol(port.c_str(), &end, 10);
    
    if (*end != '\0' || val <= 0 || val > 65535)
    {
        std::cout << formatServerMessage("ERROR", "Port out of range") << std::endl;
        return false;
    }
        
    _tport = static_cast<int>(val);
    std::cout << formatServerMessage("DEBUG", "Port valid: " + _tport) << std::endl;
    return true;
}

bool IsValidPass(std::string _pass, std::string& passwd)
{
    std::cout << formatServerMessage("DEBUG", "Validating password") << std::endl;
    
    if (_pass.length() < 8)
    {
        std::cout << formatServerMessage("ERROR", "Password too short") << std::endl;
        return false;
    }

    bool Lower = false, Upper = false, Digit = false, Special = false;
    
    for (std::string::iterator it = _pass.begin(); it != _pass.end(); ++it)
    {
        if (islower(*it)) Lower = true;
        else if (isupper(*it)) Upper = true;
        else if (isdigit(*it)) Digit = true;
        else if (ispunct(*it)) Special = true;
    }
    
    if (!(Lower && Upper && Digit && Special))
    {
        std::cout << formatServerMessage("ERROR", "Password missing required characters") << std::endl;
        return false;
    }
        
    passwd = _pass;
    std::cout << formatServerMessage("DEBUG", "Password valid") << std::endl;
    return true;
}
