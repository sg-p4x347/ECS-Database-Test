#pragma once
#include "std_lib_facilities.h"
#include "Position.h"
#include "Region.h"
#include "WorldEntityCache.h"
template<typename ... CompTypes>
class WorldEntityManager
{
	typedef Region<CompTypes...> RegionType;
public:
	WorldEntityManager(string directory, unsigned int worldWidth,unsigned int regionWidth) : 
		m_nextID(1), 
		m_worldWidth(worldWidth), 
		m_regionWidth(regionWidth),
		m_regionDimension(worldWidth / regionWidth)
	{
		SetMask<CompTypes...>();

		m_regions = vector<vector<shared_ptr<Region<CompTypes...>>>>(m_regionDimension, vector<shared_ptr<Region<CompTypes...>>>(m_regionDimension));

		for (int x = 0; x < m_regionDimension; x++) {
			for (int z = 0; z < m_regionDimension; z++) {
				m_regions[x][z] = make_shared<Region<CompTypes...>>(directory + '\\' + std::to_string(x) + ',' + std::to_string(z),m_maskIndex);
				//m_loadedRegions.insert(m_regions[x][z]);
			}
		}
	}
	~WorldEntityManager() {

	}

	template <typename HeadType>
	unsigned long GetMask() {
		return std::get<pair<unsigned long, HeadType>>(m_maskIndex).first;
	}

	template<typename HeadType, typename Next, typename ... MaskTypes>
	unsigned long GetMask() {
		return GetMask<HeadType>() | GetMask<Next,MaskTypes...>();
	}

	template<typename ... SigTypes>
	void CreateEntity(Position position,SigTypes... components) {
		unsigned long signature = GetMask<Position,SigTypes...>();
		unsigned int id = NextID();
		InsertComponent(RegionContaining(position.Pos),id,signature, position,components...);
	}
	/*template<typename CompType>
	vector<shared_ptr<ComponentCache<CompType>>> LoadComponents(unsigned long signature) {
		vector<shared_ptr<ComponentCache<CompType>>> caches;
		for (auto & region : m_loadedRegions) {
			for (auto & cache : region->LoadComponents<CompType>(signature)) {
				caches.push_back(cache);
			}
		}
		return caches;
	}*/
	template<typename HeadType, typename ... MaskTypes> 
	WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> NewEntityCache() {
		return WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...>(GetMask<HeadType,MaskTypes...>());
	}

	template<typename HeadType, typename ... MaskTypes>
	void UpdateCache(WorldEntityCache<Region<CompTypes...>, HeadType, MaskTypes...> & cache) {
		// iterate over the cached regions
		unordered_set<shared_ptr<Region<CompTypes...>>> staleRegions;
		for (auto & regionCache : cache.GetCaches()) {
			if (!m_loadedRegions.count(regionCache.first))
				staleRegions.insert(regionCache.first);
		}
		// remove stale region caches
		for (auto & staleRegion : staleRegions) {
			cache.GetCaches().erase(staleRegion);
		}
		// add new region caches to cache
		for (auto & loadedRegion : m_loadedRegions) {
			if (!cache.GetCaches().count(loadedRegion)) {
				EntityCache<HeadType, MaskTypes...> entityCache;
				loadedRegion->LoadEntities<HeadType, MaskTypes...>(entityCache, cache.GetComponentMask());
				cache.GetCaches().insert(make_pair(loadedRegion, entityCache));
				//cache.GetCaches().insert(make_pair(loadedRegion, LoadEntities<HeadType, MaskTypes...>(cache.GetComponentMask())));
			}
		}
	}

	RegionType & GetRegion(int x, int z) {
		return *(m_regions[x][z]);
	}

	void Save() {
		for (auto & region : m_loadedRegions) {
			region->Save();
		}
	}
	void ReCenter(float x, float z, float range) {
		int minX = ClampRegion((int)floor((x - range) / m_regionWidth));
		int maxX = ClampRegion((int)ceil((x + range) / m_regionWidth));
		int minZ = ClampRegion((int)floor((z - range) / m_regionWidth));
		int maxZ = ClampRegion((int)ceil((z + range) / m_regionWidth));
		
		auto oldSet = m_loadedRegions;
		m_loadedRegions.clear();
		for (x = minX; x < maxX; x++) {
			for (z = minZ; z < maxZ; z++) {
				m_loadedRegions.insert(m_regions[x][z]);
			}
		}
		// Empty caches for regions that are no longer loaded
		for (auto & old : oldSet) {
			if (!m_loadedRegions.count(old)) {
				old->EmptyCache();
			}
		}
	}
private:
	vector<vector<shared_ptr<Region<CompTypes...>>>> m_regions;
	unordered_set<shared_ptr<Region<CompTypes...>>> m_loadedRegions;
	tuple<pair<unsigned long, CompTypes>...> m_maskIndex;
	unsigned int m_nextID;
	const unsigned int m_worldWidth;
	const unsigned int m_regionWidth;
	const unsigned int m_regionDimension;
private:
	//----------------------------------------------------------------
	// ID managment
	unsigned int NextID() {
		return m_nextID++;
	}
	//----------------------------------------------------------------
	// Misc
	
	int ClampRegion(int ordinal) {
		return std::min(m_worldWidth / m_regionWidth, (unsigned int)std::max(0, ordinal));
	}
	//----------------------------------------------------------------
	// Region queries
	RegionType & RegionContaining(Vector3 & position) {
		int x = std::max(0, std::min((int)m_regionDimension - 1, int(position.x / m_regionWidth)));
		int z = std::max(0, std::min((int)m_regionDimension - 1, int(position.z / m_regionWidth)));
		return *(m_regions[x][z]);
	}
	//----------------------------------------------------------------
	// Loading  entities
	template<typename HeadType, typename ... MaskTypes>
	EntityCache<HeadType, MaskTypes...> LoadEntities(unsigned long componentMask) {
		EntityCache<HeadType, MaskTypes...> entityCache;
		for (auto & region : m_loadedRegions) {
			region->LoadEntities<HeadType, MaskTypes...>(entityCache, componentMask);
		}
		return entityCache;
	}
	//----------------------------------------------------------------
	// Set mask
	template <typename HeadType>
	void SetMask(unsigned long mask) {
		std::get<pair<unsigned long, HeadType>>(m_maskIndex).first = mask;
	}
	template<typename HeadType,typename Next, typename ... MaskTypes>
	void SetMask(unsigned long mask = 1) {
		SetMask<HeadType>(mask);
		SetMask<Next,MaskTypes...>(mask * 2);
	}
	//----------------------------------------------------------------
	// Component insertion
	template<typename CompType>
	void InsertComponent(RegionType & region, unsigned int & id, unsigned long & signature, CompType component) {
		component.ID = id;
		region.InsertComponent(component, signature);
	}
	template<typename First, typename ... MaskTypes>
	void InsertComponent(RegionType & region, unsigned int & id, unsigned long & signature, First first, MaskTypes ... rest) {
		InsertComponent(region,id,signature, first);
		InsertComponent(region,id,signature, rest...);
	}

};

