
/******************************************************
 *
 * zexy - implementation file
 *
 * copyleft (c) IOhannes m zm�lnig
 *
 *   1999:forum::f�r::uml�ute:2004
 *
 *   institute of electronic music and acoustics (iem)
 *
 ******************************************************
 *
 * license: GNU General Public License v.2
 *
 ******************************************************/


/* hack : 2108:forum::f�r::uml�ute:1999 @ iem */

#include "zexy.h"


/* =================== tabdump ====================== */

static t_class *tabdump_class;

typedef struct _tabdump
{
  t_object x_obj;
  t_symbol *x_arrayname;
  t_int startindex, stopindex;
} t_tabdump;

static void tabdump_bang(t_tabdump *x)
{
  t_garray *A;
  int npoints;
  zarray_t *vec;

  if (!(A = (t_garray *)pd_findbyclass(x->x_arrayname, garray_class)))
    error("%s: no such array", x->x_arrayname->s_name);
  else if (!zarray_getarray(A, &npoints, &vec))
    error("%s: bad template for tabdump", x->x_arrayname->s_name);
  else
    {
      int n;
      t_atom *atombuf;

      int start=x->startindex;
      int stop =x->stopindex;
      if(start<0||start>stop)start=0;
      if(stop<start||stop>npoints)stop=npoints;
      npoints=stop-start;

      atombuf = (t_atom *)getbytes(sizeof(t_atom)*npoints);
      for (n = 0; n < npoints; n++) SETFLOAT(&atombuf[n], zarray_getfloat(vec, start+n));
      outlet_list(x->x_obj.ob_outlet, &s_list, npoints, atombuf);
      freebytes(atombuf,sizeof(t_atom)*npoints);
    }
}

static void tabdump_list(t_tabdump *x, t_symbol*s,int argc, t_atom*argv)
{
  int a,b;
  ZEXY_USEVAR(s);
  switch(argc){
  case 2:
    a=atom_getint(argv);
    b=atom_getint(argv+1);
    x->startindex=(a<b)?a:b;
    x->stopindex =(a>b)?a:b;
    tabdump_bang(x);
    break;
  default:
    error("tabdump: list must be 2 floats (is %d atoms)", argc);
  }
}

static void tabdump_set(t_tabdump *x, t_symbol *s)
{
  x->x_arrayname = s;
}

static void *tabdump_new(t_symbol *s)
{
  t_tabdump *x = (t_tabdump *)pd_new(tabdump_class);
  x->x_arrayname = s;
  x->startindex=0;
  x->stopindex=-1;
  outlet_new(&x->x_obj, &s_list);

  return (x);
}

static void tabdump_helper(void)
{
  post("\n%c tabdump - object : dumps a table as a package of floats", HEARTSYMBOL);
  post("'set <table>'\t: read out another table\n"
       "'bang'\t\t: dump the table\n"
       "outlet\t\t: table-data as package of floats");
  post("creation\t: \"tabdump <table>\"");

}

void tabdump_setup(void)
{
  tabdump_class = class_new(gensym("tabdump"), (t_newmethod)tabdump_new,
			     0, sizeof(t_tabdump), 0, A_DEFSYM, 0);
  class_addbang(tabdump_class, (t_method)tabdump_bang);
  class_addlist(tabdump_class, (t_method)tabdump_list);

  class_addmethod(tabdump_class, (t_method)tabdump_set, gensym("set"),
		  A_SYMBOL, 0);

  class_addmethod(tabdump_class, (t_method)tabdump_helper, gensym("help"), 0);
  zexy_register("tabdump");
}
