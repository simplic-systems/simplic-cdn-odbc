#include "stdafx.h"
#include "Statement.h"


Statement::Statement(DbConnection* conn)
{

	m_connection = conn;
}


Statement::~Statement()
{
	m_connection->removeStatement(this);
}
