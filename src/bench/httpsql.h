#pragma once
#include "core.h"

void create_table(const Core &core, const Sql &sql);
 
void create_tables(const Core &core);

void prepare(const Core &core);

void truncate(const Core &core);
