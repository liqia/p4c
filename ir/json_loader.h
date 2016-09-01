/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef _IR_JSON_LOADER_H_
#define _IR_JSON_LOADER_H_

#include <assert.h>
#include <string>
#include <map>
#include "lib/cstring.h"
#include "lib/indent.h"
#include "lib/match.h"
#include <gmpxx.h>
#include <unordered_map>
#include "json_parser.h"

#include "ir.h"
struct TableResourceAlloc;

class JSONLoader {
    template<typename T>
    class has_fromJSON
    {
        typedef char small;
        typedef struct { char c[2]; } big;

        template<typename C> static small test(decltype(&C::fromJSON));
        template<typename C> static big test(...);
    public: 
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

public:
    std::unordered_map<int, IR::Node*> &node_refs;
    JsonData *json;

    JSONLoader(std::istream &in) :
        node_refs(*(new std::unordered_map<int, IR::Node*>()))
    { in >> json; }

    JSONLoader(JsonData *json) : 
        node_refs(*(new std::unordered_map<int, IR::Node*>())),
        json(json) {}

    JSONLoader(const JSONLoader &unpacker, const std::string &field) :
        node_refs(unpacker.node_refs)
    { 
        JsonObject* obj = dynamic_cast<JsonObject*>(unpacker.json);
        assert(obj->find(field) == obj->end());
        
        json = obj->find(field)->second;
    }

private: 
    int get_id() { return json->to<JsonObject>()->get_id(); }

    std::string get_type() { return json->to<JsonObject>()->get_type(); }

    template<typename T>
    void unpack_json(vector<T> &v) {
        (void)v;
    }

    template<typename T>
    void unpack_json(IR::Vector<T> &v) {
        (void)v;
    }

    template<typename T>
    void unpack_json(std::vector<T> &v) {
        (void)v;
    }

    template<typename T, typename U>
    void unpack_json(std::pair<T, U> &v)
    {
        (void)v;
    }

    template<typename K, typename V>
    void unpack_json(ordered_map<K, V> &v)
    { 
        (void)v;
    }

    void unpack_json(bool &v) {
       v = *json->to<JsonBoolean>();
    }
    
    template<typename T>
    typename std::enable_if<std::is_integral<T>::value>::type
    unpack_json(T &v) { 
        v = *json->to<JsonNumber>();
    }
    void unpack_json(mpz_class &v) {
        v = *json->to<JsonNumber>();
    }

    void unpack_json(cstring &v) {
        v = *json->to<std::string>();
    }

    void unpack_json(IR::ID &v) { (void)v; }
    void unpack_json(const struct TableResourceAlloc*& v) { (void)v; }
    
    template<typename T>
    typename std::enable_if<
                std::is_same<T, LTBitMatrix>::value ||
                std::is_enum<T>::value>::type
    unpack_json(T v) {
        (void)v;
    }

    void unpack_json(match_t &v) {
        (void)v;
    }

    template<typename T>
    typename std::enable_if<
                    has_fromJSON<T>::value &&
                    !std::is_base_of<IR::Node, T>::value>::type
    unpack_json(const T &v) {
        v = *(T::fromJSON(json));
    }

    void unpack_json(IR::Node &v) {
        if (get_id() > 0 
                && node_refs.find(get_id()) != node_refs.end()) { 
            v = *node_refs[get_id()];
        } else if (!get_type().empty()) {
            IR::Node* node = IR::unpacker_table[get_type()](*this);
            v = *node;
            node_refs[get_id()] = node;
        }
    }

    void unpack_json(IR::Node* &p) {
        if (get_id() > 0 
                && node_refs.find(get_id()) != node_refs.end()) {
            p = node_refs[get_id()];
        } else if (!get_type().empty()) {
            IR::Node* node = IR::unpacker_table[get_type()](*this);
            p = node;
            node_refs[get_id()] = node;
        } 
    }

    template<typename T>
    typename std::enable_if<
                    std::is_pointer<T>::value &&
                    std::is_base_of<IR::Node, 
                                    typename std::remove_pointer<T>::type
                    >::value>::type                    
    unpack_json(T &v) {
        if (get_id() > 0 
            && node_refs.find(get_id()) != node_refs.end()) {
            v = dynamic_cast<T>(node_refs[get_id()]);
        } else if (!get_type().empty()) {
            IR::Node* node = IR::unpacker_table[get_type()](*this);
            v = dynamic_cast<T>(node);
            node_refs[get_id()] = node;
        } 
    }

    template<typename T, size_t N>
    void unpack_json(T (&v)[N]) {
        (void)v;
    }
public:
    template<typename T> 
    void load(const std::string field, T &v) { 
        JSONLoader(*this, field).unpack_json(v);
    }

    template<typename T> JSONLoader& operator>>(T &v) {
        unpack_json(v);
        return *this;
    }

};


#endif /* _IR_JSON_LOADER_H_ */
