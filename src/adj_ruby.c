
#include "ruby.h"
#include "adj.h"

#define GET_ADJ_FROM_SELF Adj *adj; Data_Get_Struct( self, Adj, adj );

static void this_free( void *adj )
{
  adj_free( adj );
}

static VALUE new( VALUE class, VALUE nnode, VALUE nadj, VALUE chunk_size )
{
  Adj *adj;
  VALUE obj;
  adj = adj_create( NUM2INT(nnode), NUM2INT(nadj), NUM2INT(chunk_size) );
  obj = Data_Wrap_Struct( class, 0, this_free, adj );
  return obj;
}

static VALUE nnode( VALUE self )
{
  GET_ADJ_FROM_SELF;
  return INT2NUM( adj_nnode(adj) );
}

static VALUE nadj( VALUE self )
{
  GET_ADJ_FROM_SELF;
  return INT2NUM( adj_nadj(adj) );
}

static VALUE chunk_size( VALUE self )
{
  GET_ADJ_FROM_SELF;
  return INT2NUM( adj_chunk_size(adj) );
}

static VALUE resize( VALUE self, VALUE nnode )
{
  GET_ADJ_FROM_SELF;
  return (adj_resize( adj, NUM2INT(nnode) )==NULL?Qnil:self);
}

static VALUE add( VALUE self, VALUE node, VALUE item )
{
  GET_ADJ_FROM_SELF;
  return (adj_add( adj, NUM2INT(node), NUM2INT(item) )==NULL?Qnil:self);
}

static VALUE this_remove( VALUE self, VALUE node, VALUE item )
{
  GET_ADJ_FROM_SELF;
  return ( adj_remove( adj, NUM2INT(node), NUM2INT(item) )==NULL?Qnil:self );
}

static VALUE exists( VALUE self, VALUE node, VALUE item )
{
  GET_ADJ_FROM_SELF;
  return( adj_exists( adj, NUM2INT(node), NUM2INT(item) )?Qtrue:Qfalse );
}

static VALUE degree( VALUE self, VALUE node )
{
  GET_ADJ_FROM_SELF;
  return INT2NUM( adj_degree(adj, NUM2INT(node) ) );
}

VALUE cAdj;

void Init_Adj() 
{
  cAdj = rb_define_class( "Adj", rb_cObject );
  rb_define_singleton_method( cAdj, "new", new, 3 );
  rb_define_method( cAdj, "nnode", nnode, 0 );
  rb_define_method( cAdj, "nadj", nadj, 0 );
  rb_define_method( cAdj, "chunk_size", chunk_size, 0 );
  rb_define_method( cAdj, "resize", resize, 1 );
  rb_define_method( cAdj, "add", add, 2 );
  rb_define_method( cAdj, "remove", this_remove, 2 );
  rb_define_method( cAdj, "exists", exists, 2 );
  rb_define_method( cAdj, "degree", degree, 1 );
}
