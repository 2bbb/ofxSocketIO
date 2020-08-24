//
//  ofxSocketIOUtilities.cpp
//
//  Created by 2bit on 2020/08/24.
//

#include "ofxSocketIOUtilities.h"

ofJson toJSON(sio::message::ptr m) {
    ofJson json;
    switch(m->get_flag()) {
        case sio::message::flag_null:
            break;
        case sio::message::flag_boolean:
            json = m->get_bool();
            break;
        case sio::message::flag_integer:
            json = m->get_int();
            break;
        case sio::message::flag_double:
            json = m->get_double();
            break;
        case sio::message::flag_string:
            json = m->get_string();
            break;
        case sio::message::flag_binary: {
            std::vector<std::uint8_t> bin;
            const auto &raw = *(m->get_binary());
            const std::uint8_t *ptr = (std::uint8_t *)raw.data();
            bin.resize(raw.length());
            std::copy(ptr, ptr + raw.length(), bin.begin());
            json = bin;
            break;
        }
        case sio::message::flag_array: {
            auto vs = std::vector<ofJson>{};
            const auto &raw = m->get_vector();
            vs.resize(raw.size());
            std::transform(raw.begin(), raw.end(), vs.begin(), [](sio::message::ptr m) {
                return toJSON(m);
            });
            json = vs;
            break;
        }
        case sio::message::flag_object: {
            auto res = std::map<std::string, ofJson>{};
            const auto &raw = m->get_map();
            for(const auto &p : raw) {
                res.insert(std::make_pair(p.first, toJSON(p.second)));
            }
            json = res;
            break;
        }
        default: break;
    }
    return json;
}

sio::message::ptr toSIOMessage(const ofJson &data) {
    sio::message::ptr message;
    switch(data.type()) {
        case nlohmann::detail::value_t::null:
        case nlohmann::detail::value_t::discarded:
            message = sio::null_message::create();
            break;
        case nlohmann::detail::value_t::boolean:
            message = sio::bool_message::create(data.get<bool>());
            break;
        case nlohmann::detail::value_t::number_integer:
            message = sio::int_message::create(data.get<std::int64_t>());
            break;
        case nlohmann::detail::value_t::number_unsigned:
            message = sio::int_message::create(data.get<std::uint64_t>());
            break;
        case nlohmann::detail::value_t::number_float:
            message = sio::double_message::create(data.get<double>());
            break;
        case nlohmann::detail::value_t::string:
            message = sio::string_message::create(data.get<std::string>());
            break;
        case nlohmann::detail::value_t::array: {
            message = sio::array_message::create();
            auto &vs = message->get_vector();
            vs.resize(data.size());
            for(auto i = 0; i < data.size(); ++i) {
                vs.push_back(toSIOMessage(data[i]));
            }
            break;
        }
        case nlohmann::detail::value_t::object: {
            message = sio::object_message::create();
            auto &map = message->get_map();
            for(const auto &p : data.items()) {
                map.insert(std::make_pair(p.key(), toSIOMessage(p.value())));
            }
            break;
        }
    }
    return message;
}
