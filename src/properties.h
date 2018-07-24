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
