
#include "ruby.h"
#include "near.h"

#define GET_NEAR_FROM_SELF Near near; Data_Get_Struct( self, NearStruct, near );

static VALUE new( VALUE class, 
		  VALUE index, VALUE x, VALUE y, VALUE z, VALUE radius )
{
  Near near;
  VALUE obj;
  near = near_create( NUM2INT(index), 
		      NUM2DBL(x), NUM2DBL(y), NUM2DBL(z), NUM2DBL(radius) );
  obj = Data_Wrap_Struct( class, 0, near_free, near );
  return obj;
}

static VALUE this_index( VALUE self )
{
  GET_NEAR_FROM_SELF;
  return INT2NUM(near_index(near));
}

static VALUE clearance( VALUE rb_self, VALUE rb_other )
{
  Near self, other; 
  Data_Get_Struct( rb_self,  NearStruct, self );
  Data_Get_Struct( rb_other, NearStruct, other );
  return(rb_float_new(near_clearance(self, other)));
}

static VALUE left_index( VALUE self )
{
  GET_NEAR_FROM_SELF;
  return INT2NUM(near_left_index(near));
}

static VALUE right_index( VALUE self )
{
  GET_NEAR_FROM_SELF;
  return INT2NUM(near_right_index(near));
}

static VALUE insert( VALUE rb_self, VALUE rb_child )
{
  Near self, child; 
  Data_Get_Struct( rb_self,  NearStruct, self );
  Data_Get_Struct( rb_child, NearStruct, child );
  return (self==near_insert(self,child)?rb_self:Qnil);
}

static VALUE left_radius( VALUE self )
{
  GET_NEAR_FROM_SELF;
  return rb_float_new(near_left_radius(near));
}

static VALUE right_radius( VALUE self )
{
  GET_NEAR_FROM_SELF;
  return rb_float_new(near_right_radius(near));
}

static VALUE visualize( VALUE self )
{
  GET_NEAR_FROM_SELF;
  return (KNIFE_SUCCESS==near_visualize(near)?self:Qnil);
}

static VALUE collisions( VALUE self, VALUE rb_target )
{
  Near tree, target;
  Data_Get_Struct( self,      NearStruct, tree );
  Data_Get_Struct( rb_target, NearStruct, target );

  return INT2NUM(near_collisions(tree,target));
}

static VALUE touched( VALUE self, VALUE rb_target )
{
  Near tree, target;
  int i, collisions, found, maxfound, *list;
  VALUE array;
  Data_Get_Struct( self,      NearStruct, tree );
  Data_Get_Struct( rb_target, NearStruct, target );

  collisions = near_collisions(tree,target);
  list = malloc(MAX(1,collisions)*sizeof(int));

  maxfound = collisions;
  found = 0;
  if (KNIFE_SUCCESS != near_touched(tree,target,&found,maxfound,list)) {
    free(list);
    return Qnil;
  }

  array = rb_ary_new2(collisions);
  for(i=0;i<collisions;i++) rb_ary_store(array, i, INT2NUM(list[i]));

  free(list);
  return array;
}

VALUE cNear;

void Init_Near() 
{
  cNear = rb_define_class( "Near", rb_cObject );
  rb_define_singleton_method( cNear, "new", new, 5 );
  rb_define_method( cNear, "index", this_index, 0 );
  rb_define_method( cNear, "clearance", clearance, 1 );
  rb_define_method( cNear, "left_index", left_index, 0 );
  rb_define_method( cNear, "right_index", right_index, 0 );
  rb_define_method( cNear, "insert", insert, 1 );
  rb_define_method( cNear, "left_radius", left_radius, 0 );
  rb_define_method( cNear, "right_radius", right_radius, 0 );
  rb_define_method( cNear, "visualize", visualize, 0 );
  rb_define_method( cNear, "collisions", collisions, 1 );
  rb_define_method( cNear, "touched", touched, 1 );
}
