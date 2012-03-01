/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale), 
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


/**
 * classes for tupels, tupelsets and iteration of tupelsets.
 */

#ifndef CMPL_TUPELSET_HH
#define CMPL_TUPELSET_HH

namespace cmpl
{
	class CmplCore;

	/****** tupel ****/

	/**
	 * a tupel is an array of int values (for int indizes and for string indizes).
	 * the first int contains the rank of the tupel and a flag whether the memory is globally allocated.
	 * the following ints contains the index values of the tupel.
	 * (only used for rank higher than 1. rank 0 has no index value, rank 1 is stored in val_str->v.i)
	 */
	typedef long* tupel_t;


	/**
	 * The <code>TupelUtil</code> class contains utility functions for tupels.
	 * It doesn't contain the tupel itself, so functions need a parameter for the tupel if necessary.
	 */
	class TupelUtil
	{
	public:
		/**
		 * returns the rank of the tupel.
		 */
		inline static int rank(tupel_t tup)						{ return (*tup & 0xff); }

		/**
		 * returns whether the memory for the tupel is globally allocated.
		 */
		inline static bool globAlloc(tupel_t tup)				{ return (*tup & 0x100 ? true : false); }

		/**
		 * constructs a tupel as a val_str value from an array of indizes.
		 * @param glvar		CmplCore object of CMPL
		 * @param rank		rank for the tupel
		 * @param arr		array with indizes or tupels, must match rank
		 * @param cnt		length of arr
		 * @param glmem		global allocation of the memory
		 * @param tup		pointer to store the constructed tupel in
		 */
		static void constructVal(CmplCore *glvar, int rank, val_str *arr, long cnt, bool glmem, val_str *tup);

		/**
		 * constructs a tupel as tupel_t from an array of indizes.
		 * @param glvar		CmplCore object of CMPL
		 * @param rank		rank for the tupel
		 * @param arr		array with indizes or tupels, must match rank
		 * @param cnt		length of arr
		 * @param glmem		global allocation of the memory
		 * @return			constructed tupel
		 */
		static tupel_t construct(CmplCore *glvar, int rank, val_str *arr, long cnt, bool glmem);

		/**
		 * gets the tupel that is represented by a val_str value.
		 * @param glvar		CmplCore object of CMPL
		 * @param vp		value
		 * @param glmem		global allocation of the memory if neccessary
		 * @return			tupel
		 */
		static tupel_t tupelFromVal(CmplCore *glvar, val_str *vp, bool glmem);

		/**
		 * converts a val_str value for an index (only TP_INT or TP_ISTR) to a simple int like used for storing tupels.
		 * (in the simple int the higher negative values are used for string indizes).
		 * no checks of the parameter here!
		 * @param vp		value as val_str (can be only TP_INT o TP_ISTR)
		 * @return			value for using in <code>tupel</code>
		 */
		inline static long valToTupelval(val_str *vp)			{ return (vp->t == TP_ISTR ? LONG_MIN / 2 - vp->v.i : vp->v.i); }

		/**
		 * converts a simple int like used for storing tupels to a val_str value for an index (only TP_INT or TP_ISTR).
		 * (in the simple int the higher negative values are used for string indizes).
		 * @param i			value used in <code>tupel</code>
		 * @param vp		pointer to store the value as val_str in
		 */
		inline static void tupelvalToVal(long i, val_str *vp)	{ if (i <= LONG_MIN / 2) { vp->t = TP_ISTR; vp->v.i = LONG_MIN / 2 - i; } else { vp->t = TP_INT; vp->v.i = i; } }

		/**
		 * changes the tupel to global memory allocation.
		 * @param glvar		CmplCore object of CMPL
		 * @param tup		tupel
		 */
		static void setToGlobAlloc(CmplCore *glvar, tupel_t tup);

	private:
		/**
		 * store index values for the tupel.
		 * @param glvar		CmplCore object of CMPL
		 * @param dest		pointer to the start for storing the index values
		 * @param arr		array with indizes or tupels
		 * @param cnt		length of arr
		 */
		static void storeInd(CmplCore *glvar, long *dest, val_str *arr, long cnt);
	};



	/****** tupelset ****/
	class TupelsetEnum;

	class Tupelset
	{
		friend class TupelsetEnum;

	private:
		/**
		 * array with indizes in the tupelset.
		 * elements can be scalar values (TP_INT or TP_ISTR), sets or tupelsets (then here <code>TupelsetEnum</code> instead of <code>Tupelset</code>)
		 */
		val_str *_ind;

		/**
		 * lenght of the array <code>_ind</code>.
		 * (this is not the rank of the tupelset, because elements of <code>_ind</code> can also be tupelsets with a rank higher than 1)
		 */
		int _cnt;

		/**
		 * count of scalar elements in <code>_ind</code>.
		 * (count of non-free indizes in the tupelset)
		 */
		int _fix;

		/**
		 * count of elements in the tupelset, or -1 if the tupelset is infinite.
		 * (this is the case if at least one element of <code>_ind</code> is an infinite set)
		 * (if the tupelset is empty (_len == 0) then it has no real rank. then as default _rank = 1 and _fix = 0 are used, and _cnt = 0.)
		 */
		long _len;

		/**
		 * rank of the tupelset.
		 * or -1 if the tupelset contains tupels of different ranks.
		 */
		int _rank;

		/**
		 * at least one element in <code>_ind</code> is a non-empty <code>TupelsetEnum</code>.
		 */
		bool _hasEnum;

		/**
		 * memory is global reserved and not to release up to the end.
		 */
		bool _globAlloc;

	public:
		/**
		 * returns the count of elements in the tupelset, or -1 if the tupelset is infinite.
		 */
		inline int len()			{ return _len; }

		/**
		 * returns the count of scalar indizes in the tupelset.
		 */
		inline int fix()			{ return _fix; }

		/**
		 * returns the rank of the tupelset or -1 if the tupelset contains tupels of different ranks.
		 */
		inline int rank()			{ return _rank; }

		/**
		 * returns the count of elements in _ind.
		 */
		inline int cnt()			{ return _cnt; }

		/**
		 * returns the the i-th element in _ind.
		 */
		inline val_str* ind(int i)	{ return _ind + i; }

		/**
		 * returns whether the tupelset is infinite.
		 */
		inline bool isInfinite()	{ return (_len < 0); }

		/**
		 * returns whether the tupelset is empty.
		 */
		inline bool isEmpty()		{ return (_len == 0); }

		/**
		 * returns whether the tupelset is a pure enumeration tupelset.
		 */
		inline bool isEnum()		{ return (_cnt == 1 && TP_IS_TUPELSET(_ind->t)); }

		/**
		 * returns whether the tupelset is a fix tupel. then _cnt, _rank and _fix are equal, and _len is 1.
		 */
		inline bool isTupel()		{ return (_cnt == _fix); }

		/**
		 * returns whether the tupelset has a certain rank (contains only tupels of that rank).
		 */
		inline bool hasRank()		{ return (_rank >= 0 && _len != 0); }

		/**
		 * returns whether at least one element in <code>_ind</code> is a non-empty <code>TupelsetEnum</code>.
		 */
		inline bool hasEnum()		{ return _hasEnum; }

		/**
		 * returns whether the memory for the tupel is globally allocated.
		 */
		inline bool globAlloc()		{ return _globAlloc; }


		/**
		 * constructs a tupelset as a val_str value from an array of indizes.
		 * @param glvar		CmplCore object of CMPL
		 * @param indLen	count for the array of indizes in the tupelset
		 * @param arr		array with indizes or tupels, must match indLen
		 * @param cnt		length of arr
		 * @param glmem		global allocation of the memory
		 * @param tup		pointer to store the constructed tupelset in
		 */
		static void constructValTupel(CmplCore *glvar, int indLen, val_str *arr, long cnt, bool glmem, val_str *tup);

		/**
		 * constructs a tupelset object from an array of indizes.
		 * @param glvar		CmplCore object of CMPL
		 * @param indLen	count for the array of indizes in the tupelset
		 * @param arr		array with indizes or tupels, must match rank
		 * @param cnt		length of arr
		 * @param glmem		global allocation of the memory
		 * @return			tupelset object
		 */
		static Tupelset* constructTupel(CmplCore *glvar, int indLen, val_str *arr, long cnt, bool glmem);

		/**
		 * constructs a tupelset as a val_str value from an enumeration of tupels.
		 * @param glvar		CmplCore object of CMPL
		 * @param arr		array with tupels
		 * @param cnt		length of arr
		 * @param glmem		global allocation of the memory
		 * @param tup		pointer to store the constructed tupelset in
		 */
		static void constructValEnum(CmplCore *glvar, val_str *arr, long cnt, bool glmem, val_str *tup);

		/**
		 * frees the memory for the tupelset tps.
		 * @param glvar		CmplCore object of CMPL
		 * @param tps		tupelset to free
		 */
		static void freeTupelset(CmplCore *glvar, Tupelset *tps);

		/**
		 * changes this object to global memory allocation.
		 * @param glvar		CmplCore object of CMPL
		 */
		void setToGlobAlloc(CmplCore *glvar);

	private:
		/**
		 * allocates memory for a tupelset object and initialize it.
		 * @param glvar		CmplCore object of CMPL
		 * @param indLen	count for the array of indizes in the tupelset
		 * @param glmem		global allocation of the memory
		 * @return			tupelset object
		 */
		static Tupelset* allocTupelset(CmplCore *glvar, int indLen, bool glmem);

		/**
		 * initialize a tupelset from an array of indizes.
		 * @param glvar		CmplCore object of CMPL
		 * @param arr		array with indizes or tupels, must match rank
		 * @param cnt		length of arr
		 */
		void initTupel(CmplCore *glvar, val_str *arr, long cnt);

		/**
		 * counts the tupels and tupelsets which are to copy to an enumeration tupelset.
		 * @param cnttps	return count tupelsets to copy
		 * @return			count of tupels to copy / 0: tupelset is empty / -1: tupelset is infinite
		 */
		int countTupelToEnum(int *cnttps);

		/**
		 * @param tupels		dest array of tupels
		 * @param tupelsets		dest array of tupelsets
		 * @param tpsOrder		dest array of order position numbers for tupelsets
		 * @param curTupel		current index position for tupels, back the position after copy
		 * @param curTps		current index position for tupelsets, back the position after copy
		 * @param freemem		free memory within this tupelset if it is not globally allocated, returns whether the tupelset is to free
		 */
		void copyTupelToEnum(tupel_t *tupels, Tupelset **tupelsets, int *tpsOrder, int *curTupel, int *curTps, bool *freemem);

		/**
		 * counts and copies the tupels from this tupelset to the arrays of a <code>TupelsetEnum</code>.
		 * @param atup		dest start position in the array of tupels to copy to / NULL: don't copy, only count
		 * @param atps		dest start position in the array of tupelsets to copy to / NULL: don't copy, only count
		 * @param aord		dest start position in the array of order numbers for tupelsets to copy to / NULL: don't copy, only count
		 * @param ord		start order number for tupelsets
		 * @param cnttps	return count of counted or copied tupelsets
		 * @param freemem	free memory within this tupelset if it is not globally allocated, returns whether the tupelset is to free
		 * @return			count of counted or copied tupels / 0: tupelset is empty / -1: tupelset is infinite
		 */
		int ccTupelToEnum(tupel_t *atup, Tupelset **atps, int *aord, int ord, int *cnttps, bool *freemem);
	};


	/**
	 * The <code>TupelsetEnum</code> class is used for an enumeration set of tupels.
	 */
	class TupelsetEnum
	{
		friend class Tupelset;

	private:
		/**
		 * array of tupels in iteration order.
		 */
		tupel_t *_tupels;

		/**
		 * array of tupelsets in iteration order.
		 * these tupelset cannot have any enumeration in it (_hasEnum == false for all these tupelsets).
		 */
		Tupelset **_tupelsets;

		/**
		 * array of iteration order positions of the tupelsets <code>_tupelsets</code> to sort they in <code>_tupels</code>.
		 * this array has the same length like <code>_tupelsets</code>.
		 */
		int *_tpsOrder;

		/**
		 * length of the array <code>_tupels</code>.
		 */
		int _cntTupels;

		/**
		 * length of the arrays <code>_tupelsets</code> and <code>_tpsOrder</code>.
		 */
		int _cntTupelsets;


		/**
		 * count of elements in the tupelset (cannot be infinite).
		 */
		long _len;

		/**
		 * rank of the tupelset.
		 * or -1 if the tupelset contains tupels of different ranks.
		 */
		int _rank;

		/**
		 * max rank of tupels in the tupelset.
		 * if _rank != -1, then _rank == _maxRank
		 */
		int _maxRank;

		/**
		 * count of elements in the tupelset for each rank.
		 * length of the array is <code>_maxRank</code>+1.
		 * only used if there are tupels of different ranks (_rank == -1).
		 */
		int *_lenRank;


		/**
		 * count of elements in the array <code>_tupels</code> for each rank.
		 * length of the array is <code>_maxRank</code>+1.
		 * only used if there are tupels of different ranks (_rank == -1). filled when first needed for a tupel search.
		 */
		int *_cntRankTupels;

		/**
		 * count of elements in the array <code>_tupelsets</code> for each rank.
		 * length of the array is <code>_maxRank</code>+1.
		 * only used if there are tupels of different ranks (_rank == -1). filled when first needed for a tupel search.
		 */
		int *_cntRankTupelsets;

		/**
		 * array of the tupels in sort order, each element is the index of the tupel in the array <code>_tupels</code>.
		 * length of the array is <code>_cntTupels</code>.
		 * only used if all tupels has the same rank (_rank != -1) and _cntTupels > 0. filled when first needed for a tupel search.
		 */
		int *_sort;

		/**
		 * array of the tupels in sort order, for each rank of tupels.
		 * element is an array of sorted tupel indizes to the array <code>_tupels</code>.
		 * length of the array is <code>_maxRank</code>+1, length of the element arrays are <code>_cntRankTupels</code>.
		 * only used if there are tupels of different ranks (_rank == -1) and _cntTupels > 0. filled when first needed for a tupel search.
		 */
		int **_sortRank;

		/**
		 * indizes of the tupelsets in the array <code>_tupelsets</code> for each rank.
		 * length of the array is <code>_maxRank</code>+1, length of the element arrays are <code>_cntRankTupelsets</code>.
		 * only used if there are tupels of different ranks (_rank == -1). filled when first needed for a tupel search.
		 */
		int **_tpsRank;

		/**
		 * memory is global reserved and not to release up to the end.
		 */
		bool _globAlloc;


	private:
		/**
		 * construct a TupelsetEnum object from the value at the value stack
		 * @param m			CmplCore object of CMPL
		 * @param arr		array with tupels
		 * @param cnt		length of arr
		 * @param glmem		global allocation of the memory
		 * @return			TupelsetEnum object
		 */
		static TupelsetEnum* construct(CmplCore *m, val_str *arr, long cnt, bool glmem);

		/**
		 * allocates memory for a TupelsetEnum object and initialize it.
		 * @param glvar		CmplCore object of CMPL
		 * @param tupcnt	count for the array of tupels
		 * @param tpscnt	count for the array of tupelsets
		 * @param rank		rank of the TupelsetEnum or -1 if it contains tupels of different ranks
		 * @param maxrank	max rank of tupels in the TupelsetEnum
		 * @param glmem		global allocation of the memory
		 * @return			TupelsetEnum object
		 */
		static TupelsetEnum* allocTupelsetEnum(CmplCore *glvar, int tupcnt, int tpscnt, int rank, int maxrank, bool glmem);

		/**
		 * initialize a TupelsetEnum from an array of tupels.
		 * @param glvar		CmplCore object of CMPL
		 * @param arr		array with tupels
		 * @param cnt		length of arr
		 */
		void initEnum(CmplCore *glvar, val_str *arr, long cnt);

		/**
		 * frees the memory for the tupelset tps.
		 * @param glvar		CmplCore object of CMPL
		 * @param tpe		tupelset to free
		 */
		static void freeTupelsetEnum(CmplCore *glvar, TupelsetEnum *tpe);

		/**
		 * changes this object to global memory allocation.
		 * @param glvar		CmplCore object of CMPL
		 */
		void setToGlobAlloc(CmplCore *glvar);

		/**
		 * gets the n-th tupel of the tupelset.
		 */
		//inline tupel_t operator[] (int n)		{ return _array + n * _rank; }
	};


	// Speicherung in TupelsetEnum:
	//		- besteht aus: 1.Aufzählung einzelner Tupel, 2.Tupelset's in denen KEINE Enumerationstupel und KEINE unendlichen Sets enthalten sind
	//		- gespeichert in einem Array für Aufzählungstupel und zweitem Array für Tupelsets
	//				- beide Arrays in Iterationsreihenfolge (Definitionsreihenfolge)
	//				- Zu Tupelsets noch zusätzlich die Position innerhalb der Einzeltupel, wo sie in Iterationsreihenfolge einzufügen sind
	//		- Iteration:
	//			- Beide Arrays parallel durchgehen, wenn Positionsnummer des nächsten Tupelsets innerhalb der Einzeltupel erreicht,
	//				dann erst Tupelset iterieren, sonst weiter mit nächstem Einzeltupel
	//		- Tupelsuche:
	//			- Suche nur in den Arrays des passenden Rangs
	//			- Suche in Tupelsets: alle einzeln prüfen
	//				- pro Index ob zwischen Minimalwert und Maximalwert
	//				- nur wenn das für alle Indizes erfüllt ist prüfen ob im Set jedes Index enthalten
	//				??? (performantere Suche?, geeignete Sortierung?)
	//			- Suche in Enumeration: Intervallsuche


	// Symbolspeicherung:
	//		perspektivisch:
	//			Zwei Möglichkeiten, die gemischt eingesetzt werden:
	//				A: Auf 1 oder mehrere TupelsetEnum bezogen:
	//					- Werte zu jedem TupelsetEnum in einfachem Array gespeichert, in Definitionsreihenfolge des TupelsetEnum
	//					- Zugriff besteht also aus Suche des Tupels, damit Indizierung im Array
	//					- Wird gewählt, wenn in der definierenden Zuweisung Tupelset angegeben (also kein einzelnes Tupel), und keine Elemente in bereits vorhandenen TupelsetEnum's enthalten sind
	//								und Tupelset (eine größere Anzahl Tupel enthält und mindestens Tupel von Rang 2) oder ein Enumeration-Anteil enthält oder Tupel nicht in kanonischer Reihenfolge enthält
	//						- dann Tupelset hinzufügen und Elemente dazu als Array speichern
	//							- falls größere Anzahl TupelsetEnum's erreicht, dann vereinigen (also auch Elementarrays), zur schnelleren Suche nach Tupeln
	//						- wenn Tupelset Elemente enthält, die schon mit Alternative B gespeichert sind, aus Alternative B entfernen
	//				B: Bisherige hierarchische Speicherung:
	//					- Speicherung wie bisher (wie früher pro Rang)
	//					- Wird gewählt wenn in der definierenden Zuweisung einzelnes Tupel, oder kleiner Tupelset ohne Enumeration-Anteil in kanonischer Reihenfolge
	//			- vielleicht: Mischung vermeiden, bei erster Definition gewählte Alternative für das Symbol dauerhaft beibehalten
	//		vorläufig:
	//			- erstmal alles mit B speichern
	//
	// Abruf Tupelset für Symbol:
	//		- 1. Wenn reine Speicherung A mit nur einem TupelsetEnum: direkt zurück
	//		- 2. Wenn reine Speicherung A mit mehreren TupelsetEnum: Vereinigen in ein TupelsetEnum, verwenden für das Symbol und zurück
	//		- 3. Sonst: Erste alle Tupel aus Speicherung A in Definitionsreihenfolge, dann alle Tupel aus Speicherung B in kanonischer Reihenfolge zu einem neuen TupelsetEnum, das zurück
	//		- Vielleicht: auch in der Rückgabe mehrere TupelsetEnum in einem Container zurück, dann braucht im Fall 2 nur ein neuer solcher Container konstruiert werden,
	//						in Fall 3 nur ein TupelsetEnum für Speicherung B und nötigenfalls ein Container
	//
	// Indizierung für Symbol (bzw. auch sonstiger Ausdruck):
	//		theoretisches Vorgehen:
	//			- temporaeres Symbol konstruieren: Tupelset als Durchschnitt Indizierung zu Definitionstupelset, entsprechend Elemente
	//		praktisches (performanteres) Vorgehen:
	//			- wenn einzelnes Tupel: Wert direkt aus Symbol holen
	//			- sonst: ??
	//
	// TupelsetEnum:
	//		- besteht aus: 1.Aufzählung einzelner Tupel, 2.Tupelset's in denen KEINE Enumerationstupel und KEINE unendlichen Sets enthalten sind
	//		- gespeichert pro Rang in einem Array für Aufzählungstupel und zweitem Array für Tupelsets
	//				- Aufzählungstupel aufsteigend sortiert
	//				- Tupelsets: keine Sortierung??
	//		- daneben Speicherung der Definitionsreihenfolge
	//				- innerhalb eines Tupelsets ist Definitionsreihenfolge gleich Speicherreihenfolge, andernfalls Speicherung als Enumeration notwendig
	//		- Tupelsuche:
	//			- Suche nur in den Arrays des passenden Rangs
	//			- Suche in Tupelsets: alle einzeln prüfen
	//				- pro Index ob zwischen Minimalwert und Maximalwert
	//				- nur wenn das für alle Indizes erfüllt ist prüfen ob im Set jedes Index enthalten
	//				??? (performantere Suche?, geeignete Sortierung?)
	//			- Suche in Enumeration: Intervallsuche
	//		- Iteration: Tupel in Definitionsreihenfolge durchgehen
	//		- Tupel einfügen:
	//			- Tupel müssen eindeutig gehalten werden
	//			- Enumeration: jedes Tupel suchen; wenn nicht gefunden dann als Enumerationstupel anfügen
	//			- Tupelset: Test:
	//				- Tupelset ist Untermenge eines bisherigen Tupelsets: nichts einzufügen
	//				- Tupelset enthält keine Elemente des bisherigen ganzen TupelsetEnum (wie effizient prüfen?): Tupelset einfügen
	//				- Sonst: noch fehlende Tupel als Enumerationstupel anfügen
	//			- Spezialfall: Tupel Rang 1:
	//				- Wenn Set im Tupelset algorithmisch, und neuer Tupel das erweitert, dann Erweiterung darin ausführen (Set muss neu definiert werden)


	//	Indexoperator (Durchschnittsbildung):

	// Vereinigung: Erzeugt immer Enumerations-Tupelset; zweites Tupelset wird einfach über _next an das erste angehängt.
	//					Beide Tupelsets müsssen endlich sein, sonst Fehler.
	// Durchschnitt: Wenn ein Tupelset _fix'ed Elemente hat, dann an dieser Stelle Ergebnis _fix'ed.
	//					Wenn beide Set (oder Tupelset Rang 1) an einer Stelle, dann Ergebnis Durchschnitt der Sets
	//					Wenn Tupelset
}

#endif // !CMPL_TUPELSET_HH

