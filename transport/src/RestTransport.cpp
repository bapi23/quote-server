#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

#include "RestTransport.hpp"

std::string RestTransport::request(const std::string& address){
    std::stringstream ss;

    try {
        curlpp::Cleanup myCleanup;
        curlpp::Easy request;
        
        request.setOpt(new curlpp::options::Url(address));
        request.setOpt(new curlpp::options::UserAgent("Chrome: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.90 Safari/537.36."));

        ss << request;
        std::string output = ss.str();
        return output;
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what() << std::endl;
    }

    return "";
}