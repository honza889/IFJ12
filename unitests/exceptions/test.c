#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
	try
	{
		try
		{
			throw( NecoJinyho, 8.5 );
			TEST( false )
		}
		catch
		{
			on( NecoJinyho, e )
			{
				TEST( *e == 8.5 )
				rethrow();
			}
		}
	}
	catch
	{
		on( UndefinedVariable, e )
		{
			TEST( false )
		}
		onAll
		{
			TEST( true )
		}
	}
END_TEST
