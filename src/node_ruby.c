
#include "ruby.h"
#include "node.h"

#define GET_NODE_FROM_SELF Node node; Data_Get_Struct( self, NodeStruct, node );

static void this_free( void *node )
{
  node_free( node );
}

static VALUE new( VALUE class, VALUE x, VALUE y, VALUE z )
{
  Node node;
  VALUE obj;
  double xyz[3];
  xyz[0] = NUM2DBL(x);
  xyz[1] = NUM2DBL(y);
  xyz[2] = NUM2DBL(z);
  node = node_create( xyz );
  obj = Data_Wrap_Struct( class, 0, this_free, node );
  return obj;
}

static VALUE x( VALUE self )
{
  GET_NODE_FROM_SELF;
  return rb_float_new( node_x(node) );
}

static VALUE y( VALUE self )
{
  GET_NODE_FROM_SELF;
  return rb_float_new( node_y(node) );
}

static VALUE z( VALUE self )
{
  GET_NODE_FROM_SELF;
  return rb_float_new( node_z(node) );
}

VALUE cNode;

void Init_Node() 
{
  cNode = rb_define_class( "Node", rb_cObject );
  rb_define_singleton_method( cNode, "new", new, 3 );
  rb_define_method( cNode, "x", x, 0 );
  rb_define_method( cNode, "y", y, 0 );
  rb_define_method( cNode, "z", z, 0 );
}
