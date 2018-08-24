#include <iostream>
#include "WorldEntityManager.h"
#include "WorldEntityCache.h"
#include "Position.h"
#include "ComponentA.h"
#include "ComponentB.h"

typedef WorldEntityManager<Position, ComponentA, ComponentB> WEM;

template < typename CacheType>
void Print(CacheType & cache) {
	for (auto & entity : cache) {
		cout << "ID: " << entity.GetID() << endl;
		cout << "Signature: " << entity.GetSignature() << endl;
		Vector3 & pos = entity.Get<Position>().Pos;
		cout << "Position: x(" << pos.x << ") y(" << pos.y << ") z(" << pos.z << ")" << endl;
		//cout << "A data: " << entity.Get<ComponentA>().data << endl;
		//cout << "B data: " << entity.Get<ComponentB>().data << endl;
	}
}

int main() {
	WEM wem("C:\\Gage Omega\\Programming\\ConsoleApp\\Database",1024,256);
	/*{
		Position pos = Position(Vector3(42.f, 0.f, 42.f));
		ComponentA a = ComponentA();
		a.data = "I am the first";
		ComponentB b = ComponentB();
		b.data = 4.2;
		wem.CreateEntity(pos, a, b);
	}
	{
		Position pos = Position(Vector3(540.f, 0.f, 540.f));
		ComponentA a = ComponentA();
		a.data = "I am the second";
		ComponentB b = ComponentB();
		b.data = 4.2;
		wem.CreateEntity(pos,b);
	}*/
	wem.Save();
	auto wec = wem.NewEntityCache<Position>();

	wem.ReCenter(64, 64, 32);
	wem.UpdateCache(wec);
	Print(wec);
	cout << "moved" << endl;
	wem.ReCenter(512, 512, 32);
	wem.UpdateCache(wec);
	Print(wec);
	cout << "moved" << endl;
	wem.ReCenter(64, 64, 32);
	wem.UpdateCache(wec);
	Print(wec);

	keep_window_open();
	return 0;
} 