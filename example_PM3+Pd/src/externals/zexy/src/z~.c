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

/*
  here we do some sample-wise delay, so you can do your own FIR-filter-designs
  here are :: "z^(-1)", "z^(-N)"
  to do :: a "lattice~" section ...

  1302:forum::f�r::uml�ute:2000
*/

#include "zexy.h"

/* ----------------------------------------------------- */

static t_class *zNdelay_class;

typedef struct _zNdelay
{
  t_object x_obj;

  t_sample *buf;
  int bufsize, phase;

} t_zNdelay;

static void zdel_float(t_zNdelay *x, t_floatarg f)
{
  int i = f+1;
  if (i<1)i=1;
  if (i==x->bufsize)return;
  freebytes(x->buf, x->bufsize*sizeof(t_sample));
  x->bufsize=i;
  x->buf=(t_sample *)getbytes(x->bufsize*sizeof(t_sample));
  x->phase=0;
}

static t_int *zN_perform(t_int *w)
{
  t_sample *in = (t_sample *)(w[1]);
  t_sample *out = (t_sample *)(w[2]);
  t_zNdelay *x = (t_zNdelay *)(w[3]);
  int n = (int)(w[4]);

  t_sample *buf = x->buf;
  int bufsize=x->bufsize, ph=x->phase;

  if (bufsize==1) {
    if (in!=out)while(n--)*out++=*in++;
  } else if (bufsize==2) {
    register t_sample f, last=*buf;
    while(n--){
      f=*in++;
      *out++=last;
      last=f;
    }
    *buf=last;
  } else {
    while (n--) {
      *(buf+ph++) = *in++;
      *out++    = buf[ph%=bufsize];
    }
    x->phase=ph;
  }
  return (w+5);
}


static void zNdelay_dsp(t_zNdelay *x, t_signal **sp)
{
  dsp_add(zN_perform, 4, sp[0]->s_vec, sp[1]->s_vec, x, sp[0]->s_n);
}

static void *zNdelay_new(t_floatarg f)
{
  t_zNdelay *x = (t_zNdelay *)pd_new(zNdelay_class);
  int i = f;
  t_sample *b;

  if (i<=0) i=1;
  i++;

  x->bufsize = i;
  x->buf = (t_sample *)getbytes(sizeof(t_sample) * x->bufsize);
  b=x->buf;
  while (i--) {
    *b++=0;
  }
  x->phase = 0;

  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ft1"));
  outlet_new(&x->x_obj, gensym("signal")); 
  return (x);
}

static void zNdelay_free(t_zNdelay *x)
{
  freebytes(x->buf, sizeof(t_sample) * x->bufsize);
}


static void zdel_helper(void)
{
  post("\n%c z~\t:: samplewise delay", HEARTSYMBOL);
  post("creation :: 'z~ [<n>]' : creates a <n>-sample delay; default is 1");
}


void z_tilde_setup(void)
{
  zNdelay_class = class_new(gensym("z~"), (t_newmethod)zNdelay_new, (t_method)zNdelay_free,
			    sizeof(t_zNdelay), 0, A_DEFFLOAT, 0);
  class_addmethod(zNdelay_class, nullfn, gensym("signal"), 0);
  class_addmethod(zNdelay_class, (t_method)zNdelay_dsp, gensym("dsp"), 0);

  class_addfloat(zNdelay_class, zdel_float);
  class_addmethod(zNdelay_class, (t_method)zdel_float, gensym("ft1"), A_FLOAT, 0);
  class_addmethod(zNdelay_class, (t_method)zdel_helper, gensym("help"), 0);
  zexy_register("z~");
}
