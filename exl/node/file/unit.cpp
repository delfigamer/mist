#include <exl/node/file/unit.hpp>
#include <exl/node/block.hpp>
#include <exl/parser/ast.hpp>
#include <exl/func.hpp>
#include <exl/construct.hpp>
#include <exl/format.hpp>
#include <utils/console.hpp>

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

	void UnitFile::compile()
	{
		m_ilfile.type = ModuleType::unit;
		m_ilfile.body.reset( nullptr );
		ILStringConst* fnconst = m_ilfile.body.appendconstant< ILStringConst >();
		fnconst->type = ConstType::string;
		fnconst->string = m_textrange.filename;
		m_body->compile( &m_ilfile.body );
		ILBranch* branch = m_ilfile.body.currentblock->appendbranch();
		branch->condition.setboolean( true );
		branch->target = m_ilfile.body.exitblock;
	}

	ILModule& UnitFile::getmodule()
	{
		return m_ilfile;
	}
}
