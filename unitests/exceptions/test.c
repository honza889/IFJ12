#include "exceptions.h"
#include "../test.h"

BEGIN_TEST
	exceptions_init();
	
	try
	{
		try
		{
			throw( IncompatibleComparison, true );
			TEST( false )
		}
		catch
		{
			on( IncompatibleComparison, e )
			{
				TEST( *e == true )
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

	try
	{
		try
		{
			throw( UndefinedVariable, true );
			TEST( false )
		}
		catch
		{
			on( IncompatibleComparison, e )
			{
				TEST( false )
			}
			onAll
			{
				TEST( true )
				rethrow();
			}
		}
	}
	catch
	{
		on( UndefinedVariable, e )
		{
			TEST( *e == true )
		}
		onAll
		{
			TEST( false )
		}
	}

END_TEST
