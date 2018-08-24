#include "Position.h"



Position::Position()
{
}

Position::Position(Vector3 pos) : Pos(pos)
{

}


Position::~Position()
{
}

string Position::Name()
{
	return "Position";
}
