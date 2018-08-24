#pragma once
#include "std_lib_facilities.h"
#include "MappedFile.h"
template<typename CompType>
class ComponentCache
{
public:
	ComponentCache(unsigned long signature,MappedFile<unsigned long> & file) : m_signature(signature), m_file(file) {

	}
	~ComponentCache() {
		// Save
		Save();
		cout << to_string(m_signature) << " saved" << endl;
	}
	void Insert(CompType component) {
		m_cache.push_back(component);
	}
	vector<CompType> & Get() {
		return m_cache;
	}
	bool Get(unsigned int id, CompType & component) {
		for (auto & comp : m_cache) {
			if (comp.ID == id) {
				component = comp;
				return true;
			}
		}
		return false;
	}
	void Save() {
		char * dataPtr = (m_cache.size() > 0) ? (char*)&m_cache[0] : nullptr;
		size_t size = sizeof(CompType) * m_cache.size();
		m_file.Insert(m_signature, dataPtr, size);
	}
	void Import() {
		auto & data = m_file.Search(m_signature);
		if (data.size() > 0) {
			unsigned int count = data.size() / sizeof(CompType);
			auto compPtr = (CompType*)&data[0];
			m_cache = vector<CompType>(compPtr, compPtr + count);
		}
	}
private:
	unsigned long m_signature;
	MappedFile<unsigned long> & m_file;
	vector<CompType> m_cache;
};

