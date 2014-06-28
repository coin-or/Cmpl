/* ****************************************************************************
 * This code is part of jCMPL
 *
 * Copyright (C) 2013 Mike Steglich / B. Knie Technical University of Applied
 * Sciences Wildau, Germany
 *
 * jCMPL is a project of the Technical University of Applied Sciences Wildau
 * and the Institute for Operations Research and Business Management at the
 * Martin Luther University Halle-Wittenberg.
 *
 * Please visit the project homepage <http://www.coliop.org>
 *
 * jCMPL is free software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option) any
 * later version.
 *
 * jCMPL is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU Lesser General Public # License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 * ****************************************************************************/

package jcmpltest;

import jCMPL.*;



/**
 *
 * @author mike
 */
public class ProdMix {
    
     public ProdMix() throws CmplException {
        try {

             Cmpl m = new Cmpl("prodmix.cmpl");

             CmplSet products = new CmplSet("products");
             products.setValues(1, 3);

             CmplSet machines = new CmplSet("machines");
             machines.setValues(1, 2);

             CmplParameter c = new CmplParameter("c", products);
             int[] cVals = {75, 80, 50};
             c.setValues(cVals);

             CmplParameter xmax = new CmplParameter("xMax", products);
             int[] xVals = {250, 240, 250};
             xmax.setValues(xVals);

             CmplParameter b = new CmplParameter("b", machines);
             int[] bVals = {1000, 1000};
             b.setValues(bVals);

             CmplParameter a = new CmplParameter("a", machines, products);
             int aVals[][] = {{8, 15, 12}, {15, 10, 8}};
             a.setValues(aVals);

             m.setSets(products, machines);
             m.setParameters(c, a, b, xmax);

             m.solve();

             m.saveSolution();
             m.saveSolutionAscii();
             m.saveSolutionCsv();


         } catch (CmplException e) {
             System.out.println(e);
         }
     }

}
