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


#include "IntFuncLine.hh"
#include "gencode.h"


namespace cmpl
{
	//********************************************************
	// IntFuncLineBase
	//********************************************************

	/* Fuehre Funktion func fuer alle Variablen aus */
	void IntFuncLineBase::forall_var(int (cmpl::IntFuncLineBase::* func)(var_str *))
	{
		int n,m;
		int rc = 0;

		for(glvar->symbol_akt_p=glvar->symbol_table, n=0; n<glvar->symbol_anzahl; n++, glvar->symbol_akt_p++) {
			if(glvar->symbol_akt_p->tp_sym == TPK_VAR) {
				/* Fuer Symbol ohne Index */
				if (glvar->symbol_akt_p->val_ip0)
				{
					rc = (this->*func)(glvar->symbol_akt_p->val_ip0->elem.v);
					if(rc) break;
				}

				/* Fuer Symbol mit Indizes */
				for(m=1; m<MAX_ANZ_IND_1; m++)
				{
					rc = fforall_var(glvar->symbol_akt_p->val_ip[m-1], func);
					if(rc) break;
				}
				if(rc) break;
			}
		}
	}

	/* Fuehre Funktion func fuer Variable aus */
	int IntFuncLineBase::fforall_var(array_str *array, int (cmpl::IntFuncLineBase::* func)(var_str *))
	{
		int rc = 0;

		if (array && array->attr.t != PTR_TP_EMPTY)
		{
			int i, n;
			struct a_str *arr_p;
			union ptr_sym_u up;

			/* Alle Elemente im Array durchgehen */
			for (i=0; i<3; i++)
			{
				arr_p = (i==0 ? &(array->a_dir) : (i==1 ? &(array->a_num) : &(array->a_str)));
				for (n=0, up=arr_p->array; n<arr_p->len && !rc; n++)
				{
					if (array->attr.t == PTR_TP_ARRAY)
						rc = fforall_var(up.a+n, func);
					else if (array->attr.t == PTR_TP_VAR)
						rc = (this->*func)(up.v+n);
				}
			}
		}

		return(rc);
	}

	/* Alle Koeffizienten auf 0 */
	/*
	int IntFuncLineBase::fdef_linebeg(var_str *vp)
	{
		if(vp && vp->v_type != TPP_EMPTY) {
			vp->v_koeff.t = TP_INT;
			vp->v_koeff.v.i = 0;
		}
		return(0);
	}
	*/

	/* Variablenkoeffizienten fuer Matrix speichern */
	/*
	int IntFuncLineBase::flend_var(var_str *vp)
	{
		if(vp && vp->v_type != TPP_EMPTY) {
			double v;
			int n = vp->v_defnum;

			glvar->gl_mline_vp[n] = vp->v_koeff;
			v = DBL_VAL(vp->v_koeff);
			if(! IS_EQ(v, 0.)) {
				glvar->gl_mline_anz++;
				glvar->gl_mline_fp[n] = true;
		//TS qqq
		vp->v_is_koeff = true;
			}
			else
				glvar->gl_mline_fp[n] = false;
		}
		return(0);
	}
	*/

	/* Variablen mit Koeffizient ungleich 0 kennzeichnen */
	/*
	int IntFuncLineBase::ftouch_not0(var_str *vp)
	{
		if(vp && vp->v_type != TPP_EMPTY) {
			double v;
			v = DBL_VAL(vp->v_koeff);
			if(! IS_EQ(v, 0.)) vp->v_is_koeff = true;
		}
		return(0);
	}
	*/

	/* Auf Definitionsnummer der Variable testen */
	int IntFuncLineBase::ffvarp_defn(var_str *vp)
	{
		if(vp && vp->v_type != TPP_EMPTY) {
			if(vp->v_defnum == glvar->gl_mline_defnum) {
				glvar->gl_mline_varptr = vp;
				return(1);
			}
		}
		return(0);
	}

	/* compare two int numbers, for use in qsort() */
	int IntFuncLineBase::cmp_int(const void *p1, const void *p2)
	{
		int i1 = * (int *) p1;
		int i2 = * (int *) p2;
		return (i1 == i2 ? 0 : (i1 < i2 ? -1 : 1));
	}

	/* Beginn Zeile: Alle Koeffizienten auf 0 setzen */
	void IntFuncLineBase::fline_beg()
	{
		// Arrays gl_mline* bereitstellen
		if (!glvar->gl_mline_vp || glvar->gl_mline_cap < glvar->akt_defnum) {
            // Arrays gleich etwas mehr vergroessern, damit nicht fuer jede neue Variable einzeln
			t_addr cap, inc;
			cap = glvar->gl_mline_cap;
			inc = (glvar->akt_defnum - cap) + cap * MIN_CAP_GROW;
			if (inc < MIN_CAP_ARRAY)
				inc = MIN_CAP_ARRAY;

			glvar->incmem((void **) &(glvar->gl_mline_vp), &cap, inc, sizeof(val_str));

			cap = glvar->gl_mline_cap;
			glvar->incmem((void **) &(glvar->gl_mline_fp), &cap, inc, sizeof(char));

			cap = glvar->gl_mline_cap;
			glvar->incmem((void **) &(glvar->gl_mline_fp_any), &cap, inc, sizeof(bool));
			memset(glvar->gl_mline_fp_any + glvar->gl_mline_cap, 0, inc * sizeof(bool));

			cap = glvar->gl_mline_cap;
			glvar->incmem((void **) &(glvar->gl_mline_num), &cap, inc, sizeof(int));
			glvar->gl_mline_cap = cap;
		}

		// gl_mline_fp auf 0 setzen
		memset(glvar->gl_mline_fp, 0, glvar->gl_mline_cap * sizeof(bool));

		glvar->gl_mline_cnt_var = 0;
		glvar->gl_mline_cnt_not0 = 0;

		glvar->fonds_s_line.t = TP_INT;
		glvar->fonds_s_line.v.i = 0;

		glvar->is_left_side = true;

#ifdef VAR_PROD
		*(glvar->name_pvarb_line) = '\0';
#endif
	}

	/* Ende Matrixzeile, Zeile in Matrix speichern */
	int IntFuncLineBase::fline_end()
	{
		int n, m;
		dest_str *dp, *dp1;
		l_elem_str *lep;
		t_addr mem_alloc_vp;

		//glvar->gl_mline_vp = (val_str *) glvar->c_getmem(glvar->akt_defnum*(sizeof(val_str)+sizeof(bool)));
		//mem_alloc_vp = glvar->mem_alloc_akt - 1;
		//glvar->gl_mline_fp = (bool *)(glvar->gl_mline_vp + glvar->akt_defnum);
		//glvar->gl_mline_anz = 0;
		//forall_var(&cmpl::IntFuncLineBase::flend_var);

		dp = (dest_str *) glvar->c_getmem(sizeof(dest_str) + glvar->gl_mline_cnt_not0 * sizeof(l_elem_str));
		if(!glvar->dest_grd) {
			glvar->old_dest_akt = (dest_str *)0;
			glvar->dest_grd = dp;
		}
		else {
			glvar->old_dest_akt = glvar->dest_akt;
			glvar->dest_akt->d_nextline = dp;
		}

		if(glvar->line_vekt_bl.lv_is_vekt && glvar->line_vekt_bl.lv_is_floop)
								flbeg_lname(glvar->name_s_line, glvar->org_name_s_line, false);

		// doch nicht auf Namensgleichheit testen, da sehr aufwendig
		/*
		for(dp1=glvar->dest_grd; dp1!=dp; dp1 = dp1->d_nextline) {
			if(strcmp(dp1->d_name, glvar->name_s_line) == 0) {
				sprintf(glvar->name_s_line, SHORT_NAME_LINE, glvar->akt_s_line);
				snprintf(glvar->errbuf, ERRBUFLEN, ERROR_LINE_NAME_DUP_REPL, dp1->d_name, glvar->name_s_line);
				glvar->errbuf[ERRBUFLEN] = '\0';
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_WARN, (char *)0, glvar->errbuf);
				break;
			}
		}
		*/

		dp->d_lineno = glvar->akt_s_line;
		strcpy(dp->d_name, glvar->name_s_line);
		dp->d_mode = glvar->mode_s_line;
		dp->d_line_obj_to = glvar->akt_line_obj_to;
		dp->d_fonds = glvar->fonds_s_line;
		dp->d_anz_val = glvar->gl_mline_cnt_not0;
		dp->d_valp = (l_elem_str *)(dp + 1);
		dp->d_nextline = (dest_str *)0;
#ifdef RANGE
		dp->d_is_range = false;
#endif
		glvar->dest_akt = dp;

		//for(n=0, lep=dp->d_valp; n<glvar->akt_defnum; n++) if(glvar->gl_mline_fp[n]) {
		//	lep->le_col = n;
		//	lep->le_val = glvar->gl_mline_vp[n];
		//	lep++;
		//}
		//glvar->freemem(mem_alloc_vp);
		
		// Verwendete Variablen sortieren, damit in Definitionsreihenfolge
		qsort(glvar->gl_mline_num, glvar->gl_mline_cnt_var, sizeof(int), &cmpl::IntFuncLineBase::cmp_int);

		for (n = 0, lep = dp->d_valp; n < glvar->gl_mline_cnt_var; n++) {
			m = glvar->gl_mline_num[n];
			if (glvar->gl_mline_fp[m] == 2) {
				lep->le_col = m;
				lep->le_val = glvar->gl_mline_vp[m];
				lep++;
			}
		}


        if(glvar->args->isNoReduct() || ftest_dest()) {
			//forall_var(&cmpl::IntFuncLineBase::ftouch_not0);
			for (n = 0, lep = dp->d_valp; n < glvar->gl_mline_cnt_var; n++) {
				m = glvar->gl_mline_num[n];
				if (glvar->gl_mline_fp[m] == 2)
					glvar->gl_mline_fp_any[m] = true;
			}

			glvar->akt_s_line++;
		}
		else {
			/* Zeile eingespart */
			/*free(glvar->dest_akt);*/
			if(glvar->old_dest_akt) {
				glvar->old_dest_akt->d_nextline = (dest_str *)0;
				glvar->dest_akt = glvar->old_dest_akt;
			}
			else glvar->dest_grd = (dest_str *)0;
		}

		if(glvar->line_vekt_bl.lv_is_vekt) {
			PROTO_OUT_2("\n  vector %d", glvar->line_vekt_bl.iter_set.cur_ind+1);

			glvar->line_vekt_bl.lv_is_floop = false;
			if (glvar->line_vekt_bl.iter_set.iter_set_next())
			{
				glvar->int_ptr = glvar->line_vekt_bl.lv_beg_line;
				flbeg_lname(glvar->name_s_line, glvar->org_name_s_line, false);
				fline_beg();

#ifdef PROTO
				if (glvar->line_vekt_bl.iter_set.cur_val.t == TP_ISTR) {
					PROTO_OUT_4("\n  start vector constraint '%s': %s (from %d)",
						glvar->strings.strings_ind[glvar->line_vekt_bl.iter_set.cur_val.v.i], glvar->name_s_line, glvar->int_ptr);
				}
				else {
					PROTO_OUT_4("\n  start vector constraint %d: %s (from %d)",
						glvar->line_vekt_bl.iter_set.cur_val.v.i, glvar->name_s_line, glvar->int_ptr);
				}
#endif
				return(1);
			}
		}

		return(0);
	}

	/* Test auf moegliche Umwandlung der aktuellen Zeile */
	int IntFuncLineBase::ftest_dest()
	{
		/* potentielle Zielfunktionen auf jeden Fall in Matrix */
		if(glvar->dest_akt->d_mode==MODE_N) return(1);

		/* Bei keinem Koeffizient kann Zeile entfallen */
		if(glvar->dest_akt->d_anz_val==0) return(0);

#ifdef UPPER_BOUND
		/* Bei nur einem Koeffizient Umwandlung Restriktion in Schranke */
		if(glvar->dest_akt->d_anz_val==1) {
			double v1, v2, v3, vg;
			int i;
			bool is_neg, f1;
			val_str *valp;
			var_str *varp;
			valp = &(glvar->dest_akt->d_fonds);
			f1 = (valp->t != TP_DBL);
			v1 = DBL_VAL(*valp);
			valp = &(glvar->dest_akt->d_valp->le_val);
			if(valp->t != TP_DBL) {
				is_neg = (i = valp->v.i) < 0 ? true : false;
				if(i==1 || i==-1) v3 = is_neg ? -v1 : v1;
				else {
					v3 = v1 / i;
					f1 = false;
				}
			}
			else {
				is_neg = (v2 = valp->v.f) < 0. ? true : false;
				f1 = false;
				v3 = v1 / v2;
			}
			varp = fvarp_defn(glvar->dest_akt->d_valp->le_col);
			if(glvar->dest_akt->d_mode == MODE_G) {
				fset_dn_varb(varp, v3, f1);
				fset_up_varb(varp, v3, f1);
			}
			else {
				if((!is_neg && glvar->dest_akt->d_mode==MODE_O) ||
								(is_neg && glvar->dest_akt->d_mode==MODE_U)) 
					fset_up_varb(varp, v3, f1);
				else
					fset_dn_varb(varp, v3, f1);
			}
			return(0);
		}
#endif /* UPPER_BOUND */

		/* Test auf Zeilen mit aequivalenten Koeffizienten */
		{
			dest_str *dp;
			val_str *vp1, *vp2;
			l_elem_str *le1, *le2;
			double fakt, d, d1, d2;
			int n;
			int vn;
			for(dp=glvar->dest_grd; dp!=glvar->dest_akt; dp = dp->d_nextline) {
				if(dp->d_mode == MODE_N) continue;
				vn = 0;
#ifndef RANGE
				if(dp->d_is_range) {
					vn = 1;
				}
#endif 

				if(dp->d_anz_val-vn != glvar->dest_akt->d_anz_val ||
					dp->d_valp->le_col != glvar->dest_akt->d_valp->le_col) continue;

				le1 = dp->d_valp;
				le2 = glvar->dest_akt->d_valp;
				vp1 = &(le1->le_val);
				vp2 = &(le2->le_val);
				fakt = DBL_VAL(*vp1) / DBL_VAL(*vp2);

				for(n=1,le1++,le2++; n<glvar->dest_akt->d_anz_val; n++,le1++,le2++) {
					if(le1->le_col != le2->le_col) break;
					vp1 = &(le1->le_val);
					vp2 = &(le2->le_val);
					d = DBL_VAL(*vp1) / DBL_VAL(*vp2);
					if(!IS_EQ(fakt, d)) break;
				}
				if(n==glvar->dest_akt->d_anz_val) break;
			}
			if(dp != glvar->dest_akt) {	/* proportionale Zeile gefunden */
                                PROTO_OUT_4("\n   proportional constraints: %s # %s (multiplied by :%f)",
						glvar->dest_akt->d_name, dp->d_name, fakt);

				return(fset_lineb(dp, glvar->dest_akt, fakt));
			}
			else return(1);
		}
	}

	/* Setzen untere Variablenschranken */
	void IntFuncLineBase::fset_dn_varb(var_str *vp, double bval, bool f)
	{
		double v1;
		if(vp->v_is_up_b) {
			v1 = DBL_VAL(vp->v_up_b);
			if(bval > v1 && ! IS_EQ(bval, v1)) {
				sprintf(glvar->errbuf, ERROR_BOUND_DN_UP_COMP, bval, v1);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
		}
		if(vp->v_is_dn_b) {
			v1 = DBL_VAL(vp->v_dn_b);
			if(bval > v1) {
				vp->v_dn_b.t = (f ? TP_INT : TP_DBL);
				if(f) vp->v_dn_b.v.i = RUND(bval);
				else vp->v_dn_b.v.f = bval;
			}
		}
		else {
			vp->v_is_dn_b = true;
			vp->v_dn_b.t = (f ? TP_INT : TP_DBL);
			if(f) vp->v_dn_b.v.i = RUND(bval);
			else vp->v_dn_b.v.f = bval;
		}
	}

	/* Setzen obere Variablenschranken */
	void IntFuncLineBase::fset_up_varb(var_str *vp, double bval, bool f)
	{
		double v1;
		if(vp->v_is_dn_b) {
			v1 = DBL_VAL(vp->v_dn_b);
			if(bval < v1 && ! IS_EQ(bval, v1)) {
				sprintf(glvar->errbuf, ERROR_BOUND_UP_DN_COMP, bval, v1);
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
			}
		}
		if(vp->v_is_up_b) {
			v1 = DBL_VAL(vp->v_up_b);
			if(bval < v1) {
				vp->v_up_b.t = (f ? TP_INT : TP_DBL);
				if(f) vp->v_up_b.v.i = RUND(bval);
				else vp->v_up_b.v.f = bval;
			}
		}
		else {
			vp->v_is_up_b = true;
			vp->v_up_b.t = (f ? TP_INT : TP_DBL);
			if(f) vp->v_up_b.v.i = RUND(bval);
			else vp->v_up_b.v.f = bval;
		}
	}

	/* Zusammenfassen proportionale Zeilen */
	int IntFuncLineBase::fset_lineb(dest_str *lp, dest_str *lp1, double fakt)
	{
		double f, f1, fs;
		int mod, mod1;
		int rc = 0;
		val_str v1;

		f = DBL_VAL(lp->d_fonds);
		f1 = DBL_VAL(lp1->d_fonds) * fakt;
		v1.t = lp1->d_fonds.t;
		mod = lp->d_mode;
		mod1 = lp1->d_mode;
		if(fakt < 0.) {
			if(mod1==MODE_U) mod1 = MODE_O;
			else if(mod1==MODE_O) mod1 = MODE_U;
		}
		if(v1.t != TP_DBL) v1.v.i = RUND(f1);
		else v1.v.f = f1;

		if(lp->d_is_range) {		/* bereits beidseitig beschraenkt */
			if(mod1==MODE_G) {
				rc = make_lrange(lp, v1, v1, true, true);
			}
			else if(mod1==MODE_U) {
				rc = make_lrange(lp, v1, v1, true, false);
			}
			else {
				rc = make_lrange(lp, v1, v1, false, true);
			}
		}
		else {
			if(mod==MODE_G || mod1==MODE_G) {
				if(mod!=MODE_G) {
					lp->d_fonds = v1;
					mod1 = mod;
					lp->d_mode = MODE_G;
					fs = f;
					f = f1;
					f1 = fs;
				}
				if((mod1==MODE_G && !IS_EQ(f, f1)) ||
				   (mod1==MODE_O && f > f1) ||
				   (mod1==MODE_U && f < f1)) {
					snprintf(glvar->errbuf, ERRBUFLEN, ERROR_BOUND_CONSTRAINT, lp->d_name);
					glvar->errbuf[ERRBUFLEN] = '\0';
					FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				}
			}
			else {
				if(mod==MODE_U && mod1==MODE_U) {
					if(f1 > f) lp->d_fonds = v1;
				}
				else if(mod==MODE_O && mod1==MODE_O) {
					if(f1 < f) lp->d_fonds = v1;
				}
				else {
					if(mod==MODE_U)
						rc = make_lrange(lp, lp->d_fonds, v1, true, true);
					else
						rc = make_lrange(lp, v1, lp->d_fonds, true, true);
				}
			}
		}
		return(rc);
	}

	/* Bereich aus unterer und oberer Schranke fuer Zeile */
	int IntFuncLineBase::make_lrange(dest_str *lp, val_str uv, val_str ov, bool nu, bool no)
	{
		double ub = DBL_VAL(uv);
		double ob = DBL_VAL(ov);
		double rg;
		val_str *valp;
		bool uf = (uv.t != TP_DBL);
		bool of = (ov.t != TP_DBL);

		if(lp->d_is_range) {
#ifdef RANGE
			double o_ob = DBL_VAL(lp->d_fonds);
			double o_ub = o_ob - DBL_VAL(lp->d_range);
#else
			double o_ub, o_ob;
			valp = &(lp->d_range_vp->v_dn_b);
			o_ub = DBL_VAL(*valp);
			valp = &(lp->d_range_vp->v_up_b);
			o_ob = DBL_VAL(*valp);
#endif /* RANGE */
			if(!nu) {
				ub = o_ub;
#ifdef RANGE
				uf = lp->d_fonds.t != TP_DBL && lp->d_range.t != TP_DBL;
#else
				uf = lp->d_range_vp->v_dn_b.t != TP_DBL;
#endif
			}
			if(!no) {
				ob = o_ob;
#ifdef RANGE
				of = lp->d_fonds.t != TP_DBL;
#else
				of = lp->d_range_vp->v_up_b.t != TP_DBL;
#endif
			}
			if(ub <= o_ub && ob >= o_ob) return(0);
		}

               PROTO_OUT_4("\n   constraint %s : range %f - %f", lp->d_name, ub, ob);

		rg = ob - ub;
		if(IS_EQ(rg, 0.)) {
			lp->d_mode = MODE_G;
			if(lp->d_is_range) {
				lp->d_is_range = false;
#ifndef RANGE
				glvar->gl_mline_fp_any[lp->d_range_vp->v_defnum] = false;
				lp->d_anz_val--;
#endif /* RANGE */
			}
			return(0);
		}

		if(rg < 0.) {
			snprintf(glvar->errbuf, ERRBUFLEN, ERROR_BOUND_CONSTRAINT, lp->d_name);
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
		}

#ifdef RANGE
		if(no) {
			lp->d_mode = MODE_O;
			lp->d_fonds = ov;
		}
		lp->d_is_range = true;
		if(of && uf) {
			lp->d_range.t = TP_INT;
			lp->d_range.v.i = RUND(rg);
		}
		else {
			lp->d_range.t = TP_DBL;
			lp->d_range.v.f = rg;
		}
#else
#ifdef UPPER_BOUND
		/* Range durch Variable nachbilden */
		if(lp->d_is_range) {
			if(nu) lp->d_range_vp->v_dn_b = uv;
			if(no) lp->d_range_vp->v_up_b = ov;
		}
		else {
			char str[SYM_NAME_LEN+200];
			int n;
			int ip, ind, anz;
			var_str *vp;
			dest_str *dp;
			dest_str *akt_p;

			/* neue Variable anlegen */
			//TODO: Symbolverwaltung
			snprintf(str, SYM_NAME_LEN-1, MAKE_NAME_RANGE);
			str[SYM_NAME_LEN-1] = '\0';
			ip = search_symbol(str, &ind);
			if(ip == -1) {
				bool found;
				const char *s = glvar->symbol_main_names.elem(glvar->symbol_main_names.search_insert(glvar, str, true, &found));
				ip = insert_symbol(s, TPK_VAR);
				ind = ip;
			}
			else if(ip < 0 || glvar->symbol_table[ind].tp_sym != TPK_VAR) {
				snprintf(glvar->errbuf, ERRBUFLEN, (ip<0 ? ERROR_DEF_RVAR_KONST : ERROR_DEF_RVAR_LK), str);
				glvar->errbuf[ERRBUFLEN] = '\0';
				FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, glvar->errbuf);
				return(1);
			}
			glvar->symbol_akt_p = glvar->symbol_table + ind;
			glvar->ind_inf_ptr->i_anz_ind = 1;
			glvar->ind_inf_ptr->i_ind[0].cur_val = lp->d_lineno;
			glvar->ind_inf_ptr->i_ind[0].cur_tp = TP_INT;
			make_test_var(glvar->symbol_akt_p, 1, &vp);

			vp->v_type = TP_VAR_DBL;
#ifdef VAR_PROD
			vp->v_otype = TP_VAR_DBL;
#endif
			vp->v_defnum = glvar->akt_defnum++;

			// Arrays gl_mline* muessen vergroessert werden, da akt_defnum geaendert
			if (!glvar->gl_mline_vp || glvar->gl_mline_cap < glvar->akt_defnum) {
				// Arrays gleich etwas mehr vergroessern, damit nicht fuer jede neue Variable einzeln
				t_addr cap, inc;
				cap = glvar->gl_mline_cap;
				inc = (glvar->akt_defnum - cap) + cap * MIN_CAP_GROW;
				if (inc < MIN_CAP_ARRAY)
					inc = MIN_CAP_ARRAY;

				glvar->incmem((void **) &(glvar->gl_mline_vp), &cap, inc, sizeof(val_str));

				cap = glvar->gl_mline_cap;
				glvar->incmem((void **) &(glvar->gl_mline_fp), &cap, inc, sizeof(char));

				cap = glvar->gl_mline_cap;
				glvar->incmem((void **) &(glvar->gl_mline_fp_any), &cap, inc, sizeof(bool));
				memset(glvar->gl_mline_fp_any + glvar->gl_mline_cap, 0, inc * sizeof(bool));

				cap = glvar->gl_mline_cap;
				glvar->incmem((void **) &(glvar->gl_mline_num), &cap, inc, sizeof(int));
				glvar->gl_mline_cap = cap;
			}

			glvar->gl_mline_fp_any[vp->v_defnum] = true;
			vp->v_is_dn_b = true;
			vp->v_dn_b = uv;
			vp->v_is_up_b = true;
			vp->v_up_b = ov;

			/* Zeile muss neu angelegt und kopiert werden */
			anz = lp->d_anz_val + 1;
			dp = (dest_str *) glvar->c_getmem(sizeof(dest_str) + anz*sizeof(l_elem_str));
			*dp = *lp;

			dp->d_anz_val = anz;
			dp->d_is_range = true;
			dp->d_range_vp = vp;
			dp->d_mode = MODE_G;
			dp->d_fonds.t = TP_INT;
			dp->d_fonds.v.i = 0;
			dp->d_valp = (l_elem_str *)(dp+1);
			for(n=0; n<anz-1; n++) dp->d_valp[n] = lp->d_valp[n];
			dp->d_valp[n].le_col = vp->v_defnum;
			dp->d_valp[n].le_val.t = TP_INT;
			dp->d_valp[n].le_val.v.i = -1;

			if(glvar->dest_grd == lp) glvar->dest_grd = dp;
			else {
				for(akt_p=glvar->dest_grd; akt_p; akt_p = akt_p->d_nextline)
										if(akt_p->d_nextline == lp) break;
				akt_p->d_nextline = dp;
			}
			if(glvar->old_dest_akt == lp) glvar->old_dest_akt = dp;
			/*free(lp);*/

#ifdef PROTO
			if(glvar->is_protokoll) {
				FILEIO->ferror_symname(glvar->errbuf, false, ERRBUFLEN);
				PROTO_OUT_2("\n    variable %s for range defined", glvar->errbuf);
			}
#endif
		}
#else
		return(1);
#endif /* UPPER_BOUND */
#endif /* RANGE */
		return(0);
	}

	/* Variable zu Definitionsnummer suchen */
	var_str* IntFuncLineBase::fvarp_defn(int num)
	{
		glvar->gl_mline_defnum = num;
		forall_var(&cmpl::IntFuncLineBase::ffvarp_defn);
		return(glvar->gl_mline_varptr);
	}


	//********************************************************
	// IntFuncLineBeg
	//********************************************************

	/* Beginn Matrixzeile */
	void IntFuncLineBeg::exec(val_str *vp)
	{
		if (vp[1].t != TP_STR)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
		}
		else
		{
			char *str = vp[1].v.s;
			PROTO_OUT_2("  %s", str);

			strncpy(glvar->org_name_s_line, str, ZC_NAME_LEN-1);
			glvar->org_name_s_line[ZC_NAME_LEN-1] = '\0';
			flbeg_lname(glvar->name_s_line, glvar->org_name_s_line, false);
			glvar->beg_akt_line = glvar->int_ptr;
			glvar->akt_line_side = 0;
			glvar->akt_line_obj_to = 0;
			glvar->line_vekt_bl.lv_is_floop = true;
			fline_beg();

			glvar->line_vekt_bl.lv_beg_line = glvar->int_ptr;
			glvar->line_vekt_bl.lv_is_vekt = false;
		}
	}


	//********************************************************
	// IntFuncLineMod
	//********************************************************

	/* Zeilenmodus (O, U, G, N) setzen */
	void IntFuncLineMod::exec(val_str *vp)
	{
		int i = vp->v.c.par;
		PROTO_OUT_2("  %d", i);

		if(!glvar->is_left_side) {			/* mehrfache Relation in einer Zeile */
			if(fline_end()) return;

			glvar->int_ptr = glvar->beg_akt_line;
			glvar->akt_line_side++;
			glvar->line_vekt_bl.lv_is_floop = true;
			glvar->line_vekt_bl.lv_beg_line = glvar->int_ptr;
			glvar->line_vekt_bl.lv_is_vekt = false;
			flbeg_lname(glvar->name_s_line, glvar->org_name_s_line, false);
			fline_beg();

			PROTO_OUT_3("\n  start continued constraint %s (from %d)", glvar->name_s_line, glvar->int_ptr);
		}
		else {
			glvar->mode_s_line = i;
			glvar->is_left_side = false;
			glvar->beg_akt_line = glvar->int_ptr;
		}
	}


	//********************************************************
	// IntFuncLineObjTo
	//********************************************************

	/* Kennzeichen setzen, dass Zeile Zielfunktion mit Optimierungsrichtung (i==1:max, i==-1:min) */
	void IntFuncLineObjTo::exec(val_str *vp)
	{
		int i = vp->v.c.par;
		PROTO_OUT_2("  %d", i);
		/* Hier erstmal nur Kennzeichen fuer die aktuelle Zeile setzen */
		glvar->akt_line_obj_to = i;
	}


	//********************************************************
	// IntFuncLineEnd
	//********************************************************

	/* Ende Matrixzeile, Zeile in Matrix speichern */
	void IntFuncLineEnd::exec(val_str *vp)
	{
		fline_end();
	}


	//********************************************************
	// IntFuncLineEnd
	//********************************************************

	/* Beginn Variablenlinearisierung */
	void IntFuncVarLinBeg::exec(val_str *vp)
	{
#ifdef VAR_PROD
		glvar->pvarb_s_line = glvar->akt_s_line;
#else
		FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
#endif
	}

}

