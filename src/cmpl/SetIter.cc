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


#include "CmplCore.hh"
#include "SetIter.hh"
#include "IntFuncBase.hh"


namespace cmpl
{
	//********************************************************
	// SetUtil
	//********************************************************
	
	/* Gibt die Anzahl der Elemente im Set */
	long SetUtil::set_len(val_str *setp)
	{
		if (SET_IS_EMPTY(setp->t))
			return 0;
		if (SET_IS_INFINITE(setp->t))
			return LONG_MAX;	/* Ersatzweise fuer unendlichen Set */

		if (SET_USE_POINTER(setp->t))
			return setp->v.set->len;
		if (setp->t == TP_SET_ALG1_1)
			return setp->v.i;
		if (setp->t == TP_SET_ALG0_1)
			return setp->v.i + 1;

		if (SET_USE_ARRAY(setp->t))
		{
			array_str *arr = setp->v.array;
			if (arr->defset.t != TPP_EMPTY)
				return set_len(&(arr->defset));
			else
				return (arr->a_dir.cnt + arr->a_num.cnt + arr->a_str.cnt);
		}

		/* Hierin sollte Ausfuehrung nie kommen */
		return -1;
	}

	/* Gibt die Anzahl der Elemente im Set von einem Typ (darf nur TP_INT oder TP_ISTR sein) */
	long SetUtil::set_len_tp(val_str *setp, enum etp tp)
	{
		if (SET_IS_EMPTY(setp->t) || (tp == TP_INT && !SET_CAN_NUMBERS(setp->t)) || (tp != TP_INT && !SET_CAN_STRINGS(setp->t)))
			return 0;
		if (SET_IS_INFINITE(setp->t))
			return LONG_MAX;	/* Ersatzweise fuer unendlichen Set */

		if ((tp == TP_INT && !SET_CAN_STRINGS(setp->t)) || (tp != TP_INT && !SET_CAN_NUMBERS(setp->t)))
			return set_len(setp);

		/* Set kann hier int und string enthalten, ist also entweder TP_SET_ENUM oder TP_SET_SYM_ARR */
		if (SET_USE_ARRAY(setp->t))
		{
			array_str *arr = setp->v.array;
			if (arr->defset.t != TPP_EMPTY)
				return set_len_tp(&(arr->defset), tp);
			else
				return (tp == TP_INT ? (arr->a_dir.cnt + arr->a_num.cnt) : arr->a_str.cnt);
		}
		else
		{
			set_ea_str *ep = setp->v.set;
			if (tp == TP_INT)
				return ep->u.e.string_start;
			else
				return ep->len - ep->u.e.string_start;
		}
	}

	/* Gibt die Anzahl der Elemente des Durchschnitts zweier Sets */
	long SetUtil::set2_len(val_str *set1p, val_str *set2p)
	{
		bool isnum, isstr;
		long len1, len2;
		val_str *tst;
		long ret;

		/* Triviale Fälle */
		if (SET_IS_EMPTY(set1p->t) || SET_IS_EMPTY(set2p->t))
			return 0;
		if (set1p->t == TP_SET_ALL)
			return set_len(set2p);
		if (set2p->t == TP_SET_ALL)
			return set_len(set1p);

		isnum = (SET_CAN_NUMBERS(set1p->t) && SET_CAN_NUMBERS(set2p->t));
		isstr = (SET_CAN_STRINGS(set1p->t) && SET_CAN_STRINGS(set2p->t));
		if (!isnum && !isstr)
			return 0;

		/* Beide Sets sind unendlich */
		if (SET_IS_INFINITE(set1p->t) && SET_IS_INFINITE(set2p->t))
		{
			if (set1p->t == set2p->t || set1p->t == TP_SET_ALL_NUM || set1p->t == TP_SET_ALL_STR || set2p->t == TP_SET_ALL_NUM || set2p->t == TP_SET_ALL_STR)
			{
				return LONG_MAX;
			}
			else
			{
				/* Ein Set ist TP_SET_HALF_LB, der andere TP_SET_HALF_UB */
				int diff = (set1p->t == TP_SET_HALF_UB ? set1p->v.i : set2p->v.i) - (set1p->t == TP_SET_HALF_LB ? set1p->v.i : set2p->v.i);
				return (diff >= 0 ? diff + 1 : 0);
			}
		}

		/* Mindestens ein Set ist endlich: Set mit weniger Elementen durchgehen und jeweils pruefen, ob Element auch im anderen Set */
		len1 = set_len(set1p);
		len2 = set_len(set2p);
		SetIterator iter(glvar, (len1 <= len2 ? set1p : set2p));

		tst = (len1 <= len2 ? set2p : set1p);
		ret = 0;

		iter.iter_set_start(true);
		while (!iter.set_end)
		{
			if ((isnum && iter.cur_val.t == TP_INT) || (isstr && iter.cur_val.t == TP_ISTR))
			{
				if (set_test_elem(tst, &(iter.cur_val)))
					ret++;
			}
			iter.iter_set_next();
		}

		return ret;
	}

	/* Testet ob ein Indexwert Element eines Sets ist */
	bool SetUtil::set_test_elem(val_str *setp, val_str *ind)
	{
		bool found;

		if (setp->t == TP_SET_ALL || (setp->t == TP_SET_ALL_NUM && ind->t == TP_INT) || (setp->t == TP_SET_ALL_STR && ind->t == TP_ISTR))
			return true;
		if ((ind->t == TP_INT && !SET_CAN_NUMBERS(setp->t)) || (ind->t == TP_ISTR && !SET_CAN_STRINGS(setp->t)))
			return false;

		if (SET_IS_INFINITE(setp->t))
		{
			/* Kann hier nur noch TP_SET_HALF_LB oder TP_SET_HALF_UB sein, und Index ist TP_INT */
			if (setp->t == TP_SET_HALF_LB)
				return (ind->v.i >= setp->v.i);
			else
				return (ind->v.i <= setp->v.i);
		}

		if (!SET_CAN_STRINGS(setp->t))
		{
			/* Kann hier nur noch TP_SET_ALG0_1, TP_SET_ALG1_1 oder TP_SET_ALG sein, und Index ist TP_INT */
			if (setp->t == TP_SET_ALG0_1)
			{
				return (ind->v.i <= setp->v.i && ind->v.i >= 0);
			}
			else if (setp->t == TP_SET_ALG1_1)
			{
				return (ind->v.i <= setp->v.i && ind->v.i >= 1);
			}
			else
			{
				set_ea_str *eap = setp->v.set;
				if ((eap->u.a.step_val > 0 && (ind->v.i < eap->u.a.start_val || ind->v.i > eap->u.a.end_val)) ||
						(eap->u.a.step_val < 0 && (ind->v.i > eap->u.a.start_val || ind->v.i < eap->u.a.end_val)))
					return false;

				if (eap->u.a.step_val == 1 || eap->u.a.step_val == -1)
				{
					return true;
				}
				else
				{
					int diff = (eap->u.a.step_val > 0 ? (ind->v.i - eap->u.a.start_val) : (eap->u.a.start_val - ind->v.i));
					int step = (eap->u.a.step_val > 0 ? eap->u.a.step_val : -eap->u.a.step_val);
					return (diff % step == 0);
				}
			}
		}

		/* Kann hier nur noch TP_SET_ENUM und TP_SET_SYM_ARR sein */
		IntFuncBaseUtil search(glvar);		// Stellt Funktionen fuer Suche in Arrays bereit
		if (SET_USE_ARRAY(setp->t))
		{
			array_str *arr = setp->v.array;
			if (arr->defset.t != TPP_EMPTY)
				return set_test_elem(&(arr->defset), ind);
			else
			{
				/* Indexwert im Array suchen */
				search.search_ind_elem(arr, ind->v.i, ind->t, &found, (array_ins_info_str *)0);
				return found;
			}
		}
		else
		{
			/* Kann hier nur noch TP_SET_ENUM sein */
			set_ea_str *eap = setp->v.set;
			if ((ind->t == TP_INT && eap->u.e.string_start == 0) || (ind->t == TP_ISTR && eap->u.e.string_start == eap->len))
				return false;
			else
			{
				int *ind_arr = eap->u.e.index + (ind->t == TP_INT ? 0 : eap->u.e.string_start);
				int ind_len = (ind->t == TP_INT ? eap->u.e.string_start : (eap->len - eap->u.e.string_start));
				int found = search.search_int_sort_val_array(eap->u.e.array, ind_arr, ind_len, ind->v.i);
				return (found != -1 ? true : false);	
			}
		}
	}


	//********************************************************
	// SetIterator
	//********************************************************

	/* Beginnt Iteration ueber eine Menge (vorwaerts oder rueckwaerts) */
	bool SetIterator::iter_set_start(bool dirfwd)
	{
		/* Iteration direkt starten, ohne zweiten Set */
		set2.t = TPP_EMPTY;
		return iter_set_start_intern(&set, dirfwd);
	}

	/* Beginnt Iteration ueber den Durchschnitt zweier Sets (vorwaerts oder rueckwaerts) */
	bool SetIterator::iter_set2_start(val_str *set2p, bool dirfwd)
	{
		/* Set in iter_set ist der Set aus Index-Info; zweiter Set ist der Definitionsset des Arrays */
		val_str *order_set, *test_set;
		set2 = *set2p;
		
		/* Zweiter Set bestimmt die Reihenfolge nur, wenn erster Set unendlich ist */
		order2 = SET_IS_INFINITE(set.t);
		order_set = (order2 ? &set2 : &set);
		test_set = (order2 ? &set : &set2);

		/* Iteration auf dem Reihenfolge-bestimmenden Set starten */
		if (!iter_set_start_intern(order_set, dirfwd))
			return false;

		/* Wenn zweiter Set leer, dann jedenfalls doch keine Iteration */
		if (set_len(test_set) == 0)
		{
			set_end = true;
			return false;
		}

		/* Vorkommen in test_set pruefen, wenn dort nicht, solange iterieren, bis vorkommendes gefunden */
		while (!set_end)
		{
			if (set_test_elem(test_set, &cur_val))
				break;

			iter_set_next_intern(order_set);
		}
		
		return (!set_end);
	}

	/* Beginnt Iteration neu, mit derselben Richtung, und ohne/mit demselben zweiten Set */
	bool SetIterator::iter_set_restart()
	{
		if (set2.t == TPP_EMPTY)
			return iter_set_start_intern(&set, dir_fwd);
		else
			return iter_set2_start(&set2, dir_fwd);
	}

	/* Hilfsfunktion zum Setzen auf das erste Element fuer Iteration */
	bool SetIterator::iter_set_start_intern(val_str *setp, bool dirfwd)
	{
		long setlen;

		/* Iteration mit erstem Element starten */
		cur_ind = cur_ind1 = 0;
		dir_fwd = dirfwd;

		/* Wenn Definitionsset direkt gesetzt, dann verwenden */
		if (SET_USE_ARRAY(setp->t) && setp->v.array->defset.t != TPP_EMPTY)
			setp = &(setp->v.array->defset);

		/* Set darf nicht unendlich sein: Fehler und keine Iteration */
		if (SET_IS_INFINITE(setp->t))
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INFINITE_SET);
			set_end = true;
			return false;
		}

		/* Schrittweite 0 fuer algorithmischen Set ist nicht erlaubt */
		if (setp->t == TP_SET_ALG && setp->v.set->u.a.step_val == 0)
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_LOOP_NOEND);
			set_end = true;
			return false;
		}

		/* Leerer Set ist mit Iteration sofort am Ende */
		if ((setlen = set_len(setp)) == 0)
		{
			set_end = true;
			return false;
		}

		/* Wenn rueckwaerts zu iterieren, dann auf letztes Element */
		if (!dir_fwd)
			cur_ind = cur_ind1 = setlen - 1;

		/* Iteration starten */
		if (SET_USE_VALINT(setp->t))
		{
			/* Kann nur TP_SET_ALG0_1 oder TP_SET_ALG1_1 sein */
			cur_val.v.i = (dir_fwd ? (setp->t == TP_SET_ALG0_1 ? 0 : 1) : setp->v.i);
			cur_val.t = TP_INT;
		}
		else if (SET_USE_POINTER(setp->t))
		{
			/* Aktuelles Element setzen */
			if (setp->v.set->is_enum)
			{
				cur_val = setp->v.set->u.e.array[cur_ind];
			}
			else
			{
				cur_val.v.i = setp->v.set->u.a.start_val + cur_ind * setp->v.set->u.a.step_val;
				cur_val.t = TP_INT;
			}
		}
		else	/* Hier muss SET_USE_ARRAY gelten */
		{
			/* Ersten bzw. letzten Definitionsindex aus Array setzen */
			array_str *array = setp->v.array;
			if (dir_fwd)
			{
				array_ind_dir = (array->a_dir.cnt > 0 ? 0 : -1);
				array_ind_num = (array->a_num.cnt > 0 ? 0 : -1);
				array_ind_str = (array->a_str.cnt > 0 ? 0 : -1);
			}
			else
			{
				array_ind_dir = (array->a_dir.cnt > 0 ? (long)(array->a_dir.len-1) : -1);
				array_ind_num = (array->a_num.cnt > 0 ? (long)(array->a_num.len-1) : -1);
				array_ind_str = (array->a_str.cnt > 0 ? (long)(array->a_str.len-1) : -1);
			}

			if (!iter_find_next_array_set(array, false))
			{
				set_end = true;
				return false;
			}
		}

		set_end = false;
		return true;
	}

	/* Iteration auf naechstes Element einer Menge (nach iter_set_start() bzw. iter_set2_start() aufzurufen) */
	bool SetIterator::iter_set_next()
	{
		val_str *order_set = (set2.t == TPP_EMPTY || !order2 ? &set : &set2);
		val_str *test_set = (set2.t == TPP_EMPTY ? (val_str *)0 : (order2 ? &set : &set2));
		
		/* Iteration auf order_set ausfuehren */
		iter_set_next_intern(order_set);
		cur_ind += (dir_fwd ? 1 : -1);

		/* Wenn zweiter Set, dann Element testen, und solange weiter iterieren, bis passendes Element */
		if (test_set)
		{
			while (!set_end)
			{
				if (set_test_elem(test_set, &cur_val))
					break;

				iter_set_next_intern(order_set);
			}
		}

		return (!set_end);
	}

	/* Hilfsfunktion fuer Iteration auf naechstes Element einer Menge */
	void SetIterator::iter_set_next_intern(val_str *setp)
	{
		/* Wenn Definitionsset direkt gesetzt, dann verwenden */
		if (SET_USE_ARRAY(setp->t) && setp->v.array->defset.t != TPP_EMPTY)
			setp = &(setp->v.array->defset);

		if (SET_USE_VALINT(setp->t))
		{
			/* Kann nur TP_SET_ALG0_1 oder TP_SET_ALG1_1 sein */
			if (dir_fwd)
			{
				cur_ind1++;
				cur_val.v.i++;
				set_end = (cur_val.v.i > setp->v.i);
			}
			else
			{
				cur_ind1--;
				cur_val.v.i--;
				set_end = (cur_ind1 < 0);
			}
		}
		else if (SET_USE_POINTER(setp->t))
		{
			/* Auf naechstes Element iterieren */
			if (dir_fwd)
			{
				cur_ind1++;
				set_end = (cur_ind1 >= setp->v.set->len);
			}
			else
			{
				cur_ind1--;
				set_end = (cur_ind1 < 0);
			}

			/* Element setzen */
			if (setp->v.set->is_enum)
			{
				if (!set_end)
					cur_val = setp->v.set->u.e.array[cur_ind1];
			}
			else
			{
				cur_val.v.i = setp->v.set->u.a.start_val + cur_ind1 * setp->v.set->u.a.step_val;
			}
		}
		else if (SET_USE_ARRAY(setp->t))
		{
			cur_ind1 += (dir_fwd ? 1 : -1);
			if (!iter_find_next_array_set(setp->v.array, true))
				set_end = true;
		}
		else
		{
			/* Wenn doch fuer anderen Settyp aufgerufen (z.B. fuer leeren Set), dann als beendet zurueck */
			set_end = true;
		}
	}

	/* Sucht fuer Iteration über einen Array-Set das naechste nichtleere Element und setzt es in der Iteration; zurueck ob ein Element gefunden */
	bool SetIterator::iter_find_next_array_set(array_str *array, bool iter)
	{
		bool found = false;
		int stp = (dir_fwd ? 1 : -1);
		bool test_num;

		/* Noetigenfalls erst Element iterieren */
		if (iter)
		{
			if (cur_val.t == TP_INT)
			{
				if (cur_val.v.i == array_ind_dir)
				{
					array_ind_dir += stp;
					if (array_ind_dir >= array->a_dir.len || array_ind_dir < 0)
						array_ind_dir = -1;
				}
				else
				{
					array_ind_num += stp;
					if (array_ind_num >= array->a_num.len || array_ind_num < 0)
						array_ind_num = -1;
				}
			}
			else
			{
				array_ind_str += stp;
				if (array_ind_str >= array->a_str.len || array_ind_str < 0)
					array_ind_str = -1;
			}
		}

		/* Testet ob aktuelles Element leer, wenn nein dann gefunden, sonst zum naechsten */
		IntFuncBaseUtil funcBaseUtil(glvar);
		while (1)
		{
			if (dir_fwd)
				test_num = (array_ind_dir != -1 || array_ind_num != -1);
			else
				test_num = (array_ind_str == -1 && (array_ind_dir != -1 || array_ind_num != -1));

			if (test_num)
			{
				if (array_ind_num == -1 ||
						(dir_fwd && array_ind_dir <= array->a_num.index[array_ind_num]) ||
						(!dir_fwd && array_ind_dir >= array->a_num.index[array_ind_num]))
				{
					funcBaseUtil.ptr_symbol_elem(&(array->a_dir.array), array->attr.t, array_ind_dir, &found);
					if (found)
					{
						cur_val.t = TP_INT;
						cur_val.v.i = array_ind_dir;
						return true;
					}
					else
					{
						array_ind_dir += stp;
						if (array_ind_dir >= array->a_dir.len || array_ind_dir < 0)
							array_ind_dir = -1;
					}
				}
				else
				{
					funcBaseUtil.ptr_symbol_elem(&(array->a_num.array), array->attr.t, array_ind_num, &found);
					if (found)
					{
						cur_val.t = TP_INT;
						cur_val.v.i = array->a_num.index[array_ind_num];
						return true;
					}
					else
					{
						array_ind_num += stp;
						if (array_ind_num >= array->a_num.len || array_ind_num < 0)
							array_ind_num = -1;
					}
				}
			}
			else if (array_ind_str != -1)
			{
				funcBaseUtil.ptr_symbol_elem(&(array->a_str.array), array->attr.t, array_ind_str, &found);
				if (found)
				{
					cur_val.t = TP_ISTR;
					cur_val.v.i = array->a_str.index[array_ind_str];
					return true;
				}
				else
				{
					array_ind_str += stp;
					if (array_ind_str >= array->a_str.len || array_ind_str < 0)
						array_ind_str = -1;
				}
			}
			else
				break;
		}

		return false;
	}

	/* Setzt die Iteration auf ein bestimmtes Element einer Menge (nur fuer Iteration nur ueber einen Set), zurueck ob Wert in der Menge */
	bool SetIterator::iter_set_val(val_str *vp)
	{
		val_str val = *vp;

		/* Iteration darf nicht ueber zwei Sets sein, und kein undendlicher Set */
		if (set2.t != TPP_EMPTY || SET_IS_INFINITE(set.t))
		{
			FILEIO->output_error((io_type_str *)0, ERROR_TYPE_INT, ERROR_LVL_NORMAL, (char *)0, ERROR_INTERN_NO_FUNC);
			return false;
		}

		/* Element setzen */
		cur_val = *vp;

		/* Nur Elemente der Typen TP_INT und TP_ISTR koennen in einem Set enthalten sein */
		if (vp->t != TP_INT && vp->t != TP_ISTR)
		{
			if (vp->t == TP_BIN)
			{
				val.t = TP_INT;
			}
			else if (vp->t == TP_DBL && (double)((int)(vp->v.f)) == vp->v.f)
			{
				val.v.i = (int)(vp->v.f);
				val.t = TP_INT;
			}
			else if (vp->t == TP_STR)
			{
				if (!glvar->string_to_istr(&val, vp->v.s, false))
					free(vp->v.s);
			}
			else
				return false;
		}

		/* Test ob Element enthalten ist */
		if (!set_test_elem(&set, &val))
			return false;

		/* Bei algorithmischen Sets kann direkt festgestellt werden, auf das wievielte Element zu setzen ist */
		if (set.t == TP_SET_ALG || set.t == TP_SET_ALG0_1 || set.t == TP_SET_ALG1_1)
		{
			int ind;
			if (set.t == TP_SET_ALG0_1)
				ind = val.v.i;
			else if (set.t == TP_SET_ALG1_1)
				ind = val.v.i - 1;
			else
			{
				set_ea_str *eap = set.v.set;
				int diff = (eap->u.a.step_val > 0 ? (val.v.i - eap->u.a.start_val) : (eap->u.a.start_val - val.v.i));
				int step = (eap->u.a.step_val > 0 ? eap->u.a.step_val : -eap->u.a.step_val);
				ind = diff / step;
			}

			cur_ind = cur_ind1 = ind;

			return true;
		}

		/* Ansonsten neu durch den Set iterieren, bis das Element erreicht ist */
		iter_set_start(dir_fwd);
		while (!set_end)
		{
			if ((val.t == TP_INT && cur_val.t == TP_INT && val.v.i == cur_val.v.i) ||
					(val.t == TP_ISTR && cur_val.t == TP_ISTR && val.v.i == cur_val.v.i))
				return true;

			iter_set_next();
		}

		/* Doch nicht gefunden (sollte nicht vorkommen) */
		return false;
	}


	/* Anzahl Elemente in der Iteration */
	long SetIterator::iter_len()
	{
		if (set.t == TPP_EMPTY)
			return 0;

		if (set2.t == TPP_EMPTY)
			return set_len(&set);
		else
			return set2_len(&set, &set2);
	}
}

