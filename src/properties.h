/*
Copyright 2018 Ezequiel Conde

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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <map>

template<typename K, typename V>
class Properties {
	std::map<K, V> map;
public:
	Properties() {}
	
	void set_property(K key, V value) { map.insert(std::pair<K, V>(key, value)); } 
	
	V get_property(K key, V default_value = 0) {
		typename std::map<K, V>::iterator it = map.find(key);
		if (it != map.end())
			return it->second;
		return default_value;
	}
};

#endif
