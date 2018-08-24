#pragma once
#include "std_lib_facilities.h"
#include "ComponentCache.h"
#include "Entity.h"

template<typename ... CompTypes>
class EntityIterator;

template<typename ... CompTypes>
class EntityCache
{
	
	friend class EntityIterator<CompTypes...>;
	friend class Entity<CompTypes...>;

	typedef EntityIterator<CompTypes...> iterator;
	typedef map<unsigned long, tuple<shared_ptr<ComponentCache<CompTypes>>...>> SignatureMap;
	
public:
	EntityCache() {}
	~EntityCache() {}
	template<typename CompType>
	void Insert(unsigned long signature, shared_ptr<ComponentCache<CompType>> cache) {
		auto & it = m_cache.find(signature);
		if (it == m_cache.end()) {
			tuple<shared_ptr<ComponentCache<CompTypes>>...> tuple;
			std::get<shared_ptr<ComponentCache<CompType>>>(tuple) = cache;
			m_cache.insert(make_pair(signature, tuple));
		}
		else {
			std::get<shared_ptr<ComponentCache<CompType>>>(it->second) = cache;
		}
		
	}
	iterator begin() {
		return iterator(this, 0);
	}
	iterator end() {
		return iterator(this, GetSize());
	}
	int GetSize() {
		int size = 0;
		for (auto & sigCache : m_cache) {
			size += get<0>(sigCache.second)->Get().size();
		}
		return size;
	}
private:
	SignatureMap m_cache;
};

template<typename ... CompTypes>
class EntityIterator {
	typedef Entity<CompTypes...> EntityType;
	typedef EntityCache<CompTypes...> CacheType;
	typedef EntityIterator<CompTypes...> iterator;
	typedef typename map<unsigned long, tuple<shared_ptr<ComponentCache<CompTypes>>...>>::iterator SignatureIterator;
public:
	EntityIterator(CacheType * entityCache, int offset) : m_entityCache(entityCache), m_offset(offset), m_entity(entityCache->m_cache.begin(),0) {
		
	}
	bool operator==(const iterator & other) {
		return m_entityCache == other.m_entityCache && m_offset == other.m_offset;
	}
	bool operator!=(const iterator & other) {
		return !(*this == other);
	}
	Entity<CompTypes...> & operator*() {
		return m_entity;
	}
	iterator & operator++() {
		++m_offset;
		if (++m_entity.m_index >= get<0>(m_entity.m_mapIterator->second)->Get().size()) {
			m_entity.m_index = 0;
			++(m_entity.m_mapIterator);
		}
		return *this;
	}
	iterator operator++(int) {
		EntityIterator<CompTypes...> clone(*this);
		++(*this);
		return clone;
	}
	/*void Reset(iterator && other) {
		m_index = other.m_index;
		m_entity = other.m_entity;
		m_offset = other.m_offset;
		m_entityCache = other.m_entityCache;
		m_mapIterator = other.m_mapIterator;
	}*/
private:
	CacheType * m_entityCache;
	EntityType m_entity;
	int m_offset;
};

