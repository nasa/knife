
#include "ruby.h"
#include "primal.h"

#define GET_PRIMAL_FROM_SELF Primal primal; Data_Get_Struct( self, PrimalStruct, primal );

static void this_free( void *adj )
{
  primal_free( adj );
}

static VALUE new( VALUE class, VALUE nnode, VALUE nface, VALUE ncell )
{
  Primal primal;
  VALUE obj;
  primal = primal_create( NUM2INT(nnode), NUM2INT(nface), NUM2INT(ncell) );
  obj = Data_Wrap_Struct( class, 0, this_free, primal );
  return obj;
}

static VALUE nnode( VALUE self )
{
  GET_PRIMAL_FROM_SELF;
  return INT2NUM( primal_nnode(adj) );
}

static VALUE nface( VALUE self )
{
  GET_PRIMAL_FROM_SELF;
  return INT2NUM( primal_nface(adj) );
}

static VALUE ncell( VALUE self )
{
  GET_PRIMAL_FROM_SELF;
  return INT2NUM( primal_nface(adj) );
}

VALUE cPrimal;

void Init_Adj() 
{
  cPrimal = rb_define_class( "Primal", rb_cObject );
  rb_define_singleton_method( cPrimal, "new", new, 3 );
  rb_define_method( cPrimal, "nnode", nnode, 0 );
  rb_define_method( cPrimal, "nface", nface, 0 );
  rb_define_method( cPrimal, "ncell", ncell, 0 );
}
