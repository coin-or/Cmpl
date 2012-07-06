/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011 Thomas Schleiff - Halle(Saale), 
 *  Germany and Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany 
 *
 *  Coliop3 and CMPL are projects of the Technical University of 
 *  Applied Sciences Wildau and the Institute for Operations Research 
 *  and Business Management at the Martin Luther University 
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 * 
 *  CMPL is free software; you can redistribute it and/or modify it 
 *  under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 3 of the License, or 
 *  (at your option) any later version.
 * 
 *  CMPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
 *  License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/


#include "IntFuncTerm.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncTermBeg
	//********************************************************

	/* Beginn Term */
	void IntFuncTermBeg::exec(val_str *vp)
	{
		glvar->term_ind_bl.ti_beg_term = glvar->int_ptr;
		glvar->term_ind_bl.ti_is_floop = true;
		glvar->term_ind_bl.ti_is_gcol = false;
		glvar->term_ind_bl.ti_is_colexp = false;
		glvar->term_ind_bl.ti_anz_prod = 0;

		glvar->term_var_bl.tv_anz_var = 0;
	}


	//********************************************************
	// IntFuncTermNegFak
	//********************************************************

	/* 1.1.3 TS: Fuer negatives Vorzeichen vor Summe */
	/* Negiert das Negationskennzeichen, dass aktuell fuer Terme anzuwenden ist */
	void IntFuncTermNegFak::exec(val_str *vp)
	{
		if (glvar->neg_fak_term)
			glvar->neg_fak_term = false;
		else
			glvar->neg_fak_term = true;
	}


	//********************************************************
	// IntFuncTermVar
	//********************************************************

	/* Variable in einem Term */
	void IntFuncTermVar::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			int n;
			int c_i = i % MAX_ANZ_IND_1;
			ind_inf_one_str *inf_p;
			val_str v;

			PROTO_OUT_2("  %d", i);

			for(n=c_i-1, inf_p=glvar->ind_inf_ptr->i_ind+n; n>=0; n--, inf_p--) {
				inf_p->cur_val = pop_index(&(inf_p->cur_tp));
			}
			fterm_var(i);
		}
	}

	/* Setzt Variable fuer einen Term */
	void IntFuncTermVar::fterm_var(int i)
	{
#ifdef VAR_PROD
		int av = glvar->term_var_bl.tv_anz_var;
#endif
		struct term_var_str::tv_str *tvp;
		union ptr_sym_u up;
		bool found;
		enum ptrtp pt;
		symbol_str *s_p = glvar->symbol_table + (i / MAX_ANZ_IND_1);
		int c_i = i % MAX_ANZ_IND_1;
		int n, m;

		glvar->symbol_akt_p = s_p;
		glvar->ind_inf_ptr->i_anz_ind = c_i;

		up = ffetch_p(s_p, (array_str *)0, 0, &found, &pt);

		if (!found || pt != PTR_TP_VAR || up.v == (var_str *)0)
		{
			FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
			SPRINT_ERR_2(ERROR_NODEF_VAR, glvar->temp_buf);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			return;
		}

#ifdef VAR_PROD
		tvp = glvar->term_var_bl.tv_vars+av;
#else
		tvp = &(glvar->term_var_bl.tv_var);
#endif

		tvp->tv_varp = up.v;
		tvp->tv_sym_num = i;
		for(n=0; n<c_i; n++) {
			tvp->tv_inds[n].v.i = glvar->ind_inf_ptr->i_ind[n].cur_val;
			tvp->tv_inds[n].t = glvar->ind_inf_ptr->i_ind[n].cur_tp;
		}

		glvar->term_var_bl.tv_anz_var++;
	}


	//********************************************************
	// IntFuncTermVarFree
	//********************************************************

	/* Variable in einem Term, fuer Variable mit freien Indizes */
	void IntFuncTermVarFree::exec(val_str *vp)
	{
		if (vp[1].t != TP_INT)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			int i = vp[1].v.i;
			PROTO_OUT_2("  %d", i);

			fset_free_ind(i);
			fterm_var(i);
		}
	}


	//********************************************************
	// IntFuncTermEnd
	//********************************************************

	/* Ende Term, Koeffizient in Variable speichern, fuer Matrixprodukte wiederholte Auswertung des Terms */
	void IntFuncTermEnd::exec(val_str *vp)
	{
		Compiler *comp = glvar->interpreter->compiler;

		if(glvar->term_var_bl.tv_anz_var==0) {
			fkum_fonds();
		}
		else if(glvar->term_var_bl.tv_anz_var==1) {
			fkum_vkoeff();
		}
#ifdef VAR_PROD		/* Ersetzung Variablenprodukte */
		else {
			/* mehrere Variablen, Produkt zerlegen */
			int anz = glvar->term_var_bl.tv_anz_var;
			while(anz>1) {
				if((anz = search_rep_var_prod()) == 1) break;
				if(tv_test_swap_fakt()) break;
#ifdef PROTO
				if(glvar->is_protokoll) {
					char str1[ZC_NAME_LEN+300], str2[ZC_NAME_LEN+300];
					FILEIO->tvstr_symname(str1, glvar->term_var_bl.tv_vars, ZC_NAME_LEN+300);
					FILEIO->tvstr_symname(str2, glvar->term_var_bl.tv_vars+1, ZC_NAME_LEN+300);
					PROTO_OUT_3("\n      replace product of variables %s * %s", str1, str2);
				}
#endif /* PROTO */
				if(rep_prod(*comp)) break;
				glvar->term_var_bl.tv_vars[0] = glvar->var_prod_grd->vp_prod;
#ifdef PROTO
				if(glvar->is_protokoll) {
					char strp[ZC_NAME_LEN+300], str1[ZC_NAME_LEN+300], str2[ZC_NAME_LEN+300];
					FILEIO->tvstr_symname(strp, &(glvar->var_prod_grd->vp_prod), ZC_NAME_LEN+300);
					FILEIO->tvstr_symname(str1, &(glvar->var_prod_grd->vp_fakt1), ZC_NAME_LEN+300);
					FILEIO->tvstr_symname(str2, &(glvar->var_prod_grd->vp_fakt2), ZC_NAME_LEN+300);
					PROTO_OUT_4("\n      product of variables %s := %s * %s replaced", strp, str1, str2);
				}
#endif /* PROTO */
				if((--anz) > 1) {
					tv_shift(2, anz, -1);
					glvar->term_var_bl.tv_anz_var = anz;
				}
			}
			fkum_vkoeff();
		}
#endif /* VAR_PROD */

		fvprod_iter();
	}

	/* Koeffizient fuer Term ohne Variable */
	void IntFuncTermEnd::fkum_fonds()
	{
		bool f1, f2;
		double v1, v2, v3;
		val_str vk, v;

		pop_val(&vk);
		fconv_to_etp(&vk, &v, TPU_NUM, (char *)0, false);

		f1 = (v.t == TP_INT);
		v1 = (f1 ? (double)v.v.i : v.v.f);

		if(glvar->is_left_side) v1 = -v1;
		if(glvar->neg_fak_term) v1 = -v1;	/* 1.1.3 TS: Fuer negatives Vorzeichen vor Summe */
		v2 = (f2 = (glvar->fonds_s_line.t != TP_DBL)) ? 
						((double)glvar->fonds_s_line.v.i) : glvar->fonds_s_line.v.f;
		v3 = v2 + v1;

		if(f1 && f2 && v3 < LONG_MAX && v3 > LONG_MIN) {
			glvar->fonds_s_line.t = TP_INT;
			glvar->fonds_s_line.v.i = RUND(v3);
		}
		else {
			glvar->fonds_s_line.t = TP_DBL;
			glvar->fonds_s_line.v.f = v3;
		}

               PROTO_OUT_2("\n      right hand side = %f", v3);
	}


	/* Koeffizient in Variable speichern */
	void IntFuncTermEnd::fkum_vkoeff()
	{
		bool f1, f2;
		double v1, v2, v3;
		val_str vk, v;
#ifdef VAR_PROD
		var_str *vp = glvar->term_var_bl.tv_vars[0].tv_varp;
#else
		var_str *vp = glvar->term_var_bl.tv_var.tv_varp;
#endif

		pop_val(&vk);
		fconv_to_etp(&vk, &v, TPU_NUM, (char *)0, false);

		f1 = (v.t == TP_INT);
		v1 = (f1 ? (double)v.v.i : v.v.f);

		if(!glvar->is_left_side) v1 = -v1;
		if(glvar->neg_fak_term) v1 = -v1;	/* 1.1.3 TS: Fuer negatives Vorzeichen vor Summe */
		v2 = (f2 = (vp->v_koeff.t != TP_DBL)) ? 
						((double)vp->v_koeff.v.i) : vp->v_koeff.v.f;
		v3 = v2 + v1;
		if(f1 && f2 && v3 < LONG_MAX && v3 > LONG_MIN) vp->v_koeff.v.i = RUND(v3);
		else {
			vp->v_koeff.t = TP_DBL;
			vp->v_koeff.v.f = v3;
		}

#ifdef PROTO
		if(glvar->is_protokoll) {
#ifdef VAR_PROD
			struct term_var_str::tv_str *tvp = glvar->term_var_bl.tv_vars;
#else
			struct term_var_str::tv_str *tvp = &(glvar->term_var_bl.tv_var);
#endif /* VAR_PROD */
			FILEIO->tvstr_symname(glvar->errbuf, tvp, ERRBUFLEN);
                       PROTO_OUT_3("\n      coefficient %s = %f", glvar->errbuf, v3);
		}
#endif /* PROTO */
	}


	/* Iteration fuer Matrixprodukte */
	void IntFuncTermEnd::fvprod_iter()
	{
		int n;
		SetIterator *iter;

		if(glvar->term_ind_bl.ti_is_colexp)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_FATAL, (char *)0, ERROR_MATRIX_PROD_UNV);
		}

		for(n=glvar->term_ind_bl.ti_anz_prod-1; n>=0; n--)
		{
			iter = glvar->term_ind_bl.iter_set + n;

			if (iter->iter_set_next())
				break;
			else
				iter->iter_set_restart();
		}

		if(n >= 0)
		{
			glvar->term_ind_bl.ti_is_floop = false;
			glvar->term_ind_bl.ti_is_gcol = false;
			glvar->term_ind_bl.ti_is_colexp = false;
			glvar->term_ind_bl.ti_anz_prod = 0;

			glvar->term_var_bl.tv_anz_var = 0;

			glvar->int_ptr = glvar->term_ind_bl.ti_beg_term;
		}
	}


	/* Einfuegen generierte Variable */
	void IntFuncTermEnd::make_test_var(symbol_str *sym_p, int mi, var_str **vpp)
	{
		array_str *arr_p;
		union ptr_sym_u up;
		array_ins_info_str ret_ins_info;
		ind_inf_one_str *inf_p;
		bool found;
		int n;

		/* Einfacher Wert */
		if (mi == 0)
		{
			found = false;
			if (sym_p->val_ip0)
				ptr_symbol_elem(&(sym_p->val_ip0->elem), sym_p->val_ip0->attr.t, 0, &found);

			if (found)
			{
				FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
				SPRINT_ERR_2(ERROR_DEF_VAR, glvar->temp_buf);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
			else
			{
				if (!(sym_p->val_ip0))
				{
					sym_p->val_ip0 = (nonarray_str *) glvar->c_getmem(sizeof(nonarray_str));
					sym_p->val_ip0->attr.t = PTR_TP_VAR;
				}
				sym_p->val_ip0->elem.v = (var_str *) glvar->c_getmem(sizeof(var_str));
			}
			*vpp = sym_p->val_ip0->elem.v;
			return;
		}

		/* Noetigenfalls Ausgangsarray neu anlegen */
		arr_p = sym_p->val_ip[mi - 1];
		if (!arr_p)
		{
			arr_p = (array_str *) glvar->c_getmem(sizeof(array_str));
			sym_p->val_ip[mi - 1] = arr_p;

			arr_p->attr.t = (mi == 1 ? PTRTP_FOR_TYPECLASS(sym_p->tp_sym) : PTR_TP_ARRAY);
			arr_p->a_dir.cap = arr_p->a_dir.len = arr_p->a_dir.cnt = 0;
			arr_p->a_num.cap = arr_p->a_num.len = arr_p->a_num.cnt = 0;
			arr_p->a_str.cap = arr_p->a_str.len = arr_p->a_str.cnt = 0;
		}

		/* Indizes durchgehen */
		for (n=0, inf_p=glvar->ind_inf_ptr->i_ind; n<mi; n++, inf_p++)
		{
			up = search_ind_elem(arr_p, inf_p->cur_val, inf_p->cur_tp, &found, &ret_ins_info);
			if (n < mi - 1)
			{
				if (!found)
				{
					up = insert_ind_elem(arr_p, inf_p->cur_val, inf_p->cur_tp, NULL, &ret_ins_info);
					up.a->attr.t = (n == mi - 2 ? PTRTP_FOR_TYPECLASS(sym_p->tp_sym) : PTR_TP_ARRAY);
					up.a->a_dir.cap = up.a->a_dir.len = up.a->a_dir.cnt = 0;
					up.a->a_num.cap = up.a->a_num.len = up.a->a_num.cnt = 0;
					up.a->a_str.cap = up.a->a_str.len = up.a->a_str.cnt = 0;
				}
				arr_p = up.a;
			}
			else
			{
				/* Letzten Index erreicht, hier Variable einfuegen */
				if (found)
				{
					FILEIO->ferror_symname(glvar->temp_buf, false, TEMPBUFLEN);
					SPRINT_ERR_2(ERROR_DEF_VAR, glvar->temp_buf);
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				}
				else
				{
					up = insert_ind_elem(arr_p, inf_p->cur_val, inf_p->cur_tp, NULL, &ret_ins_info);
				}
				*vpp = up.v;
			}
		}
	}

#ifdef VAR_PROD

	/* Faktoren Variablenprodukt testen und noetigenfalls vertauschen */
	int IntFuncTermEnd::tv_test_swap_fakt()
	{
		var_str *vp1 = glvar->term_var_bl.tv_vars[0].tv_varp;
		var_str *vp2 = glvar->term_var_bl.tv_vars[1].tv_varp;
		int v1_b, v2_b, mi;
		struct term_var_str::tv_str tv_t;
		int i1 = glvar->term_var_bl.tv_vars[0].tv_sym_num;
		int i2 = glvar->term_var_bl.tv_vars[1].tv_sym_num;

		mi = (vp1->v_type==TP_VAR_BIN || vp2->v_type==TP_VAR_BIN ||
				(vp1->v_type==TP_VAR_DBL && vp2->v_type==TP_VAR_DBL))
											? MAX_ANZ_IND : MAX_ANZ_IND-1;
		if((vp1->v_type==TP_VAR_DBL && vp2->v_type==TP_VAR_DBL) ||
				!vp1->v_is_dn_b || !vp1->v_is_up_b ||
				!vp2->v_is_dn_b || !vp2->v_is_up_b ||
				i1 % MAX_ANZ_IND_1 + i2 % MAX_ANZ_IND_1 > mi) {
			struct term_var_str::tv_str *tvp1 = glvar->term_var_bl.tv_vars;
			struct term_var_str::tv_str *tvp2 = glvar->term_var_bl.tv_vars+1;
			char symn1[ZC_NAME_LEN+300], symn2[ZC_NAME_LEN+300];
			FILEIO->tvstr_symname(symn1, tvp1, ZC_NAME_LEN+300);
			FILEIO->tvstr_symname(symn2, tvp2, ZC_NAME_LEN+300);

			if(i1 % MAX_ANZ_IND_1 + i2 % MAX_ANZ_IND_1 > mi) {
				snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VARPROD_IND_MAX, symn1, symn2);
			}
			else {
				if(!vp1->v_is_dn_b)
						snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VARPROD_NO_LB, symn1);
				else if(!vp1->v_is_up_b)
						snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VARPROD_NO_UB, symn1);
				else if(!vp2->v_is_dn_b)
						snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VARPROD_NO_LB, symn2);
				else if(!vp2->v_is_up_b)
						snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VARPROD_NO_UB, symn2);

				else if(vp1->v_type==TP_VAR_DBL && vp2->v_type==TP_VAR_DBL) {
					snprintf(glvar->errbuf, ERRBUFLEN, ERROR_VARPROD_REAL_WARN, symn1, symn2);
					glvar->errbuf[ERRBUFLEN] = '\0';
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
					return(0);
				}
			}
			glvar->errbuf[ERRBUFLEN] = '\0';

			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			glvar->term_var_bl.tv_anz_var--;
			tv_shift(1, glvar->term_var_bl.tv_anz_var, -1);
			return(1);
		}

		if(vp1->v_type==TP_VAR_BIN) return(0);

		if(vp2->v_type!=TP_VAR_BIN) {
			if(vp2->v_type==TP_VAR_DBL) return(0);
			if(vp1->v_type!=TP_VAR_DBL) {
				int vd, vu;
				vd = INT_VAL(vp1->v_dn_b);
				vu = INT_VAL(vp1->v_up_b);
				v1_b = vu - vd;

				vd = INT_VAL(vp2->v_dn_b);
				vu = INT_VAL(vp2->v_up_b);
				v2_b = vu - vd;
				if(v1_b <= v2_b) return(0);
			}
		}

		tv_t = glvar->term_var_bl.tv_vars[0];
		glvar->term_var_bl.tv_vars[0] = glvar->term_var_bl.tv_vars[1];
		glvar->term_var_bl.tv_vars[1] = tv_t;
		return(0);
	}

	/* Variablen in glvar->term_var_bl verschieben */
	void IntFuncTermEnd::tv_shift(int von, int bis, int anz)
	{
		int n;
		if(anz>0) {
			for(n=bis; n>=von; n--) {
				glvar->term_var_bl.tv_vars[n+anz] = glvar->term_var_bl.tv_vars[n];
			}
		}
		else {
			for(n=von; n<=bis; n++) {
				glvar->term_var_bl.tv_vars[n+anz] = glvar->term_var_bl.tv_vars[n];
			}
		}
	}


	/* Suchen schon vorhandener Produkte und ersetzen durch Produktvariable */
	int IntFuncTermEnd::search_rep_var_prod()
	{
		int anz = glvar->term_var_bl.tv_anz_var;
		struct term_var_str::tv_str *tvp;
		struct var_prod_str *vp_akt;
		int n1, n2;

		if(anz < 2) return(anz);

		for(vp_akt=glvar->var_prod_grd; vp_akt; vp_akt = vp_akt->vp_next_vp) {
			for(n1=0, tvp=glvar->term_var_bl.tv_vars; n1<anz; n1++, tvp++) {
				if(tvp->tv_varp == vp_akt->vp_fakt1.tv_varp) break;
			}
			if(n1<anz) {
				if(vp_akt->vp_fakt1.tv_varp == vp_akt->vp_fakt2.tv_varp) {
					for(n2=n1+1, tvp++; n2<anz; n2++, tvp++) {
						if(tvp->tv_varp == vp_akt->vp_fakt2.tv_varp) break;
					}
				}
				else {
					for(n2=0, tvp=glvar->term_var_bl.tv_vars; n2<anz; n2++, tvp++) {
						if(tvp->tv_varp == vp_akt->vp_fakt2.tv_varp) break;
					}
				}
				if(n2<anz) {
					anz--;
					glvar->term_var_bl.tv_vars[n1] = vp_akt->vp_prod;
					tv_shift(n2+1, anz, -1);
					glvar->term_var_bl.tv_anz_var = anz;
					anz = search_rep_var_prod();
					break;
				}
			}
		}
		return(anz);
	}

	/* Suche schon vorhandener Binaerzerlegung, ansonsten Generierung */
	int IntFuncTermEnd::search_make_split(Compiler &cmplcomp, struct term_var_str::tv_str *tvs, struct term_var_str::tv_str *tvf)
	{
		int n, m, lb, ub, df;
		bool lf, uf;
		int inds, i, p;
		struct var_split_str *akt_spl;

		for(akt_spl=glvar->var_split_grd; akt_spl; akt_spl=akt_spl->vs_next_vs) {
			if(akt_spl->vs_ivar.tv_varp==tvf->tv_varp) break;
		}
		if(!akt_spl) {
			lf = (tvf->tv_varp->v_dn_b.t != TP_DBL);
			lb = DBL_VAL(tvf->tv_varp->v_dn_b);

			uf = (tvf->tv_varp->v_up_b.t != TP_DBL);
			ub = DBL_VAL(tvf->tv_varp->v_up_b);

			df = ub - lb;

			tvf->tv_varp->v_otype = TP_VAR_DBL;

			for(n=0; df>0; n++, df /= 2);

			COMP(LINE_BEG, 1);
			COMP_STR(glvar->name_pvarb_line);
			COMP(TERM_BEG, 0);
			COMP_PAR(LIT_VAL_SHORT, 1);
			inds = tvf->tv_sym_num % MAX_ANZ_IND_1;
			for(i=0; i<inds; i++) {
				COMP_LIT_VAL(tvf->tv_inds[i]);
			}
			COMP(TERM_VAR, 1);
			COMP_INT(tvf->tv_sym_num);
			COMP(TERM_END, 0);

			for(m=1,p=1; m<=n; m++,p*=2) {
				if(make_prod_var(cmplcomp, tvs, tvf, (struct term_var_str::tv_str *)0, m)) return(1);

				COMP(TERM_BEG, 0);
				COMP_PAR_INT(LIT_VAL, -p);
				inds = tvs->tv_sym_num % MAX_ANZ_IND_1;
				for(i=0; i<inds-1; i++) {
					COMP_LIT_VAL(tvf->tv_inds[i]);
				}
				COMP_PAR_INT(LIT_VAL, m);
				COMP(TERM_VAR, 1);
				COMP_INT(tvs->tv_sym_num);
				COMP(TERM_END, 0);
			}

			COMP_PAR(LINE_MOD, MODE_G);

			COMP(TERM_BEG, 0);
			if(lf) {
				COMP_PAR_INT(LIT_VAL, lb);
			}
			else {
				COMP(LIT_VAL, 1);
				COMP_DOUBLE((double)lb);
			}
			COMP(TERM_END, 0);

			COMP(LINE_END, 0);

			akt_spl = (struct var_split_str *) glvar->c_getmem(sizeof(struct var_split_str));
			akt_spl->vs_ivar = *tvf;
			akt_spl->vs_bvars = *tvs;
			akt_spl->vs_next_vs = glvar->var_split_grd;
			glvar->var_split_grd = akt_spl;
		}
		else *tvs = akt_spl->vs_bvars;
		return(0);
	}

	/* Suche schon vorhandene Normierung real-Variable, ansonsten Generierung */
	int IntFuncTermEnd::search_make_norm(Compiler &cmplcomp, struct term_var_str::tv_str *tvn, struct term_var_str::tv_str *tvf)
	{
		double lb, ub, df;
		bool lf, uf;
		int inds, i;
		struct var_split_str *akt_spl;
		for(akt_spl=glvar->var_split_grd; akt_spl; akt_spl=akt_spl->vs_next_vs) {
			if(akt_spl->vs_ivar.tv_varp==tvf->tv_varp) break;
		}
		if(!akt_spl) {
			lf = (tvf->tv_varp->v_dn_b.t != TP_DBL);
			lb = DBL_VAL(tvf->tv_varp->v_dn_b);

			uf = (tvf->tv_varp->v_up_b.t != TP_DBL);
			ub = DBL_VAL(tvf->tv_varp->v_up_b);

			df = ub - lb;

			if(make_prod_var(cmplcomp, tvn, tvf, (struct term_var_str::tv_str *)0, -1)) return(1);

			COMP(LINE_BEG, 1);
			COMP_STR(glvar->name_pvarb_line);
			COMP(TERM_BEG, 0);
			COMP_PAR(LIT_VAL_SHORT, 1);
			inds = tvf->tv_sym_num % MAX_ANZ_IND_1;
			for(i=0; i<inds; i++) {
				COMP_LIT_VAL(tvf->tv_inds[i]);
			}
			COMP(TERM_VAR, 1);
			COMP_INT(tvf->tv_sym_num);
			COMP(TERM_END, 0);

			COMP(TERM_BEG, 0);
			if(lf && uf) {
				COMP_PAR_INT(LIT_VAL, -RUND(df));
			}
			else {
				COMP(LIT_VAL, 1);
				COMP_DOUBLE(-df);
			}
			inds = tvn->tv_sym_num % MAX_ANZ_IND_1;
			for(i=0; i<inds; i++) {
				COMP_LIT_VAL(tvn->tv_inds[i]);
			}
			COMP(TERM_VAR, 1);
			COMP_INT(tvn->tv_sym_num);
			COMP(TERM_END, 0);

			COMP_PAR(LINE_MOD, MODE_G);

			COMP(TERM_BEG, 0);
			if(lf) {
				COMP_PAR_INT(LIT_VAL, RUND(lb));
			}
			else {
				COMP(LIT_VAL, 1);
				COMP_DOUBLE(lb);
			}
			COMP(TERM_END, 0);

			COMP(LINE_END, 0);

			akt_spl = (struct var_split_str *) glvar->c_getmem(sizeof(struct var_split_str));
			akt_spl->vs_ivar = *tvf;
			akt_spl->vs_bvars = *tvn;
			akt_spl->vs_next_vs = glvar->var_split_grd;
			glvar->var_split_grd = akt_spl;
		}
		else *tvn = akt_spl->vs_bvars;
		return(0);
	}


	/* Merken generiertes Produkt */
	void IntFuncTermEnd::ins_var_prod(struct term_var_str::tv_str *tvp, struct term_var_str::tv_str *tvf1, struct term_var_str::tv_str *tvf2)
	{
		struct var_prod_str *vp_akt;
		vp_akt = (struct var_prod_str *) glvar->c_getmem(sizeof(struct var_prod_str));
		vp_akt->vp_prod = *tvp;
		vp_akt->vp_fakt1 = *tvf1;
		vp_akt->vp_fakt2 = *tvf2;
		vp_akt->vp_next_vp = glvar->var_prod_grd;
		glvar->var_prod_grd = vp_akt;
	}


	/* Ersetzen Produkt, generieren Restriktionen zur Produktnachbildung */
	int IntFuncTermEnd::rep_prod(Compiler &cmplcomp)
	{
		struct term_var_str::tv_str *tvf1 = glvar->term_var_bl.tv_vars;
		struct term_var_str::tv_str *tvf2 = glvar->term_var_bl.tv_vars + 1;
		struct term_var_str::tv_str tvp, tvs, tvsp;
		double lb, ub;
		bool lf, uf;
		int n, m, inds, i, i2, p;
		bool u1f, l1f;

		PROTO_OUT_1("\n      rep_prod");

		if(make_prod_var(cmplcomp, &tvp, tvf1, tvf2, 0)) return(1);
		ins_var_prod(&tvp, tvf1, tvf2);

		lf = (tvf2->tv_varp->v_dn_b.t != TP_DBL);
		lb = DBL_VAL(tvf2->tv_varp->v_dn_b);

		uf = (tvf2->tv_varp->v_up_b.t != TP_DBL);
		ub = DBL_VAL(tvf2->tv_varp->v_up_b);

		COMP(SRC_LINE, 1);
		COMP_INT(glvar->akt_src_line);

		if(*glvar->name_pvarb_line == '\0') {
			flbeg_lname(glvar->name_pvarb_line, glvar->org_name_s_line, true);
			COMP(VAR_LIN_BEG, 0);
		}

		if(tvf1->tv_varp->v_type == TP_VAR_BIN) {		/* Produkt mit Binaervar. */
			if(lb < 0.)
				prod_rest_comp(cmplcomp, &tvp, tvf1, (struct term_var_str::tv_str *)0, lb, lf, MODE_U, -1);
#ifdef UPPER_BOUND
			if(ub > 0.)
#endif
				prod_rest_comp(cmplcomp, &tvp, tvf1, (struct term_var_str::tv_str *)0, ub, uf, MODE_O, -1);
			prod_rest_comp(cmplcomp, &tvp, tvf1, tvf2, lb, lf, MODE_O, -1);
			prod_rest_comp(cmplcomp, &tvp, tvf1, tvf2, ub, uf, MODE_U, -1);
		}
		else if(tvf1->tv_varp->v_type == TP_VAR_INT) {	/* Produkt mit ganzz. Var. */
			int u1b, l1b;
			t_addr lbeg, lend, gaddr, loopstart;
			loop_ctrl_str *loopp;

			if(search_make_split(cmplcomp, &tvs, tvf1)) return(1);
			inds = tvs.tv_sym_num % MAX_ANZ_IND_1;
			n = tvs.tv_inds[inds-1].v.i;
			for(m=1; m<=n; m++) {
				tvs.tv_inds[inds-1].v.i = m;
				if(make_prod_var(cmplcomp, &tvsp, &tvs, tvf2, 0)) return(1);
			}

			/* Schleife ueber Binaervariablen */
			loopp = glvar->tmp_table + MAX_ANZ_TMP;

			loopstart = COMP_PAR(LIT_VAL_SHORT, 1);
			COMP_PAR_INT(LIT_VAL, n);
			COMP_PAR(LIT_SET, TP_SET_ALG_S1);
			COMP_SUB_PAR_CNT(LOOP_CTRL, START_LOOP, MAX_ANZ_TMP, 1);
			COMP_INT(0);
			loopp->iter_start = COMP_SUB_PAR_CNT(TEST_ITER, 0, MAX_ANZ_TMP, 3);
			COMP_ADDR(loopstart);
			loopp->end_jump = COMP_ADDR(0);
			loopp->last_jump = COMP_ADDR(0);

			if(lb < 0.)
				prod_rest_comp(cmplcomp, &tvsp, &tvs, (struct term_var_str::tv_str *)0, lb, lf, MODE_U, MAX_ANZ_TMP);
#ifdef UPPER_BOUND
			if(ub > 0.)
#endif
				prod_rest_comp(cmplcomp, &tvsp, &tvs, (struct term_var_str::tv_str *)0, ub, uf, MODE_O, MAX_ANZ_TMP);
			prod_rest_comp(cmplcomp, &tvsp, &tvs, tvf2, lb, lf, MODE_O, MAX_ANZ_TMP);
			prod_rest_comp(cmplcomp, &tvsp, &tvs, tvf2, ub, uf, MODE_U, MAX_ANZ_TMP);

			COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1);
			COMP_ADDR(loopp->iter_start);
			gaddr = COMP_SUB_PAR(LOOP_CTRL, LOOP_END, MAX_ANZ_TMP);
			COMP_ADDR_AT(loopp->end_jump, gaddr);
			COMP_ADDR_AT(loopp->last_jump, gaddr);

			/* Verbinden Teilprodukte zu Gesamtprodukt */
			COMP(LINE_BEG, 1);
			COMP_STR(glvar->name_pvarb_line);
			COMP(TERM_BEG, 0);
			COMP_PAR(LIT_VAL_SHORT, 1);
			i2 = tvp.tv_sym_num % MAX_ANZ_IND_1;
			for(i=0; i<i2; i++) {
				COMP_LIT_VAL(tvp.tv_inds[i]);
			}
			COMP(TERM_VAR, 1);
			COMP_INT(tvp.tv_sym_num);
			COMP(TERM_END, 0);

			for(m=1,p=1; m<=n; m++,p*=2) {
				COMP(TERM_BEG, 0);
				COMP_PAR_INT(LIT_VAL, -p);
				i2 = tvsp.tv_sym_num % MAX_ANZ_IND_1;
				for(i=0; i<i2; i++) {
					if(i==inds-1) COMP_PAR_INT(LIT_VAL, m);
					else COMP_LIT_VAL(tvsp.tv_inds[i]);
				}
				COMP(TERM_VAR, 1);
				COMP_INT(tvsp.tv_sym_num);
				COMP(TERM_END, 0);
			}

			COMP(TERM_BEG, 0);
			l1f = (tvf1->tv_varp->v_dn_b.t != TP_DBL);
			l1b = INT_VAL(tvf1->tv_varp->v_dn_b);
			if(l1f) {
				COMP_PAR_INT(LIT_VAL, -l1b);
			}
			else {
				COMP(LIT_VAL, 1);
				COMP_DOUBLE((double)(-l1b));
			}
			i2 = tvf2->tv_sym_num % MAX_ANZ_IND_1;
			for(i=0; i<i2; i++) {
				COMP_LIT_VAL(tvf2->tv_inds[i]);
			}
			COMP(TERM_VAR, 1);
			COMP_INT(tvf2->tv_sym_num);
			COMP(TERM_END, 0);

			COMP_PAR(LINE_MOD, MODE_G);
			COMP(LINE_END, 0);
		}
		else {					/* Produkt aus stetigen Variablen */
			bool n1, n2;
			bool kp;
			double u1b, l1b;
			struct term_var_str::tv_str tvn1, tvn2, tvnp;
			struct term_var_str::tv_str *f1, *f2, *p;

			l1f = (tvf1->tv_varp->v_dn_b.t != TP_DBL);
			l1b = DBL_VAL(tvf1->tv_varp->v_dn_b);

			u1f = (tvf1->tv_varp->v_up_b.t != TP_DBL);
			u1b = DBL_VAL(tvf1->tv_varp->v_up_b);

			if(IS_EQ(lb, ub) || IS_EQ(l1b, u1b)) kp = true;
			else {
				kp = false;
				if(!IS_EQ(l1b, 0.) || !IS_EQ(u1b, 1.)) {
					search_make_norm(cmplcomp, &tvn1, tvf1);
					n1 = true;
				}
				else n1 = false;
				if(!IS_EQ(lb, 0.) || !IS_EQ(ub, 1.)) {
					search_make_norm(cmplcomp, &tvn2, tvf2);
					n2 = true;
				}
				else n2 = false;
				f1 = n1 ? &tvn1 : tvf1;
				f2 = n2 ? &tvn2 : tvf2;
			}

			if(n1 || n2 || kp) {
				if(!kp) {
					if(make_prod_var(cmplcomp, &tvnp, f1, f2, 0)) return(1);
					p = &tvnp;
				}
				COMP(LINE_BEG, 1);
				COMP_STR(glvar->name_pvarb_line);
				COMP(TERM_BEG, 0);
				COMP_PAR(LIT_VAL_SHORT, 1);
				i2 = tvp.tv_sym_num % MAX_ANZ_IND_1;
				for(i=0; i<i2; i++) {
					COMP_LIT_VAL(tvp.tv_inds[i]);
				}
				COMP(TERM_VAR, 1);
				COMP_INT(tvp.tv_sym_num);
				COMP(TERM_END, 0);

				COMP(TERM_BEG, 0);
				if(lf) {
					COMP_PAR_INT(LIT_VAL, -RUND(lb));
				}
				else {
					COMP(LIT_VAL, 1);
					COMP_DOUBLE(-lb);
				}
				i2 = tvf1->tv_sym_num % MAX_ANZ_IND_1;
				for(i=0; i<i2; i++) {
					COMP_LIT_VAL(tvf1->tv_inds[i]);
				}
				COMP(TERM_VAR, 1);
				COMP_INT(tvf1->tv_sym_num);
				COMP(TERM_END, 0);

				COMP(TERM_BEG, 0);
				if(l1f) {
					COMP_PAR_INT(LIT_VAL, -RUND(l1b));
				}
				else {
					COMP(LIT_VAL, 1);
					COMP_DOUBLE(-l1b);
				}
				i2 = tvf2->tv_sym_num % MAX_ANZ_IND_1;
				for(i=0; i<i2; i++) {
					COMP_LIT_VAL(tvf2->tv_inds[i]);
				}
				COMP(TERM_VAR, 1);
				COMP_INT(tvf2->tv_sym_num);
				COMP(TERM_END, 0);

				if(!kp) {
					COMP(TERM_BEG, 0);
					if(lf && uf && l1f && u1f) {
						COMP_PAR_INT(LIT_VAL, -RUND((ub-lb)*(u1b-l1b)));
					}
					else {
						COMP(LIT_VAL, 1);
						COMP_DOUBLE(-(ub-lb)*(u1b-l1b));
					}
					i2 = p->tv_sym_num % MAX_ANZ_IND_1;
					for(i=0; i<i2; i++) {
						COMP_LIT_VAL(p->tv_inds[i]);
					}
					COMP(TERM_VAR, 1);
					COMP_INT(p->tv_sym_num);
					COMP(TERM_END, 0);
				}

				COMP(TERM_BEG, 0);
				if(lf && l1f) {
					COMP_PAR_INT(LIT_VAL, RUND(lb*l1b));
				}
				else {
					COMP(LIT_VAL, 1);
					COMP_DOUBLE(lb*l1b);
				}
				COMP(TERM_END, 0);

				COMP_PAR(LINE_MOD, MODE_G);
				COMP(LINE_END, 0);
			}
			else p = &tvp;

			if(!kp) {
				prod_rest_comp(cmplcomp, p, f1, (struct term_var_str::tv_str *)0, (double)1., true, MODE_O, -1);
				prod_rest_comp(cmplcomp, p, f2, (struct term_var_str::tv_str *)0, (double)1., true, MODE_O, -1);
				prod_rest_comp(cmplcomp, p, f1, f2, (double)1., true, MODE_U, -1);
			}

		}
		return(0);
	}


	/* Generieren Produktvariable oder Variable zur Binaerzerlegung */
	int IntFuncTermEnd::make_prod_var(Compiler &cmplcomp, struct term_var_str::tv_str *tvp, struct term_var_str::tv_str *tvf1, struct term_var_str::tv_str *tvf2, int sn)
	{
		var_str *vp;
		int n;
		char str[SYM_NAME_LEN+11];
		symbol_str *s1, *s2;
		ind_inf_one_str *inf_p;
		int i1, i2, in, ind, ip;
		double lb, ub, l1b, u1b, l2b, u2b;
		double d;
		bool lf, uf, l1f, u1f, l2f, u2f;
		bool f;

		PROTO_OUT_1("\n      make_prod_var: ");

		/* Name */
		s1 = glvar->symbol_table + (tvf1->tv_sym_num / MAX_ANZ_IND_1);
		if(tvf2) {
			s2 = glvar->symbol_table + (tvf2->tv_sym_num / MAX_ANZ_IND_1);
			snprintf(str, SYM_NAME_LEN+10, MAKE_NAME_PROD, s1->s_name, s2->s_name);
			str[SYM_NAME_LEN+10] = '\0';
			if(strlen(str) >= SYM_NAME_LEN) {
				snprintf(str, SYM_NAME_LEN-1, MAKE_SNAME_VP, glvar->vp_sn_num);
				str[SYM_NAME_LEN-1] = '\0';
				glvar->vp_sn_num++;
			}
		}
		else {
			snprintf(str, SYM_NAME_LEN+10, MAKE_NAME_SPLIT, s1->s_name);
			str[SYM_NAME_LEN+10] = '\0';
			if(strlen(str) >= SYM_NAME_LEN) {
				snprintf(str, SYM_NAME_LEN-1, MAKE_SNAME_VP, glvar->vp_sn_num);
				str[SYM_NAME_LEN-1] = '\0';
				glvar->vp_sn_num++;
			}
		}
		PROTO_OUT_2("%s", str);

		ip = cmplcomp.search_symbol(str, &ind);
		if(ip == -1) {
			//TODO: Symbolverwaltung
			bool found;
			const char *s = glvar->symbol_main_names.elem(glvar->symbol_main_names.search_insert(glvar, str, true, &found));
			ip = cmplcomp.insert_symbol(s, TPK_VAR);
			ind = ip;
		}
		else if(ip < 0 || glvar->symbol_table[ind].tp_sym != TPK_VAR) {
			snprintf(glvar->errbuf, ERRBUFLEN, (ip<0 ? ERROR_DEF_VAR_LK : ERROR_DEF_VAR_KONST), str);
			glvar->errbuf[ERRBUFLEN] = '\0';
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			return(1);
		}

		/* Indizes */
		i1 = tvf1->tv_sym_num % MAX_ANZ_IND_1;
		if(tvf2)
		{
			i2 = tvf2->tv_sym_num % MAX_ANZ_IND_1;
			in = i1 + i2;
		}
		else in = sn==-1 ? i1 : i1+1;

		ip = ind * MAX_ANZ_IND_1 + in;
		for(n=0, inf_p=glvar->ind_inf_ptr->i_ind; n<in; n++, inf_p++)
		{
			if (n < i1)
			{
				inf_p->cur_val = tvf1->tv_inds[n].v.i;
				inf_p->cur_tp = tvf1->tv_inds[n].t;
			}
			else if (tvf2)
			{
				inf_p->cur_val = tvf2->tv_inds[n-i1].v.i;
				inf_p->cur_tp = tvf2->tv_inds[n-i1].t;
			}
			else
			{
				inf_p->cur_val = sn;
				inf_p->cur_tp = TP_INT;
			}
		}
		glvar->ind_inf_ptr->i_anz_ind = in;
		glvar->symbol_akt_p = glvar->symbol_table + ind;
		make_test_var(glvar->symbol_akt_p, in, &vp);

		/* Schranken */
		if(tvf2) {
		  vp->v_type = tvf2->tv_varp->v_type;
		  vp->v_otype = TP_VAR_DBL;

		  l2f = (tvf2->tv_varp->v_dn_b.t != TP_DBL);
		  l2b = DBL_VAL(tvf2->tv_varp->v_dn_b);

		  u2f = (tvf2->tv_varp->v_up_b.t != TP_DBL);
		  u2b = DBL_VAL(tvf2->tv_varp->v_up_b);

		  if(tvf1->tv_varp->v_type==TP_VAR_INT || tvf1->tv_varp->v_type==TP_VAR_DBL) {
			l1f = (tvf1->tv_varp->v_dn_b.t != TP_DBL);
			l1b = DBL_VAL(tvf1->tv_varp->v_dn_b);

			u1f = (tvf1->tv_varp->v_up_b.t != TP_DBL);
			u1b = DBL_VAL(tvf1->tv_varp->v_up_b);

			lb = ub = l1b*l2b;
			lf = uf = l1f && l2f;
			if(lb > (d=l1b*u2b)) {
				lb = d;
				lf = l1f && u2f;
			}
			if(ub < d) {
				ub = d;
				uf = l1f && u2f;
			}
			if(lb > (d=u1b*l2b)) {
				lb = d;
				lf = u1f && l2f;
			}
			if(ub < d) {
				ub = d;
				uf = u1f && l2f;
			}
			if(lb > (d=u1b*u2b)) {
				lb = d;
				lf = u1f && u2f;
			}
			if(ub < d) {
				ub = d;
				uf = u1f && u2f;
			}
		  }
		  else {
			if(l2b > 0) {
				lb = 0.;
				lf = true;
			}
			else {
				lb = l2b;
				lf = l2f;
			}
			if(u2b < 0) {
				ub = 0.;
				uf = true;
			}
			else {
				ub = u2b;
				uf = u2f;
			}
		  }

		  vp->v_is_dn_b = true;
		  vp->v_dn_b.t = (lf ? TP_INT : TP_DBL);
		  if(lf) vp->v_dn_b.v.i = RUND(lb);
		  else vp->v_dn_b.v.f = lb;

		  vp->v_is_up_b = true;
		  vp->v_up_b.t = (uf ? TP_INT : TP_DBL);
		  if(uf) vp->v_up_b.v.i = RUND(ub);
		  else vp->v_up_b.v.f = ub;
		}

		else {
			vp->v_otype = vp->v_type = sn==-1 ? TP_VAR_DBL : TP_VAR_BIN;
			vp->v_is_dn_b = true;
			vp->v_dn_b.t = TP_INT;
			vp->v_dn_b.v.i = 0;
			vp->v_is_up_b = true;
			vp->v_up_b.t = TP_INT;
			vp->v_up_b.v.i = 1;
		}

		vp->v_defnum = glvar->akt_defnum;
		vp->v_is_koeff = false;
		glvar->akt_defnum++;

		vp->v_koeff.t = TP_INT;
		vp->v_koeff.v.i = 0;

#ifdef PROTO
		if(glvar->is_protokoll) {
			FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
			PROTO_OUT_4("\n    variable %s for product of variables defined (%d, type %d)",
					glvar->errbuf, vp->v_defnum, (int)(vp->v_type));
		}
#endif

		tvp->tv_varp = vp;
		tvp->tv_sym_num = ip;
		for(n=0, inf_p=glvar->ind_inf_ptr->i_ind; n<in; n++, inf_p++) {
			tvp->tv_inds[n].v.i = inf_p->cur_val;
			tvp->tv_inds[n].t = inf_p->cur_tp;
		}
		return(0);
	}


	/* Generieren Zwischencode fuer Produktnachbildung */
	void IntFuncTermEnd::prod_rest_comp(Compiler &cmplcomp, struct term_var_str::tv_str *tvp, struct term_var_str::tv_str *tvf1, struct term_var_str::tv_str *tvf2,
			double bv, bool bf, int lm, int tmp_num)
	{
		int inds, n, t_ind;
		COMP(LINE_BEG, 1);
		COMP_STR(glvar->name_pvarb_line);

		COMP(TERM_BEG, 0);
		COMP_PAR(LIT_VAL_SHORT, 1);
		t_ind = tmp_num==-1 ? -1 : (tvf1->tv_sym_num % MAX_ANZ_IND_1) - 1;
		inds = tvp->tv_sym_num % MAX_ANZ_IND_1;
		for(n=0; n<inds; n++) {
			if(n==t_ind) {
				COMP(FETCH_TMP, 1);
				COMP_INT(tmp_num);
			}
			else {
				COMP_LIT_VAL(tvp->tv_inds[n]);
			}
		}
		COMP(TERM_VAR, 1);
		COMP_INT(tvp->tv_sym_num);
		COMP(TERM_END, 0);

		COMP(TERM_BEG, 0);
		if(bf) {
			COMP_PAR_INT(LIT_VAL, -RUND(bv));
		}
		else {
			COMP(LIT_VAL, 1);
			COMP_DOUBLE(-bv);
		}
		inds = tvf1->tv_sym_num % MAX_ANZ_IND_1;
		for(n=0; n<inds; n++) {
			if(n==t_ind) {
				COMP(FETCH_TMP, 1);
				COMP_INT(tmp_num);
			}
			else {
				COMP_LIT_VAL(tvf1->tv_inds[n]);
			}
		}
		COMP(TERM_VAR, 1);
		COMP_INT(tvf1->tv_sym_num);
		COMP(TERM_END, 0);

		if(tvf2) {
			COMP(TERM_BEG, 0);
			COMP_PAR(LIT_VAL_SHORT, -1);
			inds = tvf2->tv_sym_num % MAX_ANZ_IND_1;
			for(n=0; n<inds; n++) {
				COMP_LIT_VAL(tvf2->tv_inds[n]);
			}
			COMP(TERM_VAR, 1);
			COMP_INT(tvf2->tv_sym_num);
			COMP(TERM_END, 0);

			COMP(TERM_BEG, 0);
			if(bf) {
				COMP_PAR_INT(LIT_VAL, RUND(bv));
			}
			else {
				COMP(LIT_VAL, 1);
				COMP_DOUBLE(bv);
			}
			COMP(TERM_END, 0);
		}

		COMP_PAR(LINE_MOD, lm);
		COMP(LINE_END, 0);
	}

#endif /* VAR_PROD */
}

