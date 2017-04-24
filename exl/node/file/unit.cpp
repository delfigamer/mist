#include <exl/node/file/unit.hpp>
#include <exl/node/block.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>

namespace exl
{
	UnitFile::UnitFile( utils::SExpr const& s )
		: Node( s )
	{
		ASSERT( s.head == NodeHead::file_unit );
		ASSERT( s.list.size() == 3 );
		// ASSERT( s[ 1 ].head == 0 );
		m_body = Ref< Block >::create( s[ 3 ] );
	}

	UnitFile::~UnitFile()
	{
	}

	StringBuilder UnitFile::getdefstring( size_t depth )
	{
		StringBuilder defstring;
		defstring
			<< "unit"_db
			<< m_body->getdefstring( depth + 1 )
			<< "\n"_db << lineprefix( depth ) << "end"_db;
		return defstring;
	}

	void UnitFile::build( IContext* context )
	{
		m_body->build( context );
	}

	ILModule& UnitFile::compile()
	{
		if( m_ilfile.body.blocks.empty() )
		{
			m_ilfile.type = ModuleType::unit;
			initilbody( &m_ilfile.body );
			m_ilfile.body.depth = 1;
			m_body->compilereserve( &m_ilfile.body );
			m_body->compileemit( &m_ilfile.body );
			m_ilfile.body.currentblock->branches.push_back(
				ILBranch{ nullptr, m_ilfile.body.exitblock } );
		}
		return m_ilfile;
	}
}
