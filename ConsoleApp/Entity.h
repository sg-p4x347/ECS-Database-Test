#pragma once
#include "std_lib_facilities.h"
#include "ComponentCache.h"


template<typename ... CompTypes>
class Entity {
	typedef typename map<unsigned long, tuple<shared_ptr<ComponentCache<CompTypes>>...>>::iterator iterator;
public:
	Entity(iterator & mapIterator, int index) : m_mapIterator(mapIterator), m_index(index) {

	}

	template<typename CompType>
	CompType & Get() {
		return get<shared_ptr<ComponentCache<CompType>>>(m_mapIterator->second)->Get()[m_index];
	}
	unsigned int GetID() {
		return get<0>(m_mapIterator->second)->Get()[m_index].ID;
	}
	unsigned long GetSignature() {
		return m_mapIterator->first;
	}
	iterator m_mapIterator;
	int m_index;
};

