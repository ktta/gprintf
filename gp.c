/**
  gp.c     printf library for generic output

   7jun24: Initial   

Copyright (c) 2024 by Şükrü Çınar.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <limits.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <stdint.h>
#include <wchar.h>
#include <math.h>
#include <float.h>
#include <locale.h>

#ifdef gprintf_glibc_compat

#ifndef gprintf_OPTION_denormal_glibc
#define gprintf_OPTION_denormal_glibc
#endif

#ifndef gprintf_OPTION_dollar_resets_flags_glibc
#define gprintf_OPTION_dollar_resets_flags_glibc
#endif

#ifndef gprintf_OPTION_pointer_precision_glibc
#define gprintf_OPTION_pointer_precision_glibc
#endif

#endif


/** VALUE_AT_ARRAY_x are not mentioned, they are just for documentation **/
enum {
  gprintf_VALUE_FROM_LIST= -2, gprintf_VALUE_FROM_FORMAT= -1,
  gprintf_VALUE_NOT_SPECIFIED= 0,
  gprintf_VALUE_AT_ARRAY_1= 1, gprintf_VALUE_AT_ARRAY_2= 2,
  gprintf_VALUE_AT_ARRAY_3= 3, gprintf_VALUE_AT_ARRAY_4= 4,
  gprintf_VALUE_AT_ARRAY_5= 5, gprintf_VALUE_AT_ARRAY_6= 6,
  gprintf_VALUE_AT_ARRAY_7= 7, gprintf_VALUE_AT_ARRAY_8= 8,
  gprintf_VALUE_AT_ARRAY_9= 9,
};

enum {
  gprintf_LEN_h= 'h',
  gprintf_LEN_hh= 's',
  gprintf_LEN_ll= 'b',
  gprintf_LEN_j= 'j',
  gprintf_LEN_t= 't',
  gprintf_LEN_z= 'z',
  gprintf_LEN_L= 'L',
  gprintf_LEN_l= 'l',
};
                                                                               
enum {
  gprintf_ALT_FORM =  1, 
  gprintf_ZERO_PAD=   2,
  gprintf_LEFT_ADJ =  4,
  gprintf_PAD_POS =   8,
  gprintf_MARK_POS = 16,
  gprintf_GROUPED  = 32                                                        
};

enum {
  gprintf_CONVERSION_END= 0,
  gprintf_CONVERSION_RAW= 'r',
  gprintf_CONVERSION_OVERFLOW= '!',
  gprintf_CONVERSION_INVALID= '?',
};

enum {
/**
VALS!"none rc lc rs ls hi hhi ri li lli ji zi ti hu hhu ru lu llu ju zu tu rf Lf rp hhn hn rn ln lln tn zn jn"
DOIT!gprintf_TYPENAME_@,
 **/
/*** { ***/
gprintf_TYPENAME_none,
gprintf_TYPENAME_rc,
gprintf_TYPENAME_lc,
gprintf_TYPENAME_rs,
gprintf_TYPENAME_ls,
gprintf_TYPENAME_hi,
gprintf_TYPENAME_hhi,
gprintf_TYPENAME_ri,
gprintf_TYPENAME_li,
gprintf_TYPENAME_lli,
gprintf_TYPENAME_ji,
gprintf_TYPENAME_zi,
gprintf_TYPENAME_ti,
gprintf_TYPENAME_hu,
gprintf_TYPENAME_hhu,
gprintf_TYPENAME_ru,
gprintf_TYPENAME_lu,
gprintf_TYPENAME_llu,
gprintf_TYPENAME_ju,
gprintf_TYPENAME_zu,
gprintf_TYPENAME_tu,
gprintf_TYPENAME_rf,
gprintf_TYPENAME_Lf,
gprintf_TYPENAME_rp,
gprintf_TYPENAME_hhn,
gprintf_TYPENAME_hn,
gprintf_TYPENAME_rn,
gprintf_TYPENAME_ln,
gprintf_TYPENAME_lln,
gprintf_TYPENAME_tn,
gprintf_TYPENAME_zn,
gprintf_TYPENAME_jn,
/*** } ***/
};

typedef ptrdiff_t  gprintf_TYPE_ti;
typedef ptrdiff_t  gprintf_TYPE_tu;
typedef ptrdiff_t *gprintf_TYPE_tn;

typedef size_t     gprintf_TYPE_zi;
typedef size_t     gprintf_TYPE_zu;
typedef size_t    *gprintf_TYPE_zn;

typedef intmax_t   gprintf_TYPE_ji;
typedef uintmax_t  gprintf_TYPE_ju;
typedef intmax_t*  gprintf_TYPE_jn;

typedef long long gprintf_TYPE_lli;
typedef unsigned long long gprintf_TYPE_llu;
typedef long long *gprintf_TYPE_lln;

typedef long gprintf_TYPE_li;
typedef unsigned long gprintf_TYPE_lu;
typedef long *gprintf_TYPE_ln;      

typedef short gprintf_TYPE_hi;
typedef unsigned short gprintf_TYPE_hu;
typedef signed short * gprintf_TYPE_hn;

typedef signed char gprintf_TYPE_hhi;
typedef unsigned char gprintf_TYPE_hhu;
typedef signed char *gprintf_TYPE_hhn;

typedef int gprintf_TYPE_ri;
typedef unsigned int gprintf_TYPE_ru;
typedef int *gprintf_TYPE_rn;

typedef double gprintf_TYPE_rf;
typedef long double gprintf_TYPE_Lf;

typedef int gprintf_TYPE_rc;
typedef wint_t gprintf_TYPE_lc;
typedef char * gprintf_TYPE_rs;
typedef wchar_t *gprintf_TYPE_ls;

typedef void *gprintf_TYPE_rp;

typedef union {
/**
VALS!"rc lc rs ls hi hhi ri li lli ji zi ti hu hhu ru lu llu ju zu tu rf Lf rp hhn hn rn ln lln tn zn jn"
DOIT!"gprintf_TYPE_@ @;"
**/
/*** { ***/
gprintf_TYPE_rc rc; gprintf_TYPE_lc lc; gprintf_TYPE_rs rs;
gprintf_TYPE_ls ls; gprintf_TYPE_hi hi; gprintf_TYPE_hhi hhi;
gprintf_TYPE_ri ri; gprintf_TYPE_li li; gprintf_TYPE_lli lli;
gprintf_TYPE_ji ji; gprintf_TYPE_zi zi; gprintf_TYPE_ti ti;
gprintf_TYPE_hu hu; gprintf_TYPE_hhu hhu; gprintf_TYPE_ru ru;
gprintf_TYPE_lu lu; gprintf_TYPE_llu llu; gprintf_TYPE_ju ju;
gprintf_TYPE_zu zu; gprintf_TYPE_tu tu; gprintf_TYPE_rf rf;
gprintf_TYPE_Lf Lf; gprintf_TYPE_rp rp; gprintf_TYPE_hhn hhn;
gprintf_TYPE_hn hn; gprintf_TYPE_rn rn; gprintf_TYPE_ln ln;
gprintf_TYPE_lln lln; gprintf_TYPE_tn tn; gprintf_TYPE_zn zn;
gprintf_TYPE_jn jn;
/*** } ***/

} gprintf_value_t;

typedef struct
{
  const char* raw;
  size_t raw_length;

  int width;
  int precision; 
  char flags;
  char precision_at;
  char width_at;
  char length;
  char conversion;
  char conversion_upper;
  char data_at;
  char type;
} gprintf_directive_t;

typedef struct
{
  int left_spaces, left_zeroes, right_spaces;
  int sign_length; char *sign;
} gprintf_pad_t;

static int gprintf_getargpos(char *pos, const char **s)
{
  char x, y;                                                                    
  x= **s;                                                                       
  y= *(*s+1);                                                                   
  if (x>='1' && x<='9' && y=='$')                                               
  {                                                                             
    *s += 2;
    *pos= x - '0';
    return 1;
  }
  else 
  {                                                                         
    *pos= gprintf_VALUE_FROM_LIST;
    return 0;
  }  
}

static int gprintf_getint(gprintf_directive_t *D, int *dst, const char **s)
{
  int i;
  for(i=0;isdigit(**s);(*s)++)
  {
    if (i > INT_MAX/10U || **s-'0' > INT_MAX-10*i)
    {
      *dst= -1;
      D->conversion= gprintf_CONVERSION_OVERFLOW;
      return 1;
    }
    i = 10*i + (**s-'0'); 
  }
  *dst= i;
  return 0;
}

static int gprintf_getdirective(const char **fmt, gprintf_directive_t *D)
{
  const char *s;
  char t;
  int V;
  int lenmod, lenmodc;

  lenmod= 0;
  lenmodc= 0;

  memset(D, 0, sizeof(*D));
  s= *fmt;
  if (!*s) return 1; 
  if (*s!='%')
  {
    while(*s!=0 && *s!='%') s++;
    D->conversion= gprintf_CONVERSION_RAW;
    D->data_at= gprintf_VALUE_FROM_FORMAT;
    D->raw= *fmt;
    D->raw_length= s-*fmt;
    *fmt = s;
    return 0;
  }
  if (s[1]=='%')
  {
    D->conversion= gprintf_CONVERSION_RAW;
    D->data_at= gprintf_VALUE_FROM_FORMAT;
    D->raw= *fmt;
    D->raw_length= 1;
    *fmt= s+2;
    return 0;
  }
  s++;
  static const int flags[256]= {                                                
   ['#'] gprintf_ALT_FORM,                                                     
   ['0'] gprintf_ZERO_PAD,                                                     
   ['-'] gprintf_LEFT_ADJ,                                                     
   [' '] gprintf_PAD_POS,                                                      
   ['+'] gprintf_MARK_POS,                                                     
   ['\''] gprintf_GROUPED                                                      
  };        
  while((V=flags[*(unsigned char*)s])) { D->flags |= V; s++; }
  if (gprintf_getargpos(&D->data_at, &s))
  {
#ifdef gprintf_OPTION_dollar_resets_flags_glibc
    D->flags= 0;
#endif
    while((V=flags[*(unsigned char*)s])) { D->flags |= V; s++; }
  }

  if (isdigit(*s))
  {
    if (gprintf_getint(D, &D->width,&s)) return 1;
    D->width_at= gprintf_VALUE_FROM_FORMAT;
    goto width_done;
  }
  if (*s!='*') goto width_done;
  s++;
  gprintf_getargpos(&D->width_at,&s);
width_done:

  if (*s!='.') goto precision_done;
  s++;
  if (isdigit(*s))
  {
    if (gprintf_getint(D, &D->precision,&s)) return 1;
    D->precision_at= gprintf_VALUE_FROM_FORMAT;
    goto precision_done;
  }
  if (*s!='*') 
  {
    D->precision_at= gprintf_VALUE_FROM_FORMAT;
    D->precision= 0;
    goto precision_done;
  }
  s++;
  gprintf_getargpos(&D->precision_at,&s);

precision_done:
  switch(*s)
  {
  case 'h':
  case 'l':
  case 'j':
  case 't':
  case 'z':
  case 'L':
    if (lenmod!=0 && lenmod!=*s) goto inval;
    lenmod= *s;
    lenmodc++;
    if (lenmodc>2) goto inval;
    s++; 
    goto precision_done;
  }

  if (lenmodc==2)
  {
    switch(lenmod)
    {
    case 'h': lenmod= gprintf_LEN_hh; break;
    case 'l': lenmod= gprintf_LEN_ll; break;
    default: goto inval;
    }
  }
  D->length= lenmod;

  switch((t=*s++))
  {
  case 'i': t= 'd'; break;
  case 'X': case 'F': case 'G': case 'A': case 'E':
    t= tolower(t);
    D->conversion_upper= 1;
    break;
  }

  switch(t)
  {
  case 'c':
    switch(lenmod)
    {
    case 0: D->type= gprintf_TYPENAME_rc; break;
    case gprintf_LEN_l: D->type= gprintf_TYPENAME_lc; break;
    default: goto inval;
    }
    break;
  case 's':
    switch(lenmod)
    {
    case 0: D->type= gprintf_TYPENAME_rs; break;
    case gprintf_LEN_l: D->type= gprintf_TYPENAME_ls; break;
    default: goto inval;
    }
    break;
  case 'n':
    if (D->flags || D->width_at || D->precision_at) goto inval;
    switch(lenmod)
    {
    case gprintf_LEN_hh: D->type= gprintf_TYPENAME_hhn; break;
    case gprintf_LEN_h:  D->type= gprintf_TYPENAME_hn; break;
    case 0:              D->type= gprintf_TYPENAME_rn; break;
    case gprintf_LEN_l:  D->type= gprintf_TYPENAME_ln; break;
    case gprintf_LEN_ll: D->type= gprintf_TYPENAME_lln; break;
    case gprintf_LEN_j:  D->type= gprintf_TYPENAME_jn; break;
    case gprintf_LEN_z:  D->type= gprintf_TYPENAME_zn; break;
    case gprintf_LEN_t:  D->type= gprintf_TYPENAME_tn; break;
    default: goto inval;
    }
    break;
  case 'd':
    switch(lenmod)
    {
    case 0:              D->type= gprintf_TYPENAME_ri; break;
    case gprintf_LEN_hh: D->type= gprintf_TYPENAME_hhi; break;
    case gprintf_LEN_h:  D->type= gprintf_TYPENAME_hi; break;
    case gprintf_LEN_l:  D->type= gprintf_TYPENAME_li; break;
    case gprintf_LEN_ll: D->type= gprintf_TYPENAME_lli; break;
    case gprintf_LEN_j:  D->type= gprintf_TYPENAME_ji; break;
    case gprintf_LEN_z:  D->type= gprintf_TYPENAME_zi; break;
    case gprintf_LEN_t:  D->type= gprintf_TYPENAME_ti; break;
    default: goto inval;
    }
    break;
  case 'o': case 'x': case 'u':
    switch(lenmod)
    {
    case 0: D->type= gprintf_TYPENAME_ru; break;
    case gprintf_LEN_hh: D->type= gprintf_TYPENAME_hhu; break;
    case gprintf_LEN_h:  D->type= gprintf_TYPENAME_hu; break;
    case gprintf_LEN_l:  D->type= gprintf_TYPENAME_lu; break;
    case gprintf_LEN_ll: D->type= gprintf_TYPENAME_llu; break;
    case gprintf_LEN_j:  D->type= gprintf_TYPENAME_ju; break;
    case gprintf_LEN_z:  D->type= gprintf_TYPENAME_zu; break;
    case gprintf_LEN_t:  D->type= gprintf_TYPENAME_tu; break;
    default: goto inval;
    }
    break;
  case 'f': case 'g': case 'a': case 'e':
    switch(lenmod)
    {
    case 0:
    case gprintf_LEN_l: D->type= gprintf_TYPENAME_rf; break;
    case gprintf_LEN_L: D->type= gprintf_TYPENAME_Lf; break;
    default: goto inval;
    }
    break;
  case 'p':
    switch(lenmod)
    {
    case 0:  D->type= gprintf_TYPENAME_rp; break;
    default: goto inval;
    }
    break;
  default:
    goto inval;
  }
  D->conversion= t;
  *fmt= s;
  return 0;

inval:
  D->conversion= gprintf_CONVERSION_INVALID;
  return 1;
}

typedef int (*gprintf_func_t)(void * , const void *, size_t);

typedef struct
{
  gprintf_func_t func;
  void *arg;
} gprintf_out_t;

static int gprintf_out(gprintf_out_t *out, const void *buf, size_t sz)
{
  if (out && out->func) return (*out->func)(out->arg, buf, sz);
                   else return 0;
}


static int gprintf_is_type_pointer(int type)
{
  switch(type)
  {
case gprintf_TYPENAME_rp:
case gprintf_TYPENAME_hhn:
case gprintf_TYPENAME_hn:
case gprintf_TYPENAME_rn:
case gprintf_TYPENAME_ln:
case gprintf_TYPENAME_lln:
case gprintf_TYPENAME_tn:
case gprintf_TYPENAME_zn:
case gprintf_TYPENAME_jn:
case gprintf_TYPENAME_rs:
case gprintf_TYPENAME_ls:
   return 1;
  }
  return 0;
}

/**
  This function sets the corresponding m$ argument's type to the given
  type. if it was already mentioned before, then we have a type for that
  already. these two types should be compatible. 

  Pointer types are all compatible with each other. They all have the
  same size and therefore the same value within the union. Normally,
  printing a string like  %1$s %1$ls  should be illegal because you're
  referring to the same string using two different types but I won't
  try to handle that case. 
 **/
static int gprintf_set_argtype(int where, int type,int *types)
{
  if (where<=0) return 0;
  
  if (types[where]==gprintf_TYPENAME_none)
  {
    types[where]= type;
    return 0;
  }

  if (types[where]==type) return 0;

  if (gprintf_is_type_pointer(type) && gprintf_is_type_pointer(types[where]))
     return 0;
  else
     return 1;
}

static void gprintf_store_result(gprintf_value_t *V, int type, int result)
{
switch(type)
{
/**
VALS!"hhn hn rn ln lln tn zn jn"
DOIT!"case gprintf_TYPENAME_@: *V->@= result; break;"
**/
/*** { ***/
case gprintf_TYPENAME_hhn: *V->hhn= result; break;
case gprintf_TYPENAME_hn: *V->hn= result; break;
case gprintf_TYPENAME_rn: *V->rn= result; break;
case gprintf_TYPENAME_ln: *V->ln= result; break;
case gprintf_TYPENAME_lln: *V->lln= result; break;
case gprintf_TYPENAME_tn: *V->tn= result; break;
case gprintf_TYPENAME_zn: *V->zn= result; break;
case gprintf_TYPENAME_jn: *V->jn= result; break;
/*** } ***/
}
}

static void gprintf_get_arg(gprintf_value_t *V, int type, va_list *ap)
{
  switch(type)
  {
/**
VALS!"rc lc rs ls hi hhi ri li lli ji zi ti hu hhu ru lu llu ju zu tu rf Lf rp hhn hn rn ln lln tn zn jn"
DOIT!"case gprintf_TYPENAME_@: V->@= va_arg(*ap, gprintf_TYPE_@); break;"
**/
/*** { ***/
case gprintf_TYPENAME_rc: V->rc= va_arg(*ap, gprintf_TYPE_rc); break;
case gprintf_TYPENAME_lc: V->lc= va_arg(*ap, gprintf_TYPE_lc); break;
case gprintf_TYPENAME_rs: V->rs= va_arg(*ap, gprintf_TYPE_rs); break;
case gprintf_TYPENAME_ls: V->ls= va_arg(*ap, gprintf_TYPE_ls); break;
case gprintf_TYPENAME_hi: V->hi= va_arg(*ap, int); break;
case gprintf_TYPENAME_hhi: V->hhi= va_arg(*ap, int); break;
case gprintf_TYPENAME_ri: V->ri= va_arg(*ap, gprintf_TYPE_ri); break;
case gprintf_TYPENAME_li: V->li= va_arg(*ap, gprintf_TYPE_li); break;
case gprintf_TYPENAME_lli: V->lli= va_arg(*ap, gprintf_TYPE_lli); break;
case gprintf_TYPENAME_ji: V->ji= va_arg(*ap, gprintf_TYPE_ji); break;
case gprintf_TYPENAME_zi: V->zi= va_arg(*ap, gprintf_TYPE_zi); break;
case gprintf_TYPENAME_ti: V->ti= va_arg(*ap, gprintf_TYPE_ti); break;
case gprintf_TYPENAME_hu: V->hu= va_arg(*ap, int); break;
case gprintf_TYPENAME_hhu: V->hhu= va_arg(*ap, int); break;
case gprintf_TYPENAME_ru: V->ru= va_arg(*ap, gprintf_TYPE_ru); break;
case gprintf_TYPENAME_lu: V->lu= va_arg(*ap, gprintf_TYPE_lu); break;
case gprintf_TYPENAME_llu: V->llu= va_arg(*ap, gprintf_TYPE_llu); break;
case gprintf_TYPENAME_ju: V->ju= va_arg(*ap, gprintf_TYPE_ju); break;
case gprintf_TYPENAME_zu: V->zu= va_arg(*ap, gprintf_TYPE_zu); break;
case gprintf_TYPENAME_tu: V->tu= va_arg(*ap, gprintf_TYPE_tu); break;
case gprintf_TYPENAME_rf: V->rf= va_arg(*ap, gprintf_TYPE_rf); break;
case gprintf_TYPENAME_Lf: V->Lf= va_arg(*ap, gprintf_TYPE_Lf); break;
case gprintf_TYPENAME_rp: V->rp= va_arg(*ap, gprintf_TYPE_rp); break;
case gprintf_TYPENAME_hhn: V->hhn= va_arg(*ap, gprintf_TYPE_hhn); break;
case gprintf_TYPENAME_hn: V->hn= va_arg(*ap, gprintf_TYPE_hn); break;
case gprintf_TYPENAME_rn: V->rn= va_arg(*ap, gprintf_TYPE_rn); break;
case gprintf_TYPENAME_ln: V->ln= va_arg(*ap, gprintf_TYPE_ln); break;
case gprintf_TYPENAME_lln: V->lln= va_arg(*ap, gprintf_TYPE_lln); break;
case gprintf_TYPENAME_tn: V->tn= va_arg(*ap, gprintf_TYPE_tn); break;
case gprintf_TYPENAME_zn: V->zn= va_arg(*ap, gprintf_TYPE_zn); break;
case gprintf_TYPENAME_jn: V->jn= va_arg(*ap, gprintf_TYPE_jn); break;
/*** } ***/
  }
}

static gprintf_value_t *gprintf_get_value
  (gprintf_value_t *store, int where, int type, 
   gprintf_value_t *values, va_list *ap)
{
  if (where>0) return values + where;
  if (where!=gprintf_VALUE_FROM_LIST) return NULL;
  gprintf_get_arg(store, type, ap);
  return store;
}

static void gprintf_get_intvalue
  (int *R, int where, gprintf_value_t *values, va_list *ap)
{
  gprintf_value_t value_store, *V;
  switch(where)
  {
  case gprintf_VALUE_NOT_SPECIFIED: // nothing, where==0
  case gprintf_VALUE_FROM_FORMAT: break; // already in *R
  case gprintf_VALUE_FROM_LIST:
  default:
     V= gprintf_get_value(&value_store, where, gprintf_TYPENAME_ri, values, ap);
     *R= V->ri;
  }
}

static int gprintf_pad(gprintf_out_t *O, char *pad, int chr, int length)
{
  int o_length;
  if (length<=0) return 0;
  o_length= length;
  if (!pad[0]) memset(pad, chr, 256);
  while(length>256) { gprintf_out(O, pad, 256); length-=256; }
  if (length) gprintf_out(O, pad, length);
  return o_length;
}

static int gprintf_pad_zero(gprintf_out_t *O, int length)
{
  static char buf[256];
  return gprintf_pad(O, buf, '0', length);
}

static int gprintf_pad_space(gprintf_out_t *O, int length)
{
  static char buf[256];
  return gprintf_pad(O, buf, ' ', length);
}

static int gprintf_max(int a,int b) { return a>b?a:b; }
//static int gprintf_min(int a,int b) { return a<b?a:b; }

static char *gprintf_print_digits(char *o, uintmax_t V, int base, int upper)
{
  char *D = upper ? "0123456789ABCDEF" : "0123456789abcdef";
  while(V)
  {
   *--o= D[V % base];
   V/= base;
  }
  return o;
}

static uintmax_t gprintf_cast_unsigned(int type, gprintf_value_t *V)
{
  switch(type)
  {
/**
VALS!"hhu hu ru lu llu ju tu zu"
DOIT!"case gprintf_TYPENAME_@: return V->@; "
**/
/*** { ***/
case gprintf_TYPENAME_hhu: return V->hhu; 
case gprintf_TYPENAME_hu: return V->hu; 
case gprintf_TYPENAME_ru: return V->ru; 
case gprintf_TYPENAME_lu: return V->lu; 
case gprintf_TYPENAME_llu: return V->llu; 
case gprintf_TYPENAME_ju: return V->ju; 
case gprintf_TYPENAME_tu: return V->tu; 
case gprintf_TYPENAME_zu: return V->zu; 
/*** } ***/
  }
  return 0;
}

static intmax_t gprintf_cast_signed(int type, gprintf_value_t *V)
{
  switch(type)
  {
/**
VALS="hhi hi ri li lli ji ti zi"
DOIT="case gprintf_TYPENAME_@: return V->@; "
**/
/*** { ***/
case gprintf_TYPENAME_hhi: return V->hhi; 
case gprintf_TYPENAME_hi: return V->hi; 
case gprintf_TYPENAME_ri: return V->ri; 
case gprintf_TYPENAME_li: return V->li; 
case gprintf_TYPENAME_lli: return V->lli; 
case gprintf_TYPENAME_ji: return V->ji; 
case gprintf_TYPENAME_ti: return V->ti; 
case gprintf_TYPENAME_zi: return V->zi; 
/*** } ***/
  }
  return 0;
}

static void gprintf_fix_options(gprintf_directive_t *D)
{
  if (D->width_at && D->width<0)
  {
    D->width= -D->width;
    D->flags|= gprintf_LEFT_ADJ;
  }

  if (D->flags&gprintf_MARK_POS) D->flags &= ~gprintf_PAD_POS;
  if (D->precision_at && D->precision<0) D->precision_at= 0;

  if (D->flags&gprintf_LEFT_ADJ) D->flags &= ~gprintf_ZERO_PAD;

  switch(D->conversion)
  {
  case 'x': case 'u': case 'o': case 'd':
    // for integers, ZERO_PAD is ignored if a precision is given
    if (D->precision_at) D->flags &= ~gprintf_ZERO_PAD;
    break;
  case 'c':
    D->precision_at= 0; // 'c' always writes a whole character
  case 's':
    D->flags &= gprintf_LEFT_ADJ; // this is the only flag available
    break;
  }

  switch(D->conversion)
  {
  case 'x': case 'u': case 'o':
    D->flags &= ~(gprintf_MARK_POS|gprintf_PAD_POS);
    break;
  }
}

static void gprintf_get_pad
  (gprintf_directive_t *D, gprintf_pad_t *P, int length, int noz)
{
  P->left_spaces= 0;
  P->right_spaces= 0;
  if (noz) P->left_zeroes= 0;
  if (D->width_at && length<D->width)
  {
    if (D->flags&gprintf_LEFT_ADJ)
    {
      P->right_spaces= D->width - length;
    }
    else
    {
      if (D->flags&gprintf_ZERO_PAD) P->left_zeroes+= D->width- length;
                                else P->left_spaces= D->width- length;
    }
  }
}

static int gprintf_pad_left
   (gprintf_out_t *O, gprintf_pad_t *P, char *prefix, int prefix_len)
{
  int R;
  R= gprintf_pad_space(O, P->left_spaces);
  R+= gprintf_out(O, P->sign, P->sign_length);
  R+= gprintf_out(O, prefix, prefix_len);
  R+= gprintf_pad_zero(O,  P->left_zeroes);
  return R;
}

static int gprintf_pad_right
  (gprintf_out_t *O, gprintf_pad_t *P)
{
  return gprintf_pad_space(O, P->right_spaces);
}

static int gprintf_print_simple
  (gprintf_out_t *O, gprintf_pad_t *P, char *data, int length)
{
  int R;
  R= gprintf_pad_left(O, P, NULL, 0);
  R+= gprintf_out(O, data, length);
  return R+gprintf_pad_right(O, P);
}

static int gprintf_print_unsigned_int
  (gprintf_out_t *O, uintmax_t ju, gprintf_directive_t *D, int base)
{
  char *digits;
  char buffer[3+(sizeof(ju)*8+2)/3+1];
  int  digits_length;
  gprintf_pad_t pad;

  digits= gprintf_print_digits(buffer+sizeof(buffer), ju, base, 
                                                      D->conversion_upper);
  digits_length= buffer+sizeof(buffer)-digits;

  if (!D->precision_at) D->precision= 1;

  // the 'sign' field is used for the prefix 0x etc. as well
  pad.sign_length= 0;

  if (D->flags&gprintf_ALT_FORM) switch(base)
  {
  case 8:
    if (digits_length==0 || digits[0]!='0') 
       D->precision= gprintf_max(D->precision,digits_length+1);
    break;
  case 16:
    if (digits_length!=0) { pad.sign= D->conversion_upper ? "0X" : "0x";
                            pad.sign_length= 2; }
    break;
  }

  pad.left_zeroes= gprintf_max(D->precision-digits_length, 0);
  gprintf_get_pad(D, &pad, pad.sign_length+ pad.left_zeroes+digits_length, 0);
  return gprintf_print_simple(O, &pad, digits, digits_length);
}

static int gprintf_print_unsigned
  (gprintf_out_t *O, gprintf_value_t *V, gprintf_directive_t *D, int base)
{
  return gprintf_print_unsigned_int(O, gprintf_cast_unsigned(D->type, V),
                                       D, base);
}

static void gprintf_get_sign
  (int negative, gprintf_pad_t *pad, int flags)
{
  if (negative)
  {
    pad->sign_length= 1;
    pad->sign= "-";
  }
  else 
  {
    switch(flags&(gprintf_MARK_POS|gprintf_PAD_POS))
    {
    case gprintf_MARK_POS: pad->sign_length= 1; pad->sign= "+"; break;
    case gprintf_PAD_POS:  pad->sign_length= 1; pad->sign= " "; break;
    default: pad->sign_length= 0;
    }
  }
}

static int gprintf_print_signed
  (gprintf_out_t *O, gprintf_value_t *V, gprintf_directive_t *D)
{
  intmax_t ji;
  int negative;
  char buffer[1+(sizeof(ji)*8+2)/3+1];
  gprintf_pad_t pad;
  int   digits_length;
  char *digits;

  ji= gprintf_cast_signed(D->type, V);

  if (ji<0) { ji= -ji; negative= 1; }
       else { negative= 0; }

  if (!D->precision_at) D->precision= 1;

  digits= gprintf_print_digits(buffer+sizeof(buffer), ji, 10, 0);
  digits_length= buffer+sizeof(buffer)-digits;

  pad.left_zeroes= gprintf_max(0, D->precision-digits_length);

  gprintf_get_sign(negative, &pad, D->flags);
  gprintf_get_pad(D, &pad, digits_length +
                           pad.left_zeroes + pad.sign_length, 0);
  return gprintf_print_simple(O, &pad, digits, digits_length);
}

static int gprintf_print_string
  (gprintf_out_t *O, char *str, gprintf_directive_t *D)
{
  int length;
  gprintf_pad_t pad;
  if (D->precision_at) 
  {
    for(length=0;str[length]!=0 && length<D->precision;length++) ;
  }
  else
  {
    length= strlen(str);
  }
  pad.sign_length= 0;
  gprintf_get_pad(D, &pad, length, 1);
  return gprintf_print_simple(O, &pad, str, length);
}

static int gprintf_print_char
  (gprintf_out_t *O, gprintf_TYPE_rc chr, gprintf_directive_t *D)
{
  gprintf_pad_t pad;
  char C;
  C= chr;
  pad.sign_length= 0;
  gprintf_get_pad(D, &pad, 1, 1);
  return gprintf_print_simple(O, &pad, &C, 1);
}

static void float2decimal
  (long double F, int precision,
   int *sign, unsigned char *digits, int *out_len, int *exp10)
{
/** LDBL_MAX_EXP is very similar to LDB_MIN_EXP, therefore, the
requirement for the multiplication algorithm is already covered
by the size specified here. The +1 is here because the constant
defined in float.h is actually one more than the real value for
some reason.                                                **/ 
#define gprintf_FLOAT2DEC_SPACE_REQ (LDBL_MANT_DIG+1+3-LDBL_MIN_EXP+1) 

/**{
How does it all work?

The floating point number F is converted to a multi-precision decimal integer,
along with two exponents: e2 and e10. So, at every step of the algorithm
we have:
 
 F(orig value) = (negative? -1 : 1) * digits[d_start, d_end) * 2^e2 * 10^e10

Initially, we 'normalize' F so that we get rid of the exponent in the 
floating point and end up with a nice value in  [1,2). 

After this,

  if (e2>0) => digits *= 2^e2, adjust e10 to get rid of trailing 0s
  if (e2<0) => digits /= 2^e2, adjust e10 to get rid of leading 0s

Multiplication
--------------
we need to multiply our bcd integer by 2^e2. we don't do it all at
once. because we would need huge space to hold all the digits.

we do this step by step. we set e2= N*K+P where P<K. we initially
multiply by 2^K for N times and then by 2^P once. 

how to find K? At each step, we need to make sure that the carry
value isn't greater than 9. Otherwise the carry would get larger
and larger.

The maximum digit value is 9. If we multiply it with 8, we get a
7 as the carry. 16 doesn't work at all. so we set K=3.

Division
--------

we need to divide our integer by 2^e2. We again have a similar
reasoning as above to do it step by step. At each step, we will
have a remainder. This remainder should be small enough that
the carry+nextdigit doesn't overflow beyond 100. Also, it should
be big enough so that we don't do this bit by bit. Again, selecting
K=3 seems reasonable. We will essentially copy digits to the next
position.

=== Space Requirements ===

The Multi-Precision Integer
---------------------------

After frexpl, we have a normalized number in F. This has N=LDBL_MANT_DIG+1
binary digits. At each step of the conversion, F is multiplied by 10,
which has 2 as a factor. Therefore, we remove one trailing digit at each
step. This means that the loop will run for at most N times and the space
requirement for the digits is therefore N.

Multiplication
--------------

The multiplication algorithm runs for M=ceil(LDBL_MAX_EXP/3) steps.

At each step, the trailing digit moves one step further. Therefore, the 
additional space requirement is just M. 

Division
--------

The division algorithm runs for D=ceil(-LDBL_MIN_EXP/3) steps.
D is therefore   (2-LDBL_MIN_EXP)/3

At each step of the division algorithm, we will add at most 3 decimal digits
at the end due to the carry. Here are the possible remainders and their 
division to 2^shift. Note that the longest sequence is of length 3.

1/8=  .125 2/8=  .25   3/8=  .375   4/8=  .5
5/8=  .625 6/8=  .75   7/8=  .875   1/4=  .25
2/4=  .5   3/4=  .75   1/2=  .5

Therefore, the space requirement for this is 

    Sd= D*3 = ceil(-LDBL_MIN_EXP/3)*3 < -LDBL_MIN_EXP + 3

Also, at each step, we move d_start one digit back. Most of the time, this
space is reclaimed by eliminating the leading zeroes. However, I can't 
calculate precisely how much space this requires. Therefore, I'll take the
maximum and assume D digits of walkback.

So, 
- we need Sd+D digits of additional space
- the multi-precision integer must start at position D of the array
- the maximum reasonable precision is determined by the space requirement.
  anything beyond that is guaranteed to contain only zeroes.

Due to Improvement 1, we no longer move d_start one digit back. We simply
store the result starting from digits(0). Therefore the space requirement
for division has reduced to just Sd.

Number of Digits Generated
==========================

For multiplication, we generate as many digits as necessary to fully
represent the number. This might be wrong from a performance perspective,
but is necessary for correctness since rounding the integer too early
would propagate the error thru subsequent multiplications.

For division, we do the divisions until we clear out the carry. Note that
this is done a maximum of 3 times at each step, since divison by 2^N doesn't
result in repeating remainders. At some point, we need to cut our number
so that it doesn't grow too long, because processing the digits beyond the
number of digits required doesn't contribute anything. The remaining of this
argument discusses only the division case.

Precision is the number of digits after the decimal point. We need to
determine our current number of such fractional digits. 

If we have e10 >=0, then all digits in digits[d_start,d_end) are integral
and we don't do anything at all.

Let L be the length of our integer, L= d_end-d_start. If -e10 is less than
this, then some of the digits are integral and -e10 of them are fractional.
If -e10 is greater or equal to L, then all digits in the multiprecision 
integer are fractional (there are L of them). Therefore the number of
fractional digits is min( -e10, d_end-d_start ).

If this number is greater than the required precision, we will cut the end
of the integer.

We will generate 5 more digits than this because we need to round the result
at the end.

By limiting the number of digits in this way, we have also eliminated the
problem of huge precisions. Since we aren't looping until reaching a set
precision, we don't risk going beyond the array's limit.

Improvements
============

Improvement 1: Implemented
--------------------------

For division, I tried to eliminate walking the integer within the array 
towards right by putting the result of the division one cell to the left.
However, there are sometimes multiple leading zeroes which get eliminated
and the start of the array will have moved right significantly. I did
reserve space for this, but a much better alternative is possible.

At each step of the algorithm, we are currently doing:

    digits[d_start-1,d_end-1) = digits[d_start, d_end) / 2^N

Instead, we could do

    digits[d_start-M,d_end-M) = digits[d_start, d_end) / 2^N

where M is the number of leading zeroes from the previous step. We could
even do:

    digits[0, K) = digits[d_start, d_end) / 2^N

d_start would never go below 0 since we never decrement it. Let's try this,
I'm curious. It turns out this works quite nicely. Now, we don't need to
make space on the left part of the array at all.

Improvement 2
-------------

We are doing base-10 arithmetic. We could do base-1e6 or even base-1e9 since
these fit in an uint32_t nicely. The musl implementation uses the latter.
It would be 9 times faster but I've barely managed to do this correctly in
base10. 

} **/
  int e10, e2, d_start, d_end;
  const int digits_size= gprintf_FLOAT2DEC_SPACE_REQ;

  if (F<0) { F= -F; *sign= 1; } else { *sign= 0; }

  F= 2*frexpl(F, &e2);    // F is now in  [1.0,2.0) 
  if (F) e2--;

  /** We had this before Improvement 1 
  if (e2<0) d_start= (2 - LDBL_MIN_EXP+1)/3;
       else d_start= 0;

  d_end= d_start;
  **/
  d_start= d_end= 0;

//printf("sizeof digits= %zd\n", sizeof(digits)); printf("F =%Le e2= %d\n", F, e2); printf("Fx= %La e2= %d\n", F, e2); fflush(NULL);

  // get the decimal digits
  do {
    int V = F;
    digits[d_end]= V; 
    F= 10*(F - V); 
    d_end++;
  } while(F);

  // OK, now we got an integer in digits[dstart,dend) with length= dend-dstart
  // all the digits except for the first one were in the fraction of F.
  // therefore, we need to multiply by 10^-(length-1).
  e10= -(d_end-d_start-1);

//printf("initial\n"); PP;
  while (e2>0)
  {
    int shift= e2 < 3 ? e2: 3, carry= 0;
    for(int d=d_end-1;d>=d_start;d--)
    {
      uint32_t V= (digits[d]<<shift) + carry;
      /** we store at the next position because the carry will eventually
          reach the first digit and we would run out of space **/
      digits[d+1]=  V%10;
      carry= V / 10;
    }
    d_end++;
    if (carry) digits[d_start]= carry;
          else d_start++;
    while(d_end>d_start && digits[d_end-1]==0) { e10++; d_end--; }
    e2 -= shift;
//printf("after one round of multiplication by %d\n", 1<<shift); PP;
  }

  while(e2<0)
  {
    int carry= 0, shift= -e2 < 3 ? -e2 : 3;
    for(int d=d_start;d<d_end;d++)
    {
      int V= digits[d]+ 10*carry;
      digits[d-d_start]= V >> shift; // WAS digits[d-1]= V >> shift;
      carry= V & ((1<<shift)-1);
    }
    d_end-= d_start; d_start= 0; // WAS d_end--; d_start--;
    /* actually, d_end<digits_size comparison is redundant because we
       already reserved enough space for all inputs. */
    while (carry && d_end<digits_size)
    {
      int V= 10*carry;
      d_end++; 
      digits[d_end-1]= V >> shift;
      carry= V & ((1<<shift)-1); 
      e10--;
    }
    while(d_start<d_end && digits[d_start]==0) d_start++; 
    e2+= shift;

    if (e10<0)
    {
      int L= d_end-d_start, fd, delta;
      fd= (L < -e10) ? L : -e10;
      delta= fd-(precision+5);
      if (delta>0)
      {
        d_end -= delta;
        e10 += delta;
      }
    }
//printf("after one round of division by %d\n", 1<<shift); PP;
  }
  /** we are done now.. **/
  if (d_start) for(int i=d_start;i<d_end;i++) digits[i-d_start]= digits[i];
  *out_len= d_end-d_start;
  *exp10= e10;
}

static int  gprintf_print_nan
   (gprintf_out_t *O, long double F, gprintf_directive_t *D)
{
  char *str;
  gprintf_pad_t pad;
  int  str_length;

  switch(fpclassify(F))
  {
  case FP_NAN: str= D->conversion_upper ? "NAN" : "nan"; break;
  case FP_INFINITE: str= D->conversion_upper ? "INF" : "inf"; break;
  default: return 0;
  }
  str_length= 3;
  gprintf_get_sign(signbit(F), &pad, D->flags);
  D->flags &= ~gprintf_ZERO_PAD;
  gprintf_get_pad(D, &pad, str_length + pad.sign_length, 1);
  return gprintf_print_simple(O, &pad, str, str_length);
}


#ifdef gprintf_OPTION_denormal_glibc
static void gprintf_reconstruct_denormal
   (int islong, unsigned char *digits, int *out_len, int *exp2)
{
  int e2= *exp2, len= *out_len;
  int disp_e2;
  int min_exp= (islong ? LDBL_MIN_EXP : DBL_MIN_EXP) - 1;

  /** 
    the formatter adds fd*4 to exp2, this result should not be less
    than min_exp. 
   **/
  disp_e2= e2 + (len-1)*4;

  int delta= min_exp - disp_e2;

  if (delta<=0) return ;

  // this is a denormal number, with the exponent out of range
  // we have delta 0 bits at the beginning of our number,
  // these were lost when we called frexpl. now we need to reclaim
  // them. 
  int shift_bytes= delta/4, shift_bits=delta%4;

  for(int i=len-1;i>=0;i--) digits[shift_bytes+i]= digits[i];
  for(int i=0;i<shift_bytes;i++) digits[i]= 0; 
  len += shift_bytes;

  if (shift_bits)
  {
    int carry= 0;
    for(int i=0;i<len;i++)
    {
      int V= digits[i];
      digits[i]= carry | (V>>shift_bits);
      carry= (V & ((1<<shift_bits)-1)) << (4-shift_bits);
    }
    if (carry)
    {
       digits[len]= carry;
       len++;
       e2-= 4;
    }
    e2 += shift_bits;
  }

  *out_len= len;
  *exp2= e2;
}
#endif

static void gprintf_float2hex
  (long double F, int islong, int *sign, 
   unsigned char *digits, int *out_len, int *exp2)
{
#define gprintf_FLOAT2HEX_SPACE_REQ  ((LDBL_MANT_DIG+3)/4)
  // this is similar to float2dec, but is simpler since we're doing
  // base-2 arithmetic only
  int e2, len;

  if (F<0) { *sign=1; F= -F; } else { *sign= 0; }
  F= frexpl(F, &e2);

  int total_bits= islong ? LDBL_MANT_DIG : DBL_MANT_DIG;
  int bits= total_bits % 4;
  if (!bits) bits= 4;

  e2 -= total_bits;
  len= 0;
  do
  {
    int X;
    X= F= ldexpl(F, bits); // now we have it like 1a.bcdef 
    digits[len++]= X;      // store 1a.
    F -= X;
    total_bits-= bits;
    bits= 4;
  } while(total_bits);

  while(len>1 && digits[len-1]==0) { len--; e2+= 4; }

  if (len==1 && digits[0]==0) e2= 0;

#ifdef gprintf_OPTION_denormal_glibc
  gprintf_reconstruct_denormal(islong,digits, &len, &e2);
#endif

  *out_len= len;
  *exp2= e2;
}

static char *gprintf_format_exponent
  (char *end, gprintf_directive_t *D, int expval, int *explen)
{
  char sign;
  char *o_end= end;
  int digits;
  sign= expval < 0 ? '-' : '+';
  if (expval<0) expval= -expval;
  digits= 0;
  while(expval)
  {
    *--end= expval % 10 + '0';
    expval/= 10;
    digits++;
  }
  while(digits<((D->conversion=='a') ? 1:2)) 
  {
    *--end= '0';
    digits++;
  }
  *--end= sign;
  *--end= (D->conversion=='a') ? 
              (D->conversion_upper ? 'P' : 'p') :
              (D->conversion_upper ? 'E' : 'e');
  *explen = o_end - end;
  return end;
}


static void gprintf_get_crop_options(gprintf_directive_t *D, 
                                     int *crop_zeroes, int *force_point)
{
  *force_point= D->flags&gprintf_ALT_FORM ? 1 : 0;
  switch(D->conversion)
  {
  case 'a': *crop_zeroes= 0; break;
  case 'f': *crop_zeroes= 0; break;
  case 'e': *crop_zeroes= 0; break;
  case 'g': *crop_zeroes= D->flags&gprintf_ALT_FORM ? 0 : 1; break;
  }
}

static int gprintf_round_hex(unsigned char *start, unsigned char *end)
{
  if (*end<8) return 0;
  *end= 0;
  --end;
again:
  (*end)++;
  if (*end==16)
  {
    *end= 0;
    --end;
    if (end<start) return 1;
    goto again;
  }
  return 0;
}

static int gprintf_round_dec(unsigned char *start, unsigned char *end)
{
  if (*end<5) return 0;
  *end= 0;
  --end;
again:
  (*end)++;
  if (*end==10)
  {
    *end= 0;
    --end;
    if (end<start) return 1;
    goto again;
  }
  return 0;
}

static void gprintf_digits2ascii_hex(int upper, unsigned char *digits, int len)
{
  char *idx= upper ?  "0123456789ABCDEF"
                   :  "0123456789abcdef";
  for(int i=0;i<len;i++) digits[i] = idx[digits[i]];
}

static int  gprintf_print_float_hex
   (gprintf_out_t *O, long double F, gprintf_directive_t *D)
{
  int R;
  unsigned char digits_store[1+gprintf_FLOAT2HEX_SPACE_REQ];
  char exponent[20];
  unsigned char *digits; char *expstr;
  int fd, id;
  int digits_len, explen, negative, exp2;
  int have_point, force_point, crop_zeroes;
  gprintf_pad_t pad;

  if ((R=gprintf_print_nan(O,F,D))) return R;

  digits= digits_store+1;
  gprintf_float2hex(F, D->type==gprintf_TYPENAME_Lf,
                       &negative, digits, &digits_len, &exp2);

  if (!D->precision_at) D->precision= digits_len-1; 
  gprintf_get_crop_options(D, &crop_zeroes, &force_point);

  id= 1;
  fd= digits_len - id;
  if (fd>D->precision)
  {
    if (gprintf_round_hex(digits, digits+id+D->precision))
    {
      // actually, this is impossible to trigger with mantissa size 53
      // (the mantissa of a double). 
      // the mantissa is composed of 13 hex digits + 1 implicit bit. therefore
      // the highest hex digit is always 1 upon entry, which will never
      // overflow beyond 0xf. this will be possible only for denormal numbers. 
      digits--;
      digits[0]= 1;
      digits_len++;
      exp2+= 4;
      fd++;
    }
    exp2 += (fd-D->precision)*4;
    fd= D->precision;
    while(fd>0 && digits[id+fd-1]==0) { fd--; exp2+= 4; }
  }
  expstr= gprintf_format_exponent(exponent+sizeof(exponent), D, exp2+fd*4,
                                  &explen);
  gprintf_get_sign(negative, &pad, D->flags);
  have_point= force_point || (fd>0) || (crop_zeroes==0 && D->precision!=0);
  gprintf_get_pad(D, &pad,  pad.sign_length 
                          + 2
                          + id 
                          + (have_point ? 1 : 0) 
                          + (crop_zeroes ? fd : D->precision) 
                          + explen, 1);  
  R= gprintf_pad_left(O, &pad, D->conversion_upper? "0X" : "0x" , 2);
  gprintf_digits2ascii_hex(D->conversion_upper, digits, id+fd);
  R+= gprintf_out(O, digits, id);
  R+= gprintf_out(O, ".", have_point ? 1: 0);
  R+= gprintf_out(O, digits+id, fd);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0 : D->precision-fd);
  R+= gprintf_out(O, expstr, explen);
  return R+gprintf_pad_right(O, &pad);
}

static void gprintf_digits2ascii(unsigned char *digits, int len)
{
  int i;
  for(i=0;i<len;i++) digits[i] += '0';
}


static int gprintf_print_float_dec
   (gprintf_out_t *O, long double F, gprintf_directive_t *D)
{
  char zero_chr= '0';
  int length;
  char *expstr, exponent[200];
  int explen;
  unsigned char digits_store[gprintf_FLOAT2DEC_SPACE_REQ];
  unsigned char *digits= digits_store+1;
  int digits_len;
  int exp10, negative;
  int id, fd; // integral digits, fractional digits
  int force_point, crop_zeroes;
  int R;
  gprintf_pad_t pad;
  int  left_zeroes;
  int have_point, have_fraction;

  if ((length=gprintf_print_nan(O,F,D))) return length;

  if (!D->precision_at) D->precision= 6;
  float2decimal(F, D->precision, &negative, digits, &digits_len, &exp10);
  gprintf_get_crop_options(D, &crop_zeroes, &force_point);
  gprintf_get_sign(negative, &pad, D->flags);

  // we basicly have style 'e' or 'f'. the 'g' directive uses one of these.
  switch(D->conversion)
  {
  case 'g':
      {
        int P, X;
        X = exp10 + digits_len-1;
        P= gprintf_max(D->precision,1);
        if (P>X && X>=-4)
        {
          D->precision= P-1-X;
          goto print_style_f;
        }
        else
        {
          D->precision= P-1;
          goto print_style_e;
        }
       }
       break;
  case 'f': goto print_style_f;
  case 'e': goto print_style_e;
  }

print_style_f:
 
  /** we don't print the exponent. we just print 0.00..00 if the number
      is too small **/
  if (exp10>=0) goto print_style_f_intonly;
  if (-exp10<digits_len) goto print_style_f_intandfraction;
                    else goto print_style_f_fractiononly;

print_style_f_intonly:
  have_point= force_point || (D->precision!=0 && crop_zeroes==0);
  gprintf_get_pad(D, &pad, pad.sign_length + digits_len + exp10 + 
                           (have_point  ? 1 : 0) + 
                           (crop_zeroes ? 0: D->precision), 1);
  R= gprintf_pad_left(O, &pad, NULL, 0);
  gprintf_digits2ascii(digits, digits_len);
  R+= gprintf_out(O, digits, digits_len);
  R+= gprintf_pad_zero(O, exp10);
  R+= gprintf_out(O, ".", have_point  ? 1 : 0);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0 : D->precision);
  R+= gprintf_pad_right(O, &pad);
  return R;
  
print_style_f_intandfraction:
  fd= -exp10;
  id= digits_len - fd; 
  /**
    we have id integer digits and fd fractional digits.
   **/
  if (fd>D->precision)
  {
     if (gprintf_round_dec(digits, digits+id+D->precision)) 
     {
       digits--; digits_len++; id++;
       digits[0]= 1;
     }
     fd= D->precision;
     while(fd>0 && digits[id+fd-1]==0) fd--;
  }
  have_point= force_point || fd!=0 || (crop_zeroes==0 && D->precision!=0);
  gprintf_get_pad(D, &pad, pad.sign_length 
                           + id + (have_point ? 1:0) +
                                  (crop_zeroes ? fd : D->precision),1);
  R= gprintf_pad_left(O, &pad, NULL, 0);
  gprintf_digits2ascii(digits, digits_len);
  R+= gprintf_out(O, digits, id);
  R+= gprintf_out(O, ".", have_point ? 1:0);
  R+= gprintf_out(O, digits+id, fd);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0 : D->precision-fd);
  R+= gprintf_pad_right(O, &pad);
  return R;

print_style_f_fractiononly:
  if (!D->precision)
  {
    if (digits[0]>=5) goto print_style_f_one;
                 else goto print_style_f_zero;
  }
  left_zeroes= -exp10 - digits_len;  // we know -exp10 >= digits_len
  if (left_zeroes >= D->precision) goto print_style_f_zero;
 
  fd= D->precision - left_zeroes; 
      /** this can only end up in 1.0 because our fraction was 0.xxxxxx. **/
  if (gprintf_round_dec(digits, digits+fd)) goto print_style_f_one;
  while(fd>0 && digits[fd-1]==0) fd--;
  if (fd==0) goto print_style_f_zero;

  gprintf_get_pad(D, &pad, pad.sign_length + 2 + /* 0. */ + D->precision, 1);
  gprintf_digits2ascii(digits, fd);
  R= gprintf_pad_left(O, &pad, NULL, 0);
  R+= gprintf_out(O, "0.",2);
  // we know that we have fd>0 so we do have some non-zero fraction digits
  R+= gprintf_pad_zero(O, left_zeroes);
  R+= gprintf_out(O,digits, fd);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0 : D->precision - left_zeroes-fd);
  R+= gprintf_pad_right(O, &pad);
  return R;

print_style_f_one:
  zero_chr++;
print_style_f_zero:
  have_point= force_point || (D->precision!=0 && crop_zeroes==0);
  gprintf_get_pad(D, &pad, pad.sign_length
                           + 1
                           + (have_point ? 1 : 0)
                           + (crop_zeroes ? 0 : D->precision),1);
  R= gprintf_pad_left(O, &pad, NULL, 0);
  R+= gprintf_out(O, &zero_chr, 1);
  R+= gprintf_out(O, ".",  have_point ? 1 : 0);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0 : D->precision);
  R+= gprintf_pad_right(O, &pad);
  return R;

print_style_e:
  /** We need just one non-zero integer digit: i.ffffff exp10 **/
  exp10 += digits_len-1; 
  if (digits_len>D->precision+1)
  {
    if (gprintf_round_dec(digits, digits+1+D->precision)) 
    {
       digits--;
       digits[0]= 1;
       exp10++;
    }
    digits_len= D->precision+1;
  }
  while(digits_len>0 && digits[digits_len-1]==0) digits_len--;
  if (!digits_len) goto print_style_e_zero;
  expstr= gprintf_format_exponent(exponent+sizeof(exponent), D, exp10, &explen);
  fd= digits_len-1;
  id= 1;
  have_fraction= (fd!=0) || (!crop_zeroes && D->precision!=0);
  have_point= force_point || have_fraction; 
  gprintf_get_pad(D, &pad, pad.sign_length
                           + 1
                           + (have_point ? 1:0)
                           + (have_fraction ? D->precision : 0) 
                           + explen ,1);
  gprintf_digits2ascii(digits, digits_len);
  R= gprintf_pad_left(O, &pad, NULL, 0);
  R+= gprintf_out(O, digits, 1);
  R+= gprintf_out(O, ".", have_point ? 1:0);
  R+= gprintf_out(O, digits+1, fd);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0: D->precision-fd);
  R+= gprintf_out(O, expstr, explen); 
  R+= gprintf_pad_right(O, &pad);
  return R;

print_style_e_zero: 
  have_point= force_point || (!crop_zeroes && D->precision!=0);
  gprintf_get_pad(D, &pad, pad.sign_length
                           + 1
                           + (have_point ? 1 : 0)
                           + (crop_zeroes ? 0 : D->precision)
                           + 4 ,1);
  R= gprintf_pad_left(O, &pad, NULL, 0);
  R+= gprintf_out(O, "0", 1);
  R+= gprintf_out(O, ".", have_point ? 1 : 0);
  R+= gprintf_pad_zero(O, crop_zeroes ? 0 : D->precision);
  R+= gprintf_out(O, D->conversion_upper ? "E+00" : "e+00", 4);
  R+= gprintf_pad_right(O, &pad);
  return R;
}

static int gprintf_print_pointer
   (gprintf_out_t *O, void *ptr, gprintf_directive_t *D)
{
#ifndef gprintf_OPTION_pointer_precision_glibc
  if (!D->precision_at || D->precision<2*sizeof(void*))
  {
     D->precision_at= gprintf_VALUE_FROM_FORMAT;
     D->precision= 2*sizeof(void*);
  }
#endif
  D->flags |= gprintf_ALT_FORM;
  D->conversion= 'x';
  return gprintf_print_unsigned_int(O, (uintmax_t) ptr, D, 16);
}

static int gprintf_print_wide_string
   (gprintf_out_t *O, wchar_t *S, gprintf_directive_t *D)
{
  gprintf_pad_t pad;
  char mb_string[4];
  int mb_len;
  int total_len;
  int max_chars;
  mbstate_t state;
  int R;

  total_len= 0;
  max_chars= -1;
  memset(&state, 0, sizeof(state));
  for(int i=0;S[i];i++)
  {
    int len;
    mb_len= wcrtomb(mb_string, S[i], &state);
    if (mb_len==-1) mb_len= 1;
    len= mb_len + total_len;
    if (D->precision_at!=0 && len>D->precision) break;
    max_chars= i;
    total_len= len;
  }
  max_chars++;

  pad.sign_length= 0;
  gprintf_get_pad(D, &pad, total_len, 1);

  R= gprintf_pad_left(O, &pad, NULL, 0);

  memset(&state, 0, sizeof(state));
  for(int i=0;i<max_chars;i++)
  {
    mb_len= wcrtomb(mb_string, S[i], &state);
    if (mb_len==-1) { mb_len= 1; mb_string[0]= 'X'; }
    R+= gprintf_out(O, mb_string, mb_len);
  }
  return R+gprintf_pad_right(O, &pad);
}

int vgprintf(gprintf_func_t func, void *arg, const char *fmt, va_list *ap)
{
  gprintf_out_t O;
  gprintf_directive_t D;
  const char *s;
  gprintf_value_t values[10], value_store, *value;
  int types[10];
  int n_list, n_array, i;
  int total, len;
  int ibase;
  wchar_t wstr[2];

  O.func= func;
  O.arg= arg;

  for(i=1;i<=9;i++) types[i]= gprintf_TYPENAME_none;

  n_list= 0, n_array= 0;
  s= fmt;
  while(!gprintf_getdirective(&s,&D)) 
  {
    if (   gprintf_set_argtype(D.data_at, D.type, types)
        || gprintf_set_argtype(D.precision_at, gprintf_TYPENAME_ri, types)
        || gprintf_set_argtype(D.width_at, gprintf_TYPENAME_ri, types))
      goto inval;

    if (D.data_at>0) n_array++;
    if (D.precision_at>0) n_array++;
    if (D.width_at>0) n_array++;

    if (D.data_at==gprintf_VALUE_FROM_LIST) n_list++;
    if (D.precision_at==gprintf_VALUE_FROM_LIST) n_list++;
    if (D.width_at==gprintf_VALUE_FROM_LIST) n_list++;
  }
  if (D.conversion!=gprintf_CONVERSION_END) goto err;

  if (n_array) 
  {
    if (n_list) goto inval;
    for(i=1;i<=9 && types[i]!=gprintf_TYPENAME_none;i++)
      gprintf_get_arg(values+i, types[i], ap);
    for(;i<=9 && types[i]==gprintf_TYPENAME_none;i++) ;
    if (i!=10) goto inval;
  }

  total= 0;

next_directive:
  len= 0;
   // we know that there is no error because that was handled
   // in the previous loop
  if (gprintf_getdirective(&fmt, &D)) goto done;

  if (D.conversion==gprintf_CONVERSION_RAW)
  {
    if (D.raw_length > INT_MAX - total) goto overflow;
    total+= gprintf_out(&O, D.raw, D.raw_length);
    goto next_directive;
  }

  gprintf_get_intvalue(&D.width, D.width_at, values, ap);
  gprintf_get_intvalue(&D.precision, D.precision_at, values, ap); 
  value= gprintf_get_value(&value_store, D.data_at, D.type, values, ap);

  gprintf_fix_options(&D);

  ibase= 0;
  len= 0;
  switch(D.conversion)
  {
  case 'n': gprintf_store_result(value, D.type, total); break;
  case 'd': len= gprintf_print_signed(&O, value, &D); break;
  case 'x': ibase += 6;
  case 'u': ibase += 2;
  case 'o': ibase += 8; 
    len= gprintf_print_unsigned(&O, value, &D, ibase);
    break;
  case 's':
    switch(D.type)
    {
    case gprintf_TYPENAME_rs:
      len= gprintf_print_string(&O, value->rs, &D);
      break;
    case gprintf_TYPENAME_ls:
      len= gprintf_print_wide_string(&O, value->ls, &D);
    } 
    break;
  case 'c':
    switch(D.type)
    {
    case gprintf_TYPENAME_rc:
      len= gprintf_print_char(&O, value->rc, &D);
      break;
    case gprintf_TYPENAME_lc:
      wstr[0]= value->lc;
      wstr[1]= 0;
      len= gprintf_print_wide_string(&O, wstr, &D);
      break;
    }
    break;
  case 'a':
    switch(D.type)
    {
    case gprintf_TYPENAME_rf:
       len= gprintf_print_float_hex(&O, value->rf, &D); break;
    case gprintf_TYPENAME_Lf:
       len= gprintf_print_float_hex(&O, value->Lf, &D); break;
    };
    break;
  case 'f': case 'g': case 'e':
    switch(D.type)
    {
    case gprintf_TYPENAME_rf:
       len= gprintf_print_float_dec(&O, value->rf, &D); break;
    case gprintf_TYPENAME_Lf:
       len= gprintf_print_float_dec(&O, value->Lf, &D); break;
    };
    break;
  case 'p':
    len= gprintf_print_pointer(&O, value->rp, &D);
    break;
  }

  if (len > INT_MAX - total) goto overflow;
  total+= len;
  goto next_directive;

done:
  return total;

err:
  if (D.conversion==gprintf_CONVERSION_INVALID) goto inval;
overflow:
  errno= EOVERFLOW;
  return -1;
inval:
  errno= EINVAL;
  return -1;
}

int gprintf(gprintf_func_t func, void *arg, const char *fmt, ...)
{
  int R;
  va_list ap;
  va_start(ap,fmt);
  R= vgprintf(func, arg, fmt, &ap);
  va_end(ap);
  return R;
}

