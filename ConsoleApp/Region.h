#pragma once
#include "std_lib_facilities.h"
#include "ComponentCache.h"
#include "EntityCache.h"
#include "MappedFile.h"
template<typename ... CompTypes>
class Region
{
public:
	Region(string directory, tuple<pair<unsigned long, CompTypes>...> & maskIndex) : m_directory(directory), m_maskIndex(maskIndex) {
		Filesystem::create_directory(directory);
		LoadFile<CompTypes...>();
	}
	template<typename CompType>
	inline vector<std::shared_ptr<ComponentCache<CompType>>> LoadComponents(unsigned long signature) {
		vector<std::shared_ptr<ComponentCache<CompType>>> caches;
		for (auto & cacheMapping : std::get<std::map<unsigned long, shared_ptr<ComponentCache<CompType>>>>(m_componentCache)) {
			if (cacheMapping.first & signature == signature) {
				caches.push_back(cacheMapping.second);
			}
		}
		return caches;
	}
	template<typename HeadType, typename ... MaskTypes>
	inline void LoadEntities(EntityCache<HeadType, MaskTypes...> & entityCache, unsigned long componentMask) {
		// Iterate the cached entity signatures
		auto & componentMap = std::get<std::map<unsigned long, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache);
		for (auto & cacheMapping : componentMap) {
			if ((cacheMapping.first & componentMask) == componentMask) {
				// Proceed to load each component cache for this signature
				LoadComponents<EntityCache<HeadType, MaskTypes...>,HeadType,MaskTypes...>(cacheMapping.first, entityCache);
			}
		}
		// Iterate over entity signatures in the database
		unsigned long headMask = std::get<pair<unsigned long, HeadType>>(m_maskIndex).first;
		auto end = componentMap.end();
		for (auto & signature : m_files[headMask].IndexedKeys()) {
			if ((signature & componentMask) == componentMask && componentMap.find(signature) == end) {
				// Proceed to load and cache each component cache for this signature from the database
				ImportComponents<EntityCache<HeadType, MaskTypes...>, HeadType, MaskTypes...>(signature, headMask, entityCache);
			}
		}
	}
	template<typename EntityCacheType, typename HeadType, typename ... MaskTypes>
	inline void LoadComponents(unsigned long signature, EntityCacheType & cache) {
		cache.Insert(signature,std::get<std::map<unsigned long, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache)[signature]);
		LoadComponents<EntityCacheType, MaskTypes...>(signature, cache);
	}
	template<typename EntityCacheType>
	inline void LoadComponents(unsigned long signature, EntityCacheType & cache) {}

	template<typename EntityCacheType, typename HeadType, typename ... MaskTypes>
	inline void ImportComponents(unsigned long signature, unsigned long componentMask, EntityCacheType & cache) {
		auto & componentMap = std::get<std::map<unsigned long, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache);
		auto compCache = std::make_shared<ComponentCache<HeadType>>(signature, m_files[componentMask]);
		compCache->Import();
		componentMap.insert(std::make_pair(signature, compCache));
		cache.Insert(signature, compCache);
		ImportComponents<EntityCacheType, MaskTypes...>(signature, componentMask, cache);

	}
	template<typename EntityCacheType>
	inline void ImportComponents(unsigned long signature, unsigned long componentMask, EntityCacheType & cache) {}

	template<typename CompType>
	inline void InsertComponent(CompType component, unsigned long signature) {
		auto & sigMap = std::get<std::map<unsigned long, shared_ptr<ComponentCache<CompType>>>>(m_componentCache);
		auto it = sigMap.find(signature);
		if (it != sigMap.end()) {
			it->second->Insert(component);
		}
		else {
			auto cache = make_shared<ComponentCache<CompType>>(signature,m_files[std::get<pair<unsigned long, CompType>>(m_maskIndex).first]);
			cache->Insert(component);
			sigMap.insert(make_pair(signature, cache));
		}
	}
	void Save() {
		Save<CompTypes...>();
	}
	void EmptyCache() {
		EmptyCache<CompTypes...>();
	}
private:
	string m_directory;
	std::map<unsigned long, MappedFile<unsigned long>> m_files;
	tuple<pair<unsigned long, CompTypes>...> & m_maskIndex;
	std::tuple<std::map<unsigned long,shared_ptr<ComponentCache<CompTypes>>>...> m_componentCache;
private:
	template <typename HeadType>
	void LoadFile(unsigned long mask) {
		string fileName = m_directory + '\\' + std::to_string(mask);
		m_files.insert(std::make_pair(mask,MappedFile<unsigned long>(fileName + ".index", fileName + ".dat")));
	}
	template<typename HeadType, typename Next, typename ... MaskTypes>
	void LoadFile(unsigned long mask = 1) {
		LoadFile<HeadType>(mask);
		LoadFile<Next, MaskTypes...>(mask * 2);
	}

	template <typename HeadType>
	void Save() {
		auto & map = std::get<std::map<unsigned long, shared_ptr<ComponentCache<HeadType>>>> (m_componentCache);
		for (auto & cache : map) {
			cache.second->Save();
		}
	}
	template <typename HeadType, typename Next, typename ... MaskTypes>
	void Save() {
		Save<HeadType>();
		Save<Next, MaskTypes...>();
	}

	template <typename HeadType>
	void EmptyCache() {
		std::get<std::map<unsigned long, shared_ptr<ComponentCache<HeadType>>>>(m_componentCache).clear();
	}
	template <typename HeadType, typename Next, typename ... MaskTypes>
	void EmptyCache() {
		EmptyCache<HeadType>();
		EmptyCache<Next, MaskTypes...>();
	}

};

