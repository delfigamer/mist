#include <utils/path.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <common.hpp>
#include <osapi.hpp>
#include <cstdio>

	R_CLASS( name = event )
	struct Event
	{
	public:
		char const* m_name;
		int m_args[ 8 ];
	};

int main( int argc, char const* const* argv )
{
	utils::Ref< Event > ev( , 0 );
	return 0;
}
