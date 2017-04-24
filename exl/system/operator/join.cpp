#include <exl/system/operator/join.hpp>
#include <exl/system/operator/binary.hpp>
#include <exl/value/string.hpp>
#include <exl/system/typeinfo/string.hpp>
#include <cstring>

namespace exl
{
	namespace
	{
		class InvJoin: public InvBinary
		{
		public:
			InvJoin( ITypeCast* left, ITypeCast* right );
			~InvJoin();

			virtual StringBuilder getdefstring( size_t depth ) override;
			virtual void constructconstvalue() override;
		};

		class OpJoinTI: public OpBinaryTI
		{
		protected:
			virtual Ref< InvBinary > createinvocation(
				ITypeCast* left, ITypeCast* right ) override;

		public:
			OpJoinTI();
			~OpJoinTI();

			virtual StringBuilder getdefstring( size_t depth ) override;
		};

		InvJoin::InvJoin( ITypeCast* left, ITypeCast* right )
			: InvBinary( left, right )
		{
			m_fulltype.type = getstringti();
			m_operatortype = TokenType::join;
		}

		InvJoin::~InvJoin()
		{
		}

		StringBuilder InvJoin::getdefstring( size_t depth )
		{
			StringBuilder defstring;
			defstring
				<< ".system.join["_db << m_left->getdefstring( depth )
				<< ", "_db << m_right->getdefstring( depth ) << "]"_db;
			return defstring;
		}

		void InvJoin::constructconstvalue()
		{
			Ref< IConstValue > leftconst = m_left->getconstvalue();
			if( !leftconst )
			{
				return;
			}
			Ref< IStringValue > leftstring = leftconst->getstringvalue();
			if( !leftstring )
			{
				return;
			}
			Ref< IConstValue > rightconst = m_right->getconstvalue();
			if( !rightconst )
			{
				return;
			}
			Ref< IStringValue > rightstring = rightconst->getstringvalue();
			if( !rightstring )
			{
				return;
			}
			Ref< DataBuffer > leftc = leftstring->getstringcontent();
			Ref< DataBuffer > rightc = rightstring->getstringcontent();
			if( leftc )
			{
				if( rightc )
				{
					Ref< DataBuffer > db = DataBuffer::create(
						leftc->m_length + rightc->m_length );
					memcpy( db->m_data, leftc->m_data, leftc->m_length );
					memcpy( db->m_data + leftc->m_length,
						rightc->m_data, rightc->m_length );
					m_constvalue = Ref< StringValue >::create( db );
				}
				else
				{
					m_constvalue = leftconst;
				}
			}
			else
			{
				m_constvalue = rightconst;
			}
		}

		Ref< InvBinary > OpJoinTI::createinvocation(
			ITypeCast* left, ITypeCast* right )
		{
			return Ref< InvJoin >::create( left, right );
		}

		OpJoinTI::OpJoinTI()
		{
			m_argtypeinfo = getstringti();
		}

		OpJoinTI::~OpJoinTI()
		{
		}

		StringBuilder OpJoinTI::getdefstring( size_t depth )
		{
			return "type .system.join"_db;
		}
	}

	Ref< ITypeInfo > const& getopjointi()
	{
		static Ref< ITypeInfo > opjointi( new OpJoinTI, 0 );
		return opjointi;
	}
}
